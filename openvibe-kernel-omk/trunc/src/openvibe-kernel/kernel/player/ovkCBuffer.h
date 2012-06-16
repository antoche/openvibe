#ifndef __OpenViBEKernel_Kernel_Player_CBuffer_H__
#define __OpenViBEKernel_Kernel_Player_CBuffer_H__

#include "../../ovk_base.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CBuffer : public OpenViBE::CMemoryBuffer
		{
		public:

			CBuffer(void) { }
			CBuffer(const OpenViBE::Kernel::CBuffer& rBuffer);

			const CBuffer& operator=(const CBuffer& rBuffer);
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CBuffer_H__
