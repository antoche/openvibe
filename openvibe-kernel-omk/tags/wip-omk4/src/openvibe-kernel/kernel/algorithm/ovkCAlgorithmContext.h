#ifndef __OpenViBEKernel_Kernel_CAlgorithmContext_H__
#define __OpenViBEKernel_Kernel_CAlgorithmContext_H__

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CAlgorithm;

		class CAlgorithmContext : public OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IAlgorithmContext >
		{
		public:

			CAlgorithmContext(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::CAlgorithm& rAlgorithm, const OpenViBE::Plugins::IPluginObjectDesc& rPluginObjectDesc);
			virtual ~CAlgorithmContext(void);

			virtual OpenViBE::Kernel::ILogManager& getLogManager(void);

			virtual OpenViBE::CIdentifier getNextInputParameterIdentifier(
				const OpenViBE::CIdentifier& rPreviousInputParameterIdentifier) const;
			virtual OpenViBE::Kernel::IParameter* getInputParameter(
				const OpenViBE::CIdentifier& rInputParameterIdentifier);

			virtual OpenViBE::CIdentifier getNextOutputParameterIdentifier(
				const OpenViBE::CIdentifier& rPreviousOutputParameterIdentifier) const;
			virtual OpenViBE::Kernel::IParameter* getOutputParameter(
				const OpenViBE::CIdentifier& rOutputParameterIdentifier);

			virtual OpenViBE::boolean isInputTriggerActive(
				const OpenViBE::CIdentifier& rInputTriggerIdentifier) const;

			virtual OpenViBE::boolean activateOutputTrigger(
				const OpenViBE::CIdentifier& rOutputTriggerIdentifier,
				const OpenViBE::boolean bTriggerState);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IAlgorithmContext >, OVK_ClassId_Kernel_Algorithm_AlgorithmContext)

		protected:

			OpenViBE::Kernel::ILogManager* m_pLogManager;
			OpenViBE::Kernel::CAlgorithm& m_rAlgorithm;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_CAlgorithmContext_H__
