#include "ovCString.h"
#include "ovCNameValuePairList.h"

#include <cstdio>
#include <string>
#include <map>

using namespace OpenViBE;

namespace OpenViBE
{
	struct CNameValuePairListImpl
	{
		std::map<CString, CString> m_oMap;
	};
};

CNameValuePairList::CNameValuePairList(void)
{
	m_pNameValuePairListImpl=new CNameValuePairListImpl();
}

CNameValuePairList::CNameValuePairList(const CNameValuePairList& rNameValuePairList)
{
	m_pNameValuePairListImpl=new CNameValuePairListImpl();
	m_pNameValuePairListImpl->m_oMap=rNameValuePairList.m_pNameValuePairListImpl->m_oMap;
}

CNameValuePairList::~CNameValuePairList(void)
{
	delete m_pNameValuePairListImpl;
}

CNameValuePairList& CNameValuePairList::operator=(const CNameValuePairList& rNameValuePairList)
{
	m_pNameValuePairListImpl->m_oMap=rNameValuePairList.m_pNameValuePairListImpl->m_oMap;
	return *this;
}

boolean CNameValuePairList::setValue(const CString& rName, const CString& rValue)
{
	m_pNameValuePairListImpl->m_oMap[rName] = rValue;
	return true;
}

boolean CNameValuePairList::setValue(const CString& rName, const char* pValue)
{
	if(pValue == NULL)
	{
		return false;
	}
	m_pNameValuePairListImpl->m_oMap[rName] = pValue;
	return true;
}

boolean CNameValuePairList::setValue(const CString& rName, const float64& rValue)
{
	char s_pBuffer[1024];
	sprintf(s_pBuffer, "%lf", rValue);
	m_pNameValuePairListImpl->m_oMap[rName] = s_pBuffer;
	return true;
}

boolean CNameValuePairList::setValue(const CString& rName, boolean bValue)
{
	m_pNameValuePairListImpl->m_oMap[rName] = bValue ? "1" : "0";
	return true;
}

boolean CNameValuePairList::getValue(const CString& rName, CString& rValue) const
{
	if(m_pNameValuePairListImpl->m_oMap.find(rName) == m_pNameValuePairListImpl->m_oMap.end())
	{
		return false;
	}
	rValue = m_pNameValuePairListImpl->m_oMap[rName];
	return true;
}

boolean CNameValuePairList::getValue(const CString& rName, float64& rValue) const
{
	if(m_pNameValuePairListImpl->m_oMap.find(rName) == m_pNameValuePairListImpl->m_oMap.end())
	{
		return false;
	}
	float64 temp;
	if(sscanf(m_pNameValuePairListImpl->m_oMap[rName].toASCIIString(), "%lf", &temp)!=1)
	{
		return false;
	}
	rValue = temp;
	return true;
}

boolean CNameValuePairList::getValue(const CString& rName, boolean& rValue) const
{
	if(m_pNameValuePairListImpl->m_oMap.find(rName) == m_pNameValuePairListImpl->m_oMap.end())
	{
		return false;
	}
	CString l_sValue = m_pNameValuePairListImpl->m_oMap[rName];
	if(l_sValue==CString("0") || l_sValue==CString("FALSE") || l_sValue==CString("false"))
	{
		rValue = false;
		return true;
	}
	else if(l_sValue==CString("1") || l_sValue==CString("TRUE") || l_sValue==CString("true"))
	{
		rValue = true;
		return true;
	}
	else
	{
		return false;
	}
}
