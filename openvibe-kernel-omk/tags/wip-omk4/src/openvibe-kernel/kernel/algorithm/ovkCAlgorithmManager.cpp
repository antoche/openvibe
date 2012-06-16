#include "ovkCAlgorithmManager.h"
#include "ovkCAlgorithmProxy.h"
#include "ovkCAlgorithm.h"

#include <system/Math.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace std;

CAlgorithmManager::CAlgorithmManager(const IKernelContext& rKernelContext)
	:TKernelObject<IAlgorithmManager>(rKernelContext)
{
}

CAlgorithmManager::~CAlgorithmManager(void)
{
	map < CIdentifier, pair < CAlgorithm*, CAlgorithmProxy* > >::iterator itAlgorithm;
	for(itAlgorithm=m_vAlgorithm.begin(); itAlgorithm!=m_vAlgorithm.end(); itAlgorithm++)
	{
		IAlgorithm& l_rAlgorithm=itAlgorithm->second.first->getAlgorithm();
		delete itAlgorithm->second.second;
		delete itAlgorithm->second.first;
		getKernelContext().getPluginManager().releasePluginObject(&l_rAlgorithm);
	}
}

CIdentifier CAlgorithmManager::createAlgorithm(
	const CIdentifier& rAlgorithmClassIdentifier)
{
	const IAlgorithmDesc* l_pAlgorithmDesc=NULL;
	IAlgorithm* l_pAlgorithm=getKernelContext().getPluginManager().createAlgorithm(rAlgorithmClassIdentifier, &l_pAlgorithmDesc);
	if(!l_pAlgorithm || !l_pAlgorithmDesc)
	{
		getLogManager() << LogLevel_Warning << "Algorithm creation failed, class identifier :" << rAlgorithmClassIdentifier << "\n";
		return OV_UndefinedIdentifier;
	}

	getLogManager() << LogLevel_Trace << "Creating algorithm with class identifier " << rAlgorithmClassIdentifier << "\n";

	CIdentifier l_oAlgorithmIdentifier=getUnusedIdentifier();
	CAlgorithm* l_pTrueAlgorithm=new CAlgorithm(getKernelContext(), *l_pAlgorithm, *l_pAlgorithmDesc);
	CAlgorithmProxy* l_pAlgorithmProxy=new CAlgorithmProxy(getKernelContext(), *l_pTrueAlgorithm);
	m_vAlgorithm[l_oAlgorithmIdentifier]=pair < CAlgorithm*, CAlgorithmProxy* >(l_pTrueAlgorithm, l_pAlgorithmProxy);
	return l_oAlgorithmIdentifier;
}

boolean CAlgorithmManager::releaseAlgorithm(
	const CIdentifier& rAlgorithmIdentifier)
{
	map < CIdentifier, pair < CAlgorithm*, CAlgorithmProxy* > >::iterator itAlgorithm;
	itAlgorithm=m_vAlgorithm.find(rAlgorithmIdentifier);
	if(itAlgorithm==m_vAlgorithm.end())
	{
		getLogManager() << LogLevel_Warning << "Algorithm release failed, identifier " << rAlgorithmIdentifier << "\n";
		return false;
	}
	getLogManager() << LogLevel_Trace << "Releasing algorithm with identifier " << rAlgorithmIdentifier << "\n";
	IAlgorithm& l_rAlgorithm=itAlgorithm->second.first->getAlgorithm();
	delete itAlgorithm->second.second;
	delete itAlgorithm->second.first;
	m_vAlgorithm.erase(itAlgorithm);
	getKernelContext().getPluginManager().releasePluginObject(&l_rAlgorithm);
	return true;
}

boolean CAlgorithmManager::releaseAlgorithm(
	IAlgorithmProxy& rAlgorithm)
{
	map < CIdentifier, pair < CAlgorithm*, CAlgorithmProxy* > >::iterator itAlgorithm;
	for(itAlgorithm=m_vAlgorithm.begin(); itAlgorithm!=m_vAlgorithm.end(); itAlgorithm++)
	{
		if((IAlgorithmProxy*)itAlgorithm->second.second==&rAlgorithm)
		{
			getLogManager() << LogLevel_Trace << "Releasing algorithm\n";
			IAlgorithm& l_rAlgorithm=itAlgorithm->second.first->getAlgorithm();
			delete itAlgorithm->second.second;
			delete itAlgorithm->second.first;
			m_vAlgorithm.erase(itAlgorithm);
			getKernelContext().getPluginManager().releasePluginObject(&l_rAlgorithm);
			return true;
		}
	}
	getLogManager() << LogLevel_Warning << "Algorithm release failed\n";
	return false;
}

IAlgorithmProxy& CAlgorithmManager::getAlgorithm(
	const CIdentifier& rAlgorithmIdentifier)
{
	map < CIdentifier, pair < CAlgorithm*, CAlgorithmProxy* > >::const_iterator itAlgorithm;
	itAlgorithm=m_vAlgorithm.find(rAlgorithmIdentifier);
	if(itAlgorithm==m_vAlgorithm.end())
	{
		log() << LogLevel_Fatal << "Algorithm " << rAlgorithmIdentifier << " does not exist !\n";
	}
	return *itAlgorithm->second.second;
}

CIdentifier CAlgorithmManager::getNextAlgorithmIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	map < CIdentifier, pair < CAlgorithm*, CAlgorithmProxy* > >::const_iterator itAlgorithm=m_vAlgorithm.begin();

	if(rPreviousIdentifier==OV_UndefinedIdentifier)
	{
		itAlgorithm=m_vAlgorithm.begin();
	}
	else
	{
		itAlgorithm=m_vAlgorithm.find(rPreviousIdentifier);
		if(itAlgorithm==m_vAlgorithm.end())
		{
			return OV_UndefinedIdentifier;
		}
		itAlgorithm++;
	}

	return itAlgorithm!=m_vAlgorithm.end()?itAlgorithm->first:OV_UndefinedIdentifier;
}

CIdentifier CAlgorithmManager::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=System::Math::randomUInteger64();
	CIdentifier l_oResult;
	map < CIdentifier, pair < CAlgorithm*, CAlgorithmProxy* > >::const_iterator i;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		i=m_vAlgorithm.find(l_oResult);
	}
	while(i!=m_vAlgorithm.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}
