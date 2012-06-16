#ifndef __OpenViBEKernel_Kernel_CAlgorithm_H__
#define __OpenViBEKernel_Kernel_CAlgorithm_H__

#include "../ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CAlgorithm : public OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IKernelObject >
		{
		public:

			CAlgorithm(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Plugins::IAlgorithm& rAlgorithm, const OpenViBE::Plugins::IAlgorithmDesc& rAlgorithmDesc);
			virtual ~CAlgorithm(void);

			virtual OpenViBE::Plugins::IAlgorithm& getAlgorithm(void);
			virtual const OpenViBE::Plugins::IAlgorithm& getAlgorithm(void) const;
			virtual const OpenViBE::Plugins::IAlgorithmDesc& getAlgorithmDesc(void) const;

			virtual OpenViBE::boolean addInputParameter(
				const OpenViBE::CIdentifier& rInputParameterIdentifier,
				const OpenViBE::CString& sInputName,
				const OpenViBE::Kernel::EParameterType eParameterType,
				const OpenViBE::CIdentifier& rSubTypeIdentifier);
			virtual OpenViBE::CIdentifier getNextInputParameterIdentifier(
				const OpenViBE::CIdentifier& rPreviousInputParameterIdentifier) const;
			virtual OpenViBE::Kernel::IParameter* getInputParameter(
				const OpenViBE::CIdentifier& rInputParameterIdentifier);
			virtual OpenViBE::Kernel::EParameterType getInputParameterType(
				const OpenViBE::CIdentifier& rInputParameterIdentifier) const;
			virtual OpenViBE::CString getInputParameterName(
				const OpenViBE::CIdentifier& rInputParameterIdentifier) const;
			virtual OpenViBE::boolean removeInputParameter(
				const OpenViBE::CIdentifier& rInputParameterIdentifier);

			virtual OpenViBE::boolean addOutputParameter(
				const OpenViBE::CIdentifier& rOutputParameterIdentifier,
				const OpenViBE::CString& sOutputName,
				const OpenViBE::Kernel::EParameterType eParameterType,
				const OpenViBE::CIdentifier& rSubTypeIdentifier);
			virtual OpenViBE::CIdentifier getNextOutputParameterIdentifier(
				const OpenViBE::CIdentifier& rPreviousOutputParameterIdentifier) const;
			virtual OpenViBE::Kernel::IParameter* getOutputParameter(
				const OpenViBE::CIdentifier& rOutputParameterIdentifier);
			virtual OpenViBE::Kernel::EParameterType getOutputParameterType(
				const OpenViBE::CIdentifier& rOutputParameterIdentifier) const;
			virtual OpenViBE::CString getOutputParameterName(
				const OpenViBE::CIdentifier& rOutputParameterIdentifier) const;
			virtual OpenViBE::boolean removeOutputParameter(
				const OpenViBE::CIdentifier& rOutputParameterIdentifier);

			virtual OpenViBE::boolean addInputTrigger(
				const OpenViBE::CIdentifier& rInputTriggerIdentifier,
				const OpenViBE::CString& rInputTriggerName);
			virtual OpenViBE::CIdentifier getNextInputTriggerIdentifier(
				const OpenViBE::CIdentifier& rPreviousInputTriggerIdentifier) const;
			virtual OpenViBE::CString getInputTriggerName(
				const OpenViBE::CIdentifier& rInputTriggerIdentifier) const;
			virtual OpenViBE::boolean isInputTriggerActive(
				const OpenViBE::CIdentifier& rInputTriggerIdentifier) const;
			virtual OpenViBE::boolean activateInputTrigger(
				const OpenViBE::CIdentifier& rInputTriggerIdentifier,
				const OpenViBE::boolean bTriggerState);
			virtual OpenViBE::boolean removeInputTrigger(
				const OpenViBE::CIdentifier& rInputTriggerIdentifier);

			virtual OpenViBE::boolean addOutputTrigger(
				const OpenViBE::CIdentifier& rOutputTriggerIdentifier,
				const OpenViBE::CString& rOutputTriggerName);
			virtual OpenViBE::CIdentifier getNextOutputTriggerIdentifier(
				const OpenViBE::CIdentifier& rPreviousOutputTriggerIdentifier) const;
			virtual OpenViBE::CString getOutputTriggerName(
				const OpenViBE::CIdentifier& rOutputTriggerIdentifier) const;
			virtual OpenViBE::boolean isOutputTriggerActive(
				const OpenViBE::CIdentifier& rOutputTriggerIdentifier) const;
			virtual OpenViBE::boolean activateOutputTrigger(
				const OpenViBE::CIdentifier& rOutputTriggerIdentifier,
				const OpenViBE::boolean bTriggerState);
			virtual OpenViBE::boolean removeOutputTrigger(
				const OpenViBE::CIdentifier& rOutputTriggerIdentifier);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean process(void);
			virtual OpenViBE::boolean process(
				const OpenViBE::CIdentifier& rTriggerIdentifier);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IKernelObject >, OVK_ClassId_Kernel_Algorithm_Algorithm)

		protected:

			OpenViBE::Kernel::IConfigurable* m_pInputConfigurable;
			OpenViBE::Kernel::IConfigurable* m_pOutputConfigurable;
			std::map<OpenViBE::CIdentifier, OpenViBE::CString> m_vInputParameterName;
			std::map<OpenViBE::CIdentifier, OpenViBE::CString> m_vOutputParameterName;
			std::map<OpenViBE::CIdentifier, std::pair<OpenViBE::CString, OpenViBE::boolean> > m_vInputTrigger;
			std::map<OpenViBE::CIdentifier, std::pair<OpenViBE::CString, OpenViBE::boolean> > m_vOutputTrigger;

		protected:

			void setAllInputTriggers(const OpenViBE::boolean bTriggerStatus);
			void setAllOutputTriggers(const OpenViBE::boolean bTriggerStatus);

			void handleCrash(const char* sWhere);

			OpenViBE::uint32 m_ui32CrashCount;
			OpenViBE::boolean m_bActive;

		private:

			const OpenViBE::Plugins::IAlgorithmDesc& m_rAlgorithmDesc;
			OpenViBE::Plugins::IAlgorithm& m_rAlgorithm;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_CAlgorithm_H__
