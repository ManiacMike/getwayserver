#include "WorkThread.h"

WorkThread::WorkThread()
{

}

WorkThread::~WorkThread()
{

}


bool WorkThread::start()
{	

	if(SUCCESS_INDEX == createThread())
	{
		std::cout<<"Thread Create Success, ID: "<<ConfigSvr::intToStr(m_tid)<<std::endl;
		return true;
	}

	perror("Thread Create Failed"<<std::endl;
	return false;
}

void WorkThread::close()
{
	if(SUCCESS_INDEX == pthread_cancel(m_tid))
	{
		if(pthread_join(m_tid, NULL) != SUCCESS_INDEX)
		{
			perror("Thread Join Error, ID: " + ConfigSvr::intToStr(m_tid));
		}
		else
		{
			std::cout<<"Thread Join Success, ID: " + ConfigSvr::intToStr(m_tid)<<std::endl;
		}
	}
	else
	{
		std::cout<<"Send Cancel Failed"<<std::endl;
	}
}

pthread_t WorkThread::gettid()
{
	return m_tid;
}	

int WorkThread::createThread()
{
	union {
		void* (*aliasCallback)(void *);
		void* (WorkThread::*memberCallback)();
	} SwitchProc;
	SwitchProc.memberCB = &WorkThread::threadCallback;
	
	if(pthread_create(&(this->m_tid), NULL, SwitchProc.aliasCallback, this) != 0)
	{
		return -1;
	}

	return 0;

}


void* WorkThread::threadCallback()
{
	//initialRabbitMQ(m_tid);
	
	Config *cfg = &m_cfg;
	//pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL); // 设置其他线程可以cancel掉此线程
	
	
	string sLogicPipeName = m_cfg[PIPE_NAMEPREFIX] + "0";
	string sIMPipeName = m_cfg[PIPE_NAMEPREFIX] + "1";
	string sOtherPipeName = m_cfg[PIPE_NAMEPREFIX] + "2";
	m_LogicPipeReader.open(sLogicPipeName, m_cfg[PIPE_MODE]);
	m_IMPipeReader.open(sIMPipeName, m_cfg[PIPE_MODE]);
	m_OtherPipeReader.open(sOtherPipeName, m_cfg[PIPE_MODE]);

	m_events = (epoll_event*)calloc(EPOLL_MAX_EVENTS, sizeof(m_events[0]));
	while(1)
	{
		std::cout<<m_tid<<" thread running"<<std::endl;

		MsgData msgData;
		int iRet = -1;
		int iWorkType = -1;
		bool bLogicState = isLogicConnected();
		if(false == bLogicState)
		{
			reconnectLogic();
			continue;
		}
		else if(true == bLogicState)
		{
			std::cout<<m_tid<<" epoll start wait..."<<std::endl;
			int iEventCount = epoll_wait(owner->m_fdEpoll, m_events, EPOLL_MAX_EVENTS, std::stoi(m_cfg[THREADPOOL_TIMEOUT]));
			

			if(-1 == iEventCount)
			{
				std::cout<<m_tid<<" Epoll Error:"<<errno<<std::endl;
			}
			if(0 == iEventCount)
			{
				//Timeout
				loopAllMQ();
				std::cout<<m_tid<<" SIGNAL_HEARTBEAT"<<std::endl;
				iWorkType = SIGNAL_HEARTBEAT;
				
				doWork(iWorkType);
			}
			else
			{
				std::cout<<m_tid<<" epoll got wait!!!"<<std::endl;
				std::cout<<m_tid<<" Signal Count: "<<iEventCount<<std::endl;
				
				for(int i = 0; i < iEventCount; ++i)
				{
					if ((m_events[i].events & EPOLLERR) ||  
              					(m_events[i].events & EPOLLHUP) ||  
              					(!(m_events[i].events & EPOLLIN)))  
					{
						std::cout<<m_tid<<" epoll_wait error"<<std::endl;
              					::close(m_events[i].data.fd); 
              					continue;
					}
					else if(owner->m_fdLogicPipe == m_events[i].data.fd)
					{
						if(FAILED_INDEX ==  m_QuickPipeReader.read())
							continue;
						std::cout<<m_tid<<" SIGNAL_QUICK"<<std::endl;
						iWorkType = SIGNAL_QUICK;
					}
					else if(owner->m_fdIMPipe == m_events[i].data.fd)
					{
						if(FAILED_INDEX == m_IMPipeReader.read())
							continue;
						std::cout<<m_tid<<" SIGNAL_NORMAL"<<std::endl;
						iWorkType = SIGNAL_NORMAL;
					}
					else if(owner->m_fdOtherPipe == m_events[i].data.fd)
					{
						if(FAILED_INDEX == m_OtherPipeReader.read())
							continue;
						std::cout<<m_tid<<" SIGNAL_RETRY"<<std::endl;
						iWorkType = SIGNAL_RETRY;
					}
					else
					{
						perror("Unknow Event Tyoe");
					}

					doWork(iWorkType);
				}
				continue;
			}
		}
	}

	m_spBaseDB->close();
	pthread_exit(NULL); //退出线程a
}



}













