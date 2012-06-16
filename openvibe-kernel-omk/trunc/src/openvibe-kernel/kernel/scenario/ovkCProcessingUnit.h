#ifndef __OpenViBEKernel_Kernel_Scenario_CProcessingUnit_H__
#define __OpenViBEKernel_Kernel_Scenario_CProcessingUnit_H__

#include "../ovkTKernelObject.h"

#include "ovkTAttributable.h"

#include <vector>
#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CProcessingUnit : public OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IProcessingUnit > >
		{
		public:

			CProcessingUnit(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual OpenViBE::boolean setIdentifier(
				const OpenViBE::CIdentifier& rIdentifier);
			virtual OpenViBE::CIdentifier getIdentifier(void) const;

			virtual OpenViBE::boolean setName(
				const OpenViBE::CString& sName);
			virtual OpenViBE::CString getName(void) const;

			virtual OpenViBE::boolean acceptVisitor(
				OpenViBE::IObjectVisitor& rObjectVisitor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IProcessingUnit > >, OVK_ClassId_Kernel_Scenario_ProcessingUnit)

		protected:

			OpenViBE::CIdentifier m_oIdentifier;
			OpenViBE::CString m_sName;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CProcessingUnit_H__
