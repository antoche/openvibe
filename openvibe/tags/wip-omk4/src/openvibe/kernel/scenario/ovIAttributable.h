#ifndef __OpenViBE_Kernel_Scenario_IAttributable_H__
#define __OpenViBE_Kernel_Scenario_IAttributable_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IAttributable
		 * \author Yann Renard (IRISA/INRIA)
		 * \date 2006-12-07
		 * \brief
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 */
		class OV_API IAttributable : public OpenViBE::Kernel::IKernelObject
		{
		public:

			virtual OpenViBE::boolean addAttribute(
				const OpenViBE::CIdentifier& rAttributeIdentifier,
				const OpenViBE::CString& sAttributeValue)=0;
			virtual OpenViBE::boolean removeAttribute(
				const OpenViBE::CIdentifier& rAttributeIdentifier)=0;
			virtual OpenViBE::boolean removeAllAttributes(void)=0;

			virtual OpenViBE::CString getAttributeValue(
				const OpenViBE::CIdentifier& rAttributeIdentifier) const=0;
			virtual OpenViBE::boolean setAttributeValue(
				const OpenViBE::CIdentifier& rAttributeIdentifier,
				const OpenViBE::CString& sAttributeValue)=0;

			virtual OpenViBE::boolean hasAttribute(
				const OpenViBE::CIdentifier& rAttributeIdentifier) const=0;
			virtual OpenViBE::boolean hasAttributes(void) const=0;

			virtual OpenViBE::CIdentifier getNextAttributeIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_Attributable);
		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_IAttributable_H__
