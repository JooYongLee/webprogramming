#include "qtproject.h"
#include <qimage.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qdebug.h>
#include <qlayout.h>
#include "TcpClient.h"

#define PORT_NUM      5560
#define MAX_MSG_LEN   256
//#define SERVER_IP     "127.0.0.1"//서버 IP 주소
#define SERVER_IP     "192.168.25.40"//서버 IP 주소


class Goo : public QObject
{
	Q_OBJECT
public:
	Goo() {
		connect(this, &Goo::someSignal, this, &QObject::deleteLater);
	}

signals:
	void someSignal();
};


TcpClient *g_client = nullptr;

TcpClient* getClient()
{
	if (g_client == nullptr)
	{
		g_client = new TcpClient;
	}
	return g_client;
}
void releseClient()
{
	if (g_client != nullptr)
	{
		delete g_client;
		g_client = nullptr;
	}
}
void onIncomingMsg(const char * msg, size_t size) {
	std::cout << "Got msg from server: " << msg << std::endl;
}

void onDisconnection(const pipe_ret_t & ret) {
	std::cout << "Server disconnected: " << ret.msg << std::endl;
	std::cout << "Closing client..." << std::endl;
	pipe_ret_t finishRet = g_client->finish();
	if (finishRet.success) {
		std::cout << "Client closed." << std::endl;
	}
	else {
		std::cout << "Failed to close client: " << finishRet.msg << std::endl;
	}
}


int qtProject::connectServer()
{
	bool clientOpen = false;
	//g_client = new TcpClient;
	getClient();
	//signal(SIGINT, sig_exit);

	client_observer_t observer;
	observer.wantedIp = SERVER_IP;
	observer.incoming_packet_func = onIncomingMsg;
	observer.disconnected_func = onDisconnection;
	g_client->subscribe(observer);

	pipe_ret_t connectRet = g_client->connectTo(SERVER_IP, PORT_NUM);
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
		//g_client->Run();
	}
	//delete g_client;
	//releseClient();
	//std::cout << "종료 끝!!";
}


qtProject::qtProject(QWidget *parent)
	: QMainWindow(parent)
{
	std::cout << __FUNCTION__;
	ui.setupUi(this);

	QString imagPath = "2 (5).jpg";
	QImage image;
	image.load(imagPath);
	QImage rszImage = image.scaled(QSize(400, 400));

	QLabel *imagLabel = new QLabel();
	imagLabel->setSizePolicy(QSizePolicy::Expanding,
		QSizePolicy::Expanding);
	imagLabel->setAlignment(Qt::AlignCenter);
	imagLabel->setMinimumSize(400, 400);
	imagLabel->setPixmap(QPixmap::fromImage(rszImage));

	QImage newimage = QImage(rszImage);
	
	QLabel *resultLabel = new QLabel();
	resultLabel->setSizePolicy(QSizePolicy::Expanding,
		QSizePolicy::Expanding);
	resultLabel->setAlignment(Qt::AlignCenter);
	resultLabel->setMinimumSize(400, 400);
	resultLabel->setPixmap(QPixmap::fromImage(rszImage));

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(imagLabel, 0, 0);
	layout->addWidget(resultLabel, 0, 1);
	
	QWidget *tabs = new QWidget;
	tabs->setLayout(layout);
	//this->setLayout(layout);
	this->connectServer();

	std::string to_trasnfer_filename = "hdimage.png";
	//size_t size = getFilesize(to_trasnfer_filename.c_str());

	//this->SendFile(to_trasnfer_filename.c_str());
	getClient()->SendFile(to_trasnfer_filename.c_str());
	//connect(getClient(), TcpClient::TestRun, this, mainrun);
	setCentralWidget(tabs);
}

void qtProject::mainrun()
{

}
qtProject::~qtProject()
{

}
