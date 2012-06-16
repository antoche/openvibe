#include "INodeFactory.h"

#include "node/CNodeAction.hpp"
#include "node/CNodeLoop.hpp"
#include "node/CNodeSwitch.hpp"
#include "node/CNodeRandomOrder.hpp"

using namespace Automaton;

namespace Automaton
{
	class Automaton_API CNodeFactory : public INodeFactory
	{
		public:
			static INodeFactory* m_pFactory;
			static uint64 m_ui64ReferenceCount;

		public:
			virtual ~CNodeFactory(){}

			virtual INode * createNode(const CIdentifier& oIdentifier);
			virtual void releaseNode(INode * pNode);
			virtual void release(){ delete this; }

	};
};


//___________________________________________________________________________________________________
//

INodeFactory* CNodeFactory::m_pFactory = NULL;
uint64 CNodeFactory::m_ui64ReferenceCount = 0;


INode * CNodeFactory::createNode(const CIdentifier& oIdentifier)
{

	if(oIdentifier == Automaton_Node_Identifier_Action)
	{
		return new CNodeAction();
	}
	else if (oIdentifier == Automaton_Node_Identifier_Loop)
	{
		return new CNodeLoop();
	}
	else if (oIdentifier == Automaton_Node_Identifier_Switch)
	{
		return new CNodeSwitch();
	}
	else if (oIdentifier == Automaton_Node_Identifier_RandomOrder)
	{
		return new CNodeRandomOrder();
	}
	else
	{
		return NULL;
	}

}

void CNodeFactory::releaseNode(INode * pNode)
{
	delete pNode;
}

//___________________________________________________________________________________________________
//

Automaton_API Automaton::INodeFactory* Automaton::createNodeFactory()
{
	if(!CNodeFactory::m_pFactory)
	{
		CNodeFactory::m_pFactory = new CNodeFactory();
	}

	CNodeFactory::m_ui64ReferenceCount++;

	return CNodeFactory::m_pFactory;
}

Automaton_API void Automaton::releaseNodeFactory(Automaton::INodeFactory* pFactory)
{
	CNodeFactory::m_ui64ReferenceCount--;

	if(CNodeFactory::m_ui64ReferenceCount == 0 && CNodeFactory::m_pFactory)
	{
		CNodeFactory::m_pFactory->release();
		CNodeFactory::m_pFactory = NULL;
	}
}

