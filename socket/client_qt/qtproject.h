#ifndef QTPROJECT_H
#define QTPROJECT_H
#pragma comment(linker, "/subsystem:console /entry:mainCRTStartup")
#include <QtWidgets/QMainWindow>

#include "ui_qtproject.h"
class TcpClient;
class qtProject : public QMainWindow
{
	Q_OBJECT

public:
	qtProject(QWidget *parent = 0);
	~qtProject();

	int connectServer();
	void mainrun();
private:
	Ui::qtProjectClass ui;


};

extern 	TcpClient *g_client;
#endif // QTPROJECT_H
