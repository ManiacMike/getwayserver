#include "core.h"

Core::Core()
{
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

		//
		if( (fd_listenSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 )
		{
			perror("socket failed");
			break;
		}
		
		//in case of 'socket already in use' err msg
		int yes = 1;
		if(setsockopt(fd_listenSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)))
		{
			perror("setsockopt failed");
			break;
		}
		//set non-blocking
		SetSockNonBlock(fd_listenSock);
		
		//build socket addr to be bind
		struct sockaddr_in svraddr;
		memset(&svraddr, 0, sizeof(svraddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(5188);//host byte order -> network byte order
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//accept any addr 
		
		//bind
		if(bind(fd_listenSock, (struct sockaddr*) &svraddr, sizeof(svraddr)) == -1)
		{
			perror("bind socket failed");
			break;
		}
		
		//listen
		if(listen(fd_listenSock, 5) == -1)
		{
			perror("listen socket failed");
			break;
		}
		
		//put fd_listenSock to epoll
		struct epoll_event event;
		event.evenets = EPOLLIN;
		evenet.data.fd = fd_listenSock;
		if(epoll_ctl(m_fdEpoll, EPOLL_CTL_ADD, fd_listenSock, &event) == -1)
		{
			perror("epoll_ctl failed");
			break;
		}
		
		//init params of epoll_wait 
		struct epoll_event events[EPOLL_MAXEVNENTS];
		memset(events, 0, sizeof(events));

		//
		int fd_connSock;
		struct sockaddr_in clientaddr;
		socketlen_t clientaddr_len;
		char client_ip_str[INET_ADDRSTRLEN];
		int iCount = -1;
		char buffer[BUFF_SIZE];
		int recv_size;

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
				else
				{
					if(events[i].data.fd == fd_listenSock)
					{
						//当前fd是server的socket，不进行读而是accept所有client连接请求
						while(1)
						{
							clientaddr_len = sizeof(clientaddr);
							fd_connSock = accept(fd_listenSock, (struct sockaddr*) &clientaddr, clientaddr_len);
							if(-1 == fd_connSock)
							{
								if ( (errno == EAGAIN) || (errno == EWOULDBLOCK) ) 
								{
									//non-blocking模式下无新connection请求，跳出while (1)
									break;
								}
							} 
							else 
							{
								perror("accept socket ailed");
								exit(1);
							}
							if (!inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip_str, sizeof(client_ip_str))) 
							{
								perror("inet_ntop failed");
								exit(1);
							}

							std::cout<<"accept a client from "<<client_ip_str<<std::endl;
							setSockNonBlock(fd_connSock);
							//把conn_sock添加到epoll的侦听中
							event.events = EPOLLIN;
							event.data.fd = conn_sock;
							if ( epoll_ctl(epfd, EPOLL_CTL_ADD, conn_sock, &event) == -1 ) 
							{																		         
								perror("epoll_ctl(EPOLL_CTL_ADD) failed");
								exit(1);
							}
						}
					}	
					else
					{
						//当前fd是client连接的socket，可以读(read from client)
						fd_connSock = events[i].data.fd;
						memset(buffer, 0, sizeof(buffer));
						if((recv_size = recv(fd_connSock, buffer, sizeof(buffer), 0)) == -1 
							&& (errno != EAGAIN))
						{
							//recv在non-blocking模式下，返回-1且errno为EAGAIN表示当前无可读数据，并不表示错误
							perror("recv socket failed");
							exit(1);
						}
						printf("recved from conn_sock=%d : %s(%d length string)\n", conn_sock, buffer, recv_size);

						//TODO
						//立即将收到的内容写回去
						if ( send(conn_sock, buffer, recv_size, 0) == -1 && (errno != EAGAIN) && (errno != EWOULDBLOCK) ) 
						{
							//send在non-blocking模式下，返回-1且errno为EAGAIN或EWOULDBLOCK表示当前无可写数据，并不表示错误
							perror("send failed");
							exit(1);
						}
						printf("send to conn_sock=%d done\n", conn_sock);

						if(close(fd_connSock) == -1)
						{
							perror("clost socket failed");
							exit(1);
						}
						printf("close conn_sock=%d done\n", conn_sock);
					}
				}
			}
		}
		close(sock);    //关闭server的listening socket
		close(epfd);    //关闭epoll file descriptor



		//start threads
		int iThreadCount = std::stoi(m_cfg[THREADPOOL_SIZE]);
		for(int i = 0; i < iThreadCount; ++i)
		{
			WorkThread *worker = new WorkThread(this);
			if(worker->open())
			{
				m_vecWorkers.push_back(worker);
			}
		}

			//start AgentServer
			m_agentServer = new AgentServer();
			m_agentServer->open();
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







