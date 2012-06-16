#ifndef __OpenViBEDesigner_TAttributeHandler_H__
#define __OpenViBEDesigner_TAttributeHandler_H__

#include "ovd_base.h"

namespace OpenViBEDesigner
{
	class TAttributeHandler
	{
	public:

		TAttributeHandler(OpenViBE::Kernel::IAttributable& rAttributable);
		TAttributeHandler(const OpenViBE::Kernel::IAttributable& rAttributable);

		template <class T>
		OpenViBE::boolean addAttribute(
			const OpenViBE::CIdentifier& rAttributeIdentifier,
			const T& rValue) const;

		OpenViBE::boolean removeAttribute(
			const OpenViBE::CIdentifier& rAttributeIdentifier);

		OpenViBE::boolean removeAllAttributes(void);

		template<class T>
		T getAttributeValue(
			const OpenViBE::CIdentifier& rAttributeIdentifier) const;

		template <class T>
		OpenViBE::boolean setAttributeValue(
			const OpenViBE::CIdentifier& rAttributeIdentifier,
			const T& rValue);

		OpenViBE::boolean hasAttribute(
			const OpenViBE::CIdentifier& rAttributeIdentifier) const;

		OpenViBE::boolean hasAttributes(void) const;

	protected:

		const OpenViBE::Kernel::IAttributable* m_pConstAttributable;
		OpenViBE::Kernel::IAttributable* m_pAttributable;
	};
};

#endif // __OpenViBEDesigner_TAttributeHandler_H__
