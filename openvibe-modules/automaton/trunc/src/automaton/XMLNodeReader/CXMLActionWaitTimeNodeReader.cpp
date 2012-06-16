#include "CXMLActionWaitTimeNodeReader.hpp"

#include <sstream>
#include <iostream>

using namespace std;
using namespace Automaton;



CXMLActionWaitTimeNodeReader::CXMLActionWaitTimeNodeReader() :
	m_pNodeFactory(NULL),
	m_pParameterParser(NULL),
	m_pNode(NULL),
	m_eStatus(ParsingStatus_Node)
{
	m_pNodeFactory = createNodeFactory();
}

CXMLActionWaitTimeNodeReader::~CXMLActionWaitTimeNodeReader()
{
	releaseNodeFactory(m_pNodeFactory);
}

EParsingStatus CXMLActionWaitTimeNodeReader::openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64 ui64AttributeCount, IAutomatonContext* pContext)
{
	string l_oChildName(sName);
	m_oParsedXMLNodes.push(l_oChildName);

#ifdef DEBUG
	cout<<"ActionWaitTimeReader : Opening node : "<<l_oChildName<<endl;
#endif

	if(l_oChildName == "Node" && m_eStatus==ParsingStatus_Node)
	{
		//creates the action node
		m_pNode = dynamic_cast<CNodeAction*>(m_pNodeFactory->createNode(Automaton_Node_Identifier_Action));

		m_eStatus = ParsingStatus_Action;
	}
	else if(l_oChildName == "Property" && m_eStatus==ParsingStatus_Action)
	{
		string l_oValue = getAttributeValue(sAttributeName, sAttributeValue, ui64AttributeCount, "class");

		if(l_oValue == "Duration")
		{
			m_eStatus = ParsingStatus_Property;
		}
		else if(l_oValue == "RandomDuration")
		{
			m_eStatus = ParsingStatus_Property;
		}
		else
		{
			cout<<"Couldn't find class (=Duration or RandomDuration) attribute in property node"<<endl;
			m_eStatus = ParsingStatus_Error;
		}
	}
	else if(l_oChildName == "Parameter" && m_eStatus==ParsingStatus_Property)
	{
		m_pParameterParser = new CXMLParameterReader(m_oParameter);
		m_eStatus = m_pParameterParser->openChild(sName, sAttributeName, sAttributeValue, ui64AttributeCount, pContext);
	}
	else
	{
		cout<<"Unexpected node : "<<l_oChildName<<endl;
		m_eStatus = ParsingStatus_Error;
	}

	return m_eStatus;
}

EParsingStatus CXMLActionWaitTimeNodeReader::processChildData(const char* sData, IAutomatonContext* pContext)
{
	string& l_oChildName = m_oParsedXMLNodes.top();

#ifdef DEBUG
	cout<<"ActionWaitTimeReader : Processing node : "<<l_oChildName<<"..."<<endl;
#endif

	if(l_oChildName == "Parameter")
	{
		m_eStatus = m_pParameterParser->processChildData(sData, pContext);
	}

	return m_eStatus;
}

EParsingStatus CXMLActionWaitTimeNodeReader::closeChild(IAutomatonContext* pContext)
{
	string& l_oChildName = m_oParsedXMLNodes.top();

#ifdef DEBUG
	cout<<"ActionWaitTimeReader : Closing node : "<<l_oChildName<<endl;
#endif

	if(l_oChildName == "Node" && m_eStatus == ParsingStatus_Node)
	{
		//return the node Identifier if it has been completely parsed
		m_oNodeIdentifier = pContext->addNode(m_pNode);

		m_eStatus = ParsingStatus_Complete;
	}
	else if(l_oChildName == "Property" && m_eStatus == ParsingStatus_Property)
	{
		m_pNode->setAction("WaitTime", m_oParameter, pContext);

		m_eStatus = ParsingStatus_Node;
	}
	else if(l_oChildName == "Parameter" && m_eStatus == ParsingStatus_Parameter)
	{
		m_eStatus = m_pParameterParser->closeChild(pContext);
		delete 	m_pParameterParser;
	}
	else
	{
		cout<<"Unexpected node : "<<l_oChildName<<endl;
		m_eStatus = ParsingStatus_Error;
	}

	m_oParsedXMLNodes.pop();

	if(m_oParsedXMLNodes.size() == 0)
	{
		m_eStatus = ParsingStatus_Complete;
	}


	return m_eStatus;
}

void CXMLActionWaitTimeNodeReader::release()
{
	delete this;
}

