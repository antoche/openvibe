#include "ovkCPluginManager.h"
#include "ovkCPluginModule.h"

#include "../../ovk_tools.h"

#include <fs/IEntryEnumerator.h>
#include <fs/Files.h>

#include <system/Memory.h>

#include <cstdio>
#include <iostream>
#include <map>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace std;

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginManagerEntryEnumeratorCallBack : public TKernelObject < IObject >, public FS::IEntryEnumeratorCallBack
		{
		public:

			CPluginManagerEntryEnumeratorCallBack(const IKernelContext& rKernelContext, vector<IPluginModule*>& rPluginModule, map<IPluginObjectDesc*, IPluginModule*>& rPluginObjectDesc)
				:TKernelObject < IObject >(rKernelContext)
				,m_rPluginManager(rKernelContext.getPluginManager())
				,m_rPluginModule(rPluginModule)
				,m_rPluginObjectDesc(rPluginObjectDesc)
			{
			}

			virtual FS::boolean callback(
				FS::IEntryEnumerator::IEntry& rEntry,
				FS::IEntryEnumerator::IAttributes& rAttributes)
			{
				vector<IPluginModule*>::iterator i;
				for(i=m_rPluginModule.begin(); i!=m_rPluginModule.end(); i++)
				{
					CString l_sPluginModuleName;
					if(!(*i)->getFileName(l_sPluginModuleName))
					{
						return true;
					}

					if(FS::Files::equals(rEntry.getName(), (const char*)l_sPluginModuleName))
					{
						this->getLogManager() << LogLevel_Warning << "Module [" << CString(rEntry.getName()) << "] has already been loaded\n";
						return true;
					}
				}

				IPluginModule* l_pPluginModule=new CPluginModule(this->getKernelContext());
				CString l_sLoadError;
				if(!l_pPluginModule->load(rEntry.getName(), &l_sLoadError))
				{
					delete l_pPluginModule;
					this->getLogManager() << LogLevel_Warning << "File [" << CString(rEntry.getName()) << "] is not a plugin module (error:" << l_sLoadError << ")\n";
					return true;
				}

				if(!l_pPluginModule->initialize())
				{
					l_pPluginModule->uninitialize();
					l_pPluginModule->unload();
					delete l_pPluginModule;
					this->getLogManager() << LogLevel_Warning << "Module [" << CString(rEntry.getName()) << "] did not initialize correctly\n";
					return true;
				}

				boolean l_bPluginObjectDescAdded=false;
				uint32 l_ui32Index=0;
				uint32 l_ui32Count=0;
				IPluginObjectDesc* l_pPluginObjectDesc=NULL;
				while(l_pPluginModule->getPluginObjectDescription(l_ui32Index, l_pPluginObjectDesc))
				{
					boolean l_bFound=false;

					map<IPluginObjectDesc*, IPluginModule*>::const_iterator i;
					for(i=m_rPluginObjectDesc.begin(); i!=m_rPluginObjectDesc.end() && !l_bFound; i++)
					{
						if(i->first->getClassIdentifier()==l_pPluginObjectDesc->getClassIdentifier())
						{
							this->getLogManager() << LogLevel_ImportantWarning << "Duplicate plugin object descriptor class identifier [" << i->first->getName() << "] and [" << l_pPluginObjectDesc->getName() << "]... second one is ignored.\n";
							l_bFound=true;
						}
					}

					if(!l_bFound)
					{
						if(!l_bPluginObjectDescAdded)
						{
							m_rPluginModule.push_back(l_pPluginModule);
							l_bPluginObjectDescAdded=true;
						}
						m_rPluginObjectDesc[l_pPluginObjectDesc]=l_pPluginModule;
						l_ui32Count++;
					}
					l_ui32Index++;
					l_pPluginObjectDesc=NULL;
				}

				if(l_bPluginObjectDescAdded)
				{
					this->getLogManager() << LogLevel_Info << "Added " << l_ui32Count << " plugin object descriptor(s) from [" << CString(rEntry.getName()) << "]\n";
				}
				else
				{
					this->getLogManager() << LogLevel_Warning << "No 'plugin object descriptor' found from [" << CString(rEntry.getName()) << "] even if it looked like a plugin module\n";
				}

				return true;
			}

			_IsDerivedFromClass_Final_(TKernelObject < IObject >, OV_UndefinedIdentifier)

		protected:

			IPluginManager& m_rPluginManager;
			vector<IPluginModule*>& m_rPluginModule;
			map<IPluginObjectDesc*, IPluginModule*>& m_rPluginObjectDesc;
		};
	};
};

