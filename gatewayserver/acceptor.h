#ifndef _AGENT_SERVER_H_
#define _AGENT_SERVER_H_

#include <map>



class Acceptor
{
public:
	Acceptor();
	~Acceptor();

	void epollAccept();
	void epollRead();
	void epollWrite();

	bool deserializeAgentData(Agent* pAgent, char* buff, int size);
	std::map<std::string, Agent> m_AgentMap;
	std::queue<AgentData*> m_AgentDataQueue;
	int m_accepterTID;
	int m_listenSock;

private:
};










#endif

