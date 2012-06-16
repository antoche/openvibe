#include "ovdCInterfacedObject.h"

using namespace OpenViBE;
using namespace OpenViBEDesigner;

CInterfacedObject::CInterfacedObject(void)
	:m_ui32ConnectorType(Connector_None)
	,m_ui32ConnectorIndex(0)
{
}

CInterfacedObject::CInterfacedObject(const CIdentifier& rIdentifier)
	:m_oIdentifier(rIdentifier)
	,m_ui32ConnectorType(Connector_None)
	,m_ui32ConnectorIndex(0)
{
}

CInterfacedObject::CInterfacedObject(const CIdentifier& rIdentifier, const uint32 ui32ConnectorType, const uint32 ui32ConnectorIndex)
	:m_oIdentifier(rIdentifier)
	,m_ui32ConnectorType(ui32ConnectorType)
	,m_ui32ConnectorIndex(ui32ConnectorIndex)
{
}
