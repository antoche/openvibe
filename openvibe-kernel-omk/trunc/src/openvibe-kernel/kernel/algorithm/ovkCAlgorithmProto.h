#ifndef __OpenViBEKernel_Kernel_Algorithm_CAlgorithmProto_H__
#define __OpenViBEKernel_Kernel_Algorithm_CAlgorithmProto_H__

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CAlgorithm;

		class CAlgorithmProto : public OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IAlgorithmProto >
		{
		public:

			CAlgorithmProto(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::CAlgorithm& rAlgorithm);

			virtual OpenViBE::boolean addInputParameter(
				const OpenViBE::CIdentifier& rInputParameterIdentifier,
				const OpenViBE::CString& sInputName,
				const OpenViBE::Kernel::EParameterType eParameterType,
				const OpenViBE::CIdentifier& rSubTypeIdentifier);

			virtual OpenViBE::boolean addOutputParameter(
				const OpenViBE::CIdentifier& rOutputParameterIdentifier,
				const OpenViBE::CString& sOutputName,
				const OpenViBE::Kernel::EParameterType eParameterType,
				const OpenViBE::CIdentifier& rSubTypeIdentifier);

			virtual OpenViBE::boolean addInputTrigger(
				const OpenViBE::CIdentifier& rInputTriggerIdentifier,
				const OpenViBE::CString& rInputTriggerName);

			virtual OpenViBE::boolean addOutputTrigger(
				const OpenViBE::CIdentifier& rOutputTriggerIdentifier,
				const OpenViBE::CString& rOutputTriggerName);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IAlgorithmProto >, OVK_ClassId_Kernel_Algorithm_AlgorithmProto)

		protected:

			OpenViBE::Kernel::CAlgorithm& m_rAlgorithm;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Algorithm_CAlgorithmProto_H__
