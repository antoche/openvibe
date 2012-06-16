#include "IConnection.h"
#include "IConnectionServer.h"
#include "IConnection.inl"

#include <cstring>
#include <iostream>

#if defined Socket_OS_Linux
 #include <netinet/in.h>
 #include <netinet/tcp.h>
 #include <netdb.h>
 #include <cerrno>
#elif defined Socket_OS_Windows
#else
#endif

namespace Socket
{
	class CConnectionServer : public TConnection<IConnectionServer>
	{
	public:

		virtual boolean listen(
			uint32 ui32Port)
		{
			if(!open())
			{
				return false;
			}

			int l_iReuseAddress=1;
#if defined Socket_OS_Windows
			::setsockopt(m_i32Socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&l_iReuseAddress), sizeof(l_iReuseAddress));
#else
			::setsockopt(m_i32Socket, SOL_SOCKET, SO_REUSEADDR, &l_iReuseAddress, sizeof(l_iReuseAddress));
#endif

			struct sockaddr_in l_oLocalHostAddress;
			memset(&l_oLocalHostAddress, 0, sizeof(l_oLocalHostAddress));
			l_oLocalHostAddress.sin_family=AF_INET;
			l_oLocalHostAddress.sin_port=htons((unsigned short)ui32Port);
			l_oLocalHostAddress.sin_addr.s_addr=htonl(INADDR_ANY);

			if(::bind(m_i32Socket, (struct sockaddr*)&l_oLocalHostAddress, sizeof(l_oLocalHostAddress))==-1)
			{
/*
				switch(errno)
				{
					case EBADF: std::cout << "EBADF" << std::endl; break;
					case ENOTSOCK: std::cout << "ENOTSOCK" << std::endl; break;
					case EADDRINUSE: std::cout << "EADDRINUSE" << std::endl; break;
					case EINVAL: std::cout << "EINVAL" << std::endl; break;
					case EROFS: std::cout << "EROFS" << std::endl; break;
					case EFAULT: std::cout << "EFAULT" << std::endl; break;
					case ENAMETOOLONG: std::cout << "ENAMETOOLONG" << std::endl; break;
					case ENOENT: std::cout << "ENOENT" << std::endl; break;
					case ENOMEM: std::cout << "ENOMEM" << std::endl; break;
					case ENOTDIR: std::cout << "ENOTDIR" << std::endl; break;
					case EACCES: std::cout << "EACCES" << std::endl; break;
					case ELOOP: std::cout << "ELOOP" << std::endl; break;
					default: std::cout << "Bind_unknown" << std::endl;
				}
*/
				return false;
			}

			if(::listen(m_i32Socket, 32)==-1)
			{
/*
				switch(errno)
				{
					case EADDRINUSE: std::cout << "EADDRINUSE" << std::endl; break;
					case EBADF: std::cout << "EBADF" << std::endl; break;
					case ENOTSOCK: std::cout << "ENOTSOCK" << std::endl; break;
					case EOPNOTSUPP: std::cout << "EOPNOTSUPP" << std::endl; break;
					default: std::cout << "Listen_unknown" << std::endl;
				}
*/
				return false;
			}

			return true;
		}

		virtual IConnection* accept(void)
		{
			struct sockaddr_in l_oClientAddress;
#if defined Socket_OS_Linux
			socklen_t l_iClientAddressSize=sizeof(l_oClientAddress);
#elif defined Socket_OS_Windows
			int l_iClientAddressSize=sizeof(l_oClientAddress);
#else
#endif
			int32 l_i32ClientSocket=::accept(m_i32Socket, (struct sockaddr*)&l_oClientAddress, &l_iClientAddressSize);
			if(l_i32ClientSocket==-1)
			{
				return NULL;
			}
			return new TConnection<IConnection>(l_i32ClientSocket);
		}
	};

	IConnectionServer* createConnectionServer(void)
	{
		return new CConnectionServer();
	}
};
