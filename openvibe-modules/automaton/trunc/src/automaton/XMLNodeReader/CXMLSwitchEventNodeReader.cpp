#include "CXMLSwitchEventNodeReader.hpp"

#include <sstream>

#include <iostream>


using namespace std;
using namespace Automaton;

CXMLSwitchEventNodeReader::CXMLSwitchEventNodeReader() :
        m_pNodeReaderFactory(NULL),
	m_pNodeFactory(NULL),
	m_pParameterParser(NULL),
	m_pNode(NULL),
	m_eStatus(ParsingStatus_Nothing)
{
	m_pNodeFactory = createNodeFactory();
	m_pNodeReaderFactory = createXMLNodeReaderFactory();
}

CXMLSwitchEventNodeReader::~CXMLSwitchEventNodeReader()
{
	releaseNodeFactory(m_pNodeFactory);
	releaseXMLNodeReaderFactory(m_pNodeReaderFactory);
}

EParsingStatus CXMLSwitchEventNodeReader::openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64 ui64AttributeCount, IAutomatonContext* pContext)
{
	//If we have to redirect to another reader
	if(m_oReaderStack.size() != 0)
	{
		if(m_oReaderStack.top()->openChild(sName, sAttributeName, sAttributeValue, ui64AttributeCount, pContext) == ParsingStatus_Error)
		{
			m_eStatus = ParsingStatus_Error;
		}
	}
	else
	{
		string l_oChildName(sName);
		m_oParsedXMLNodes.push(l_oChildName);

#ifdef DEBUG
		cout<<"SwitchEventReader : Opening node : "<<l_oChildName<<endl;
#endif
		if(l_oChildName == "Node")
		{
			//If this is the first encountered "Node" Child, it is the one corresponding to the loop node
			if(m_eStatus == ParsingStatus_Nothing)
			{
				m_pNode = dynamic_cast<CNodeSwitch*>(m_pNodeFactory->createNode(Automaton_Node_Identifier_Switch));

				//add the switch node
				m_oNodeIdentifier = pContext->addNode(m_pNode);

				m_eStatus = ParsingStatus_Switch;
			}
			//else it's a child
			else if(m_eStatus == ParsingStatus_Child)
			{
				const char * l_pValue = getAttributeValue(sAttributeName, sAttributeValue, ui64AttributeCount, "class");

				if(l_pValue)
				{
					string l_oClassAttribute = l_pValue;

					CIdentifier l_oChildIdentifier = pContext->getIdentifier(l_oClassAttribute);
					m_oReaderStack.push(m_pNodeReaderFactory->createNodeReader(l_oChildIdentifier));
					m_oReaderStack.top()->openChild(sName, sAttributeName, sAttributeValue, ui64AttributeCount, pContext);
				}
				else
				{
					cout<<"Missing attribute class in node"<<endl;
					m_eStatus = ParsingStatus_Error;
				}
			}
			else
			{
				cout<<"Unexpected node : "<<l_oChildName<<endl;
				m_eStatus = ParsingStatus_Error;
			}
		}
		else if(l_oChildName == "Child" && m_eStatus==ParsingStatus_Switch)
		{
			m_eStatus = ParsingStatus_Child;
		}
		else if(l_oChildName == "Property" && m_eStatus==ParsingStatus_Child)
		{
			string l_oValue = getAttributeValue(sAttributeName, sAttributeValue, ui64AttributeCount, "class");

			if(l_oValue == "Event")
			{
				m_eStatus=ParsingStatus_Property;
			}
			else
			{
				cout<<"Missing attribute class(=Event) in Property node"<<endl;
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
	}

	return m_eStatus;
}

EParsingStatus CXMLSwitchEventNodeReader::processChildData(const char* sData, IAutomatonContext* pContext)
{
	//If we have to redirect to another reader
	if(m_oReaderStack.size() != 0)
	{
		if(m_oReaderStack.top()->processChildData(sData, pContext) == ParsingStatus_Error)
		{
			m_eStatus = ParsingStatus_Error;
		}
	}
	else
	{

		string& l_oChildName = m_oParsedXMLNodes.top();

#ifdef DEBUG
		cout<<"SwitchEventReader : Processing node : "<<l_oChildName<<"..."<<endl;
#endif

		if(l_oChildName == "Node" || l_oChildName == "Property" || l_oChildName == "Child" )
		{
		}
		else if(l_oChildName == "Parameter")
		{
			m_eStatus = m_pParameterParser->processChildData(sData, pContext);
		}
		else
		{
			cout<<"Unexpected node : "<<l_oChildName<<endl;
			m_eStatus = ParsingStatus_Error;
		}
	}

	return m_eStatus;
}

EParsingStatus CXMLSwitchEventNodeReader::closeChild(IAutomatonContext* pContext)
{
	//If we have to redirect to another reader
	if(m_oReaderStack.size() != 0)
	{
		EParsingStatus l_eChildStatus = m_oReaderStack.top()->closeChild(pContext);

		//the node has been read
		if(l_eChildStatus==ParsingStatus_Complete)
		{
			CIdentifier l_oLastNode = m_oReaderStack.top()->getNodeIdentifier();

			m_pNodeReaderFactory->releaseNodeReader(m_oReaderStack.top());
			m_oReaderStack.pop();

			m_oParsedXMLNodes.pop();

			m_oNodes.push_back(l_oLastNode);

			m_eStatus = ParsingStatus_Child;

			if(m_oNodes.size() > 1)
			{
#ifdef DEBUG
				cout<<"link between"<< (uint64)m_oNodes[m_oNodes.size()-2]<<" and "<<(uint64)l_oLastNode<<endl;
#endif
				pContext->addSuccessor(m_oNodes[m_oNodes.size()-2], l_oLastNode);
			}
		}
		else if(l_eChildStatus==ParsingStatus_Error)
		{
			m_eStatus=ParsingStatus_Error;
		}
	}
	else
	{

		string& l_oChildName = m_oParsedXMLNodes.top();

#ifdef DEBUG
		cout<<"SwitchEventReader : Closing node : "<<l_oChildName<<endl;
#endif

		if(l_oChildName == "Node" && m_eStatus==ParsingStatus_Switch)
		{
			//we know we've read all parameters, so we can configure the switch node
			m_pNode->setSwitchCondition("Event", m_oParameter, pContext);

			m_eStatus = ParsingStatus_Complete;
		}
		else if(l_oChildName == "Child" && m_eStatus==ParsingStatus_Child)
		{
#ifdef DEBUG
			cout<<"Link "<<(uint64)m_oNodeIdentifier<<" and "<<(uint64)m_oNodes.front()<<endl;
			cout<<"Link "<<(uint64)m_oNodes.back()<<" and "<<(uint64)m_oNodeIdentifier<<endl;
#endif

			//link the branch with its parent switch node
			pContext->addSuccessor(m_oNodeIdentifier, m_oNodes.front());
			pContext->addSuccessor(m_oNodes.back(), m_oNodeIdentifier);

			//reset the node vector (for the next branch)
			m_oNodes.clear();

			m_eStatus = ParsingStatus_Switch;
		}
		else if(l_oChildName == "Property" && m_eStatus==ParsingStatus_Property)
		{
			m_eStatus = ParsingStatus_Child;
		}
		else if(l_oChildName == "Parameter" && m_eStatus==ParsingStatus_Parameter)
		{
			m_eStatus = m_pParameterParser->closeChild(pContext);
			delete m_pParameterParser;
		}
		else
		{
			cout<<"Unexpected node : "<<l_oChildName<<endl;
			m_eStatus=ParsingStatus_Error;
		}

		m_oParsedXMLNodes.pop();

	}

	return m_eStatus;

}

void CXMLSwitchEventNodeReader::release()
{
	delete this;
}

