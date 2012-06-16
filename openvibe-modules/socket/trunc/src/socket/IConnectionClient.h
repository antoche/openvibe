#ifndef __Socket_IConnectionClient_H__
#define __Socket_IConnectionClient_H__

#include "IConnection.h"

namespace Socket
{
	class Socket_API IConnectionClient : public Socket::IConnection
	{
	public:

		virtual Socket::boolean connect(
			const char* sServerName,
			Socket::uint32 ui32ServerPort,
			Socket::uint32 ui32TimeOut=0xffffffff)=0;

	};

	extern Socket_API Socket::IConnectionClient* createConnectionClient(void);
};

#endif // __Socket_IConnectionClient_H__
