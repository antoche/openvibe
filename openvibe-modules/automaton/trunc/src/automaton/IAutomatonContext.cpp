#include "IAutomatonContext.h"

#include "INode.h"
#include "CIdentifier.h"
#include "defines.h"

#include <map>
#include <string>

#include <iostream>

using namespace Automaton;
using namespace std;

namespace Automaton
{

	class CAutomatonContext : public IAutomatonContext
	{
		std::map<CIdentifier, INode*> m_oNodes;

		std::vector<CIdentifier> m_oReceivedEvents;

		std::vector<CIdentifier> m_oSentEvents;

		std::map<std::string, CIdentifier> m_oXMLNodeToIdentifier;

		Automaton::uint64 m_ui64CurrentTime;

		std::map<CIdentifier, string> m_oParameterName;
		std::map<CIdentifier, Automaton::uint64> m_oParameterValue;

		public:
			CAutomatonContext();

			virtual ~CAutomatonContext();
			virtual void release(){ delete this; }

			virtual INode * getNode(const CIdentifier& oIdentifier);
			virtual INode * getInitialNode();

			virtual CIdentifier addNode(INode* pNode);
			virtual boolean addSuccessor(CIdentifier oStart, CIdentifier oEnd);

			virtual const CIdentifier * getReceivedEvents()
			{
				if(m_oReceivedEvents.size() != 0)
				{
					return &m_oReceivedEvents[0];
				}
				return NULL;
			}

			virtual Automaton::uint32 getReceivedEventsCount()
			{
				return m_oReceivedEvents.size();
			}

			virtual void clearReceivedEvents()
			{
				m_oReceivedEvents.clear();
			}

			virtual void addReceivedEvent(CIdentifier& oIdentifier)
			{
				m_oReceivedEvents.push_back(oIdentifier);
			}

			virtual const CIdentifier * getSentEvents()
			{
				if(m_oSentEvents.size() != 0)
				{
					return &m_oSentEvents[0];
				}
				return NULL;
			}

			virtual Automaton::uint32 getSentEventsCount()
			{
				return m_oSentEvents.size();
			}

			virtual void clearSentEvents()
			{
				m_oSentEvents.clear();
			}

			virtual void sendEvent(CIdentifier& oIdentifier);

			virtual CIdentifier getIdentifier(const std::string& oXMLNodeName);

			virtual void setCurrentTime(Automaton::uint64 ui64Time);
			virtual Automaton::uint64 getCurrentTime();

			virtual CIdentifier addParameter(const char * pName, Automaton::uint64 ui64Value);
			virtual CIdentifier getVariableIdentifier(const char * pName);

			virtual const char* getParameterName(const CIdentifier& oIdentifier)
			{
				if(m_oParameterName.count(oIdentifier) == 0)
				{
					return NULL;
				}
				else
				{
					return m_oParameterName[oIdentifier].c_str();
				}
			}

			virtual Automaton::uint64 getParameterValue(const CIdentifier& oIdentifier)
			{
				return m_oParameterValue[oIdentifier];
			}

	};


};

//___________________________________________________________________________________________________
//

CAutomatonContext::CAutomatonContext() : m_ui64CurrentTime(0)
{
	m_oXMLNodeToIdentifier["Automaton"] = Automaton_Node_Identifier_Automaton;

	m_oXMLNodeToIdentifier["Action"] = Automaton_Node_Identifier_Action;
	m_oXMLNodeToIdentifier["ActionSendEvent"] = Automaton_Node_Identifier_ActionSendEvent;
	m_oXMLNodeToIdentifier["ActionWaitEvent"] = Automaton_Node_Identifier_ActionWaitEvent;
	m_oXMLNodeToIdentifier["ActionWaitTime"] = Automaton_Node_Identifier_ActionWaitTime;

	m_oXMLNodeToIdentifier["Loop"] = Automaton_Node_Identifier_Loop;
	m_oXMLNodeToIdentifier["LoopFinite"] = Automaton_Node_Identifier_LoopFinite;
	m_oXMLNodeToIdentifier["LoopInfinite"] = Automaton_Node_Identifier_LoopInfinite;
	m_oXMLNodeToIdentifier["LoopWaitEvent"] = Automaton_Node_Identifier_LoopWaitEvent;


	m_oXMLNodeToIdentifier["Switch"] = Automaton_Node_Identifier_Switch;
	m_oXMLNodeToIdentifier["SwitchProbabilist"] = Automaton_Node_Identifier_SwitchProbabilist;
	m_oXMLNodeToIdentifier["SwitchEvent"] = Automaton_Node_Identifier_SwitchEvent;


	m_oXMLNodeToIdentifier["RandomOrder"] = Automaton_Node_Identifier_RandomOrder;
}

