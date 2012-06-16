#include "socket/IConnectionServer.h"

#include <iostream>
#include <list>

int main(int argc, char** argv)
{
	std::list <Socket::IConnection*> l_vConnection;
	Socket::IConnectionServer* l_pConnectionServer=Socket::createConnectionServer();

	std::cout<<"SERVER: Ready to listen to port "<<atoi(argv[1])<<std::endl;

	if(!l_pConnectionServer->listen(atoi(argv[1])))
	{
		std::cout<<"SERVER: Could not listen !"<<std::endl;
		return 0;
	}

	std::cout<<"SERVER: Listen ok !"<<std::endl;

	char l_sBuffer[20];
	while(true)
	{
		if(l_pConnectionServer->isReadyToReceive())
		{
			std::cout<<"SERVER: Ready to receive !"<<std::endl;
			l_vConnection.push_back(l_pConnectionServer->accept());
			std::cout<<"SERVER: Accepted new client ! Number of clients : "<<l_vConnection.size()<<std::endl;
		}

		std::list<Socket::IConnection*>::iterator i;
		for(i=l_vConnection.begin(); i!=l_vConnection.end(); i++)
		{
			if(*i)
			{
				if((*i)->isReadyToReceive())
				{
					std::cout<<"SERVER CONNECTION: Ready to receive buffer..."<<std::endl;
					Socket::uint32 r=(*i)->receiveBuffer(l_sBuffer, sizeof(l_sBuffer));
					std::cout<<"SERVER CONNECTION: Received buffer ["<<l_sBuffer<<"] ("<<r<<" bytes)"<<std::endl;
				}
			}
		}
	}
	return 0;
}
