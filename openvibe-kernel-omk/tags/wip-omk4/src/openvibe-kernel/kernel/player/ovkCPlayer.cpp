#include "ovkCPlayer.h"

// #define __Distributed_Controller__
// #define __Has_XML_Simulation_File__
// #define __Show_Timings__

#include "ovkEventId.h"

#include "ovkPsTypeChunk.h"

#include "ovkPsSimulatedBox.h"
#include "../visualisation/ovkOMKSimulatedEntity.h"

#include <system/Time.h>

#include <xml/IReader.h>

#include <OMKController.h>
#include <OMKMultipleConfigurationParameter.h>
#include <OMKUniqueConfigurationParameter.h>
#include <OMKSimpleOpenMASK3Loader.h>
#include <OMKPluginsLoader.h>

//#if defined __Has_XML_Simulation_File__
// #include <OmkXml.h>
//#endif
//#if defined __Distributed_Controller__
// #include <PsPvmController.h>
//#endif

#include <string>
#include <iostream>
#include <fstream>

#define __ControllerFrequency__  200

//___________________________________________________________________//
//                                                                   //

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
#define boolean OpenViBE::boolean

//___________________________________________________________________//
//                                                                   //

#define idToName(id) (OMK::Name((const char*)id.toString()))

//___________________________________________________________________//
//                                                                   //

#if 0
class PsSimulatedBoxCreator : public ::PsSimulatedObjectCreator
{
public:
	PsSimulatedBoxCreator(const IKernelContext& rKernelContext, const IScenario& rScenario)
		:m_rKernelContext(rKernelContext)
		,m_rScenario(rScenario)
	{
	}

	virtual ::PsSimulatedObject* createSimulatedObject(
		::PsController& rControler,
		const ::PsObjectDescriptor& rObjectDescriptor) const
	{
		return new ::PsSimulatedBox(rControler, rObjectDescriptor, m_rKernelContext, m_rScenario);
	}

protected:
	const IKernelContext& m_rKernelContext;
	const IScenario& m_rScenario;
};
#endif

//___________________________________________________________________//
//                                                                   //

#define OVD_AttributeId_SettingOverrideFilename             OpenViBE::CIdentifier(0x8D21FF41, 0xDF6AFE7E)

class CBoxSettingModifierVisitor : public IObjectVisitor, public XML::IReaderCallback
{
public:

	virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount)
	{
		if(!m_bIsParsingSettingOverride)
		{
			if(string(sName)==string("OpenViBE-SettingsOverride"))
			{
				m_bIsParsingSettingOverride=true;
			}
		}
		else if(string(sName)==string("SettingValue"))
		{
			m_bIsParsingSettingValue=true;
		}
		else
		{
			m_bIsParsingSettingValue=false;
		}
	}

	virtual void processChildData(const char* sData)
	{
		if(m_bIsParsingSettingValue)
		{
			m_pObjectVisitorContext->getLogManager() << LogLevel_Debug << "Using [" << CString(sData) << "] as setting " << m_ui32SettingIndex << "...\n";
			m_pBox->setSettingValue(m_ui32SettingIndex, sData);
			m_ui32SettingIndex++;
		}
	}

	virtual void closeChild(void)
	{
		m_bIsParsingSettingValue=false;
	}

	virtual boolean processBegin(IObjectVisitorContext& rObjectVisitorContext, IBox& rBox)
	{
		m_pObjectVisitorContext=&rObjectVisitorContext;

		// checks if this box should override
		// settings from external file
		if(rBox.hasAttribute(OVD_AttributeId_SettingOverrideFilename))
		{
			CString l_sSettingOverrideFilename=rBox.getAttributeValue(OVD_AttributeId_SettingOverrideFilename);;

			// message
			rObjectVisitorContext.getLogManager() << LogLevel_Trace << "Trying to override [" << rBox.getName() << "] box settings with file [" << l_sSettingOverrideFilename << "] !\n";

			// creates XML reader
			XML::IReader* l_pReader=XML::createReader(*this);

			// adds new box settings
			m_pBox=&rBox;
			m_ui32SettingIndex=0;
			m_bIsParsingSettingValue=false;
			m_bIsParsingSettingOverride=false;

			// 1. Open settings file (binary because read would conflict with tellg for text files)
			// 2. Loop until end of file, reading it
			//    and sending what is read to the XML parser
			// 3. Close the settings file
			ifstream l_oFile(l_sSettingOverrideFilename.toASCIIString(), ios::binary);
			if(l_oFile.is_open())
			{
				char l_sBuffer[1024];
				size_t l_iBufferLen=0;
				size_t l_iFileLen;
				bool l_bStatusOk=true;
				l_oFile.seekg(0, ios::end);
				l_iFileLen=l_oFile.tellg();
				l_oFile.seekg(0, ios::beg);
				while(l_iFileLen && l_bStatusOk)
				{
					l_iBufferLen=(l_iFileLen>sizeof(l_sBuffer)?sizeof(l_sBuffer):l_iFileLen);
					l_oFile.read(l_sBuffer, l_iBufferLen);
					l_iFileLen-=l_iBufferLen;
					l_bStatusOk=l_pReader->processData(l_sBuffer, l_iBufferLen);
				}
				l_oFile.close();
			}

			// message
			rObjectVisitorContext.getLogManager() << LogLevel_Trace << "Overrided " << m_ui32SettingIndex << " settings with this configuration file...\n";

			// cleans up internal state
			m_pBox=NULL;
			m_ui32SettingIndex=0;
			m_bIsParsingSettingValue=false;
			m_bIsParsingSettingOverride=false;

			// releases XML reader
			l_pReader->release();
			l_pReader=NULL;
		}

		return true;
	}

	virtual boolean processEnd(IObjectVisitorContext& rObjectVisitorContext, IBox& rBox)
	{
		m_pObjectVisitorContext=&rObjectVisitorContext;
		return true;
	}

	IObjectVisitorContext* m_pObjectVisitorContext;
	IBox* m_pBox;
	uint32 m_ui32SettingIndex;
	boolean m_bIsParsingSettingValue;
	boolean m_bIsParsingSettingOverride;

