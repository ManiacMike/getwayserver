#ifndef _AGENT_SERVER_H_
#define _AGENT_SERVER_H_

class Agent
{
public:
	Agent();
	~Agent();

	unsigned int uuid;
}





class AgentServer
{
public:
	AgentServer();
	~AgentServer();

	bool start();
	std::vector<Agent> agentList;
	int m_accepterTID;
	int m_listenSock;

private:
};










#endif

