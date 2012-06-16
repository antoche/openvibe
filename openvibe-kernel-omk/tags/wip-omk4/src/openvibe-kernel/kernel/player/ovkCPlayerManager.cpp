#include "ovkCPlayerManager.h"

#include "../../ovk_tools.h"

#include <system/Math.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace std;

CPlayerManager::CPlayerManager(const IKernelContext& rKernelContext)
	:TKernelObject<IPlayerManager>(rKernelContext)
{
}

boolean CPlayerManager::createPlayer(
	CIdentifier& rPlayerIdentifier)
{
	IPlayer* l_pPlayer=OpenViBE::Tools::CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).createObject<IPlayer*>(OV_ClassId_Kernel_Player_Player);
	rPlayerIdentifier=getUnusedIdentifier();
	m_vPlayer[rPlayerIdentifier]=l_pPlayer;
	return true;
}

boolean CPlayerManager::releasePlayer(
	const CIdentifier& rPlayerIdentifier)
{
	map<CIdentifier, IPlayer*>::iterator itPlayer;
	itPlayer=m_vPlayer.find(rPlayerIdentifier);
	if(itPlayer==m_vPlayer.end())
	{
		return false;
	}
	OpenViBE::Tools::CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).releaseObject(itPlayer->second);
	m_vPlayer.erase(itPlayer);
	return true;
}

IPlayer& CPlayerManager::getPlayer(
	const CIdentifier& rPlayerIdentifier)
{
	map<CIdentifier, IPlayer*>::const_iterator itPlayer;
	itPlayer=m_vPlayer.find(rPlayerIdentifier);
	if(itPlayer==m_vPlayer.end())
	{
		log() << LogLevel_Fatal << "Player " << rPlayerIdentifier << " does not exist !\n";
	}
	return *itPlayer->second;
}

CIdentifier CPlayerManager::getNextPlayerIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextIdentifier < IPlayer* >(m_vPlayer, rPreviousIdentifier);
}

CIdentifier CPlayerManager::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=System::Math::randomUInteger64();
	CIdentifier l_oResult;
	map<CIdentifier, IPlayer*>::const_iterator i;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		i=m_vPlayer.find(l_oResult);
	}
	while(i!=m_vPlayer.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}
