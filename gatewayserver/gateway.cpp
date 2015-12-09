/*
* Auther:			xiangzhou
* Initial Date: 	2015/12/08
* Modify History:
*
*
*/
#include "define.h"
#include "core.h"
#include "utils.cpp"
#include <unistd.h>

int main()
{
	Core core;
	core.start();

	char cCmd[MAXLINE];
	while(fgets(cCmd, MAXLINE, stdin) != NULL)
	{
		if(0 == strcmp(cCmd, STOP_GATEWAY.c_str()))
		{
			print("Gateway Server stopping...");
			core.stop();
			break;
		}
		
		if(0 == strcmp(cCmd, RESTART_GATEWAY.c_str()))
		{
			print("Gateway Server Stopping...");
			core.stop();
			print("Gateway Server Starting...");
			core.start();
		}

		sleep(10000);
		std::cout<<"Gateway Server Running..."<<std::endl;
	}
	print("Quit Gateway");
	return 1;
}
