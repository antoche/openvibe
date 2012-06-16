#include "IConnection.h"

#include <iostream>

#if defined Socket_OS_Linux || defined Socket_OS_MacOS
 #include <sys/select.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 // #include <netinet/in.h>
 // #include <netinet/tcp.h>
 #include <arpa/inet.h>
 #include <unistd.h>
 // #include <netdb.h>
 #include <ctime>

 #if defined Socket_OS_MacOS
  #define Socket_SendFlags    0
  #define Socket_ReceiveFlags 0
 #else
  #define Socket_SendFlags    MSG_NOSIGNAL
  #define Socket_ReceiveFlags MSG_NOSIGNAL
 #endif
#elif defined Socket_OS_Windows
 #include <winsock2.h>
 #include <windows.h>

 #define Socket_SendFlags    0
 #define Socket_ReceiveFlags 0
#else

#endif

namespace Socket
{
	static boolean FD_ISSET_PROXY(int fd, fd_set* set)
	{
		return FD_ISSET(fd, set)?true:false;
	}

	template <class T> class TConnection : public T
	{
	public:

		TConnection(void)
			:m_i32Socket(-1)
		{
#if defined Socket_OS_Linux
#elif defined Socket_OS_Windows
			int32 l_i32VersionHigh=2;
			int32 l_i32VersionLow=0;
			WORD l_oWinsockVersion=MAKEWORD(l_i32VersionHigh, l_i32VersionLow);
			WSADATA l_oWSAData;
			::WSAStartup(l_oWinsockVersion, &l_oWSAData);
#else
#endif
		}

		TConnection(int32 i32Socket)
			:m_i32Socket(i32Socket)
		{
#if defined Socket_OS_Linux
#elif defined Socket_OS_Windows
			int32 l_i32VersionHigh=2;
			int32 l_i32VersionLow=0;
			WORD l_oWinsockVersion=MAKEWORD(l_i32VersionHigh, l_i32VersionLow);
			WSADATA l_oWSAData;
			::WSAStartup(l_oWinsockVersion, &l_oWSAData);
#else
#endif
		}

		virtual ~TConnection(void)
		{
#if defined Socket_OS_Linux
#elif defined Socket_OS_Windows
			::WSACleanup();
#else
#endif
		}

	protected:

		virtual boolean open(void)
		{
			if(isConnected())
			{
				return false;
			}

			m_i32Socket=::socket(AF_INET, SOCK_STREAM, 0);
			if(m_i32Socket==-1)
			{
				return false;
			}

			return true;
		}

	public:

		virtual boolean close(void)
		{
			if(!isConnected())
			{
				return false;
			}

#if defined Socket_OS_Linux
			::shutdown(m_i32Socket, SHUT_RDWR);
			::close(m_i32Socket);
#elif defined Socket_OS_Windows
			::shutdown(m_i32Socket, SD_BOTH);
			::closesocket(m_i32Socket);
#else
#endif

			m_i32Socket=-1;
			return true;
		}

		virtual boolean isReadyToSend(
			uint32 ui32TimeOut=0) const
		{
			if(!isConnected())
			{
				return false;
			}

			struct timeval l_oTimeVal;
			l_oTimeVal.tv_sec=(ui32TimeOut/1000);
			l_oTimeVal.tv_usec=((ui32TimeOut-l_oTimeVal.tv_sec*1000)*1000);

			fd_set l_oWriteFileDescriptors;
			FD_ZERO(&l_oWriteFileDescriptors);
			FD_SET(m_i32Socket, &l_oWriteFileDescriptors);

			if(::select(m_i32Socket+1, NULL, &l_oWriteFileDescriptors, NULL, &l_oTimeVal)<0)
			{
				return false;
			}
			if(!FD_ISSET_PROXY(m_i32Socket, &l_oWriteFileDescriptors))
			{
				return false;
			}
			return true;
		}

		virtual boolean isReadyToReceive(
			uint32 ui32TimeOut=0) const
		{
			if(!isConnected())
			{
				return false;
			}

			struct timeval l_oTimeVal;
			l_oTimeVal.tv_sec=(ui32TimeOut/1000);
			l_oTimeVal.tv_usec=((ui32TimeOut-l_oTimeVal.tv_sec*1000)*1000);

			fd_set l_oReadFileDescriptors;
			FD_ZERO(&l_oReadFileDescriptors);
			FD_SET(m_i32Socket, &l_oReadFileDescriptors);

			if(::select(m_i32Socket+1, &l_oReadFileDescriptors, NULL, NULL, &l_oTimeVal)<0)
			{
				return false;
			}
			if(!(FD_ISSET_PROXY(m_i32Socket, &l_oReadFileDescriptors)))
			{
				return false;
			}
			return true;
		}

		virtual uint32 sendBuffer(
			const void* pBuffer,
			const uint32 ui32BufferSize)
		{
			if(!isConnected())
			{
				return 0;
			}
#if 0
			int l_iTrue=1;
			setsockopt(m_i32Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&l_iTrue, sizeof(l_iTrue));
#endif
			int l_iResult=::send(m_i32Socket, static_cast<const char*>(pBuffer), ui32BufferSize, Socket_SendFlags);
			if(ui32BufferSize!=0 && l_iResult<=0)
			{
				close();
			}
			return l_iResult<=0?0:(uint32)l_iResult;
		}

		virtual uint32 receiveBuffer(
			void* pBuffer,
			const uint32 ui32BufferSize)
		{
			if(!isConnected() || !ui32BufferSize)
			{
				return 0;
			}
#if 0
			int l_iTrue=1;
			setsockopt(m_i32Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&l_iTrue, sizeof(l_iTrue));
#endif
			int l_iResult=::recv(m_i32Socket, static_cast<char *>(pBuffer), ui32BufferSize, Socket_ReceiveFlags);
			if(ui32BufferSize!=0 && l_iResult<=0)
			{
				close();
			}
			return l_iResult<=0?0:(uint32)l_iResult;
		}

		virtual boolean sendBufferBlocking(
			const void* pBuffer,
			const uint32 ui32BufferSize)
		{
			uint32 l_ui32LeftBytes=ui32BufferSize;
			const char* l_pBuffer=static_cast<const char*>(pBuffer);
			do
			{
				l_ui32LeftBytes-=sendBuffer(l_pBuffer+ui32BufferSize-l_ui32LeftBytes, l_ui32LeftBytes);
				if(!isConnected())
				{
					return false;
				}
			}
			while(l_ui32LeftBytes!=0);
			return true;
		}

		virtual boolean receiveBufferBlocking(
			void* pBuffer,
			const uint32 ui32BufferSize)
		{
			uint32 l_ui32LeftBytes=ui32BufferSize;
			char* l_pBuffer=static_cast<char*>(pBuffer);
			do
			{
				l_ui32LeftBytes-=receiveBuffer(l_pBuffer+ui32BufferSize-l_ui32LeftBytes, l_ui32LeftBytes);
				if(!isConnected())
				{
					return false;
				}
			}
			while(l_ui32LeftBytes!=0);
			return true;
		}

		virtual boolean isConnected(void) const
		{
			return m_i32Socket!=-1;
		}

		virtual void release(void)
		{
			if(isConnected())
			{
				close();
			}
			delete this;
		}

	protected:

		int32 m_i32Socket;
	};
};
