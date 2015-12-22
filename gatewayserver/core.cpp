#include "core.h"

Core::Core()
{
	m_pAcceptor = new Acceptor();
	m_pDispatcher = new Dispatcher();
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

	if(m_pAcceptor)
		delete m_pAcceptor;
	m_pAcceptor = nullptr;
	
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
			
			//start acceptor
			if(m_pAcceptor->start() == FAILURE_INDEX)
			{
				std::cout<<"Agent Server Start Failed!!"<<std::endl;
				break;
			}	
			//start acceptor workers
			m_pAcceptor->startThreadPool(1);
			
			//start dispatcher
			if(m_pDispatcher->start() == FAILURE_INDEX)
			{
				std::cout<<"Dispatcher Start Failed!!"<<std::endl;
				break;

			}
			//start dispatcher workers
			m_pDispatcher->startThreadPool(1);


			
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


