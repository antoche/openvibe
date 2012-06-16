#include "socket/IConnectionClient.h"

#include <iostream>

int main(int argc, char** argv)
{
	Socket::IConnectionClient* l_pConnectionClient=Socket::createConnectionClient();

	std::cout<<"CLIENT: Ready to connect to "<<argv[1]<<":"<<atoi(argv[2])<<std::endl;

	if(!l_pConnectionClient->connect(argv[1], atoi(argv[2])))
	{
		std::cout<<"CLIENT: Could not connect !"<<std::endl;
		return 0;
	}

	std::cout<<"CLIENT: Connected !"<<std::endl;

	char* l_sBuffer="some data";
	while(l_pConnectionClient->isConnected())
	{
		if(l_pConnectionClient->isReadyToSend())
		{
			std::cout<<"CLIENT: Ready to send buffer ["<<l_sBuffer<<"]..."<<std::endl;
			l_pConnectionClient->sendBuffer(l_sBuffer, strlen(l_sBuffer)+1);
			std::cout<<"CLIENT: Buffer sent !"<<std::endl;
		}
	}

	std::cout<<"CLIENT: disconnected !"<<std::endl;

	l_pConnectionClient->release();
	l_pConnectionClient=NULL;
	return 0;
}