#undef boolean
	_IsDerivedFromClass_Final_(IObjectVisitor, OV_UndefinedIdentifier);
#define boolean OpenViBE::boolean
};

//___________________________________________________________________//
//                                                                   //

CPlayer::CPlayer(const IKernelContext& rKernelContext)
	:TKernelObject<IPlayer>(rKernelContext)
	,m_pController(NULL)
	,m_pControllerHandle(NULL)
	,m_pSimulation(NULL)

	,m_ui32ControllerSteps(0)
	,m_ui32SecondsLate(0)

	,m_ui64LastSimulatedTime(0)
	,m_ui64CurrentSimulatedTime(0)
	,m_eStatus(PlayerStatus_Stop)
	,m_bIsInitialized(false)
{
}

CPlayer::~CPlayer(void)
{
	if(m_bIsInitialized)
	{
		this->uninitialize();
	}
}

//___________________________________________________________________//
//                                                                   //

extern char** g_argv;
extern int g_argc;

boolean CPlayer::setScenario(
	const CIdentifier& rScenarioIdentifier)
{
	log() << LogLevel_Trace << "Player setScenario\n";

	if(m_bIsInitialized)
	{
		log() << LogLevel_Warning << "Trying to configure an intialized player !\n";
		return false;
	}

	m_oScenarioIdentifier=rScenarioIdentifier;
	return true;
}