CPluginManager::CPluginManager(const IKernelContext& rKernelContext)
	:TKernelObject<IPluginManager>(rKernelContext)
{
}

CPluginManager::~CPluginManager(void)
{
	map < IPluginObjectDesc*, vector < IPluginObject* > >::iterator i;
	vector < IPluginObject* >::iterator j;
	for(i=m_vPluginObject.begin(); i!=m_vPluginObject.end(); i++)
	{
		for(j=i->second.begin(); j!=i->second.end(); j++)
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Trying to release plugin object with class id " << (*j)->getClassIdentifier() << " and plugin object descriptor " << i->first->getName() << " at plugin manager destruction time\n";
			(*j)->release();
		}
	}
}

boolean CPluginManager::addPluginsFromFiles(
	const CString& rFileNameWildCard)
{
	this->getLogManager() << LogLevel_Info << "Adding [" << rFileNameWildCard << "]\n";

	boolean l_bResult;
	CPluginManagerEntryEnumeratorCallBack l_rCB(this->getKernelContext(), m_vPluginModule, m_vPluginObjectDesc);
	FS::IEntryEnumerator* l_pEntryEnumerator=FS::createEntryEnumerator(l_rCB);
	l_bResult=l_pEntryEnumerator->enumerate(rFileNameWildCard);
	l_pEntryEnumerator->release();
	return l_bResult;
}

CIdentifier CPluginManager::getNextPluginObjectDescIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	boolean l_bFoundPrevious=(rPreviousIdentifier==OV_UndefinedIdentifier);
	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); i++)
	{
		if(!l_bFoundPrevious)
		{
			if(i->first->getClassIdentifier()==rPreviousIdentifier)
			{
				l_bFoundPrevious=true;
			}
		}
		else
		{
			return i->first->getClassIdentifier();
		}
	}
	return OV_UndefinedIdentifier;
}

CIdentifier CPluginManager::getNextPluginObjectDescIdentifier(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBaseClassIdentifier) const
{
	boolean l_bFoundPrevious=(rPreviousIdentifier==OV_UndefinedIdentifier);
	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); i++)
	{
		if(!l_bFoundPrevious)
		{
			if(i->first->getClassIdentifier()==rPreviousIdentifier)
			{
				l_bFoundPrevious=true;
			}
		}
		else
		{
			if(i->first->isDerivedFromClass(rBaseClassIdentifier))
			{
				return i->first->getClassIdentifier();
			}
		}
	}
	return OV_UndefinedIdentifier;
}

boolean CPluginManager::canCreatePluginObject(
	const CIdentifier& rClassIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Searching if can build plugin object\n";

	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); i++)
	{
		if(i->first->getCreatedClass()==rClassIdentifier)
		{
			return true;
		}
	}

	return false;
}

const IPluginObjectDesc* CPluginManager::getPluginObjectDesc(
	const CIdentifier& rClassIdentifier) const
{
	this->getLogManager() << LogLevel_Debug << "Searching plugin object descriptor\n";

	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); i++)
	{
		if(i->first->getClassIdentifier()==rClassIdentifier)
		{
			return i->first;
		}
	}

	this->getLogManager() << LogLevel_Debug << "Plugin object descriptor class identifier " << rClassIdentifier << " not found\n";
	return NULL;
}

