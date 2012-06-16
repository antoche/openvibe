#include "ovkCKernelContext.h"
#include "ovkCKernelObjectFactory.h"
#include "ovkCTypeManager.h"

#include "algorithm/ovkCAlgorithmManager.h"
#include "configuration/ovkCConfigurationManager.h"
#include "player/ovkCPlayerManager.h"
#include "plugins/ovkCPluginManager.h"
#include "scenario/ovkCScenarioManager.h"
#include "log/ovkCLogManager.h"
#include "log/ovkCLogListenerConsole.h"
#include "log/ovkCLogListenerFile.h"
#include "visualisation/ovkCVisualisationManager.h"

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

namespace
{
	// because std::tolower has multiple signatures,
	// it can not be easily used in std::transform
	// this workaround is taken from http://www.gcek.net/ref/books/sw/cpp/ticppv2/
	template <class charT>
	charT to_lower(charT c)
	{
		return std::tolower(c);
	}

};

#if defined OVK_OS_Linux

	#include <signal.h>

	static void _openvibeKernelSignalHandler_(int iSignal)
	{
		throw;
	}

#endif

namespace OpenViBE
{
	namespace Kernel
	{
		namespace
		{
			class CLogManagerNULL : public OpenViBE::Kernel::ILogManager
			{
			public:

				virtual OpenViBE::boolean isActive(OpenViBE::Kernel::ELogLevel eLogLevel) { return false; }
				virtual OpenViBE::boolean activate(OpenViBE::Kernel::ELogLevel eLogLevel, OpenViBE::boolean bActive) { return false; }
				virtual OpenViBE::boolean activate(OpenViBE::Kernel::ELogLevel eStartLogLevel, OpenViBE::Kernel::ELogLevel eEndLogLevel, OpenViBE::boolean bActive) { return false; }
				virtual OpenViBE::boolean activate(OpenViBE::boolean bActive) { return false; }

				virtual void log(const OpenViBE::time64 time64Value) { }

				virtual void log(const OpenViBE::uint64 ui64Value) { }
				virtual void log(const OpenViBE::uint32 ui32Value) { }
				virtual void log(const OpenViBE::uint16 ui16Value) { }
				virtual void log(const OpenViBE::uint8 ui8Value) { }

				virtual void log(const OpenViBE::int64 i64Value) { }
				virtual void log(const OpenViBE::int32 i32Value) { }
				virtual void log(const OpenViBE::int16 i16Value) { }
				virtual void log(const OpenViBE::int8 i8Value) { }

				virtual void log(const OpenViBE::float64 f64Value) { }
				virtual void log(const OpenViBE::float32 f32Value) { }

				virtual void log(const OpenViBE::boolean bValue) { }

				virtual void log(const OpenViBE::CIdentifier& rValue) { }
				virtual void log(const OpenViBE::CString& rValue) { }
				virtual void log(const char* pValue) { }

				virtual void log(const OpenViBE::Kernel::ELogLevel eLogLevel) { }
				virtual void log(const OpenViBE::Kernel::ELogColor eLogColor) { }

				virtual OpenViBE::boolean addListener(OpenViBE::Kernel::ILogListener* pListener) { return false; }
				virtual OpenViBE::boolean removeListener(OpenViBE::Kernel::ILogListener* pListener) { return false; }

				_IsDerivedFromClass_Final_(OpenViBE::Kernel::ILogManager, OV_UndefinedIdentifier);
			};
		};
	};
};

CKernelContext::CKernelContext(const IKernelContext* pMasterKernelContext, const CString& rApplicationName, const CString& rConfigurationFile)
	:m_rMasterKernelContext(pMasterKernelContext?*pMasterKernelContext:*this)
	,m_pThis(this)
	,m_pAlgorithmManager(NULL)
	,m_pConfigurationManager(NULL)
	,m_pKernelObjectFactory(NULL)
	,m_pPlayerManager(NULL)
	,m_pPluginManager(NULL)
	,m_pScenarioManager(NULL)
	,m_pTypeManager(NULL)
	,m_pLogManager(NULL)
	,m_pVisualisationManager(NULL)
	,m_bIsInitialized(false)
	,m_sApplicationName(rApplicationName)
	,m_sConfigurationFile(rConfigurationFile)
	,m_pLogListenerConsole(NULL)
	,m_pLogListenerFile(NULL)
{
#if defined OVK_OS_Linux

	signal(SIGILL,  _openvibeKernelSignalHandler_); // Illegal instruction
	signal(SIGFPE,  _openvibeKernelSignalHandler_); // Floadint point exception
	signal(SIGSEGV, _openvibeKernelSignalHandler_); // Invalid memory reference
	signal(SIGPIPE, _openvibeKernelSignalHandler_); // Broken pipe: write to pipe with no readers

	signal(SIGBUS,  _openvibeKernelSignalHandler_); // Bus error (bad memory access)
	signal(SIGSYS,  _openvibeKernelSignalHandler_); // Bad argument to routine (SVID)
	signal(SIGTRAP, _openvibeKernelSignalHandler_); // Trace/breakpoint trap

#endif
}

