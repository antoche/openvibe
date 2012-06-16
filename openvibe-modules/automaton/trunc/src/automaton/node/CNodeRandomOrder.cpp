#include "CNodeRandomOrder.hpp"

using namespace Automaton;
using namespace std;

Automaton::boolean CNodeRandomOrder::process(IAutomatonContext* pContext)
{
	if(m_oBranchCount.size() != 0)
	{
		//random index in m_oBranchCount
		Automaton::uint64 l_ui64RandomIndex = rand() % m_oBranchCount.size();

		m_ui64BranchNumber = m_oBranchCount[(size_t)l_ui64RandomIndex].first;

		//if this branch "ttl" is equal to 0, suppress its entry
		if(--m_oBranchCount[(size_t)l_ui64RandomIndex].second == 0)
		{
			m_oBranchCount.erase(m_oBranchCount.begin() + (size_t)l_ui64RandomIndex );
		}
	}
	else
	{
		//we can quit this node

		//reset it
		for(uint64 i=0 ; i<m_ui64BranchCount ; i++)
		{
			m_oBranchCount.push_back(pair<uint64,uint64>(i, m_ui64IterationCount));
		}


		m_ui64BranchNumber = TNode<INode>::m_oSuccessor.size() - 1;
	}

	return true;
}

Automaton::CIdentifier CNodeRandomOrder::getSuccessor(IAutomatonContext* pContext)
{
	return TNode<INode>::m_oSuccessor[(size_t)m_ui64BranchNumber];
}


void CNodeRandomOrder::setIterationCount(const std::vector<Automaton::CIdentifier>& oParameter, IAutomatonContext * pContext)
{
	m_ui64IterationCount = pContext->getParameterValue(oParameter[0]);
	m_ui64BranchCount = pContext->getParameterValue(oParameter[1]);

	//Doesn't take into account the fact that the switch's successor is stored in m_oSuccessor
	//only valid if called before it (the switch's successor) is added
	for(uint64 i=0 ; i<m_ui64BranchCount ; i++)
	{
		m_oBranchCount.push_back(pair<uint64,uint64>(i, m_ui64IterationCount));
	}
}

