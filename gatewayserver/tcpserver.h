#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <map>


class AgentData
{
public:
	unsigned int uuid;
	int sock;
	int dest;
	char* buff;
	int size;
}

class Agent
{
public:
	Agent();
	~Agent();

	unsigned int uuid;
	int	sock;

	char* buff;
	int size;
}



class TcpServer
{
public:
	TcpServer();
	~TcpServer();

	bool start();
	void stop();
	void* threadCallback();
	int createListener(int isReuse = 0);	
	void bindSocket();
	void epollProcess();
	virtual void epollAccept() = 0;
	virtual void epollRead() = 0;
	virtual void epollWrite() = 0;
	void TcpServer::startThreadPool(int iCount=1);
	bool deserializeAgentData(Agent* pAgent, char* buff, int size);

	int m_fdEpoll;
	int m_fdNotifyEpoll;
	std::map<std::string, Agent> m_AgentMap;
	std::queue<AgentData*> m_AgentDataQueue;
	int m_accepterTID;
	int m_listenSock;
	std::vector<WorkThread> m_workers;
private:
};










#endif

