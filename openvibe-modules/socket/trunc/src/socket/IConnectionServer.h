#ifndef __Socket_IConnectionServer_H__
#define __Socket_IConnectionServer_H__

#include "IConnection.h"

namespace Socket
{
	class Socket_API IConnectionServer : public Socket::IConnection
	{
	public:

		virtual Socket::boolean listen(
			Socket::uint32 ui32Port)=0;

		virtual Socket::IConnection* accept(void)=0;
	};

	extern Socket_API Socket::IConnectionServer* createConnectionServer(void);
};

#endif // __Socket_IConnectionServer_H__
