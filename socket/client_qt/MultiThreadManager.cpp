#include <qdebug.h>
#include "MultiThreadManager.h"

void MultiThreadManager::initThread()
{
	if (m_thread == nullptr)
	{
		this->m_continue = true;
		//new std::thread(&TcpClient::ReceiveTask, this);
		m_thread = new std::thread(&MultiThreadManager::ReceveThread, this);

	}
}
void MultiThreadManager::ReceveThread()
{
	while (m_continue)
	{
		if (this->m_msg == true)
		{
			qDebug() << __FUNCTION__ << "got";
		}
		//qDebug() << this->m_msg;
	}
	qDebug() << __FUNCTION__ << "Terminated!!!";
}
void MultiThreadManager::finish()
{
	if (m_thread)
	{
		m_continue = false;
		m_thread->detach();
		delete m_thread;
		m_thread = nullptr;
	}
}