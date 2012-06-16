#include "ovkCPluginModule.h"

#include "../../tools/ovk_bridge_bind_function.h"

#include <map>
#include <vector>

#if defined OVK_OS_Linux
	#include <dlfcn.h>
#elif defined OVK_OS_Windows
	#include <windows.h>
#else
#endif

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModuleBase : public TKernelObject<IPluginModule>
		{
		public:
			CPluginModuleBase(const IKernelContext& rKernelContext);

			virtual boolean initialize(void);
			virtual boolean getPluginObjectDescription(
				uint32 ui32Index,
				IPluginObjectDesc*& rpPluginObjectDescription);
			virtual boolean uninitialize(void);
			virtual boolean getFileName(CString& rFileName) const;

			_IsDerivedFromClass_Final_(IPluginModule, OV_UndefinedIdentifier);

		protected:

			virtual boolean isOpen(void) const=0;

			vector<IPluginObjectDesc*> m_vPluginObjectDescriptor;
			CString m_sFileName;
			boolean m_bGotDescriptions;

			boolean (*onInitializeCB)(const IPluginModuleContext&);
			boolean (*onGetPluginObjectDescriptionCB)(const IPluginModuleContext&, uint32, Plugins::IPluginObjectDesc*&);
			boolean (*onUninitializeCB)(const IPluginModuleContext&);
		};
	};
};

namespace OpenViBE
{
	namespace Kernel
	{
		namespace
		{
			class CLogManagerBridge : public TKernelObject<ILogManager>
			{
			public:

