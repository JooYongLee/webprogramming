#include "TcpClient.h"
#include <sstream>  
#include <fstream>
#include <ctime>
#include "opencv2/opencv.hpp"

#include "utils.h"

using namespace std;
using namespace std::chrono;

void TcpClient::terminateReceiveThread() {
	if (m_pReceiveTask != nullptr) {
		m_pReceiveTask->detach();
		delete m_pReceiveTask;
		m_pReceiveTask = nullptr;
	}
}
TcpClient::~TcpClient() 
{
	terminateReceiveThread();
}



pipe_ret_t TcpClient::finish() 
{
	stop = true;
	terminateReceiveThread();
	pipe_ret_t ret;
	if (closesocket(m_sock) == -1) { // close failed
		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
	}
	ret.success = true;
	return ret;
}

void tproc()
{

}

size_t getFilesize(const char* filename)
{
	struct stat st;
	if (stat(filename, &st) != 0) {
		return 0;
	}
	return st.st_size;
}

void TcpClient::SendFile(const char *filepath)
{
	std::fstream fileReader;
	fileReader.open(filepath, std::ios::in | std::ios::binary);

	const int c_strLen = MAX_PATH;
	if (fileReader.is_open() && fileReader.good())
	{
		fileReader.seekg(0, std::ios::end);
		size_t size = fileReader.tellg();
		char headerMsg[c_strLen] = "";
		//std::string mesg = "size:" + std::to_string(size);
		fileReader.seekg(0, std::ios::beg);

		sprintf(headerMsg, "image:%d", size);

		char *buffer = new char[size + c_strLen];
		memset(buffer, 0, size + c_strLen);
		//strcpy(buffer, headerMsg)
		memcpy(buffer, headerMsg, strlen(headerMsg));

		fileReader.read(buffer + c_strLen, size);
		
		send(m_sock, buffer, size + c_strLen, 0);
		//int numBytesSent = send(m_sock, msg, size, 0);
		delete[]buffer;
		std::cout << "전송 완료:" << size+c_strLen << std::endl;
	}

}

void TcpClient::TestRun()
{

}
void TcpClient::Run()
{
	std::string to_trasnfer_filename = "hdimage.png";
	size_t size = getFilesize(to_trasnfer_filename.c_str());
	
	this->SendFile(to_trasnfer_filename.c_str());
	//std::string mesg = "size:" + std::to_string(size);
	//this->sendMsg(mesg.c_str(), mesg.length());
	
	while (!stop)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		//std::cout << __FUNCTION__ << "\n";
		//std::cout << stop;
	//	std::cout << "--------->\n";
	}
	std::cout <<  __FUNCTION__ <<":"<< stop << "\n";
}
pipe_ret_t TcpClient::sendMsg(const char * msg, size_t size) 
{
	pipe_ret_t ret;
	int numBytesSent = send(m_sock, msg, size, 0);
	if (numBytesSent < 0) { // send failed
		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
	}
	if ((int)numBytesSent < size) { // not all bytes were sent
		ret.success = false;
		char msg[100] = "";
		sprintf(msg, "Only %d bytes out of %lu was sent to client", numBytesSent, size);
		ret.msg = msg;
		return ret;
	}
	ret.success = true;
	return ret;

}
void TcpClient::subscribe(const client_observer_t & observer) 
{
	m_subscibers.push_back(observer);
}

void TcpClient::unsubscribeAll() 
{
	m_subscibers.clear();
}

pipe_ret_t TcpClient::connectTo(const std::string & address, int port) {
	
	pipe_ret_t ret;

	//m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//if (m_sockfd == -1) { //socket failed
	//	ret.success = false;
	//	ret.msg = strerror(errno);
	//	return ret;
	//}

	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);//윈속 초기화	

	//SOCKET sock;
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//소켓 생성
	if (m_sock == -1) { //socket failed
		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
	}


	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_addr.s_addr = inet_addr(address.c_str());
	m_serverAddr.sin_port = htons(port);

	int connectRet = 0;
	connectRet = ::connect(m_sock, (struct sockaddr *)&m_serverAddr, sizeof(m_serverAddr));//연결 요청


	if (connectRet == -1) {
		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
	}


	m_pReceiveTask = new std::thread(&TcpClient::ReceiveTask, this);
	ret.success = true;
	return ret;
}

void TcpClient::TranslateMsg(std::string msg)
{
	std::cout << "translate msg:" << msg <<"\n";
	if (!msg.compare("y"))
	{
		std::cout << "stop!!\n";
		stop = true;
		std::cout << __FUNCTION__ << "bool addr" << &stop << "\n";
	}
	else
	{
		std::cout << "continue!!\n";
	}
}

