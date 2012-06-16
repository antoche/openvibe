#include "ovkCKernelContext.h"
#include "ovkCKernelObjectFactory.h"
#include "ovkCTypeManager.h"

#include "algorithm/ovkCAlgorithmManager.h"
#include "player/ovkCPlayerManager.h"
#include "plugins/ovkCPluginManager.h"
#include "scenario/ovkCScenarioManager.h"
#include "log/ovkCLogManager.h"
#include "visualisation/ovkCVisualisationManager.h"

#include <string>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

CKernelContext::CKernelContext(IKernel& rKernel)
	:m_rKernel(rKernel)
	,m_pAlgorithmManager(NULL)
	,m_pKernelObjectFactory(NULL)
	,m_pPlayerManager(NULL)
	,m_pPluginManager(NULL)
	,m_pScenarioManager(NULL)
	,m_pTypeManager(NULL)
	,m_pLogManager(NULL)
	,m_pVisualisationManager(NULL)
{
	m_pKernelObjectFactory=new CKernelObjectFactory(*this);

	m_pLogManager=new CLogManager(*this);
	m_pLogManager->addListener(dynamic_cast<ILogListener*>(m_pKernelObjectFactory->createObject(OVK_ClassId_Kernel_Log_LogListenerConsole)));
	m_pLogManager->addListener(dynamic_cast<ILogListener*>(m_pKernelObjectFactory->createObject(OVK_ClassId_Kernel_Log_LogListenerFile)));
	(*m_pLogManager) << LogLevel_Debug << "Added Console Log Listener - should be removed\n";
	(*m_pLogManager) << LogLevel_Debug << "Added File Log Listener - should be removed\n";

	m_pAlgorithmManager=new CAlgorithmManager(*this);

	m_pPlayerManager=new CPlayerManager(*this);

	m_pTypeManager=new CTypeManager(*this);

	m_pTypeManager->registerType(OV_TypeId_Boolean, "boolean");
	m_pTypeManager->registerType(OV_TypeId_Integer, "integer");
	m_pTypeManager->registerType(OV_TypeId_Float, "float");
	m_pTypeManager->registerType(OV_TypeId_String, "string");
	m_pTypeManager->registerType(OV_TypeId_Filename, "filename");

	m_pTypeManager->registerEnumerationType(OV_TypeId_Stimulation, "stimulation");

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
	m_pTypeManager->registerStreamType(  OV_TypeId_ChannelLocalisation, "channel localisation", OV_TypeId_EBMLStream);
	m_pTypeManager->registerStreamType(  OV_TypeId_ExperimentationInformation, "experiment information", OV_TypeId_EBMLStream);
	m_pTypeManager->registerStreamType(  OV_TypeId_Stimulations, "stimulations", OV_TypeId_EBMLStream);
	m_pTypeManager->registerStreamType(  OV_TypeId_StreamedMatrix, "streamed matrix", OV_TypeId_EBMLStream);
	m_pTypeManager->registerStreamType(    OV_TypeId_FeatureVector, "feature vector", OV_TypeId_StreamedMatrix);
	m_pTypeManager->registerStreamType(    OV_TypeId_Signal, "signal", OV_TypeId_StreamedMatrix);
	m_pTypeManager->registerStreamType(    OV_TypeId_Spectrum, "spectrum", OV_TypeId_StreamedMatrix);

	m_pScenarioManager=new CScenarioManager(*this);

	m_pVisualisationManager=new CVisualisationManager(*this);

	m_pPluginManager=new CPluginManager(*this);
}

CKernelContext::~CKernelContext(void)
{
	delete m_pPluginManager;
	delete m_pVisualisationManager;
	delete m_pScenarioManager;
	delete m_pTypeManager;
	delete m_pPlayerManager;
	delete m_pAlgorithmManager;
	delete m_pLogManager;
	delete m_pKernelObjectFactory;
}

IAlgorithmManager& CKernelContext::getAlgorithmManager(void) const
{
	return *m_pAlgorithmManager;
}

IKernelObjectFactory& CKernelContext::getKernelObjectFactory(void) const
{
	return *m_pKernelObjectFactory;
}

IPlayerManager& CKernelContext::getPlayerManager(void) const
{
	return *m_pPlayerManager;
}

IPluginManager& CKernelContext::getPluginManager(void) const
{
	return *m_pPluginManager;
}

IScenarioManager& CKernelContext::getScenarioManager(void) const
{
	return *m_pScenarioManager;
}

ITypeManager& CKernelContext::getTypeManager(void) const
{
	return *m_pTypeManager;
}

ILogManager& CKernelContext::getLogManager(void) const
{
	return *m_pLogManager;
}

IVisualisationManager& CKernelContext::getVisualisationManager(void) const
{
	return *m_pVisualisationManager;
}