				CLogManagerBridge(const IKernelContext& rKernelContext) : TKernelObject<ILogManager>(rKernelContext) { }

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const time64, time64Value)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const uint64, ui64Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const uint32, ui32Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const uint16, ui16Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const uint8, ui8Value)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const int64, i64Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const int32, i32Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const int16, i16Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const int8, i8Value)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const float32, f32Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const float64, f64Value)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const boolean, bValue)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const CIdentifier&, rValue)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const CString&, rValue);
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const char*, rValue);

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const ELogLevel, eLogLevel)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const ELogColor, eLogColor)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), boolean, addListener, , ILogListener*, pListener)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), boolean, removeListener, , ILogListener*, pListener)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), boolean, isActive, , ELogLevel, eLogLevel)
				__BridgeBindFunc2__(getKernelContext().getLogManager(), boolean, activate, , ELogLevel, eLogLevel, boolean, bActive)
				__BridgeBindFunc3__(getKernelContext().getLogManager(), boolean, activate, , ELogLevel, eStartLogLevel, ELogLevel, eEndLogLevel, boolean, bActive)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), boolean, activate, , boolean, bActive)

				_IsDerivedFromClass_Final_(TKernelObject<ILogManager>, OV_UndefinedIdentifier);
			};

			class CTypeManagerBridge : public TKernelObject<ITypeManager>
			{
			public:

				CTypeManagerBridge(const IKernelContext& rKernelContext) : TKernelObject<ITypeManager>(rKernelContext) { }

				__BridgeBindFunc1__(getKernelContext().getTypeManager(), CIdentifier, getNextTypeIdentifier, const, const CIdentifier&, rPreviousIdentifier);
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), boolean, registerType, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName)
				__BridgeBindFunc3__(getKernelContext().getTypeManager(), boolean, registerStreamType, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName, const CIdentifier&, rParentTypeIdentifier)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), boolean, registerEnumerationType, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName)
				__BridgeBindFunc3__(getKernelContext().getTypeManager(), boolean, registerEnumerationEntry, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName, const uint64, ui64ValueIndex)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), boolean, registerBitMaskType, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName)
				__BridgeBindFunc3__(getKernelContext().getTypeManager(), boolean, registerBitMaskEntry, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName, const uint64, ui64ValueIndex)

				__BridgeBindFunc1__(getKernelContext().getTypeManager(), boolean, isRegistered, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc1__(getKernelContext().getTypeManager(), boolean, isStream, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), boolean, isDerivedFromStream, const, const CIdentifier&, rTypeIdentifier, const CIdentifier&, rParentTypeIdentifier)
				__BridgeBindFunc1__(getKernelContext().getTypeManager(), boolean, isEnumeration, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc1__(getKernelContext().getTypeManager(), boolean, isBitMask, const, const CIdentifier&, rTypeIdentifier)

				__BridgeBindFunc1__(getKernelContext().getTypeManager(), CString, getTypeName, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc1__(getKernelContext().getTypeManager(), CIdentifier, getStreamParentType, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc1__(getKernelContext().getTypeManager(), uint64, getEnumerationEntryCount, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc4__(getKernelContext().getTypeManager(), boolean, getEnumerationEntry, const, const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryIndex, CString&, sEntryName, uint64&, ui64EntryValue)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), CString, getEnumerationEntryNameFromValue, const, const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryValue)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), uint64, getEnumerationEntryValueFromName, const, const CIdentifier&, rTypeIdentifier, const CString&, sEntryName)
				__BridgeBindFunc1__(getKernelContext().getTypeManager(), uint64, getBitMaskEntryCount, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc4__(getKernelContext().getTypeManager(), boolean, getBitMaskEntry, const, const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryIndex, CString&, sEntryName, uint64&, ui64EntryValue)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), CString, getBitMaskEntryNameFromValue, const, const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryValue)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), uint64, getBitMaskEntryValueFromName, const, const CIdentifier&, rTypeIdentifier, const CString&, sEntryName)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), CString, getBitMaskEntryCompositionNameFromValue, const, const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryCompositionValue)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), uint64, getBitMaskEntryCompositionValueFromName, const, const CIdentifier&, rTypeIdentifier, const CString&, sEntryCompositionName)

				_IsDerivedFromClass_Final_(TKernelObject<ITypeManager>, OV_UndefinedIdentifier);
			};

			class CScenarioManagerBridge : public TKernelObject<IScenarioManager>
			{
			public:

				CScenarioManagerBridge(const IKernelContext& rKernelContext) : TKernelObject<IScenarioManager>(rKernelContext) { }

				__BridgeBindFunc1__(getKernelContext().getScenarioManager(), CIdentifier, getNextScenarioIdentifier, const, const CIdentifier&, rPreviousIdentifier)
				__BridgeBindFunc1__(getKernelContext().getScenarioManager(), boolean, createScenario, , CIdentifier&, rScenarioIdentifier)
				__BridgeBindFunc1__(getKernelContext().getScenarioManager(), boolean, releaseScenario, , const CIdentifier&, rScenarioIdentifier)
				__BridgeBindFunc1__(getKernelContext().getScenarioManager(), IScenario&, getScenario, , const CIdentifier&, rScenarioIdentifier)

				_IsDerivedFromClass_Final_(TKernelObject<IScenarioManager>, OV_UndefinedIdentifier);
			};

			class CPluginModuleContext : public TKernelObject<IPluginModuleContext>
			{
			public:

				CPluginModuleContext(const IKernelContext& rKernelContext)
					:TKernelObject<IPluginModuleContext>(rKernelContext)
					,m_oLogManagerBridge(rKernelContext)
					,m_oTypeManagerBridge(rKernelContext)
					,m_oScenarioManagerBridge(rKernelContext)
				{
				}

				virtual ILogManager& getLogManager(void) const { return m_oLogManagerBridge; };
				virtual ITypeManager& getTypeManager(void) const { return m_oTypeManagerBridge; }
				virtual IScenarioManager& getScenarioManager(void) const { return m_oScenarioManagerBridge; }

				_IsDerivedFromClass_Final_(TKernelObject<IPluginModuleContext>, OVK_ClassId_Kernel_Plugins_PluginModuleContext);

			protected:

				mutable CLogManagerBridge m_oLogManagerBridge;
				mutable CTypeManagerBridge m_oTypeManagerBridge;
				mutable CScenarioManagerBridge m_oScenarioManagerBridge;
			};
		};
	};
};

#define boolean OpenViBE::boolean

//___________________________________________________________________//
//                                                                   //

CPluginModuleBase::CPluginModuleBase(const IKernelContext& rKernelContext)
	:TKernelObject<IPluginModule>(rKernelContext)
	,m_bGotDescriptions(false)
	,onInitializeCB(NULL)
	,onGetPluginObjectDescriptionCB(NULL)
	,onUninitializeCB(NULL)
{
}

boolean CPluginModuleBase::initialize(void)
{
	if(!isOpen())
	{
		return false;
	}
	if(!onInitializeCB)
	{
		return true;
	}
	return onInitializeCB(CPluginModuleContext(getKernelContext()));
}

