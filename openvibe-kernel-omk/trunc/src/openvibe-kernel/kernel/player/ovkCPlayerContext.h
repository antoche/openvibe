#ifndef __OpenViBEKernel_Kernel_Player_CPlayerContext_H__
#define __OpenViBEKernel_Kernel_Player_CPlayerContext_H__

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CSimulatedBox;

		class CPlayerContext : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayerContext>
		{
		public:

			CPlayerContext(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::CSimulatedBox* pSimulatedBox);
			virtual ~CPlayerContext();

			virtual OpenViBE::boolean sendSignal(
				const OpenViBE::CMessageSignal& rMessageSignal);
			virtual OpenViBE::boolean sendMessage(
				const OpenViBE::CMessageEvent& rMessageEvent,
				const OpenViBE::CIdentifier& rTargetIdentifier);
			virtual OpenViBE::boolean sendMessage(
				const OpenViBE::CMessageEvent& rMessageEvent,
				const OpenViBE::CIdentifier* pTargetIdentifier,
				const OpenViBE::uint32 ui32TargetIdentifierCount);

			virtual OpenViBE::uint64 getCurrentTime(void);

			virtual OpenViBE::boolean stop(void);
			virtual OpenViBE::boolean pause(void);
			virtual OpenViBE::boolean play(void);
			virtual OpenViBE::boolean forward(void);
			virtual OpenViBE::Kernel::EPlayerStatus getStatus(void) const;

			//@}

			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void);
			virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void);
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void);
			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void);
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayerContext>, OVK_ClassId_Kernel_Player_PlayerContext)

		protected:

			OpenViBE::Kernel::CSimulatedBox* m_pSimulatedBox;
			OpenViBE::Kernel::IAlgorithmManager* m_pAlgorithmManagerBridge;
			OpenViBE::Kernel::IConfigurationManager* m_pConfigurationManagerBridge;
			OpenViBE::Kernel::ILogManager* m_pLogManagerBridge;
			OpenViBE::Kernel::IScenarioManager* m_pScenarioManagerBridge;
			OpenViBE::Kernel::ITypeManager* m_pTypeManagerBridge;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CPlayerContext_H__
