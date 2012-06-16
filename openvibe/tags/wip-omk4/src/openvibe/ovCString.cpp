#include "ovCString.h"

#include <string>

using namespace OpenViBE;

namespace OpenViBE
{
	struct CStringImpl
	{
		std::string m_sValue;
	};
};

CString::CString(void)
{
	m_pStringImpl=new CStringImpl();
}

CString::CString(const CString& rString)
{
	m_pStringImpl=new CStringImpl();
	m_pStringImpl->m_sValue=rString.m_pStringImpl->m_sValue;
}

CString::CString(const char* pString)
{
	m_pStringImpl=new CStringImpl();
	if(pString)
	{
		m_pStringImpl->m_sValue=pString;
	}
}

CString::~CString(void)
{
	delete m_pStringImpl;
}

CString::operator const char* (void) const
{
	return m_pStringImpl->m_sValue.c_str();
}

CString& CString::operator=(const CString& rString)
{
	m_pStringImpl->m_sValue=rString.m_pStringImpl->m_sValue;
	return *this;
}

const CString OpenViBE::operator+(const CString& rString1, const CString& rString2)
{
	std::string l_oResult;
	l_oResult=rString1.m_pStringImpl->m_sValue+rString2.m_pStringImpl->m_sValue;
	return l_oResult.c_str();
}

boolean OpenViBE::operator==(const CString& rString1, const CString& rString2)
{
	return (rString1.m_pStringImpl->m_sValue)==(rString2.m_pStringImpl->m_sValue);
}

boolean OpenViBE::operator!=(const CString& rString1, const CString& rString2)
{
	return (rString1.m_pStringImpl->m_sValue)!=(rString2.m_pStringImpl->m_sValue);
}

boolean CString::set(const CString& rString)
{
	m_pStringImpl->m_sValue=rString.m_pStringImpl->m_sValue;
	return true;
}

boolean CString::set(const char* pString)
{
	if(pString)
	{
		m_pStringImpl->m_sValue=pString;
	}
	else
	{
		m_pStringImpl->m_sValue="";
	}
	return true;
}

const char* CString::toASCIIString(void) const
{
	return m_pStringImpl->m_sValue.c_str();
}
