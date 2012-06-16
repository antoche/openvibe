#include "ovkCKernelObjectFactory.h"

#include "ovkCConfigurable.h"

#include "plugins/ovkCPluginModule.h"

#include "visualisation/ovkCVisualisationWidget.h"
#include "visualisation/ovkCVisualisationTree.h"

#include <string>
#include <algorithm>

using namespace std;
using namespace OpenViBE;
#define boolean OpenViBE::boolean

#define create(rcid,cid,sptr,cl) \
	if(rcid==cid) \
	{ \
		sptr=new cl(getKernelContext()); \
		if(sptr) \
		{ \
			m_oCreatedObjects.push_back(sptr); \
		} \
	}

Kernel::CKernelObjectFactory::CKernelObjectFactory(const Kernel::IKernelContext& rKernelContext)
	:TKernelObject<IKernelObjectFactory>(rKernelContext)
{
}

IObject* Kernel::CKernelObjectFactory::createObject(
	const CIdentifier& rClassIdentifier)
{
	IObject* l_pResult=NULL;

	create(rClassIdentifier, OV_ClassId_Kernel_Plugins_PluginModule,              l_pResult, Kernel::CPluginModule);

	create(rClassIdentifier, OV_ClassId_Kernel_Visualisation_VisualisationTree,   l_pResult, Kernel::CVisualisationTree);
	create(rClassIdentifier, OV_ClassId_Kernel_Visualisation_VisualisationWidget, l_pResult, Kernel::CVisualisationWidget);

	create(rClassIdentifier, OV_ClassId_Kernel_Configurable,                      l_pResult, Kernel::CConfigurable);

	// create(rClassIdentifier, OV_ClassId_, l_pResult, Plugins::CBoxContext);

	if(l_pResult)
	{
		this->getLogManager() << LogLevel_Debug << "Created object with class id " << rClassIdentifier << " and final class id " << l_pResult->getClassIdentifier() << "\n";
	}
	else
	{
		this->getLogManager() << LogLevel_Warning << "Unable to allocate object with class id " << rClassIdentifier << "\n";
	}

	return l_pResult;
}

boolean Kernel::CKernelObjectFactory::releaseObject(
	IObject* pObject)
{
	if(!pObject)
	{
		return true;
	}

	CIdentifier l_rClassIdentifier;
	l_rClassIdentifier=pObject->getClassIdentifier();

	vector<IObject*>::iterator i;
	i=find(m_oCreatedObjects.begin(), m_oCreatedObjects.end(), pObject);
	if(i==m_oCreatedObjects.end())
	{
		this->getLogManager() << LogLevel_Warning << "Can not release object with final class id " << l_rClassIdentifier << " - it is not owned by this fatory\n";
		return false;
	}
	m_oCreatedObjects.erase(i);
	delete pObject;

	this->getLogManager() << LogLevel_Debug << "Released object with final class id " << l_rClassIdentifier << "\n";

	return true;
}