CKernelContext::~CKernelContext(void)
{
	if(m_bIsInitialized)
	{
		this->uninitialize();
	}
}

boolean CKernelContext::initialize(void)
{
	if(m_bIsInitialized)
	{
		return true;
	}

	m_bIsInitialized=true;

	m_pKernelObjectFactory=new CKernelObjectFactory(m_rMasterKernelContext);

	this->getLogManager() << LogLevel_Trace << "Creating log manager\n";

	m_pLogManager=new CLogManager(m_rMasterKernelContext);
	m_pLogManager->activate(true);

	this->getLogManager() << LogLevel_Trace << "Creating and configuring file log listener\n";
	m_pLogListenerFile=new CLogListenerFile(m_rMasterKernelContext, m_sApplicationName, CString("../log/openvibe-")+m_sApplicationName+CString(".log"));
	m_pLogListenerFile->activate(true);
	this->getLogManager().addListener(m_pLogListenerFile);

	this->getLogManager() << LogLevel_Trace << "Creating and configuring console log listener\n";
	m_pLogListenerConsole=new CLogListenerConsole(m_rMasterKernelContext, m_sApplicationName);
	m_pLogListenerConsole->activate(false);
	m_pLogListenerConsole->activate(LogLevel_Info, LogLevel_Last, true);
	this->getLogManager().addListener(m_pLogListenerConsole);

	this->getLogManager() << LogLevel_Trace << "Creating configuration manager\n";
	m_pConfigurationManager=new CConfigurationManager(m_rMasterKernelContext);

#if defined OVK_BUILDTYPE_Release
	m_pConfigurationManager->createConfigurationToken("BuildType",                    "Release");
#elif defined OVK_BUILDTYPE_Debug
	m_pConfigurationManager->createConfigurationToken("BuildType",                    "Debug");
#else
	m_pConfigurationManager->createConfigurationToken("BuildType",                    "Unknown");
#endif

#if defined OVK_OS_Windows
	m_pConfigurationManager->createConfigurationToken("OperatingSystem",              "Windows");
	m_pConfigurationManager->createConfigurationToken("UserHome",                     "$Environment{USERPROFILE}");
#elif defined OVK_OS_Linux
	m_pConfigurationManager->createConfigurationToken("OperatingSystem",              "Linux");
	m_pConfigurationManager->createConfigurationToken("UserHome",                     "$Environment{HOME}");
#else
	m_pConfigurationManager->createConfigurationToken("OperatingSystem",              "Unknown");
	m_pConfigurationManager->createConfigurationToken("UserHome",                     "");
#endif

	m_pConfigurationManager->createConfigurationToken("ApplicationName",              m_sApplicationName);
	m_pConfigurationManager->createConfigurationToken("Path_Root",                    "..");
	m_pConfigurationManager->createConfigurationToken("Path_Bin",                     "${Path_Root}/bin");
	m_pConfigurationManager->createConfigurationToken("Path_Lib",                     "${Path_Root}/lib");
	m_pConfigurationManager->createConfigurationToken("Path_Log",                     "${Path_Root}");
	m_pConfigurationManager->createConfigurationToken("Path_Tmp",                     "${Path_Root}/tmp");
	m_pConfigurationManager->createConfigurationToken("Path_Data",                    "${Path_Root}");
	m_pConfigurationManager->createConfigurationToken("Kernel_PluginsPatternLinux",   "libOpenViBE-plugins-*.so");
	m_pConfigurationManager->createConfigurationToken("Kernel_PluginsPatternWindows", "OpenViBE-plugins-*.dll");
	m_pConfigurationManager->createConfigurationToken("Kernel_Plugins",               "${Path_Lib}/${Kernel_PluginsPattern${OperatingSystem}}");
	m_pConfigurationManager->createConfigurationToken("Kernel_MainLogLevel",          "Debug");
	m_pConfigurationManager->createConfigurationToken("Kernel_ConsoleLogLevel",       "Information");
	m_pConfigurationManager->createConfigurationToken("Kernel_FileLogLevel",          "Debug");
	m_pConfigurationManager->createConfigurationToken("Kernel_PlayerFrequency",       "128");

	m_pConfigurationManager->addConfigurationFromFile(m_sConfigurationFile);

	ELogLevel l_eMainLogLevel   =this->earlyGetLogLevel(m_pConfigurationManager->expand("${Kernel_MainLogLevel}"));
	ELogLevel l_eConsoleLogLevel=this->earlyGetLogLevel(m_pConfigurationManager->expand("${Kernel_ConsoleLogLevel}"));
	ELogLevel l_eFileLogLevel   =this->earlyGetLogLevel(m_pConfigurationManager->expand("${Kernel_FileLogLevel}"));

	m_pLogManager->activate(false);
	m_pLogManager->activate(l_eMainLogLevel, LogLevel_Last, true);
	m_pLogListenerFile->activate(false);
	m_pLogListenerFile->activate(l_eFileLogLevel, LogLevel_Last, true);
	m_pLogListenerConsole->activate(false);
	m_pLogListenerConsole->activate(l_eConsoleLogLevel, LogLevel_Last, true);

	this->getLogManager() << LogLevel_Trace << "Creating algorithm manager\n";
	m_pAlgorithmManager=new CAlgorithmManager(m_rMasterKernelContext);

	this->getLogManager() << LogLevel_Trace << "Creating player manager\n";
	m_pPlayerManager=new CPlayerManager(m_rMasterKernelContext);

	this->getLogManager() << LogLevel_Trace << "Creating and configuring type manager\n";
	m_pTypeManager=new CTypeManager(m_rMasterKernelContext);

	m_pTypeManager->registerType(OV_TypeId_Boolean,  "Boolean");
	m_pTypeManager->registerType(OV_TypeId_Integer,  "Integer");
	m_pTypeManager->registerType(OV_TypeId_Float,    "Float");
	m_pTypeManager->registerType(OV_TypeId_String,   "String");
	m_pTypeManager->registerType(OV_TypeId_Filename, "Filename");
	m_pTypeManager->registerType(OV_TypeId_Script,   "Script");
	m_pTypeManager->registerType(OV_TypeId_Color,    "Color");
	m_pTypeManager->registerType(OV_TypeId_ColorGradient, "Color Gradient");

	m_pTypeManager->registerEnumerationType(OV_TypeId_Stimulation, "Stimulation");

	m_pTypeManager->registerEnumerationType(OV_TypeId_LogLevel, "Log level");
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "None",                     LogLevel_None);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Debug",                    LogLevel_Debug);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Benchmarking / Profiling", LogLevel_Benchmark);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Trace",                    LogLevel_Trace);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Information",              LogLevel_Info);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Warning",                  LogLevel_Warning);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Important warning",        LogLevel_ImportantWarning);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Error",                    LogLevel_Error);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Fatal error",              LogLevel_Fatal);

	m_pTypeManager->registerStreamType(OV_TypeId_EBMLStream, "EBML stream", OV_UndefinedIdentifier);
	m_pTypeManager->registerStreamType(  OV_TypeId_ExperimentationInformation, "Experiment information", OV_TypeId_EBMLStream);
	m_pTypeManager->registerStreamType(  OV_TypeId_Stimulations, "Stimulations", OV_TypeId_EBMLStream);
	m_pTypeManager->registerStreamType(  OV_TypeId_StreamedMatrix, "Streamed matrix", OV_TypeId_EBMLStream);
	m_pTypeManager->registerStreamType(    OV_TypeId_ChannelLocalisation, "Channel localisation", OV_TypeId_StreamedMatrix);
	m_pTypeManager->registerStreamType(    OV_TypeId_FeatureVector, "Feature vector", OV_TypeId_StreamedMatrix);
	m_pTypeManager->registerStreamType(    OV_TypeId_Signal, "Signal", OV_TypeId_StreamedMatrix);
	m_pTypeManager->registerStreamType(    OV_TypeId_Spectrum, "Spectrum", OV_TypeId_StreamedMatrix);

	this->getLogManager() << LogLevel_Trace << "Creating scenario manager\n";
	m_pScenarioManager=new CScenarioManager(m_rMasterKernelContext);

	this->getLogManager() << LogLevel_Trace << "Creating visualisation manager\n";
	m_pVisualisationManager=new CVisualisationManager(m_rMasterKernelContext);

	this->getLogManager() << LogLevel_Trace << "Creating plugin manager\n";
	m_pPluginManager=new CPluginManager(m_rMasterKernelContext);

	return true;
}

