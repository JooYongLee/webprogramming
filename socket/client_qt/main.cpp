#include "qtproject.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	qtProject w;
	w.show();
	return a.exec();
}