CAutomatonContext::~CAutomatonContext()
{
	//deletes all the recorded nodes
	map<CIdentifier, INode*>::iterator l_oIterator;
	for(l_oIterator=m_oNodes.begin() ; l_oIterator!=m_oNodes.end() ; l_oIterator++)
	{
		delete l_oIterator->second;
	}
}


INode * CAutomatonContext::getNode(const CIdentifier& oIdentifier)
{
	if(m_oNodes.count(oIdentifier) == 0)
	{
		return NULL;
	}

	return m_oNodes[oIdentifier];
}

INode * CAutomatonContext::getInitialNode()
{
	return m_oNodes[CIdentifier(0)];
}

CIdentifier CAutomatonContext::addNode(INode* pNode)
{
	CIdentifier l_oCurrentIdentifier = m_oNodes.size();
	m_oNodes[l_oCurrentIdentifier] = pNode;

	return l_oCurrentIdentifier;
}

boolean CAutomatonContext::addSuccessor(CIdentifier oStart, CIdentifier oEnd)
{
	if(m_oNodes.count(oStart) == 0 || m_oNodes.count(oEnd) == 0 )
	{
		return false;
	}
	else
	{
		m_oNodes[oStart]->addSuccessor(oEnd);
		return true;
	}
}


void CAutomatonContext::sendEvent(CIdentifier& oIdentifier)
{
	m_oSentEvents.push_back(oIdentifier);
}

CIdentifier CAutomatonContext::getIdentifier(const std::string& oXMLNodeName)
{
	if(m_oXMLNodeToIdentifier.count(oXMLNodeName) == 0)
	{
		return Automaton_Node_Identifier_No_Node;
	}

	return m_oXMLNodeToIdentifier[oXMLNodeName];
}

void CAutomatonContext::setCurrentTime(Automaton::uint64 ui64Time)
{
	m_ui64CurrentTime = ui64Time;
}

Automaton::uint64 CAutomatonContext::getCurrentTime()
{
	return m_ui64CurrentTime;
}

CIdentifier CAutomatonContext::getVariableIdentifier(const char * pName)
{
	map<CIdentifier,string>::iterator iter;

	for(iter=m_oParameterName.begin() ; iter!=m_oParameterName.end() ; iter++)
	{
		if(iter->second == pName)
		{
			return iter->first;
		}
	}
	//didn't find it
	return CIdentifier(0);
}

CIdentifier CAutomatonContext::addParameter(const char * pName, Automaton::uint64 ui64Value)
{
	//uses the value map's size plus 1 to create the identifier (plus 1 since 0 is used
	//to indicate that an error occured)
	CIdentifier l_oCurrentIdentifier(m_oParameterValue.size() + 1);

	if(pName)
	{
		m_oParameterName[l_oCurrentIdentifier] = pName;
	}
	m_oParameterValue[l_oCurrentIdentifier] = ui64Value;

	return l_oCurrentIdentifier;
}

//___________________________________________________________________________________________________
//

Automaton_API IAutomatonContext* Automaton::createAutomatonContext()
{
	return new CAutomatonContext;
}

Automaton_API void Automaton::releaseAutomatonContext(IAutomatonContext* pContext)
{
	pContext->release();
}

