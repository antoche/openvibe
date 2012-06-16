#ifndef __OpenViBEKernel_Kernel_Player_IPlayerManager_H__
#define __OpenViBEKernel_Kernel_Player_IPlayerManager_H__

#include "../ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CPlayerManager : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayerManager>
		{
		public:

			CPlayerManager(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual OpenViBE::boolean createPlayer(
				OpenViBE::CIdentifier& rPlayerIdentifier);
			virtual OpenViBE::boolean releasePlayer(
				const OpenViBE::CIdentifier& rPlayerIdentifier);
			virtual OpenViBE::Kernel::IPlayer& getPlayer(
				const OpenViBE::CIdentifier& rPlayerIdentifier);
			virtual OpenViBE::CIdentifier getNextPlayerIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayerManager>, OVK_ClassId_Kernel_Player_PlayerManager);

		protected:

			virtual OpenViBE::CIdentifier getUnusedIdentifier(void) const;

		protected:

			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::IPlayer*> m_vPlayer;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_IPlayerManager_H__