void write_buffer(void *buffer, long long filesize)
{
	cv::Mat rawData(1, filesize, CV_8UC1, buffer);
	cv::Mat decodedImage = cv::imdecode(rawData, cv::IMREAD_UNCHANGED);
	if (decodedImage.data == NULL)
	{
		std::cout << "decoding failed\n";
	}
	if (!decodedImage.empty())
	{
		cv::imwrite("loaded22.png", decodedImage);
	}



	//std::chrono::system_clock::time_point t;
	//auto ttime_t = system_clock::to_time_t(tp);
	//auto tp_sec = system_clock::from_time_t(ttime_t);
	//milliseconds ms = duration_cast<milliseconds>(tp - tp_sec);

	//std::tm * ttm = localtime(&ttime_t);

	//char date_time_format[] = "%Y.%m.%d-%H.%M.%S";

	//char time_str[] = "yyyy.mm.dd.HH-MM.SS.fff";

	//strftime(time_str, strlen(time_str), date_time_format, ttm);
}
/*
* Receive server packets, and notify user
*/
void TcpClient::ReceiveTask() {

	long long receiveSize = 0;
	const int headerSize = MAX_PATH;
	long long bodySize = 0;
	char *buffer = nullptr;
	char header[MAX_PATH] = "";
	while (!stop) 
	{
		char msg[headerSize];
		int numOfBytesReceived = recv(m_sock, msg, headerSize, 0);


			
		//std::cout << __FUNCTION__ << "\n";
		if (numOfBytesReceived < 1) {
			pipe_ret_t ret;
			ret.success = false;
			stop = true;
			if (numOfBytesReceived == 0) { //server closed connection
				ret.msg = "Server closed connection";
			}
			else {
				ret.msg = strerror(errno);
			}
			
			publishServerDisconnected(ret);
			finish();
			break;
		}
		else {

			receiveSize += numOfBytesReceived;

			if (receiveSize < headerSize)
			{
				memcpy(header + receiveSize, msg, headerSize - receiveSize);
			}

			if (buffer !=nullptr && receiveSize > headerSize)
			{
				//if(receiveSize <3000 || receiveSize > 5000000)
				//if (numOfBytesReceived!=260)
				//	std::cout << "body receive size" << receiveSize - headerSize << ","
				//	<< receiveSize<< ","
				//	<< headerSize << ","
				//	<< numOfBytesReceived <<"\n";
					
				memcpy(&buffer[receiveSize - headerSize - numOfBytesReceived], msg, numOfBytesReceived);
			}




			if (receiveSize <= MAX_PATH)
			{
				std::cout << "message header" <<msg << "\n";
				std::vector<std::string>  strSplit;
				std::string headerMsg = msg;
				str_split(headerMsg, strSplit, ':');
				try
				{
					bodySize = std::stoll(strSplit.at(1));
				}
				catch (std::exception &e)
				{
					std::cout << e.what() << "\n";
				}
				if (buffer)
				{
					delete[]buffer;
				}
				std::cout << "body size:" << bodySize << "\n";
				buffer = new char[bodySize];
			}
			else if (receiveSize == bodySize + headerSize)//complete
			{
				std::cout << "receive complete\n" << receiveSize << "\n";
				std::cout << "body receive size" << receiveSize - headerSize << ","
					<< receiveSize << ","
					<< headerSize << "\n";

				write_buffer(buffer, bodySize);


				// clear
				receiveSize = 0;
				bodySize = 0;

				
				delete[]buffer;
				buffer = nullptr;
			}
			else
			{
				//if (receiveSize < 1000)
				//{
				//	std::cout << "receiving...." << receiveSize;
				//}
				//std::cout << receiveSize << ","
				//	<< bodySize << ","
				//	<< headerSize << "\n";
			}

			//printf("receive msg:%d\n", receiveSize);
			//TranslateMsg(msg);
			//publishServerMsg(msg, numOfBytesReceived);
		}
	}
}


/*
* Publish incoming client message to observer.
* Observers get only messages that originated
* from clients with IP address identical to
* the specific observer requested IP
*/
void TcpClient::publishServerMsg(const char * msg, size_t msgSize) {
	for (int i = 0; i<m_subscibers.size(); i++) {
		if (m_subscibers[i].incoming_packet_func != NULL) {
			(*m_subscibers[i].incoming_packet_func)(msg, msgSize);
		}
	}
}

/*
* Publish client disconnection to observer.
* Observers get only notify about clients
* with IP address identical to the specific
* observer requested IP
*/
void TcpClient::publishServerDisconnected(const pipe_ret_t & ret) {
	for (int i = 0; i<m_subscibers.size(); i++) {
		if (m_subscibers[i].disconnected_func != NULL) {
			(*m_subscibers[i].disconnected_func)(ret);
		}
	}
}