boolean CPlayer::initialize(void)
{
	log() << LogLevel_Trace << "Player initialize\n";

	if(m_bIsInitialized)
	{
		log() << LogLevel_Warning << "Trying to initialize an intialized player !\n";
		return false;
	}

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	CIdentifier l_oNewScenarioIdentifier=m_oScenarioIdentifier;
/*
	getScenarioManager().getScenario(m_oScenarioIdentifier).save("tmp.scenario.xml", CIdentifier(0x77075b3b, 0x3d632492)); // $$$
	getScenarioManager().createScenario(l_oNewScenarioIdentifier);
	getScenarioManager().getScenario(l_oNewScenarioIdentifier).load("tmp.scenario.xml", CIdentifier(0x440BF3AC, 0x2D960300)); // $$$
*/
	IScenario& l_rScenario=getScenarioManager().getScenario(l_oNewScenarioIdentifier);
/* */
	CBoxSettingModifierVisitor l_oBoxSettingModifierVisitor;
	l_rScenario.acceptVisitor(l_oBoxSettingModifierVisitor);
/* */

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	// PVM
	// OMK::MultipleConfigurationParameter* l_pSimulationSchedulingDescription=new OMK::MultipleConfigurationParameter();
	// OMK::MultipleConfigurationParameter* l_pSimulationSchedulingMachinesDescription=new OMK::MultipleConfigurationParameter();
	// l_pSimulationSchedulingMachinesDescription->appendSubDescriptorNamed("ProcessA", new OMK::MultipleConfigurationParameter("hiboux.irisa.fr"));
	// l_pSimulationSchedulingDescription->appendSubDescriptorNamed("Machines", l_pSimulationSchedulingMachinesDescription);
	// l_pSimulationSchedulingDescription->appendSubDescriptorNamed("Latency", new OMK::UniqueConfigurationParameter("10"));

#if 1
	OMK::SimpleOpenMASK3Loader* l_pLoader = new OMK::SimpleOpenMASK3Loader("../share/openvibe-kernel-omk/cfgvis.OpenMASK3");

	m_pSimulation = l_pLoader->getRootObjectDescriptor();
#else
	m_pSimulation=new OMK::ObjectDescriptor("root", "Controller", (OMK::MultipleConfigurationParameter*)(NULL) /* l_pSimulationSchedulingDescription */, NULL);
#endif

	CIdentifier l_oBoxIdentifier=l_rScenario.getNextBoxIdentifier(OV_UndefinedIdentifier);
	while(l_oBoxIdentifier!=OV_UndefinedIdentifier)
	{
		// TODO choose a valid object frequency
		log() << LogLevel_Debug << "Player callback - TODO choose a valid object frequency\n";

		OMK::MultipleConfigurationParameter* l_pSimulatedBoxConfiguration=new OMK::MultipleConfigurationParameter();
		OMK::ObjectDescriptor* l_pSimulationBox=new OMK::ObjectDescriptor(idToName(l_oBoxIdentifier), "OpenViBEKernelSimulatedBox", "ProcessA", __ControllerFrequency__, l_pSimulatedBoxConfiguration);
		m_pSimulation->addSon(l_pSimulationBox);

		l_oBoxIdentifier=l_rScenario.getNextBoxIdentifier(l_oBoxIdentifier);
	}

//#if defined __Has_XML_Simulation_File__
//	::omk::xml::save("/tmp/OpenViBE-log-[dumpedconfig.OpenMASK3].log", m_pSimulation);
//#endif

	std::stringstream l_sStringStream;
	l_sStringStream << *m_pSimulation;
	log() << LogLevel_Debug << "OpenMASK 3 Scenario :\n" << CString(l_sStringStream.str().c_str()) << "\n";

#ifdef __Distributed_Controller__
	m_pController=new OMK::PvmController(*m_pSimulation, 0, g_argc, g_argv);
#else
	m_pController=new OMK::Controller(*m_pSimulation, 0);
	// OMK::PluginsLoader::getInstance().init(*m_pController) ;
#endif

#if 0
	m_pController->addInstanceCreator("PsSimulatedBox", new OMK::SimulatedBoxCreator(getKernelContext(), l_rScenario));
#else
	CSimulatedBoxCreator::ms_pKernelContext=&getKernelContext(); // this code sucks
	CSimulatedBoxCreator::ms_pScenario=&l_rScenario; // this code sucks
#endif

	// cerr << "[  INF  ] Initializing player : "; // this is to introduce cerr'ed string from OpenMASK

	m_pController->init();
	m_pControllerHandle=dynamic_cast< OMK::ReferenceObjectHandle*>(m_pController->getObjectHandle());

	m_ui32ControllerSteps=0;
	m_ui32SecondsLate=0;

	m_ui64LastSimulatedTime=0;
	m_ui64CurrentSimulatedTime=0;
	m_eStatus=PlayerStatus_Stop;
	m_bIsInitialized=true;

	m_oBenchmarkChrono.reset(__ControllerFrequency__);
	return true;

}

boolean CPlayer::uninitialize(void)
{
	log() << LogLevel_Trace << "Player uninitialize\n";

	if(!m_bIsInitialized)
	{
		log() << LogLevel_Warning << "Trying to uninitialize an unintialized player !\n";
		return false;
	}

	if(m_pController)
	{
// $$$ TODO
#if 0
		m_pController->finish();
		m_pController->runControllersStep(m_pControllerHandle);

		delete m_pController;
#endif
		m_pController=NULL;
		m_pControllerHandle=NULL;
	}

	if(m_pSimulation)
	{
// $$$ TODO
/*
		delete m_pSimulation;
*/
		m_pSimulation=NULL;
	}

	m_bIsInitialized=false;
	return true;
}

