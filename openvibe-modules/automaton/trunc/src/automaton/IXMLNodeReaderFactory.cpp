#include "IXMLNodeReaderFactory.h"

#include "XMLNodeReader/CXMLActionSendEventNodeReader.hpp"
#include "XMLNodeReader/CXMLActionWaitEventNodeReader.hpp"
#include "XMLNodeReader/CXMLActionWaitTimeNodeReader.hpp"

#include "XMLNodeReader/CXMLLoopFiniteNodeReader.hpp"
#include "XMLNodeReader/CXMLLoopInfiniteNodeReader.hpp"
#include "XMLNodeReader/CXMLLoopWaitEventNodeReader.hpp"

#include "XMLNodeReader/CXMLSwitchProbabilistNodeReader.hpp"
#include "XMLNodeReader/CXMLSwitchEventNodeReader.hpp"

#include "XMLNodeReader/CXMLRandomOrderNodeReader.hpp"


using namespace Automaton;


namespace Automaton
{
	class CXMLNodeReaderFactory : public IXMLNodeReaderFactory
	{
		public:
			static IXMLNodeReaderFactory* m_pFactory;
			static uint64 m_ui64ReferenceCounter;

		public:
			virtual ~CXMLNodeReaderFactory(){}

			virtual IXMLNodeReader * createNodeReader(const CIdentifier& oIdentifier);
			virtual void releaseNodeReader(IXMLNodeReader * pReader);

			virtual void release(){ delete this; }
	};
};

//___________________________________________________________________________________________________
//

IXMLNodeReaderFactory* CXMLNodeReaderFactory::m_pFactory = NULL;
uint64 CXMLNodeReaderFactory::m_ui64ReferenceCounter = 0;


Automaton::IXMLNodeReader * CXMLNodeReaderFactory::createNodeReader(const CIdentifier& oIdentifier)
{
	if(oIdentifier == Automaton_Node_Identifier_ActionSendEvent)
	{
		return new CXMLActionSendEventNodeReader();
	}
	else if(oIdentifier == Automaton_Node_Identifier_ActionWaitEvent)
	{
		return new CXMLActionWaitEventNodeReader();
	}
	else if(oIdentifier == Automaton_Node_Identifier_ActionWaitTime)
	{
		return new CXMLActionWaitTimeNodeReader();
	}

	else if(oIdentifier == Automaton_Node_Identifier_LoopFinite)
	{
		return new CXMLLoopFiniteNodeReader();
	}
	else if(oIdentifier == Automaton_Node_Identifier_LoopInfinite)
	{
		return new CXMLLoopInfiniteNodeReader();
	}
	else if(oIdentifier == Automaton_Node_Identifier_LoopWaitEvent)
	{
		return new CXMLLoopWaitEventNodeReader();
	}

	else if(oIdentifier == Automaton_Node_Identifier_SwitchProbabilist)
	{
		return new CXMLSwitchProbabilistNodeReader();
	}
	else if(oIdentifier == Automaton_Node_Identifier_SwitchEvent)
	{
		return new CXMLSwitchEventNodeReader();
	}

	else if(oIdentifier == Automaton_Node_Identifier_RandomOrder)
	{
		return new CXMLRandomOrderNodeReader();
	}
	else
	{
		return NULL;
	}
}

void CXMLNodeReaderFactory::releaseNodeReader(Automaton::IXMLNodeReader * pReader)
{
	pReader->release();
}

//___________________________________________________________________________________________________
//

Automaton_API Automaton::IXMLNodeReaderFactory* Automaton::createXMLNodeReaderFactory()
{
	if(!CXMLNodeReaderFactory::m_pFactory)
	{
		CXMLNodeReaderFactory::m_pFactory = new CXMLNodeReaderFactory();
	}
	CXMLNodeReaderFactory::m_ui64ReferenceCounter++;

	return CXMLNodeReaderFactory::m_pFactory;
}

Automaton_API void Automaton::releaseXMLNodeReaderFactory(Automaton::IXMLNodeReaderFactory* pFactory)
{
	CXMLNodeReaderFactory::m_ui64ReferenceCounter--;

	if(CXMLNodeReaderFactory::m_ui64ReferenceCounter == 0 && CXMLNodeReaderFactory::m_pFactory)
	{
		CXMLNodeReaderFactory::m_pFactory->release();
		CXMLNodeReaderFactory::m_pFactory = NULL;
	}

}

