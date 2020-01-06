#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <exception>
#include "TcpClient.h"

#include "utils.h"

#pragma comment(lib,"ws2_32")

#define PORT_NUM      5560
#define MAX_MSG_LEN   256
//#define SERVER_IP     "127.0.0.1"//辑滚 IP 林家
#define SERVER_IP     "192.168.25.40"//辑滚 IP 林家

#include <process.h>


TcpClient *client;
//unsigned int _beginthread(void(*_ThreadEntryPoint) (void *), unsigned _stacksize, void * param);
void RecvThreadPoint(void *param);

void RecvThreadPoint(void *param)
{
	SOCKET sock = (SOCKET)param;
	char msg[MAX_MSG_LEN] = "";

	SOCKADDR_IN cliaddr = { 0 };
	int len = sizeof(cliaddr);

	while (recv(sock, msg, MAX_MSG_LEN, 0)>0)
	{
		printf("%s\n", msg);
	}
	closesocket(sock);
	std::cout << __FUNCTION__;
}


int test_client_example()
{
	printf("clinet\n");
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);//扩加 檬扁拳	

	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//家南 积己
	if (sock == -1)
	{
		return -1;
	}

	SOCKADDR_IN servaddr = { 0 };//家南 林家
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	servaddr.sin_port = htons(PORT_NUM);

	int re = 0;
	re = connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));//楷搬 夸没
	if (re == -1)
	{
		return -1;
	}
	_beginthread(RecvThreadPoint, 0, (void *)sock);
	char msg[MAX_MSG_LEN] = "";
	while (true)
	{
		//sprintf_s(msg, "ascii");
		//send(sock, msg, sizeof(msg), 0);
		//recv(sock, )

		gets_s(msg, MAX_MSG_LEN);
		send(sock, msg, sizeof(msg), 0);//价脚
		if (strcmp(msg, "exit") == 0)
		{
			break;
		}
	}
	closesocket(sock);//家南 摧扁    

	WSACleanup();//扩加 秦力拳
}

void onIncomingMsg(const char * msg, size_t size) {
	std::cout << "Got msg from server: " << msg << std::endl;
}

void onDisconnection(const pipe_ret_t & ret) {
	std::cout << "Server disconnected: " << ret.msg << std::endl;
	std::cout << "Closing client..." << std::endl;
	pipe_ret_t finishRet = client->finish();
	if (finishRet.success) {
		std::cout << "Client closed." << std::endl;
	}
	else {
		std::cout << "Failed to close client: " << finishRet.msg << std::endl;
	}
}





int tcp_client_run()
{
	bool clientOpen = false;
	client = new TcpClient;
	//signal(SIGINT, sig_exit);

	client_observer_t observer;
	observer.wantedIp = "127.0.0.1";
	observer.incoming_packet_func = onIncomingMsg;
	observer.disconnected_func = onDisconnection;
	client->subscribe(observer);

	pipe_ret_t connectRet = client->connectTo(SERVER_IP, PORT_NUM);
	if (connectRet.success) {
		std::cout << "Client connected successfully" << std::endl;
		clientOpen = true;
	}
	else {
		std::cout << "Client failed to connect: " << connectRet.msg << std::endl;
		return 1;
	}
	if (clientOpen)
	{
		client->Run();
		//std::string msg = "1";
		//pipe_ret_t sendRet = client.sendMsg(msg.c_str(), msg.size());
		//std::cout << "send message to server:" << msg << "\n\n";
		//if (!sendRet.success) {
		//	std::cout << "Failed to send msg: " << sendRet.msg << std::endl;
		//	break;
		//}
		//std::this_thread::sleep_for(std::chrono::seconds(1));
		////sleep(1);
	}
	delete client;
	std::cout << "辆丰 场!!";
}

void stoi_test()
{
	std::string msg = "image:101212";
	std::vector<std::string> msglist;
	str_split(msg, msglist, ':');
	for (auto it = msglist.begin(); it != msglist.end(); ++it)
	{
		std::cout << *it << "\n";
	}


	try
	{
		std::cout << "------->" << std::stoi(msglist.at(0));
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << "\n";
	}
}
int main()
{
	tcp_client_run();
	//msglist[1]

	return 0;
}