const IPluginObjectDesc* CPluginManager::getPluginObjectDescCreating(
	const CIdentifier& rClassIdentifier) const
{
	this->getLogManager() << LogLevel_Debug << "Searching plugin object descriptor\n";

	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); i++)
	{
		if(i->first->getCreatedClass()==rClassIdentifier)
		{
			return i->first;
		}
	}

	this->getLogManager() << LogLevel_Debug << "Plugin object descriptor class identifier " << rClassIdentifier << " not found\n";
	return NULL;

}

namespace
{
	struct SBoxProto : public IBoxProto
	{
	public:

		SBoxProto(void)
			:m_bIsDeprecated(false)
			,m_bIsUnstable(false)
			,m_ui64InputCountHash  (0x64AC3CB54A35888CLL)
			,m_ui64OutputCountHash (0x21E0FAAFE5CAF1E1LL)
			,m_ui64SettingCountHash(0x6BDFB15B54B09F63LL)
		{
		}
		virtual uint32 addInput(const CString& sName, const CIdentifier& rTypeIdentifier)
		{
			uint64 v=rTypeIdentifier.toUInteger();
			swap_byte(v, m_ui64InputCountHash);
			swap_byte(m_ui64InputCountHash, 0x7936A0F3BD12D936LL);
			m_oHash=m_oHash.toUInteger()^v;
			return true;
		}
		virtual uint32 addOutput(const CString& sName, const CIdentifier& rTypeIdentifier)
		{
			uint64 v=rTypeIdentifier.toUInteger();
			swap_byte(v, m_ui64OutputCountHash);
			swap_byte(m_ui64OutputCountHash, 0xCBB66A5B893AA4E9LL);
			m_oHash=m_oHash.toUInteger()^v;
			return true;
		}
		virtual uint32 addSetting(const CString& sName, const CIdentifier& rTypeIdentifier, const CString& sDefaultValue)
		{
			uint64 v=rTypeIdentifier.toUInteger();
			swap_byte(v, m_ui64SettingCountHash);
			swap_byte(m_ui64SettingCountHash, 0x3C87F3AAE9F8303BLL);
			m_oHash=m_oHash.toUInteger()^v;
			return true;
		}
		virtual boolean addFlag(const EBoxFlag eBoxFlag)
		{
			switch(eBoxFlag)
			{
				case BoxFlag_CanAddInput:       m_oHash=m_oHash.toUInteger()^OpenViBE::CIdentifier(0x07507AC8, 0xEB643ACE).toUInteger(); break;
				case BoxFlag_CanModifyInput:    m_oHash=m_oHash.toUInteger()^OpenViBE::CIdentifier(0x5C985376, 0x8D74CDB8).toUInteger(); break;
				case BoxFlag_CanAddOutput:      m_oHash=m_oHash.toUInteger()^OpenViBE::CIdentifier(0x58DEA69B, 0x12411365).toUInteger(); break;
				case BoxFlag_CanModifyOutput:   m_oHash=m_oHash.toUInteger()^OpenViBE::CIdentifier(0x6E162C01, 0xAC979F22).toUInteger(); break;
				case BoxFlag_CanAddSetting:     m_oHash=m_oHash.toUInteger()^OpenViBE::CIdentifier(0xFA7A50DC, 0x2140C013).toUInteger(); break;
				case BoxFlag_CanModifySetting:  m_oHash=m_oHash.toUInteger()^OpenViBE::CIdentifier(0x624D7661, 0xD8DDEA0A).toUInteger(); break;
				case BoxFlag_IsDeprecated:      m_bIsDeprecated=true; break;
				case BoxFlag_IsUnstable:        m_bIsUnstable=true;   break;
				default:
					return false;
					break;
			}
			return true;
		}
		void swap_byte(uint64& v, const uint64 s)
		{
			uint8 t;
			uint8 V[sizeof(v)];
			uint8 S[sizeof(s)];
			System::Memory::hostToLittleEndian(v, V);
			System::Memory::hostToLittleEndian(s, S);
			for(uint32 i=0; i<sizeof(s); i+=2)
			{
				uint32 j=S[i  ]%sizeof(v);
				uint32 k=S[i+1]%sizeof(v);
				t=V[j];
				V[j]=V[k];
				V[k]=t;
			}
			System::Memory::littleEndianToHost(V, &v);
		}

