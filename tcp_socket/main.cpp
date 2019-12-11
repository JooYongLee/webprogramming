#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <chrono>

#include <iostream>
#include <string>
#include <direct.h>   // _mkdir
#include <errno.h>    // errno, ENOENT, EEXIST

#include <sys/stat.h>

#include <locale>
#include <codecvt>
#include "opencv2/opencv.hpp"

char to_trasnfer_filename[] = "1539517690-1.png";
size_t getFilesize(const char* filename);
std::string get_current_time_str();
//using namespace std;
// ref:http://ehpub.co.kr/5-1-%EC%B1%84%ED%8C%85-%EC%84%9C%EB%B2%84-%EA%B5%AC%ED%98%84-tcpip-%EC%86%8C%EC%BC%93-%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%98%EB%B0%8D-with-%EC%9C%88%EB%8F%84%EC%9A%B0%EC%A6%88/
#define PORT_NUM      10200
#define BLOG_SIZE     5
#define MAX_MSG_LEN   1024
#pragma comment(lib,"ws2_32")

SOCKET SetTCPServer(short pnum, int blog);//대기 소켓 설정
void EventLoop(SOCKET sock);//Event 처리 Loop

char REQUEST_IMAGE_INFO[] = "request_image_info";
char REQUEST_IMAGE[] = "request_image";

char READY_FROM[] = "ready_client";
bool is_take_enable = false;
bool is_transfer_enable = false;
long long image_size = -1;



IN_ADDR GetDefaultMyIP()
{
	char localhostname[MAX_PATH];
	IN_ADDR addr = { 0, };

	if (gethostname(localhostname, MAX_PATH) == SOCKET_ERROR)//호스트 이름 얻어오기
	{
		return addr;
	}
	HOSTENT *ptr = gethostbyname(localhostname);//호스트 엔트리 얻어오기
	while (ptr && ptr->h_name)
	{
		if (ptr->h_addrtype == PF_INET)//IPv4 주소 타입일 때
		{
			memcpy(&addr, ptr->h_addr_list[0], ptr->h_length);//메모리 복사
			break;//반복문 탈출
		}
		ptr++;
	}
	return addr;
}

void run_socket_main()
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);//윈속 초기화	
	SOCKET sock = SetTCPServer(PORT_NUM, BLOG_SIZE);//대기 소켓 설정
	if (sock == -1)
	{
		perror("대기 소켓 오류");
	}
	else
	{
		EventLoop(sock);
	}
	WSACleanup();//윈속 해제화
}
#include <fstream>

