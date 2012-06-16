#ifndef __OpenViBEKernel_Kernel_Player_CMessageEvent_H__
#define __OpenViBEKernel_Kernel_Player_CMessageEvent_H__

#include "../ovkTKernelObject.h"
#include "ovkTMessage.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CMessageEvent : public OpenViBE::Kernel::TMessage<OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMessageEvent> >
		{
		public:

			CMessageEvent(const OpenViBE::Kernel::IKernelContext& rKernelContext)
				:OpenViBE::Kernel::TMessage<OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMessageEvent> >(rKernelContext)
			{
			}

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TMessage<OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMessageEvent> >, OVK_ClassId_Kernel_Player_MessageEvent);
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CMessageEvent_H__
