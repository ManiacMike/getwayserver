#ifndef _AGENT_SERVER_H_
#define _AGENT_SERVER_H_

#include <map>

class AgentData
{
public:
	unsigned int uuid;
	int sock;
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




class AgentServer
{
public:
	AgentServer();
	~AgentServer();

	bool start();
	void stop();
	void* threadCallback();
	bool deserializeAgentData(Agent* pAgent, char* buff, int size);
	std::map<std::string, Agent> m_AgentMap;
	std::queue<AgentData*> m_AgentDataQueue;
	int m_accepterTID;
	int m_listenSock;

private:
};










#endif

