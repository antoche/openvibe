#ifndef __OpenViBEKernel_Kernel_Player_CMessageClock_H__
#define __OpenViBEKernel_Kernel_Player_CMessageClock_H__

#include "../ovkTKernelObject.h"
#include "ovkTMessage.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CMessageClock : public OpenViBE::Kernel::TMessage<OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMessageClock> >
		{
		public:

			CMessageClock(const OpenViBE::Kernel::IKernelContext& rKernelContext)
				:OpenViBE::Kernel::TMessage<OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMessageClock> >(rKernelContext)
			{
			}

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TMessage<OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMessageClock> >, OVK_ClassId_Kernel_Player_MessageClock);
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CMessageClock_H__
