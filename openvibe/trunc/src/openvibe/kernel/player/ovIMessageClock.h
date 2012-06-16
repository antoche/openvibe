#ifndef __OpenViBE_Kernel_Player_IMessageClock_H__
#define __OpenViBE_Kernel_Player_IMessageClock_H__

#include "ovIMessage.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IMessageClock
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-05-30
		 * \brief Clock message
		 * \ingroup Group_Player
		 * \ingroup Group_Kernel
		 */
		class OV_API IMessageClock : public OpenViBE::Kernel::IMessage
		{
		public:

			_IsDerivedFromClass_(OpenViBE::Kernel::IMessage, OV_ClassId_Kernel_Player_MessageClock)

		};
	};
};

#endif // __OpenViBE_Kernel_Player_IMessageClock_H__
