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

				const CBuffer& operator=(const CBuffer& rBuffer);

				virtual OpenViBE::boolean appendOutputChunkData(
					const OpenViBE::uint8* pBuffer,
					const OpenViBE::uint64 ui64BufferSize);

			protected:

				OpenViBE::uint8* m_pBuffer;
				OpenViBE::uint64 m_ui64BufferSize;
				OpenViBE::uint64 m_ui64AllocatedSize;
			};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CBuffer_H__
