#ifndef _CORE_H_
#define _CORE_H_

#include <unistd>
#include <stdio.h>
#include <string>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

class Core
{
public:
	Core();
	~Core();
	void start();
	void stop();

	std::vector<WorkThread*> m_vecWorkers;
	AgentServer *m_pAgents;
	int m_fdEpoll;
private:
	
}




#endif
