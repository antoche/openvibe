#include "CNodeLoop.hpp"

using namespace Automaton;
using namespace std;

Automaton::boolean CNodeLoop::process(IAutomatonContext* pContext)
{
	m_bComplete = (*m_pLoopCondition)(pContext);

	if(m_bComplete)
	{
		m_pLoopCondition->reset();
	}

	return true;
}

Automaton::CIdentifier CNodeLoop::getSuccessor(IAutomatonContext* pContext)
{
	if(m_bComplete)
	{
#ifdef DEBUG
		cout<<"Quit loop"<<endl;
#endif
		return TNode<INode>::m_oSuccessor[1];
	}
	else
	{
#ifdef DEBUG
		cout<<"Enter loop"<<endl;
#endif
		return TNode<INode>::m_oSuccessor[0];
	}
}

Automaton::boolean CNodeLoop::setLoopCondition(const std::string& oCondition, const std::vector<Automaton::CIdentifier>& oParameter, IAutomatonContext * pContext)
{
	if(oCondition == "Infinite")
	{
		m_pLoopCondition = new CLoopConditionInfinite();
	}
	else if(oCondition == "Finite")
	{
		m_pLoopCondition = new CLoopConditionFinite(pContext->getParameterValue(oParameter[0]));
	}
	else if(oCondition == "WaitEvent")
	{
		m_pLoopCondition = new CLoopConditionEvent(pContext->getParameterValue(oParameter[0]));
	}

	return true;


}

