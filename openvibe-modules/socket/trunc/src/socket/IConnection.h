#ifndef __Socket_IConnection_H__
#define __Socket_IConnection_H__

#include "defines.h"

namespace Socket
{
	class Socket_API IConnection
	{
	protected:

		virtual Socket::boolean open(void)=0;

	public:

		virtual Socket::boolean close(void)=0;

		virtual Socket::boolean isReadyToSend(
			Socket::uint32 ui32TimeOut=0) const=0;
		virtual Socket::boolean isReadyToReceive(
			Socket::uint32 ui32TimeOut=0) const=0;

		virtual Socket::uint32 sendBuffer(
			const void* pBuffer,
			const Socket::uint32 ui32BufferSize)=0;
		virtual Socket::uint32 receiveBuffer(
			void* pBuffer,
			const Socket::uint32 ui32BufferSize)=0;

		virtual Socket::boolean sendBufferBlocking(
			const void* pBuffer,
			const Socket::uint32 ui32BufferSize)=0;
		virtual Socket::boolean receiveBufferBlocking(
			void* pBuffer,
			const Socket::uint32 ui32BufferSize)=0;

		virtual Socket::boolean isConnected(void) const=0;

		virtual void release(void)=0;

	protected:

		virtual ~IConnection(void);
	};
};

#endif // __Socket_IConnection_H__
