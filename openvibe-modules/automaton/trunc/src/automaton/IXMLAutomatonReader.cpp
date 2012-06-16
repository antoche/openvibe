#include "IXMLAutomatonReader.h"
#include "defines.h"
#include "IXMLNodeReaderFactory.h"
#include "INodeFactory.h"
#include "IAutomatonContext.h"

#include "CIdentifier.h"

#include <xml/IReader.h>

#include <stack>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <cstdio>

#include "TXMLReader.hpp"

using namespace std;
using namespace Automaton;

namespace Automaton
{


	class CXMLAutomatonReader : public IXMLAutomatonReader, virtual public TXMLReader<XML::IReaderCallback>
	{
		//! The XML reader
		XML::IReader * m_pXMLReader;

		//! The node reader factory
		IXMLNodeReaderFactory * m_pNodeReaderFactory;

		//! The node factory
		INodeFactory * m_pNodeFactory;

		IAutomatonContext * m_pContext;

		stack<IXMLNodeReader*> m_oReaderStack;

		stack<std::string> m_oParsedXMLNodes;

		vector <CIdentifier> m_oNodes;

		EParsingStatus m_eStatus;

		public:
			CXMLAutomatonReader();
			virtual ~CXMLAutomatonReader();

			virtual Automaton::boolean processData(const void * pBuffer, const Automaton::uint64 ui64BufferSize);
			virtual IAutomatonController* getAutomatonController()
			{
				return createAutomatonController(m_pContext);
			}


			virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount);
			virtual void processChildData(const char* sData);
			virtual void closeChild(void);
			virtual void release(){ delete this; }

	};

};

//___________________________________________________________________________________________________
//

CXMLAutomatonReader::CXMLAutomatonReader() :
	m_pXMLReader(NULL),
	m_pNodeReaderFactory(NULL),
	m_pNodeFactory(NULL),
	m_pContext(NULL),
	m_eStatus(ParsingStatus_Nothing)
{
	m_pXMLReader= XML::createReader(*this);
	m_pNodeFactory = createNodeFactory();
	m_pNodeReaderFactory = createXMLNodeReaderFactory();

	m_pContext = createAutomatonContext();
}

CXMLAutomatonReader::~CXMLAutomatonReader()
{
	m_pXMLReader->release();
	releaseNodeFactory(m_pNodeFactory);
	releaseXMLNodeReaderFactory(m_pNodeReaderFactory);

}

boolean CXMLAutomatonReader::processData(const void * pBuffer, const Automaton::uint64 ui64BufferSize)
{
	return m_pXMLReader->processData(pBuffer, ui64BufferSize);
}

void CXMLAutomatonReader::openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount)
{
	if(m_eStatus == ParsingStatus_Error)
	{
		return;
	}

	//If we have to redirect to another reader
	if(m_oReaderStack.size() != 0)
	{
		//if there was an error in a child reader, propagates it
		if(m_oReaderStack.top()->openChild(sName, sAttributeName, sAttributeValue, ui64AttributeCount, m_pContext) == ParsingStatus_Error)
		{
			m_eStatus = ParsingStatus_Error;
		}
	}
	else
	{
		string l_oChildName(sName);
		m_oParsedXMLNodes.push(l_oChildName);

		if(l_oChildName == "Automaton" && m_eStatus==ParsingStatus_Nothing)
		{
			m_eStatus = ParsingStatus_Automaton;
		}
		else if(l_oChildName == "Node" && m_eStatus==ParsingStatus_Automaton)
		{
			m_eStatus = ParsingStatus_Node;

			const char * l_pValue = getAttributeValue(sAttributeName, sAttributeValue, ui64AttributeCount, "class");

			if(l_pValue)
			{
				string l_oClassAttribute = l_pValue;

				//gets the identifier associated with this kind of node
				CIdentifier l_oChildIdentifier = m_pContext->getIdentifier(l_oClassAttribute);

				//creates the associated node reader and adds it to the stack
				m_oReaderStack.push(m_pNodeReaderFactory->createNodeReader(l_oChildIdentifier));

				//calls it on the current node
				m_oReaderStack.top()->openChild(sName, sAttributeName, sAttributeValue, ui64AttributeCount, m_pContext);
			}
			//couldn't find class attribute in node
			else
			{
				cout<<"Error : couldn't find class attribute in Node child"<<endl;
				m_eStatus = ParsingStatus_Error;
			}
		}
		else if(l_oChildName == "Variable" && m_eStatus==ParsingStatus_Automaton)
		{
			m_eStatus = ParsingStatus_Variable;

			string l_oVariableName = getAttributeValue(sAttributeName, sAttributeValue, ui64AttributeCount, "name");
			string l_oVariableValue = getAttributeValue(sAttributeName, sAttributeValue, ui64AttributeCount, "value");

			if(!l_oVariableName.empty() && !l_oVariableValue.empty())
			{
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
				m_pContext->addParameter(l_oVariableName.c_str(), l_ui64Value);
			}
			else
			{
				cout<<"Problem encountered while parsing Variable node!"<<endl;
			}
		}
		//Error case
		else
		{
			m_eStatus = ParsingStatus_Error;
			cout<<"Unexpected Node! Problem in the input data found while parsing : "<<l_oChildName<<endl;
		}
	}

}

