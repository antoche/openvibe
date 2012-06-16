#include "ovkCBuffer.h"

#include <system/Memory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

CBuffer::CBuffer(const CBuffer& rBuffer)
{
	this->setSize(rBuffer.getSize(), true);
	System::Memory::copy(this->getDirectPointer(), rBuffer.getDirectPointer(), rBuffer.getSize());
}

const CBuffer& CBuffer::operator=(
	const CBuffer& rBuffer)
{
	this->setSize(rBuffer.getSize(), true);
	System::Memory::copy(this->getDirectPointer(), rBuffer.getDirectPointer(), rBuffer.getSize());
	return *this;
}
