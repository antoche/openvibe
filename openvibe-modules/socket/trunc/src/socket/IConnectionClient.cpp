#include "IConnection.h"
#include "IConnectionClient.h"
#include "IConnection.inl"

#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <cerrno>

#if defined Socket_OS_Linux
 #include <netinet/in.h>
 #include <netinet/tcp.h>
 #include <netdb.h>
 #include <unistd.h>
#elif defined Socket_OS_Windows
 #include <fcntl.h>
 #include <cerrno>
#else
#endif

namespace Socket
{
	class CConnectionClient : public TConnection<IConnectionClient>
	{
	public:

		virtual boolean connect(
			const char* sServerName,
			uint32 ui32ServerPort,
			uint32 ui32TimeOut)
		{
#if defined Socket_OS_Linux

			long l_iValue;

#elif defined Socket_OS_Windows

			unsigned long l_uiMode;

#endif

			if(!open())
			{
				return false;
			}

			// Looks up host name
			struct hostent* l_pServerHostEntry=gethostbyname(sServerName);
			if(!l_pServerHostEntry)
			{
				close();
				return false;
			}

#if defined Socket_OS_Linux

			// Sets non blocking
			if((l_iValue=::fcntl(m_i32Socket, F_GETFL, NULL))<0)
			{
				close();
				return false;
			}
			l_iValue|=O_NONBLOCK;
			if(::fcntl(m_i32Socket, F_SETFL, l_iValue)<0)
			{
				close();
				return false;
			}

#elif defined Socket_OS_Windows

			// Sets non blocking
			l_uiMode=1;
			::ioctlsocket(m_i32Socket, FIONBIO, &l_uiMode);

#endif

			// Connects
			struct sockaddr_in l_oServerAddress;
			memset(&l_oServerAddress, 0, sizeof(l_oServerAddress));
			l_oServerAddress.sin_family=AF_INET;
			l_oServerAddress.sin_port=htons((unsigned short)ui32ServerPort);
			l_oServerAddress.sin_addr=*((struct in_addr*)l_pServerHostEntry->h_addr);
			if(::connect(m_i32Socket, (struct sockaddr*)&l_oServerAddress, sizeof(struct sockaddr_in))<0)
			{
				boolean l_bInProgress=false;

#if defined Socket_OS_Linux

				l_bInProgress=(errno==EINPROGRESS);

#elif defined Socket_OS_Windows

				l_bInProgress=(WSAGetLastError()==WSAEINPROGRESS || WSAGetLastError()==WSAEWOULDBLOCK);

#endif

				if(l_bInProgress)
				{
					// Performs time out
					if(ui32TimeOut==0xffffffff)
					{
						ui32TimeOut=125;
					}

					struct timeval l_oTimeVal;
					l_oTimeVal.tv_sec=(ui32TimeOut/1000);
					l_oTimeVal.tv_usec=((ui32TimeOut-l_oTimeVal.tv_sec*1000)*1000);

					fd_set l_oWriteFileDescriptors;
					FD_ZERO(&l_oWriteFileDescriptors);
					FD_SET(m_i32Socket, &l_oWriteFileDescriptors);

					if(::select(m_i32Socket+1, NULL, &l_oWriteFileDescriptors, NULL, &l_oTimeVal)<0)
					{
						close();
						return false;
					}
					if(!FD_ISSET_PROXY(m_i32Socket, &l_oWriteFileDescriptors))
					{
						close();
						return false;
					}

					// Checks error status
					int l_iOption=0;
#if defined Socket_OS_Linux
					socklen_t l_iOptionLength=sizeof(l_iOption);
					::getsockopt(m_i32Socket, SOL_SOCKET, SO_ERROR, (void*)(&l_iOption), &l_iOptionLength);
#elif defined Socket_OS_Windows
					int l_iOptionLength=sizeof(l_iOption);
					::getsockopt(m_i32Socket, SOL_SOCKET, SO_ERROR, (char*)(&l_iOption), &l_iOptionLength);
#endif
					if(l_iOption!=0)
					{
						close();
						return false;
					}
				}
				else
				{
					close();
					return false;
				}
			}

#if defined Socket_OS_Linux

			// Sets back to blocking
			if((l_iValue=::fcntl(m_i32Socket, F_GETFL, NULL))<0)
			{
				close();
				return false;
			}
			l_iValue&=~O_NONBLOCK;
			if(::fcntl(m_i32Socket, F_SETFL, l_iValue)<0)
			{
				close();
				return false;
			}

#elif defined Socket_OS_Windows

			// Sets back to blocking
			l_uiMode=0;
			::ioctlsocket(m_i32Socket, FIONBIO, &l_uiMode);

#endif

			return true;
		}
	};

	IConnectionClient* createConnectionClient(void)
	{
		return new CConnectionClient();
	}
};
