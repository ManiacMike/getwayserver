#include "TcpServer.h"

TcpServer::TcpServer()
{
	
	m_fdEpoll = epoll_create1(EPOLL_CLOEXEC);
	m_fdNotifyEpoll = epoll_create1(EPOLL_CLOEXEC);
	if(-1 == m_fdEpoll || -1 == fdNotifyEpoll)
	{
		perror("epoll_create1");
		exit(1);
	}
}


TcpServer::~TcpServer()
{

}


bool TcpServer::start()
{
	openServer();

	//start dispatcherPool
	startThreadPool(5);

}

bool TcpServer::openServer()
{
	//start accept thread
    if(FAILURE_INDEX == createThread())
    {
    	perror("Create Thread Failed");
        break;
    }
    std::cout<<"Create Thread Success"<<std::endl;

}


int TcpServer::createThread()
{   
	union
	{   
		void* (*aliasCallback)(void *);
		void* (TcpServer::*memberCallback)();
	} SwitchUnion;

	SwitchUnion.memberCallback = &TcpServer::threadCallback;
	if(SUCCESS_INDEX != pthread_create(&(this->m_accepterTID), nullptr, SwitchUnion.aliasCallback, this))
	{   
		perror("Pthread create failed");
		return FAILURE_INDEX;
	}
	return SUCCESS_INDEX;
}


bool TcpServer::threadCallback()
{
	bool bRet = false;
	try
	{
		//TODO
		//ConfigSvr::loadServiceOption(m_cfg);

		//create listener
		m_listenSock = createListener(1);
		//bind socket
		bindSocket();
		//start listen
		startListen();
		//add to epoll
		add2Epoll();
		//process
		processEpoll();
		
		close(sock);    //关闭server的listening socket
		close(epfd);    //关闭epoll file descriptor


	}
	catch(...)
	{
		
	}

	return bRet;
}

int TcpServer::createListener(int isReuse)
{
	if( (m_listenSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 )
	{
		perror("socket failed");
		break;
	}
	
	//in case of 'socket already in use' err msg
	if(setsockopt(m_listenSock, SOL_SOCKET, SO_REUSEADDR, &isReuse, sizeof(int)))
	{
		perror("setsockopt failed");
		break;
	}
	//set non-blocking
	SetSockNonBlock(m_listenSock);
			
}

void TcpServer::bindSocket()
{
	struct sockaddr_in svraddr;
	memset(&svraddr, 0, sizeof(svraddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);//host byte order -> network byte order
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//accept any addr 
	
	//bind
	if(bind(m_listenSock, (struct sockaddr*) &svraddr, sizeof(svraddr)) == -1)
	{
		perror("bind socket failed");
		break;
	}
}

void TcpServer::startListen()
{
	if(listen(m_listenSock, 5) == -1)
	{
		perror("listen socket failed");
		break;
	}
}

void TcpServer::add2Epoll()
{
	//put m_listenSock to epoll
	struct epoll_event event;
	event.evenets = EPOLLIN;
	evenet.data.fd = m_listenSock;
	if(epoll_ctl(m_fdEpoll, EPOLL_CTL_ADD, m_listenSock, &event) == -1)
	{
		perror("epoll_ctl failed");
		break;
	}
	
}

void TcpServer::processEpoll()
{

	//
	int connSock;
	struct sockaddr_in clientaddr;
	socketlen_t clientaddrLen;
	char cClientIP[INET_ADDRSTRLEN];
	int iCount = -1;
	char buffer[BUFF_SIZE];
	int recv_size;

	//init params of epoll_wait 
	struct epoll_event events[EPOLL_MAXEVNENTS];
	memset(events, 0, sizeof(events));
	
	while(1)
	{
		iCount = epoll_wait(m_fdEpoll, events, EPOLL_MAXEVENTS, EPOLL_TIMEOUT);
		if(iCount < 0)
		{
			perror("epoll_wait failed");
			exit(1);
		}
		else if(0 == iCount)//timeout
		{
			print("no socket ready, timeout");
			continue;
		}

		for(int i = 0; i < iCount; ++i)
		{
			//events[i]即为检测到的event，
			//域events[i].events表示具体哪些events，域events[i].data.fd即对应的IO fd
			if ( (events[i].events & EPOLLERR) || 
				 (events[i].events & EPOLLHUP) ||
				 (!(events[i].events & EPOLLIN)) ) 
			{
			//由于events[i].events使用每个bit表示event，因此判断是否包含某个具体事件可以使用`&`操作符
			//这里判断是否存在EPOLLERR, EPOLLHUP等event
				perror("epoll  error");
				close(events[i].data.fd);
				continue;
			}

			if(events[i].data.fd == m_listenSock)
			{
				epollAccept();
			}	
			else if(events[i].events && EPOLLIN)
			{
				epollRead();
			}
			else if(events[i].events && EPOLLOUT)
			{
				epollWrite();
			}
		}

		//
		processCached();
	}
}




bool TcpServer::pushMessage(AgentData *pAgentData)
{
	bool bRet = false;

	pthread_mutex_lock(&mutQueue);
	if(m_AgentDataQueue.size() <= MAX_QUEUE_SIZE)
	{
		m_AgentDataQueue.push_back(pAgentData);
		bRet = true;
	}
	else
	{
		perror("Agent Queue is Full");
	}
	pthread_mutex_unlock(&mutQueue);

	
}


void TcpServer::startThreadPool(int iCount)
{
	//start threads
	for(int i = 0; i < iCount; ++i)
	{
		WorkThread *worker = new WorkThread(this);
		if(worker->open())
		{
			m_workers.push_back(worker);
		}
		else
		{
			perror("worker open failed");
		}
	}
}