boolean CPluginModuleBase::getPluginObjectDescription(
	uint32 ui32Index,
	IPluginObjectDesc*& rpPluginObjectDescription)
{
	if(!m_bGotDescriptions)
	{
		if(!isOpen())
		{
			return false;
		}
		if(!onGetPluginObjectDescriptionCB)
		{
			return false;
		}

		uint32 l_ui32Index=0;
		IPluginObjectDesc* l_pPluginObjectDescriptor=NULL;
		while(onGetPluginObjectDescriptionCB(CPluginModuleContext(getKernelContext()), l_ui32Index, l_pPluginObjectDescriptor))
		{
			if(l_pPluginObjectDescriptor)
			{
				m_vPluginObjectDescriptor.push_back(l_pPluginObjectDescriptor);
			}
			l_ui32Index++;
		}

		m_bGotDescriptions=true;
	}

	if(ui32Index>=m_vPluginObjectDescriptor.size())
	{
		rpPluginObjectDescription=NULL;
		return false;
	}

	rpPluginObjectDescription=m_vPluginObjectDescriptor[ui32Index];
	return true;
}

boolean CPluginModuleBase::uninitialize(void)
{
	if(!isOpen())
	{
		return false;
	}
	if(!onUninitializeCB)
	{
		return true;
	}
	return onUninitializeCB(CPluginModuleContext(getKernelContext()));
}

boolean CPluginModuleBase::getFileName(
	CString& rFileName) const
{
	if(!isOpen())
	{
		return false;
	}
	rFileName=m_sFileName;
	return true;
}

//___________________________________________________________________//
//                                                                   //

#if defined OVK_OS_Linux

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModuleLinux : public CPluginModuleBase
		{
		public:

			CPluginModuleLinux(const IKernelContext& rKernelContext);

			virtual boolean load(
				const CString& sFileName,
				CString* pError);
			virtual boolean unload(
				CString* pError);
			virtual boolean isOpen(void) const;

		protected:

			void* m_pFileHandle;
		};
	};
};

#elif defined OVK_OS_Windows

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModuleWindows : public CPluginModuleBase
		{
		public:

			CPluginModuleWindows(const IKernelContext& rKernelContext);

			virtual boolean load(
				const CString& sFileName,
				CString* pError);
			virtual boolean unload(
				CString* pError);

		protected:

			virtual boolean isOpen(void) const;

			HMODULE m_pFileHandle;

		private:

			CString getLastErrorMessageString(void);
		};
	};
};

#else

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModuleDummy : public CPluginModuleBase
		{
		public:

			CPluginModuleDummy(const IKernelContext& rKernelContext);

			virtual boolean load(
				const CString& sFileName,
				CString* pError);
			virtual boolean unload(
				CString* pError);

		protected:

			virtual boolean isOpen(void) const;
		};
	};
};

#endif

//___________________________________________________________________//
//                                                                   //

#if defined OVK_OS_Linux

CPluginModuleLinux::CPluginModuleLinux(const IKernelContext& rKernelContext)
	:CPluginModuleBase(rKernelContext)
	,m_pFileHandle(NULL)
{
}

boolean CPluginModuleLinux::load(
	const CString& sFileName,
	CString* pError)
{
	if(m_pFileHandle)
	{
		if(pError) *pError="plugin module already loaded";
		return false;
	}

	// m_pFileHandle=dlopen(sFileName, RTLD_NOW|RTLD_LOCAL);
	m_pFileHandle=dlopen(sFileName, RTLD_LAZY|RTLD_GLOBAL);
	if(!m_pFileHandle)
	{
		if(pError) *pError=dlerror();
		return false;
	}

	onInitializeCB=(boolean (*)(const IPluginModuleContext&))dlsym(m_pFileHandle, "onInitialize");
	onUninitializeCB=(boolean (*)(const IPluginModuleContext&))dlsym(m_pFileHandle, "onUninitialize");
	onGetPluginObjectDescriptionCB=(boolean (*)(const IPluginModuleContext&, uint32, Plugins::IPluginObjectDesc*&))dlsym(m_pFileHandle, "onGetPluginObjectDescription");

	if(!onGetPluginObjectDescriptionCB)
	{
		if(pError) *pError=dlerror();

		dlclose(m_pFileHandle);
		m_pFileHandle=NULL;
		onInitializeCB=NULL;
		onGetPluginObjectDescriptionCB=NULL;
		onUninitializeCB=NULL;
		return false;
	}

	m_sFileName=sFileName;
	return true;
}

boolean CPluginModuleLinux::unload(
	CString* pError)
{
	if(!m_pFileHandle)
	{
		if(pError) *pError="no plugin module currently loaded";
		return false;
	}

	dlclose(m_pFileHandle);
	m_pFileHandle=NULL;
	onInitializeCB=NULL;
	onGetPluginObjectDescriptionCB=NULL;
	onUninitializeCB=NULL;
	return true;
}

