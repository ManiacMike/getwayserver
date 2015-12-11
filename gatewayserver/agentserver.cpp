#include "AgentServer.h"

AgentServer::AgentServer()
{
	
}


AgentServer::~AgentServer()
{

}


bool AgentServer::start()
{
	openServer();
}

bool AgentServer::openServer()
{
	//start accept thread
    if(FAILURE_INDEX == createThread())
    {
    	std::cout<<"Create Thread Failed"<<std::endl;
           break;
    }
         std::cout<<"Create Thread Success"<<std::endl;

}


int AgentServer::createThread()
{   
	union
	{   
		void* (*aliasCallback)(void *);
		void* (Core::*memberCallback)();
	} SwitchUnion;
	SwitchUnion.memberCallback = &Core::threadCallback;
	if(SUCCESS_INDEX != pthread_create(&(this->m_accepterTID), nullptr, SwitchUnion.aliasCallback, this))
	{   
		return FAILURE_INDEX;
	}
	return SUCCESS_INDEX;
}

