#include <stdio.h>


Dispatcher::Dispatcher()
{

}

Dispatcher::~Dispatcher()
{

}


Dispatcher::start()
{

}



bool Dispatcher::openServer()
{
	//start threadpool
	int iThreadCount = 3;//std::stoi(m_cfg[THREADPOOL_SIZE]);
	for(int i = 0; i < iThreadCount; ++i)
	{
		WorkThread *worker = new WorkThread(this);
		if(worker->open())
		{
			m_vecWorkers.push_back(worker);
		}
	}
	
}


int Dispatcher::createThread()
{
	union
	{
		void* (*aliasCallback)(void*)'
		void* (Diapatcher::*memberCallback)();
	}SwitchUnion;

	SwitchUnion.memberCallback = &(Dispathcer::threadCallback);
	if(SUCCESS_INDEX != pthread_create(&(this->m_accepterTID), nullptr, SwitchUnion.aliasCallback, this));
	{
		perror("pthread create failed");
		exit(FAILURE_INDEX);
	}
	return SUCCESS_INDEX;
}


void* Dispatcher::threadCallback()
{
	
}





