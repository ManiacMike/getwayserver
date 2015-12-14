#ifndef _DIAPATCHER_H_20151214
#define _DISPATCHER_H_20151214

class Dispatcher
{
public:
	Dispatcher();
	~Dispatcher();

	bool start();
	void stop();
	int createThread();
	void* threadCallback();

	std::vector<WorkThread*> m_vecWorkers;
	
}


#endif