boolean CPluginModuleLinux::isOpen(void) const
{
	return m_pFileHandle!=NULL;
}

#elif defined OVK_OS_Windows

CPluginModuleWindows::CPluginModuleWindows(const IKernelContext& rKernelContext)
	:CPluginModuleBase(rKernelContext)
	,m_pFileHandle(NULL)
{
}

boolean CPluginModuleWindows::load(
	const CString& sFileName,
	CString* pError)
{
	if(m_pFileHandle)
	{
		if(pError) *pError="plugin module already loaded";
		return false;
	}

	m_pFileHandle=LoadLibrary(sFileName);
	if(!m_pFileHandle)
	{
		if(pError)
		{
			*pError=this->getLastErrorMessageString();
		}
		return false;
	}

	onInitializeCB=(boolean (*)(const IPluginModuleContext&))GetProcAddress(m_pFileHandle, "onInitialize");
	onUninitializeCB=(boolean (*)(const IPluginModuleContext&))GetProcAddress(m_pFileHandle, "onUninitialize");
	onGetPluginObjectDescriptionCB=(boolean (*)(const IPluginModuleContext&, uint32, Plugins::IPluginObjectDesc*&))GetProcAddress(m_pFileHandle, "onGetPluginObjectDescription");
	if(!onGetPluginObjectDescriptionCB)
	{
		if(pError)
		{
			*pError=this->getLastErrorMessageString();
		}

		FreeLibrary(m_pFileHandle);
		m_pFileHandle=NULL;
		onInitializeCB=NULL;
		onGetPluginObjectDescriptionCB=NULL;
		onUninitializeCB=NULL;
		return false;
	}

	m_sFileName=sFileName;
	return true;
}

boolean CPluginModuleWindows::unload(
	CString* pError)
{
	if(!m_pFileHandle)
	{
		if(pError) *pError="no plugin module currently loaded";
		return false;
	}

	FreeLibrary(m_pFileHandle);
	m_pFileHandle=NULL;
	onInitializeCB=NULL;
	onGetPluginObjectDescriptionCB=NULL;
	onUninitializeCB=NULL;
	return true;
}

boolean CPluginModuleWindows::isOpen(void) const
{
	return m_pFileHandle!=NULL;
}

CString CPluginModuleWindows::getLastErrorMessageString(void)
{
	CString l_sResult;

	char* l_pMessageBuffer=NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM|
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		0, // Default language
		(LPTSTR)&l_pMessageBuffer,
		0,
		NULL);
	if(l_pMessageBuffer)
	{
		int l_iMessageLength=::strlen(l_pMessageBuffer);
		for(int i=0; i<l_iMessageLength; i++)
		{
			if(l_pMessageBuffer[i]=='\n' || l_pMessageBuffer[i]=='\r')
			{
				l_pMessageBuffer[i]=' ';
			}
		}
		l_sResult=l_pMessageBuffer;
	}
	LocalFree((LPVOID)l_pMessageBuffer);

	return l_sResult;
}

#else

#endif

//___________________________________________________________________//
//                                                                   //

CPluginModule::CPluginModule(const IKernelContext& rKernelContext)
	:TKernelObject<IPluginModule>(rKernelContext)
	,m_pImplementation(NULL)
{
#if defined OVK_OS_Linux
	m_pImplementation=new CPluginModuleLinux(getKernelContext());
#elif defined OVK_OS_Windows
	m_pImplementation=new CPluginModuleWindows(getKernelContext());
#else
#endif
}

CPluginModule::~CPluginModule(void)
{
	delete m_pImplementation;
}

boolean CPluginModule::load(
	const CString& sFileName,
	CString* pError)
{
	return !m_pImplementation?false:m_pImplementation->load(sFileName, pError);
}

boolean CPluginModule::unload(
	CString* pError)
{
	return !m_pImplementation?false:m_pImplementation->unload(pError);
}

boolean CPluginModule::initialize(void)
{
	return !m_pImplementation?false:m_pImplementation->initialize();
}

boolean CPluginModule::getPluginObjectDescription(
	uint32 ui32Index,
	IPluginObjectDesc*& rpPluginObjectDescription)
{
	return !m_pImplementation?false:m_pImplementation->getPluginObjectDescription(ui32Index, rpPluginObjectDescription);
}

boolean CPluginModule::uninitialize(void)
{
	return !m_pImplementation?false:m_pImplementation->uninitialize();
}

boolean CPluginModule::getFileName(
	CString& rFileName) const
{
	return !m_pImplementation?false:m_pImplementation->getFileName(rFileName);
}