void read_image_from_buffer(void *buffer, long long file_size, bool is_show=false)
{
	cv::Mat rawData(1, file_size, CV_8UC1, (void*)buffer);

	cv::Mat decodedImage = cv::imdecode(rawData /*, flags */, cv::IMREAD_UNCHANGED);
	if (decodedImage.data == NULL)
	{
		std::cout << "failed\n";
		// Error reading raw image data
	}
	else
	{
		if (is_show)
		{
			cv::imshow("r", decodedImage);
			cv::waitKey();
		}

		std::string current_time = get_current_time_str();
		current_time += ".png";
		//cv::imwrite("")
		//int ret = mkdir(path.c_str(), mode);
		std::string save_path = "result";

		//if (stat(save_path.c_str(), &status) != 0)
		//{
		//	/* Directory does not exist. EEXIST for race condition */

		//}
		//cv::imwrite()
		//struct mode_t mode = 0755;
		std::string filename = save_path + "\\" + current_time;
		struct stat status;
		if (stat(save_path.c_str(), &status) != 0)
		{
			// path does not exist
			if (mkdir(save_path.c_str()) != 0 && errno != EEXIST)
			{
				
			}
			
		}
		cv::imwrite(filename.c_str(), decodedImage);
		

		//t->tm_hour()
	}
}
//time_in_HH_MM_SS_MMM()
//https://stackoverflow.com/questions/24686846/get-current-time-in-milliseconds-or-hhmmssmmm-format
std::string get_current_time_str()
{
	using namespace std::chrono;

	// get current time
	auto now = system_clock::now();

	// get number of milliseconds for the current second
	// (remainder after division into seconds)
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

	// convert to std::time_t in order to convert to std::tm (broken time)
	auto timer = system_clock::to_time_t(now);

	// convert to broken time
	std::tm bt = *std::localtime(&timer);

	std::ostringstream oss;

	oss << std::put_time(&bt, "%H_%M_%S"); // HH:MM:SS
	oss << '_' << std::setfill('0') << std::setw(3) << ms.count();
	
	return oss.str();
}
void opencv_image_read_test()
{
	std::string file_path = "siba.jpg";
	//cv::Mat img = cv::imread("siba.jpg", -1);
	//cv::imshow("", img);
	//cv::waitKey();
	std::fstream file_reader;
	file_reader.open(file_path, std::ios::in | std::ios::binary);
	file_reader.seekg(0, std::ios::end);
	long long file_size = file_reader.tellg();
	std::cout << "readed size" << file_size << std::endl;

	file_reader.seekg(0, std::ios::beg);
	char *buffer = new char[file_size];
	file_reader.read(buffer, file_size);

	read_image_from_buffer(buffer, file_size);
	delete[]buffer;
	file_reader.close();
}
int main()
{
	
	run_socket_main();
	//std::cout << get_current_time_str() << std::endl;
	//Sleep(50);
	//std::cout << get_current_time_str() << std::endl;
	//Sleep(1000);
	//std::cout << get_current_time_str() << std::endl;
	return 0;
}
SOCKET SetTCPServer(short pnum, int blog)
{
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//소켓 생성
	if (sock == -1)
	{
		return -1;
	}

	SOCKADDR_IN servaddr = { 0 };//소켓 주소
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr = GetDefaultMyIP();
	servaddr.sin_port = htons(PORT_NUM);

	int re = 0;
	//소켓 주소와 네트워크 인터페이스 결합
	
	re = bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (re == -1)
	{
		return -1;
	}

	re = listen(sock, blog);//백 로그 큐 설정
	if (re == -1)
	{
		return -1;
	}
	return sock;
}

SOCKET  sock_base[FD_SETSIZE];
HANDLE hev_base[FD_SETSIZE];
int cnt;

HANDLE AddNetworkEvent(SOCKET sock, long net_event)
{
	HANDLE hev = WSACreateEvent();

	sock_base[cnt] = sock;
	hev_base[cnt] = hev;
	cnt++;

	WSAEventSelect(sock, hev, net_event);
	return hev;
}
void AcceptProc(int index);
void ReadProc(int index);
void CloseProc(int index);
void EventLoop(SOCKET sock)
{
	AddNetworkEvent(sock, FD_ACCEPT);

	while (true)
	{
		int index = WSAWaitForMultipleEvents(cnt, hev_base, false, INFINITE, false);
		WSANETWORKEVENTS net_events;
		WSAEnumNetworkEvents(sock_base[index], hev_base[index], &net_events);
		switch (net_events.lNetworkEvents)
		{
		case FD_ACCEPT: AcceptProc(index); break;
		case FD_READ: ReadProc(index); break;
		case FD_CLOSE: CloseProc(index); break;
		}
	}
	closesocket(sock);//소켓 닫기
}

