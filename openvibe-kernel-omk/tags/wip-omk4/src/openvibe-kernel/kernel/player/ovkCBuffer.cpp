#include "ovkCBuffer.h"

#include <system/Memory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

const CBuffer& CBuffer::operator=(
	const CBuffer& rBuffer)
{
	this->setSize(rBuffer.getSize(), true);
	System::Memory::copy(this->getDirectPointer(), rBuffer.getDirectPointer(), rBuffer.getSize());
	return *this;
}

boolean CBuffer::appendOutputChunkData(
	const uint8* pBuffer,
	const uint64 ui64BufferSize)
{
	uint64 l_ui64LastBufferSize=this->getSize();
	if(!this->setSize(l_ui64LastBufferSize+ui64BufferSize, false))
	{
		return false;
	}
	System::Memory::copy(this->getDirectPointer()+l_ui64LastBufferSize, pBuffer, ui64BufferSize);
	return true;
}
