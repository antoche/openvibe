#include "CNodeSwitch.hpp"

using namespace Automaton;
using namespace std;

Automaton::boolean CNodeSwitch::process(IAutomatonContext* pContext)
{
	m_i64BranchNumber = (*m_pCondition)(pContext);
	return (m_i64BranchNumber != -1);
}

Automaton::CIdentifier CNodeSwitch::getSuccessor(IAutomatonContext* pContext)
{
	if(m_bContinue)
	{
#ifdef DEBUG
		cout<<"Quit switch"<<endl;
#endif
		m_bContinue = false;
		return TNode<INode>::m_oSuccessor[TNode<INode>::m_oSuccessor.size()-1];
	}
	else
	{
#ifdef DEBUG
		cout<<"Enter branch"<<m_i64BranchNumber<<endl;
#endif
		m_bContinue = true;
		return TNode<INode>::m_oSuccessor[(size_t)m_i64BranchNumber];
	}

}

void CNodeSwitch::setSwitchCondition(const std::string& oCondition, std::vector<Automaton::CIdentifier>& oParameter, Automaton::IAutomatonContext * pContext)
{
	if(oCondition == "Probability")
	{
		std::vector<Automaton::uint64> l_oProbability;
		l_oProbability.resize(oParameter.size());

		for(uint32 i=0 ; i<oParameter.size() ; i++)
		{
			l_oProbability[i] = pContext->getParameterValue(oParameter[i]);
		}

		m_pCondition = new CSwitchConditionProbability(l_oProbability);
	}
	else if(oCondition == "Event")
	{
		std::vector<Automaton::CIdentifier> l_oEventsIdentifiers;
		l_oEventsIdentifiers.resize(oParameter.size());

		for(uint32 i=0 ; i<oParameter.size() ; i++)
		{
			l_oEventsIdentifiers[i] = pContext->getParameterValue(oParameter[i]);
		}

		m_pCondition = new CSwitchConditionEvent(l_oEventsIdentifiers);
	}
	else
	{

	}
}

