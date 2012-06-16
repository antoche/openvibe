#include "CXMLParameterReader.hpp"

//rand
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace Automaton;


EParsingStatus CXMLParameterReader::openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64 ui64AttributeCount, IAutomatonContext* pContext)
{
	m_oParameterType = getAttributeValue(sAttributeName, sAttributeValue, ui64AttributeCount, "class");

	if(m_oParameterType == "IValue" || m_oParameterType == "IVariable" || m_oParameterType == "IInterval")
	{
		m_eStatus = ParsingStatus_Parameter;

	}
	else
	{
		cout<<"Couldn't find valid class attribute in parameter node"<<endl;
		m_eStatus = ParsingStatus_Error;
	}

	return m_eStatus;
}

EParsingStatus CXMLParameterReader::processChildData(const char* sData, IAutomatonContext* pContext)
{
	if(m_oParameterType=="IValue")
	{
		string l_oVariableValue=sData;
		uint64 l_ui64Value=0;
		if(l_oVariableValue.length()>=3 && l_oVariableValue[0]=='0' && l_oVariableValue[1]=='x')
		{
			unsigned int l_uiValue;
			sscanf(l_oVariableValue.c_str(), "%x", &l_uiValue);
			l_ui64Value=static_cast<uint64>(l_uiValue);
		}
		else if(l_oVariableValue.length()>=2 && l_oVariableValue[0]=='0')
		{
			unsigned int l_uiValue;
			sscanf(l_oVariableValue.c_str(), "%o", &l_uiValue);
			l_ui64Value=static_cast<uint64>(l_uiValue);
		}
		else
		{
			unsigned int l_uiValue;
			sscanf(l_oVariableValue.c_str(), "%u", &l_uiValue);
			l_ui64Value=static_cast<uint64>(l_uiValue);
		}
		m_oParameter.push_back(pContext->addParameter(NULL, l_ui64Value));
	}
	else if(m_oParameterType=="IVariable")
	{
		CIdentifier l_oVariableIdentifier = pContext->getVariableIdentifier(sData);

		if(l_oVariableIdentifier == CIdentifier(0))
		{
			cout<<"Variable used wasn't declared! Incorrect variable : "<<sData<<endl;
			m_eStatus = ParsingStatus_Error;
		}
		else
		{
			m_oParameter.push_back(l_oVariableIdentifier);
		}
	}
	else if(m_oParameterType=="IInterval")
	{
		istringstream l_oStringData(sData);
		uint64 l_ui64LowestValue;
		uint64 l_ui64HighestValue;

		l_oStringData>>l_ui64LowestValue>>l_ui64HighestValue;

		if(l_ui64LowestValue>=l_ui64HighestValue)
		{
			cout<<"Wrong interval : "<<sData<<endl;
			m_eStatus = ParsingStatus_Error;
		}

		m_oParameter.push_back(pContext->addParameter(NULL,l_ui64LowestValue));
		m_oParameter.push_back(pContext->addParameter(NULL,l_ui64HighestValue));
	}

	return m_eStatus;
}

EParsingStatus CXMLParameterReader::closeChild(IAutomatonContext* pContext)
{
	return ParsingStatus_Property;
}


