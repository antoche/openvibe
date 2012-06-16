#include "ovkCScenarioManager.h"
#include "ovkCScenario.h"

#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

CScenarioManager::CScenarioManager(const IKernelContext& rKernelContext)
	:TKernelObject<IScenarioManager>(rKernelContext)
{
}

CScenarioManager::~CScenarioManager(void)
{
	for(map<CIdentifier, CScenario*>::iterator i=m_vScenario.begin(); i!=m_vScenario.end(); i++)
	{
		delete i->second;
	}
}

CIdentifier CScenarioManager::getNextScenarioIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	map<CIdentifier, CScenario*>::const_iterator itScenario;

	if(rPreviousIdentifier==OV_UndefinedIdentifier)
	{
		itScenario=m_vScenario.begin();
	}
	else
	{
		itScenario=m_vScenario.find(rPreviousIdentifier);
		if(itScenario==m_vScenario.end())
		{
			return OV_UndefinedIdentifier;
		}
		itScenario++;
	}

	return itScenario!=m_vScenario.end()?itScenario->first:OV_UndefinedIdentifier;
}

boolean CScenarioManager::createScenario(
	CIdentifier& rScenarioIdentifier)
{
	//create scenario object
	rScenarioIdentifier=getUnusedIdentifier();
	CScenario* l_pScenario=new CScenario(getKernelContext(), rScenarioIdentifier);
	m_vScenario[rScenarioIdentifier]=l_pScenario;

	//create a scenario visualisation object as well
	CIdentifier l_oVisualisationTreeIdentifier;
	getKernelContext().getVisualisationManager().createVisualisationTree(l_oVisualisationTreeIdentifier);

	//initialize it
	IVisualisationTree& l_rVisualisationTree = getKernelContext().getVisualisationManager().getVisualisationTree(l_oVisualisationTreeIdentifier);
	l_rVisualisationTree.init(l_pScenario);

	//store identifier of visualisation tree in Scenario
	l_pScenario->setVisualisationTreeIdentifier(l_oVisualisationTreeIdentifier);

	return true;
}

boolean CScenarioManager::releaseScenario(
	const CIdentifier& rScenarioIdentifier)
{
	//retrieve iterator to scenario
	map<CIdentifier, CScenario*>::iterator itScenario;
	itScenario=m_vScenario.find(rScenarioIdentifier);
	if(itScenario==m_vScenario.end())
	{
		return false;
	}

	//release scenario visualisation
	CScenario* l_pScenario = itScenario->second;
	getKernelContext().getVisualisationManager().releaseVisualisationTree(l_pScenario->getVisualisationTreeIdentifier());

	//release scenario
	delete l_pScenario;
	m_vScenario.erase(itScenario);
	return true;
}

IScenario& CScenarioManager::getScenario(
	const CIdentifier& rScenarioIdentifier)
{
	map<CIdentifier, CScenario*>::const_iterator itScenario;
	itScenario=m_vScenario.find(rScenarioIdentifier);
	if(itScenario==m_vScenario.end())
	{
		this->getLogManager() << LogLevel_Fatal << "Scenario " << rScenarioIdentifier << " does not exist !\n";
	}
	return *itScenario->second;
}

CIdentifier CScenarioManager::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	map<CIdentifier, CScenario*>::const_iterator i;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		i=m_vScenario.find(l_oResult);
	}
	while(i!=m_vScenario.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}