boolean CKernelContext::uninitialize(void)
{
	if(!m_bIsInitialized)
	{
		return true;
	}

	this->getLogManager() << LogLevel_Trace << "Releasing plugin manager\n";
	delete m_pPluginManager;
	m_pPluginManager=NULL;

	this->getLogManager() << LogLevel_Trace << "Releasing visualisation manager\n";
	delete m_pVisualisationManager;
	m_pVisualisationManager=NULL;

	this->getLogManager() << LogLevel_Trace << "Releasing scenario manager\n";
	delete m_pScenarioManager;
	m_pScenarioManager=NULL;

	this->getLogManager() << LogLevel_Trace << "Releasing type manager\n";
	delete m_pTypeManager;
	m_pTypeManager=NULL;

	this->getLogManager() << LogLevel_Trace << "Releasing player manager\n";
	delete m_pPlayerManager;
	m_pPlayerManager=NULL;

	this->getLogManager() << LogLevel_Trace << "Releasing algorithm manager\n";
	delete m_pAlgorithmManager;
	m_pAlgorithmManager=NULL;

	this->getLogManager() << LogLevel_Trace << "Releasing configuration manager\n";
	delete m_pConfigurationManager;
	m_pConfigurationManager=NULL;

	this->getLogManager() << LogLevel_Trace << "Releasing log manager - no more log possible with log manager !\n";
	delete m_pLogManager;
	m_pLogManager=NULL;

	this->getLogManager() << LogLevel_Trace << "Releasing log file console\n";
	delete m_pLogListenerConsole;
	m_pLogListenerConsole=NULL;

	this->getLogManager() << LogLevel_Trace << "Releasing log file listener\n";
	delete m_pLogListenerFile;
	m_pLogListenerFile=NULL;

	delete m_pKernelObjectFactory;
	m_pKernelObjectFactory=NULL;

	m_bIsInitialized=false;

	return true;
}

