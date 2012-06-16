#include "ovkCVisualisationTree.h"
#include "ovkCVisualisationManager.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Tools;
using namespace std;

CVisualisationManager::CVisualisationManager(const OpenViBE::Kernel::IKernelContext& rKernelContext) :
	OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IVisualisationManager>(rKernelContext)
{
}

boolean CVisualisationManager::createVisualisationTree(CIdentifier& rVisualisationTreeIdentifier)
{
	//create visualisation tree
	IVisualisationTree* l_pVisualisationTree =
		CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).createObject<IVisualisationTree*>(OV_ClassId_Kernel_Visualisation_VisualisationTree);

	//generate an identifier for visualisation tree
	rVisualisationTreeIdentifier=getUnusedIdentifier();

	//store pointer to visualisation tree
	m_vVisualisationTree[rVisualisationTreeIdentifier] = l_pVisualisationTree;

	return true;
}

boolean CVisualisationManager::releaseVisualisationTree(const CIdentifier& rVisualisationTreeIdentifier)
{
	map<CIdentifier, IVisualisationTree*>::iterator it = m_vVisualisationTree.find(rVisualisationTreeIdentifier);
	if(it != m_vVisualisationTree.end())
	{
		CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).releaseObject(it->second);
		m_vVisualisationTree.erase(it);
		return true;
	}

	return false;
}

IVisualisationTree& CVisualisationManager::getVisualisationTree(const CIdentifier& rVisualisationTreeIdentifier)
{
	map<CIdentifier, IVisualisationTree*>::const_iterator it = m_vVisualisationTree.find(rVisualisationTreeIdentifier);
	if(it == m_vVisualisationTree.end())
	{
		log() << LogLevel_Fatal << "Visualisation Tree " << rVisualisationTreeIdentifier << " does not exist !\n";
	}
	return *it->second;
}

boolean CVisualisationManager::enumerateVisualisationTrees(IVisualisationManager::IVisualisationTreeEnum& rCallback) const
{
	map<CIdentifier, IVisualisationTree*>::const_iterator it;
	for(it = m_vVisualisationTree.begin(); it != m_vVisualisationTree.end(); it++)
	{
		if(!rCallback.callback(it->first, *it->second))
		{
			return true;
		}
	}
	return true;
}

boolean CVisualisationManager::setWidgets(const CIdentifier& rVisualisationTreeIdentifier, const CString& rVisualisationBoxName, ::GtkWidget* pWidget, ::GtkWidget* pToolbarWidget)
{
	IVisualisationTree& l_rVisualisationTree = getVisualisationTree(rVisualisationTreeIdentifier);

	l_rVisualisationTree.setWidgets(rVisualisationBoxName, pWidget, pToolbarWidget);

	return true;
}

CIdentifier CVisualisationManager::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	map<CIdentifier, IVisualisationTree*>::const_iterator it;
	do
	{
		l_oResult=CIdentifier(l_ui64Identifier++);
		it=m_vVisualisationTree.find(l_oResult);
	}
	while(it!=m_vVisualisationTree.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}
