#ifndef __OpenViBEDesigner_CInterfacedObject_H__
#define __OpenViBEDesigner_CInterfacedObject_H__

#include "ovd_base.h"

namespace OpenViBEDesigner
{
	class CInterfacedObject
	{
	public:

		CInterfacedObject(void);
		CInterfacedObject(const OpenViBE::CIdentifier& rIdentifier);
		CInterfacedObject(const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::uint32 ui32ConnectorType, const OpenViBE::uint32 ui32ConnectorIndex);

		OpenViBE::CIdentifier m_oIdentifier;
		OpenViBE::uint32 m_ui32ConnectorType;
		OpenViBE::uint32 m_ui32ConnectorIndex;
	};
};

#endif // __OpenViBEDesigner_CInterfacedObject_H__