		_IsDerivedFromClass_Final_(IBoxProto, OV_UndefinedIdentifier)

		CIdentifier m_oHash;
		boolean m_bIsDeprecated;
		boolean m_bIsUnstable;
		uint64 m_ui64InputCountHash;
		uint64 m_ui64OutputCountHash;
		uint64 m_ui64SettingCountHash;
	};
}

CIdentifier CPluginManager::getPluginObjectHashValue(
	const CIdentifier& rClassIdentifier) const
{
	const IPluginObjectDesc* l_pPluginObjectDesc=this->getPluginObjectDescCreating(rClassIdentifier);
	const IBoxAlgorithmDesc* l_pBoxAlgorithmDesc=dynamic_cast<const IBoxAlgorithmDesc*>(l_pPluginObjectDesc);
	if(l_pBoxAlgorithmDesc)
	{
		SBoxProto l_oBoxPrototype;
		l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxPrototype);
		return l_oBoxPrototype.m_oHash;
	}
	return OV_UndefinedIdentifier;
}

boolean CPluginManager::isPluginObjectFlaggedAsDeprecated(
	const CIdentifier& rClassIdentifier) const
{
	const IPluginObjectDesc* l_pPluginObjectDesc=this->getPluginObjectDescCreating(rClassIdentifier);
	const IBoxAlgorithmDesc* l_pBoxAlgorithmDesc=dynamic_cast<const IBoxAlgorithmDesc*>(l_pPluginObjectDesc);
	if(l_pBoxAlgorithmDesc)
	{
		SBoxProto l_oBoxPrototype;
		l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxPrototype);
		return l_oBoxPrototype.m_bIsDeprecated;
	}
	return false;
}

boolean CPluginManager::isPluginObjectFlaggedAsUnstable(
	const CIdentifier& rClassIdentifier) const
{
	const IPluginObjectDesc* l_pPluginObjectDesc=this->getPluginObjectDescCreating(rClassIdentifier);
	const IBoxAlgorithmDesc* l_pBoxAlgorithmDesc=dynamic_cast<const IBoxAlgorithmDesc*>(l_pPluginObjectDesc);
	if(l_pBoxAlgorithmDesc)
	{
		SBoxProto l_oBoxPrototype;
		l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxPrototype);
		return l_oBoxPrototype.m_bIsUnstable;
	}
	return false;
}

IPluginObject* CPluginManager::createPluginObject(
	const CIdentifier& rClassIdentifier)
{
	return createPluginObjectT<IPluginObject, IPluginObjectDesc>(rClassIdentifier, NULL);
}

boolean CPluginManager::releasePluginObject(
	IPluginObject* pPluginObject)
{
	this->getLogManager() << LogLevel_Debug << "Releasing plugin object\n";

	map < IPluginObjectDesc*, vector < IPluginObject* > >::iterator i;
	vector < IPluginObject* >::iterator j;
	for(i=m_vPluginObject.begin(); i!=m_vPluginObject.end(); i++)
	{
		for(j=i->second.begin(); j!=i->second.end(); j++)
		{
			if((*j)==pPluginObject)
			{
				i->second.erase(j);
				pPluginObject->release();
				return true;
			}
		}
	}

	this->getLogManager() << LogLevel_Warning << "Plugin object has not been created by this plugin manager (class id was " << pPluginObject->getClassIdentifier() << ")\n";
	return false;
}


