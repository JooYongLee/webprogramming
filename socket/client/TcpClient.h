#pragma once
#include <thread>
#include <errno.h>
#include <vector>
#include <iostream>
#include <string>
#include <winsock2.h>



#define MAX_PACKET_SIZE 4096

struct pipe_ret_t {
	bool success;
	std::string msg;
	pipe_ret_t() {
		success = false;
		msg = "";
	}
};

typedef void (incoming_packet_func)(const char * msg, size_t size);
typedef incoming_packet_func* incoming_packet_func_t;

typedef void (disconnected_func)(const pipe_ret_t & ret);
typedef disconnected_func* disconnected_func_t;

struct client_observer_t {

	std::string wantedIp;
	incoming_packet_func_t incoming_packet_func;
	disconnected_func_t disconnected_func;

	client_observer_t() {
		wantedIp = "";
		incoming_packet_func = NULL;
		disconnected_func = NULL;
	}
};



class TcpClient
{
private:
	//int m_sockfd = 0;
	bool stop = false;
	SOCKET m_sock;
	std::thread *m_pReceiveTask = nullptr;
	std::vector<client_observer_t> m_subscibers;
	SOCKADDR_IN m_serverAddr = { 0 };//家南 林家


	void publishServerMsg(const char * msg, size_t msgSize);
	void publishServerDisconnected(const pipe_ret_t & ret);
	void ReceiveTask();
	void terminateReceiveThread();
	void TranslateMsg(std::string msg);
public:
	void SendFile(const char *filepath);
	void Run();
	pipe_ret_t finish();
	~TcpClient();

	void subscribe(const client_observer_t & observer);
	void unsubscribeAll();
	pipe_ret_t connectTo(const std::string & address, int port);
	pipe_ret_t sendMsg(const char * msg, size_t size);
};
