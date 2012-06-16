#include "CIdentifier.h"

using namespace Automaton;

CIdentifier::CIdentifier(void)
	:m_ui64Identifier(0)
{
}

CIdentifier::CIdentifier(const uint64 ui64Identifier)
	:m_ui64Identifier(ui64Identifier)
{
}

CIdentifier::CIdentifier(const uint32 ui32Identifier1, const uint32 ui32Identifier2)
{
	m_ui64Identifier=(((uint64)ui32Identifier1)<<32)+ui32Identifier2;
}

CIdentifier::CIdentifier(const CIdentifier& rIdentifier)
	:m_ui64Identifier(rIdentifier.m_ui64Identifier)
{
}

const CIdentifier& CIdentifier::operator=(const CIdentifier& rIdentifier)
{
	m_ui64Identifier=rIdentifier.m_ui64Identifier;
	return *this;
}

namespace Automaton
{
	boolean operator==(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier==rIdentifier2.m_ui64Identifier;
	}

	boolean operator!=(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier!=rIdentifier2.m_ui64Identifier;
	}

	boolean operator<=(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier<=rIdentifier2.m_ui64Identifier;
	}

	boolean operator>=(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier>=rIdentifier2.m_ui64Identifier;
	}

	boolean operator<(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier<rIdentifier2.m_ui64Identifier;
	}

	boolean operator>(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier>rIdentifier2.m_ui64Identifier;
	}
};

CIdentifier::operator uint64(void) const
{
	return m_ui64Identifier;
}
