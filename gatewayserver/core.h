#ifndef _CORE_H_
#define _CORE_H_

#include <unistd>
#include <stdio.h>
#include <string>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include "datastruct.h"

class Core
{
public:
	Core();
	~Core();
	void start();
	void stop();
	int createThread();
	void* threadCallback();
	std::vector<WorkThread*> m_vecWorkers;
	int m_fdEpoll;
	AgentServer *m_pAgentServer;
private:
	
}




#endif
