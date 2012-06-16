#include "ovCIdentifier.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>

using namespace OpenViBE;

CIdentifier::CIdentifier(void)
	:m_ui64Identifier(0xffffffffffffffffll)
{
}

CIdentifier::CIdentifier(const uint32 ui32Identifier1, const uint32 ui32Identifier2)
	:m_ui64Identifier((((uint64)ui32Identifier1)<<32)+ui32Identifier2)
{
}

CIdentifier::CIdentifier(const uint64 ui64Identifier)
	:m_ui64Identifier(ui64Identifier)
{
}

CIdentifier::CIdentifier(const CIdentifier& rIdentifier)
	:m_ui64Identifier(rIdentifier.m_ui64Identifier)
{
}

CIdentifier& CIdentifier::operator=(const CIdentifier& rIdentifier)
{
	m_ui64Identifier=rIdentifier.m_ui64Identifier;
	return *this;
}

CIdentifier& CIdentifier::operator++(void)
{
	if(m_ui64Identifier!=0xffffffffffffffffll)
	{
		m_ui64Identifier++;
		if(m_ui64Identifier==0xffffffffffffffffll)
		{
			m_ui64Identifier=0ll;
		}
	}
	return *this;
}

CIdentifier& CIdentifier::operator--(void)
{
	if(m_ui64Identifier!=0xffffffffffffffffll)
	{
		m_ui64Identifier--;
		if(m_ui64Identifier==0xffffffffffffffffll)
		{
			m_ui64Identifier=0xfffffffffffffffell;
		}
	}
	return *this;
}

namespace OpenViBE
{
	boolean operator==(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier==rIdentifier2.m_ui64Identifier;
	}

	boolean operator!=(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return !(rIdentifier1==rIdentifier2);
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

CString CIdentifier::toString(void) const
{
	char l_sBuffer[1024];
	unsigned int l_uiIdentifier1=(unsigned int)(m_ui64Identifier>>32);
	unsigned int l_uiIdentifier2=(unsigned int)(m_ui64Identifier);
	sprintf(l_sBuffer, "(0x%08x, 0x%08x)", l_uiIdentifier1, l_uiIdentifier2);
	return CString(l_sBuffer);
}

boolean CIdentifier::fromString(const CString& rString)
{
	const char* l_sBuffer=rString;
	unsigned int l_uiIdentifier1;
	unsigned int l_uiIdentifier2;
	if(sscanf(l_sBuffer, "(0x%x, 0x%x)", &l_uiIdentifier1, &l_uiIdentifier2)!=2)
	{
		return false;
	}
	m_ui64Identifier=(((uint64)l_uiIdentifier1)<<32)+l_uiIdentifier2;
	return true;
}

uint64 CIdentifier::toUInteger(void) const
{
	return m_ui64Identifier;
}

CIdentifier CIdentifier::random(void)
{
	uint64 l_ui64Identifier=0;
	for(uint32 i=0; i<4 || l_ui64Identifier==0xffffffffffffffffll; i++)
	{
		l_ui64Identifier<<=16;
		l_ui64Identifier+=(rand()&0xffff);
	}
	return l_ui64Identifier;
}