void AcceptProc(int index)
{
	SOCKADDR_IN cliaddr = { 0 };
	int len = sizeof(cliaddr);
	SOCKET dosock = accept(sock_base[0], (SOCKADDR *)&cliaddr, &len);

	if (cnt == FD_SETSIZE)
	{
		printf("채팅 방에 꽉 차서 %s:%d 입장하지 못하였음!\n",
			inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
		closesocket(dosock);
		return;
	}

	AddNetworkEvent(dosock, FD_READ | FD_CLOSE);
	printf("%s:%d 입장\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
}

void ReadProc(int index)
{

	

	if (is_take_enable == true)
	{
		if (image_size > 0)
		{	
			//char msg[MAX_MSG_LEN] = "";
			char *img = new char[image_size];
			recv(sock_base[index], img, image_size, 0);

			//std::fstream file_reader;			
			//std::string file_path = "siba.jpg";
			//file_reader.open(file_path, std::ios::in | std::ios::binary);
			//
			//file_reader.seekg(0, std::ios::end);
			//long long file_size = file_reader.tellg();
			//std::cout << "readed size" << file_size << std::endl;
			//file_reader.seekg(0, std::ios::beg);
			//char *buffer = new char[file_size];
			//file_reader.read(buffer, file_size);

			read_image_from_buffer(img, image_size);

			is_take_enable = false;
			image_size = -1;
		}
		else
		{

			char msg[MAX_MSG_LEN] = "";
			recv(sock_base[index], msg, MAX_MSG_LEN, 0);
			image_size = atoi(msg);
			std::cout << "read image size :" << image_size;

			char msg_clinet[MAX_MSG_LEN] = "";
			sprintf(msg_clinet, REQUEST_IMAGE);
			for (int i = 1; i<cnt; i++)
			{
				send(sock_base[i], msg_clinet, MAX_MSG_LEN, 0);
			}
		}
	}
	else if(is_transfer_enable)
	{
		char msg[MAX_MSG_LEN] = "";
		recv(sock_base[index], msg, MAX_MSG_LEN, 0);
		std::cout << "이미지 전송 준비 단계 from client:" << msg << std::endl;
		if (!strcmp(msg, READY_FROM))
		{
			std::fstream stream;
			stream.open(to_trasnfer_filename, std::ios::in | std::ios::binary);
			if (stream.is_open())
			{
				stream.seekg(0, std::ios::end);
				size_t size = stream.tellg();
				char *buffer = new char[size];
				stream.seekg(0, std::ios::beg);

				stream.read(buffer, size);
				for (int i = 1; i < cnt; i++)
				{
					send(sock_base[i], buffer, size, 0);
				}
				delete[]buffer;
				std::cout << "전송 완료" << std::endl;
			}
			else
			{
				// faile message
			}
			is_transfer_enable = false;
		}
		//READY_FROM
	}
	else
	{
		char msg[MAX_MSG_LEN] = "";
		recv(sock_base[index], msg, MAX_MSG_LEN, 0);

		SOCKADDR_IN cliaddr = { 0 };
		int len = sizeof(cliaddr);
		getpeername(sock_base[index], (SOCKADDR *)&cliaddr, &len);



		char smsg[MAX_MSG_LEN] = "";
		//std::string_con
		sprintf(smsg, "[%s:%d]:%s from server\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), msg);

		if (!strcmp(msg, "image"))
		{
			is_take_enable = true;
			std::cout << "image 받을 준비가 되었습니다!\n" << std::endl;
			char msg_clinet[MAX_MSG_LEN] = "";
			sprintf(msg_clinet, REQUEST_IMAGE_INFO);
			for (int i = 1; i<cnt; i++)
			{
				send(sock_base[i], msg_clinet, MAX_MSG_LEN, 0);
			}
		}
		else if (!strcmp(msg, "giveme"))
		{
			size_t size = getFilesize(to_trasnfer_filename);
			std::cout << "이미지 전송 준비 중..\n";
			std::cout << size << std::endl;
			
			char msg_clinet[MAX_MSG_LEN] = "";
			sprintf(msg_clinet, "%d", size);
			for (int i = 1; i<cnt; i++)
			{
				send(sock_base[i], msg_clinet, MAX_MSG_LEN, 0);
			}
		
			is_transfer_enable = true;
		}
		else
		{
			//	wchar_t uchars[] = { 0x5e9, 0x5dc, 0x5d5, 0x5dd, 0 };
			//std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
			//std::wstring ws2 = conv.from_bytes(msg);
			//std::cout << ws2.c_str() <<  std::endl;
			for (int i = 1; i<cnt; i++)
			{
				send(sock_base[i], smsg, MAX_MSG_LEN, 0);
			}

		}

	}


}

size_t getFilesize(const char* filename) 
{
	struct stat st;
	if (stat(filename, &st) != 0) {
		return 0;
	}
	return st.st_size;

}
void CloseProc(int index)
{
	SOCKADDR_IN cliaddr = { 0 };
	int len = sizeof(cliaddr);
	getpeername(sock_base[index], (SOCKADDR *)&cliaddr, &len);
	printf("[%s:%d]  님 나감~\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

	closesocket(sock_base[index]);
	WSACloseEvent(hev_base[index]);

	cnt--;
	sock_base[index] = sock_base[cnt];
	hev_base[index] = hev_base[cnt];

	char msg[MAX_MSG_LEN];
	sprintf(msg, "[%s:%d]님 나감~\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
	for (int i = 1; i<cnt; i++)
	{
		send(sock_base[i], msg, MAX_MSG_LEN, 0);
	}
}