IAlgorithmManager& CKernelContext::getAlgorithmManager(void) const
{
	if(!m_bIsInitialized) m_pThis->initialize();
	return *m_pAlgorithmManager;
}

IConfigurationManager& CKernelContext::getConfigurationManager(void) const
{
	if(!m_bIsInitialized) m_pThis->initialize();
	return *m_pConfigurationManager;
}

IKernelObjectFactory& CKernelContext::getKernelObjectFactory(void) const
{
	if(!m_bIsInitialized) m_pThis->initialize();
	return *m_pKernelObjectFactory;
}

IPlayerManager& CKernelContext::getPlayerManager(void) const
{
	if(!m_bIsInitialized) m_pThis->initialize();
	return *m_pPlayerManager;
}

IPluginManager& CKernelContext::getPluginManager(void) const
{
	if(!m_bIsInitialized) m_pThis->initialize();
	return *m_pPluginManager;
}

IScenarioManager& CKernelContext::getScenarioManager(void) const
{
	if(!m_bIsInitialized) m_pThis->initialize();
	return *m_pScenarioManager;
}

ITypeManager& CKernelContext::getTypeManager(void) const
{
	if(!m_bIsInitialized) m_pThis->initialize();
	return *m_pTypeManager;
}

ILogManager& CKernelContext::getLogManager(void) const
{
	if(!m_bIsInitialized) m_pThis->initialize();
	static CLogManagerNULL l_oLogManagerNULL;
	return m_pLogManager?*m_pLogManager:l_oLogManagerNULL;
}

IVisualisationManager& CKernelContext::getVisualisationManager(void) const
{
	if(!m_bIsInitialized) m_pThis->initialize();
	return *m_pVisualisationManager;
}

ELogLevel CKernelContext::earlyGetLogLevel(const CString& rLogLevelName)
{
	if(!m_bIsInitialized) m_pThis->initialize();
	std::string l_sValue(rLogLevelName.toASCIIString());
	std::transform(l_sValue.begin(), l_sValue.end(), l_sValue.begin(), ::to_lower<std::string::value_type>);

	if(l_sValue=="none")                     return LogLevel_None;
	if(l_sValue=="debug")                    return LogLevel_Debug;
	if(l_sValue=="benchmarking / profiling") return LogLevel_Benchmark;
	if(l_sValue=="trace")                    return LogLevel_Trace;
	if(l_sValue=="information")              return LogLevel_Info;
	if(l_sValue=="warning")                  return LogLevel_Warning;
	if(l_sValue=="important warning")        return LogLevel_ImportantWarning;
	if(l_sValue=="error")                    return LogLevel_Error;
	if(l_sValue=="fatal error")              return LogLevel_Fatal;

	(*m_pLogManager) << LogLevel_Warning << "Invalid log level " << rLogLevelName << " specified in configuration file, falling back to " << CString("Debug") << "\n";

	return LogLevel_Debug;
}
