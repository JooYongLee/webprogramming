#ifndef MULTI_THREAD_MANAGER_H
#define MULTI_THREAD_MANAGER_H

#include <thread>

class MultiThreadManager 
{	
public:
	MultiThreadManager()
		: m_thread(nullptr)
		, m_continue(true)
	{}
	std::thread *m_thread;
	void initThread();
	void ReceveThread();
	void finish();

public:
	bool m_continue = true;
	bool m_msg = false;
};
#endif // MULTI_THREAD_MANAGER_H
