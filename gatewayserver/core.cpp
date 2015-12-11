#include "core.h"

Core::Core()
{
	m_pAgentServer = new AgentServer(this);
	m_fdEpoll = epoll_create1(EPOLL_CLOEXEC);
	if(-1 == m_fdEpoll)
	{
		perror("epoll_create1");
		exit(1);
	}
}

Core::~Core()
{
	stop();
	
	std::vector<WorkThread*>::iterator iter = m_vecWorkers.begin();
	for(;iter != m_vecWorkers.end(); ++iter)
	{
		delete (*iter);
		m_vecWorkers.erase(iter);
		print("Work Threads Deleted!!");
	}

	if(m_pAgents)
		delete m_pAgents;
	m_pAgents = nullptr;
	
	if(m_pAgentServer)
		delete m_pAgentServer;
	m_pAgentServer = nullptr;
}

bool Core::start()
{
	bool bRet = false;
	try
	{
		do
		{
			//TODO
			//ConfigSvr::loadServiceOption(m_cfg);

			//start agentserver
			m_pAgentServer->start();


			//start threadpool
			int iThreadCount = std::stoi(m_cfg[THREADPOOL_SIZE]);
			for(int i = 0; i < iThreadCount; ++i)
			{
				WorkThread *worker = new WorkThread(this);
				if(worker->open())
				{
					m_vecWorkers.push_back(worker);
				}
			}

			bRet = true;
		}
		while(0)

	}
	catch(...)
	{
		
	}

	return bRet;
}


Core::stop()
{

}