IAlgorithm* CPluginManager::createAlgorithm(
	const CIdentifier& rClassIdentifier,
	const IAlgorithmDesc** ppAlgorithmDesc)
{
	return createPluginObjectT<IAlgorithm, IAlgorithmDesc>(rClassIdentifier, ppAlgorithmDesc);
}

IBoxAlgorithm* CPluginManager::createBoxAlgorithm(
	const CIdentifier& rClassIdentifier,
	const IBoxAlgorithmDesc** ppBoxAlgorithmDesc)
{
	return createPluginObjectT<IBoxAlgorithm, IBoxAlgorithmDesc>(rClassIdentifier, ppBoxAlgorithmDesc);
}

template <class IPluginObjectT, class IPluginObjectDescT>
IPluginObjectT* CPluginManager::createPluginObjectT(
	const CIdentifier& rClassIdentifier,
	const IPluginObjectDescT** ppPluginObjectDescT)
{
	if(ppPluginObjectDescT)
	{
		*ppPluginObjectDescT=NULL;
	}

	CIdentifier l_oSubstitutionTokenIdentifier;
	char l_sSubstitutionTokenName[1024];
	uint64 l_ui64SourceClassIdentifier=rClassIdentifier.toUInteger();
	uint64 l_ui64TargetClassIdentifier=l_ui64SourceClassIdentifier;
	::sprintf(l_sSubstitutionTokenName, "Kernel_PluginSubstitution_%0llx", l_ui64SourceClassIdentifier);
	if((l_oSubstitutionTokenIdentifier=this->getConfigurationManager().lookUpConfigurationTokenIdentifier(l_sSubstitutionTokenName))!=OV_UndefinedIdentifier)
	{
		CString l_sSubstitutionTokenValue;
		l_sSubstitutionTokenValue=this->getConfigurationManager().getConfigurationTokenValue(l_oSubstitutionTokenIdentifier);
		l_sSubstitutionTokenValue=this->getConfigurationManager().expand(l_sSubstitutionTokenValue);
		::sscanf(l_sSubstitutionTokenValue.toASCIIString(), "%llx", &l_ui64TargetClassIdentifier);
	}
	if(l_ui64TargetClassIdentifier!=l_ui64SourceClassIdentifier)
	{
		this->getLogManager() << LogLevel_Trace << "Substituting plugin class identifier " << CIdentifier(l_ui64SourceClassIdentifier) << " with new class identifier " << CIdentifier(l_ui64TargetClassIdentifier) << "\n";
	}
	else
	{
		this->getLogManager() << LogLevel_Debug << "Not substitute plugin found for class identifier " << CIdentifier(l_ui64SourceClassIdentifier) << " (configuration token name was " << CString(l_sSubstitutionTokenName) << ")\n";
	}

	IPluginObjectDesc* l_pPluginObjectDesc=NULL;
	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); i++)
	{
		if(i->first->getCreatedClass()==CIdentifier(l_ui64TargetClassIdentifier))
		{
			l_pPluginObjectDesc=i->first;
		}
	}
	if(!l_pPluginObjectDesc)
	{
		return NULL;
	}

	IPluginObject* l_pPluginObject=l_pPluginObjectDesc->create();
	if(!l_pPluginObject)
	{
		return NULL;
	}

	IPluginObjectDescT* l_pPluginObjectDescT=dynamic_cast<IPluginObjectDescT*>(l_pPluginObjectDesc);
	IPluginObjectT* l_pPluginObjectT=dynamic_cast<IPluginObjectT*>(l_pPluginObject);
	if(!l_pPluginObjectDescT || !l_pPluginObjectT)
	{
		l_pPluginObject->release();
		return NULL;
	}

	if(ppPluginObjectDescT)
	{
		*ppPluginObjectDescT=l_pPluginObjectDescT;
	}

	m_vPluginObject[l_pPluginObjectDescT].push_back(l_pPluginObjectT);
	return l_pPluginObjectT;
}
