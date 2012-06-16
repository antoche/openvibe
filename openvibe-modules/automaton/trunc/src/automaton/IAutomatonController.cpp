#include "IAutomatonController.h"

#include "IAutomatonContext.h"

#include <iostream>
using namespace std;

using namespace Automaton;

namespace Automaton
{

	class CAutomatonController : public IAutomatonController
	{
		IAutomatonContext * m_pContext;
		INode * m_pCurrentNode;

		public:
			CAutomatonController(IAutomatonContext * pContext);
			virtual ~CAutomatonController(){}
			virtual void release(){ delete this; }

			virtual IAutomatonContext* getAutomatonContext()
			{
				return m_pContext;
			}

			virtual Automaton::boolean process();
	};

};

//___________________________________________________________________________________________________
//


CAutomatonController::CAutomatonController(IAutomatonContext * pContext) : m_pContext(pContext)
{
	m_pCurrentNode = m_pContext->getInitialNode();
}


Automaton::boolean CAutomatonController::process()
{
	CIdentifier l_NodeIdentifier;

	while(l_NodeIdentifier!=Automaton_No_Successor && m_pCurrentNode->process(m_pContext))
	{

		l_NodeIdentifier = m_pCurrentNode->getSuccessor(m_pContext);

#ifdef DEBUG
		cout<<"going to next node ... "<< l_NodeIdentifier<<endl;
#endif

		m_pCurrentNode = m_pContext->getNode(l_NodeIdentifier);
	}

	return (l_NodeIdentifier == Automaton_No_Successor);
}

//___________________________________________________________________________________________________
//

Automaton_API Automaton::IAutomatonController* Automaton::createAutomatonController(Automaton::IAutomatonContext* pContext)
{
	return new Automaton::CAutomatonController(pContext);
}

Automaton_API void Automaton::releaseAutomatonController(Automaton::IAutomatonController * pController)
{
	pController->release();
}