boolean CPlayer::stop(void)
{
	log() << LogLevel_Trace << "Player stop\n";

	if(m_bIsInitialized)
	{
		this->uninitialize();
	}
	m_eStatus=PlayerStatus_Stop;

	return true;
}

boolean CPlayer::pause(void)
{
	log() << LogLevel_Trace << "Player pause\n";

	if(!m_bIsInitialized)
	{
		this->initialize();
	}
	m_eStatus=PlayerStatus_Pause;

	return true;
}

boolean CPlayer::step(void)
{
	log() << LogLevel_Trace << "Player step\n";

	if(!m_bIsInitialized)
	{
		this->initialize();
	}
	m_eStatus=PlayerStatus_Step;

	return true;
}

boolean CPlayer::play(void)
{
	log() << LogLevel_Trace << "Player play\n";

	if(!m_bIsInitialized)
	{
		this->initialize();
	}
	m_eStatus=PlayerStatus_Play;

	return true;
}

boolean CPlayer::forward(void)
{
	log() << LogLevel_Trace << "Player forward\n";

	if(!m_bIsInitialized)
	{
		this->initialize();
	}
	m_eStatus=PlayerStatus_Forward;

	return true;
}

EPlayerStatus CPlayer::getStatus(void) const
{
	return m_eStatus;
}

boolean CPlayer::loop(
	const uint64 ui64ElapsedTime)
{
	if(!m_bIsInitialized)
	{
		return false;
	}

	switch(m_eStatus)
	{
		// Calls a single controller loop and goes back to pause state
		case PlayerStatus_Step:
			m_ui64LastSimulatedTime=m_ui64CurrentSimulatedTime;
			m_ui64CurrentSimulatedTime+=(1LL<<32)/__ControllerFrequency__;
			m_eStatus=PlayerStatus_Pause;
			break;

		// Calls 16 controller loops
		case PlayerStatus_Forward:
			m_ui64LastSimulatedTime=m_ui64CurrentSimulatedTime;
			m_ui64CurrentSimulatedTime+=(16LL<<32)/__ControllerFrequency__;
			break;

		// Simply updates time according to delta time
		case PlayerStatus_Play:
			m_ui64LastSimulatedTime=m_ui64CurrentSimulatedTime;
			m_ui64CurrentSimulatedTime+=ui64ElapsedTime;
			break;

		default:
			return true;
			break;
	}

	uint32 l_ui32CurrentTime=((m_ui64CurrentSimulatedTime>>10)*1000)>>22;
	// TODO check how this loop should behave in case of realtime not reachable
	while(l_ui32CurrentTime>m_ui32ControllerSteps*(1000.0/__ControllerFrequency__))
	{
		// log() << LogLevel_ImportantWarning << "Step at time : " << l_ui32CurrentTime << "\n";

		m_oBenchmarkChrono.stepIn();
		m_pController->runControllersStep(m_pControllerHandle);
		m_oBenchmarkChrono.stepOut();

		m_ui32ControllerSteps++;
		if((m_ui32ControllerSteps%__ControllerFrequency__)==0)
		{
			log() << LogLevel_Debug
				<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
				<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "elapsed time" << LogColor_PopStateBit << "> "
				<< m_ui32ControllerSteps/__ControllerFrequency__ << "s\n";
		}

		if(m_oBenchmarkChrono.hasNewEstimation())
		{
			log() << LogLevel_Benchmark
				<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
				<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "processor use" << LogColor_PopStateBit << "> "
				<< m_oBenchmarkChrono.getStepInPercentage() << "%\n";
		}

		uint32 l_ui32SecondsLate=static_cast<uint32>((l_ui32CurrentTime-m_ui32ControllerSteps*(1000.0/__ControllerFrequency__))/1000);
		if(l_ui32SecondsLate!=m_ui32SecondsLate)
		{
			log() << (l_ui32SecondsLate==0?LogLevel_Info:(l_ui32SecondsLate>=10?LogLevel_ImportantWarning:LogLevel_Warning))
				<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
				<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "can not reach realtime" << LogColor_PopStateBit << "> "
				<< l_ui32SecondsLate << " second(s) late...\n";

			m_ui32SecondsLate=l_ui32SecondsLate;
		}
	}

	return true;
}

uint64 CPlayer::getCurrentSimulatedTime(void) const
{
	return m_ui64CurrentSimulatedTime;
}
