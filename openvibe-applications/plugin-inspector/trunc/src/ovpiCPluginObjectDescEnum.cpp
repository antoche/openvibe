#include "ovpiCPluginObjectDescEnum.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

CPluginObjectDescEnum::CPluginObjectDescEnum(const IKernelContext& rKernelContext)
	:m_rKernelContext(rKernelContext)
{
}

CPluginObjectDescEnum::~CPluginObjectDescEnum(void)
{
}

boolean CPluginObjectDescEnum::enumeratePluginObjectDesc(void)
{
	CIdentifier l_oIdentifier;
	while((l_oIdentifier=m_rKernelContext.getPluginManager().getNextPluginObjectDescIdentifier(l_oIdentifier))!=OV_UndefinedIdentifier)
	{
		this->callback(*m_rKernelContext.getPluginManager().getPluginObjectDesc(l_oIdentifier));
	}
	return true;
}

boolean CPluginObjectDescEnum::enumeratePluginObjectDesc(
	const CIdentifier& rParentClassIdentifier)
{
	CIdentifier l_oIdentifier;
	while((l_oIdentifier=m_rKernelContext.getPluginManager().getNextPluginObjectDescIdentifier(l_oIdentifier, rParentClassIdentifier))!=OV_UndefinedIdentifier)
	{
		this->callback(*m_rKernelContext.getPluginManager().getPluginObjectDesc(l_oIdentifier));
	}
	return true;
}

std::string CPluginObjectDescEnum::transform(const std::string& sInput, const boolean bRemoveSlash)
{
	std::string l_sInput(sInput);
	std::string l_sOutput;
	bool l_bLastWasSeparator=true;

	for(std::string::size_type i=0; i<l_sInput.length(); i++)
	{
		if((l_sInput[i]>='a' && l_sInput[i]<='z') || (l_sInput[i]>='A' && l_sInput[i]<='Z') || (l_sInput[i]>='0' && l_sInput[i]<='9') || (!bRemoveSlash && l_sInput[i]=='/'))
		{
			if(l_sInput[i]=='/')
			{
				l_sOutput+="_";
			}
			else
			{
				if(l_bLastWasSeparator)
				{
					if('a' <= l_sInput[i] && l_sInput[i] <= 'z')
					{
						l_sOutput+=l_sInput[i]+'A'-'a';
					}
					else
					{
						l_sOutput+=l_sInput[i];
					}
				}
				else
				{
					l_sOutput+=l_sInput[i];
				}
			}
			l_bLastWasSeparator=false;
		}
		else
		{
/*
			if(!l_bLastWasSeparator)
			{
				l_sOutput+="_";
			}
*/
			l_bLastWasSeparator=true;
		}
	}
	return l_sOutput;
}
