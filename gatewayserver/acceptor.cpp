#include "Acceptor.h"

Acceptor::Acceptor()
{
	
}


Acceptor::~Acceptor()
{

}

void Acceptor::epollAccept()
{
	int connSock;
	struct sockaddr_in clientaddr;
	socketlen_t clientaddrLen;
	char cClientIP[INET_ADDRSTRLEN];
	int iCount = -1;
	char buffer[BUFF_SIZE];
	int recv_size;
	
	//当前fd是server的socket，不进行读而是accept所有client连接请求
	while(1)//Notice, here is while
	{
		clientaddr_len = sizeof(clientaddr);
		connSock = accept(m_listenSock, (struct sockaddr*) &clientaddr, clientaddr_len);
		if(-1 == connSock)
		{
			if ( (errno == EAGAIN) || (errno == EWOULDBLOCK) ) 
			{
				//non-blocking模式下无新connection请求，跳出while (1)
				break;
			}
			else 
			{
				perror("accept socket ailed");
				exit(1);
			}
		} 
		
		if (!inet_ntop(AF_INET, &(client_addr.sin_addr), cClientIP, sizeof(cClientIP))) 
		{
			perror("inet_ntop failed");
			exit(1);
		}

		Agent *pAgent = nullptr;
		std::string sClientIP(cClinetIP);
		std::cout<<"accept a outter client from IP "<<sClientIP<<", fd "<<connSock<<std::endl;
		std::map<int, Agent>::iterator iter = m_AgentMap.find(connSock);
		if(iter == m_AgentMap.end())
		{
			pAgent = new Agent();
			pAgent->sClientIP = sClientIP;
			pAgent->sock = connSock;
			m_AgentMap.insert(make_pair(connSock, pAgent));
			std::cout<<"New Client inserted, "<<sClientIP<<std::endl;
		}
		else
		{
			//TODO,test this
			perror("Client already exists, " + sClientIP);
			continue;
		}
		
		setSockNonBlock(pAgent->sock);
		//把conn_sock添加到epoll的侦听中
		event.events = EPOLLIN | EPOLLET;
		event.data.fd = pAgent->sock;
		if ( epoll_ctl(m_fdEpoll, EPOLL_CTL_ADD, pAgent->sock, &event) == -1 ) 
		{																		         
			perror("epoll_ctl(EPOLL_CTL_ADD) failed");
			exit(1);
		}
	}
}

void Acceptor::epollRead()
{
	//当前fd是client连接的socket，可以读(read from client)
	connSock = events[i].data.fd;
	memset(buffer, 0, sizeof(buffer));
	if((recv_size = recv(connSock, buffer, sizeof(buffer), 0)) == -1 
		&& (errno != EAGAIN))
	{
		//recv在non-blocking模式下，返回-1且errno为EAGAIN表示当前无可读数据，并不表示错误
		perror("recv socket failed");
		exit(1);
	}
	printf("recved from conn_sock=%d : %s(%d length string)\n", connSock, buffer, recv_size);

	//deal with recvd data
	std::map<int, Agent>::iterator iter = m_AgentMap.find(connSock); 
	if(m_AgentMap.end() == iter)
	{
		perror("fd " + Int2Str(connSock) + " not found in agentmap");
		continue;
	}
	//可以是将Agent序列化为AgentData后存储，但也可以直接存储Agent
	AgentData *agentData = deserializeAgentData(*iter, buffer, recv_size);
	//swapPointer();
	dispatcher.pushMessage();
	dispatcher.notify();
	/*
	//立即将收到的内容写回去
	if ( send(conn_sock, buffer, recv_size, 0) == -1 && (errno != EAGAIN) && (errno != EWOULDBLOCK) ) 
	{
		//send在non-blocking模式下，返回-1且errno为EAGAIN或EWOULDBLOCK表示当前无可写数据，并不表示错误
		perror("send failed");
		exit(1);
	}
	printf("send to conn_sock=%d done\n", conn_sock);
	*/

}


AgentData* Acceptor::deserializeAgentData(Agent *pAgent, char* buff, int size)
{
	pAgent->data = buff;
	pAgent->size = size;
	
	AgentData *pAgentData = new AgentData();
	pAgentData->uuid = pAgent->uuid;
	pAgentData->sClientIP = pAgent->sClientIP;
	pAgentData->buff = pAgent->buff;
	pAgentData->size = pAgent->size;

	return pAgentData;
}

void Acceptor::swapPointer(Agent *pAgentOrig, Agent *pAgentNew)
{
	pAgentNew = pAgentOrig;
	pAgentOrig = new Agent();
}


bool Acceptor::pushMessage(AgentData *pAgentData)
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