void CXMLAutomatonReader::processChildData(const char* sData)
{
	if(m_eStatus == ParsingStatus_Error)
	{
		return;
	}

	//If we have to redirect to another reader
	if(m_oReaderStack.size() != 0)
	{
		if(m_oReaderStack.top()->processChildData(sData, m_pContext) == ParsingStatus_Error)
		{
			m_eStatus = ParsingStatus_Error;
		}
	}
	else
	{
#ifdef DEBUG
		string& l_oChildName = m_oParsedXMLNodes.top();
		cout<<"Processing node : "<<l_oChildName <<"..."<<endl;
#endif

	}
}

void CXMLAutomatonReader::closeChild(void)
{
	if(m_eStatus == ParsingStatus_Error)
	{
		return;
	}

	//If we have to redirect to another reader
	if(m_oReaderStack.size() != 0)
	{
		EParsingStatus l_eChildStatus = m_oReaderStack.top()->closeChild(m_pContext);

		//the node has been read
		if(l_eChildStatus == ParsingStatus_Complete)
		{
			//gets the identifier of the new node
			CIdentifier l_oLastNode = m_oReaderStack.top()->getNodeIdentifier();

			//we don't need the associated reader anymore (desallocates it, removes it from the stack)
			m_pNodeReaderFactory->releaseNodeReader(m_oReaderStack.top());
			m_oReaderStack.pop();

			m_oParsedXMLNodes.pop();

			//we are now back to the main parsin state
			m_eStatus = ParsingStatus_Automaton;

			//adds it to the list of current nodes
			m_oNodes.push_back(l_oLastNode);

			if(m_oNodes.size() > 1)
			{
#ifdef DEBUG
				cout<<"link between"<< (uint64)m_oNodes[m_oNodes.size()-2]<<" and "<<(uint64)l_oLastNode<<endl;
#endif
				//if there was already a node in the list, links it with the newly added one
				m_pContext->addSuccessor(m_oNodes[m_oNodes.size()-2], l_oLastNode);
			}
		}
		else if(l_eChildStatus == ParsingStatus_Error)
		{
			m_eStatus = ParsingStatus_Error;
		}
	}
	else
	{
		string& l_oChildName = m_oParsedXMLNodes.top();

#ifdef DEBUG
		cout<<"Closing node : "<<l_oChildName<<endl;
#endif

		if(l_oChildName == "Automaton" && m_eStatus==ParsingStatus_Automaton)
		{
			//sets no successor for the last node
			m_pContext->getNode(m_oNodes.back())->addSuccessor(Automaton_No_Successor);

			m_eStatus = ParsingStatus_Nothing;
		}
		else if(l_oChildName == "Node" && m_eStatus==ParsingStatus_Node)
		{
			m_eStatus = ParsingStatus_Automaton;
			m_oReaderStack.pop();
		}
		else if(l_oChildName == "Variable" && m_eStatus==ParsingStatus_Variable)
		{
			m_eStatus = ParsingStatus_Automaton;
		}
		else
		{
			m_eStatus = ParsingStatus_Error;
			cout<<"Unexpected closing tag! "<<l_oChildName<<endl;
		}


		m_oParsedXMLNodes.pop();
	}
}

//___________________________________________________________________________________________________
//

Automaton_API IXMLAutomatonReader* Automaton::createXMLAutomatonReader()
{
	return new CXMLAutomatonReader;
}

Automaton_API void Automaton::releaseXMLAutomatonReader(IXMLAutomatonReader* pReader)
{
	pReader->release();
}

