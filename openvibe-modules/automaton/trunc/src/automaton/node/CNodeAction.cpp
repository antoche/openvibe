#include "CNodeAction.hpp"

using namespace std;
using namespace Automaton;

Automaton::boolean CNodeAction::process(IAutomatonContext * pContext)
{
	return (*m_pAction)(pContext);
}

Automaton::boolean CNodeAction::setAction(const std::string& oAction, const std::vector<Automaton::CIdentifier>& oParameter, IAutomatonContext * pContext)
{
	if(oAction == "SendEvent")
	{
		m_pAction = new CSendEvent(pContext->getParameterValue(oParameter[0]));
	}
	else if(oAction == "WaitEvent")
	{
		m_pAction = new CWaitEvent(pContext->getParameterValue(oParameter[0]));
	}
	else if(oAction == "WaitTime")
	{
		if(oParameter.size() == 1)
		{
			m_pAction = new CWaitTime(pContext->getParameterValue(oParameter[0]));
		}
		else if(oParameter.size() > 1)
		{
			m_pAction = new CWaitTime(pContext->getParameterValue(oParameter[0]), pContext->getParameterValue(oParameter[1]));
		}
	}
	else
	{
		return false;
	}

	return true;
}

