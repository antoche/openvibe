#include "ovkPsSimulatedBox.h"
#include "ovkCPlayer.h"
#include "ovkCBoxAlgorithmContext.h"
#include "ovkCMessageClock.h"
#include "ovkCMessageEvent.h"
#include "ovkCMessageSignal.h"

#include "OMKVisBase.h"
#include "OMKOgreVis.h"
#include "../visualisation/ovkCOgreWidget.h"

#include <iostream>

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
#define boolean OpenViBE::boolean

// #define _ScopeTester_
#define _MaxCrash_ 5

namespace
{
	template <typename T>
	T& _my_get_(list<T>& rList, uint32 ui32Index)
	{
		uint32 i;
		typename list<T>::iterator it=rList.begin();
		for(i=0; i<ui32Index; i++)
		{
			++it;
		}
		return *it;
	}

	template <typename T>
	const T& _my_get_(const list<T>& rList, uint32 ui32Index)
	{
		uint32 i;
		typename list<T>::const_iterator it=rList.begin();
		for(i=0; i<ui32Index; i++)
		{
			++it;
		}
		return *it;
	}
}

const IKernelContext* OpenViBE::Kernel::CSimulatedBoxCreator::ms_pKernelContext=NULL;
const IScenario* OpenViBE::Kernel::CSimulatedBoxCreator::ms_pScenario=NULL;

OMK::Name CSimulatedBox::CLASS_ID("OpenViBEKernelSimulatedBox");
const bool CSimulatedBox::REGISTERED(OMK::SimulatedObjectFactory::getInstance().registerCreator< CSimulatedBoxCreator >(CLASS_ID));

CSimulatedBox::CSimulatedBox(
	OMK::Controller& rController,
	const OMK::ObjectDescriptor& rObjectDescriptor,
	const IKernelContext& rKernelContext,
	const IScenario& rScenario)
	:CSimulatedBoxBase(rController, rObjectDescriptor)
	,TKernelObject<IBoxIO>(rKernelContext)
	,m_ui32CrashCount(0)
	,m_bReadyToProcess(false)
	,m_bActive(true)
	,m_pBoxAlgorithm(NULL)
	,m_pScenario(&rScenario)
	,m_pBox(NULL)
	,m_ui64LastClockActivationDate(0)
	,m_ui64ClockFrequency(0)
	,m_ui64ClockActivationStep(0)
	,m_bAlreadyClockActivatedOnce(false)
	,m_pGtkOgreWidget(NULL)
{
	log() << LogLevel_Debug << "CSimulatedBox::CSimulatedBox\n";
}

CSimulatedBox::~CSimulatedBox(void)
{
	log() << LogLevel_Debug << "CSimulatedBox::~CSimulatedBox\n";
}

void CSimulatedBox::init(void)
{
	::CSimulatedBoxBase::init();

	CIdentifier l_oIdentifier;
	::OMK::Name l_oNameIdentifier=getName();
	string l_oNameString=l_oNameIdentifier.getString();
	l_oIdentifier.fromString(l_oNameIdentifier.getCString());

	m_pBox=m_pScenario->getBoxDetails(l_oIdentifier);

	m_vInput.resize(m_pBox->getInputCount());
	m_vOutput.resize(m_pBox->getOutputCount());
	m_vCurrentOutput.resize(m_pBox->getOutputCount());

	log() << LogLevel_Debug << "CSimulatedBox::init(" << CString(getName().getCString()) << "|" << m_pBox->getName() << ")\n";

	m_oBenchmarkChronoProcessClock.reset(1024);
	m_oBenchmarkChronoProcessInput.reset(1024);
	m_oBenchmarkChronoProcess.reset(1024);

	m_bAlreadyClockActivatedOnce=false;
}

void CSimulatedBox::computeParameters(void)
{
	log() << LogLevel_Debug << "computeParameters::init(" << CString(getName().getCString()) << "|" << m_pBox->getName() << "|" << getSimulatedDate() << ")\n";

	if(!m_bActive) return;

	uint64 l_ui64CurrentDate=(((OpenViBE::uint64)this->getSimulatedDate())<<32);
	boolean l_bShouldProcessClock=(m_ui64ClockFrequency!=0) && (!m_bAlreadyClockActivatedOnce || (l_ui64CurrentDate-m_ui64LastClockActivationDate>=m_ui64ClockActivationStep));

	if(l_bShouldProcessClock)
	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
#if defined _ScopeTester_
		Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::processClock)"));
#endif
		try
		{
			m_oBenchmarkChronoProcessClock.stepIn();
			Kernel::CMessageClock l_oClockMessage(this->getKernelContext());
			m_ui64LastClockActivationDate+=(m_bAlreadyClockActivatedOnce?m_ui64ClockActivationStep:0);
			l_oClockMessage.setTime(m_ui64LastClockActivationDate/1000);
			m_pBoxAlgorithm->processClock(l_oBoxAlgorithmContext, l_oClockMessage);
			m_oBenchmarkChronoProcessClock.stepOut();
		}
		catch (...)
		{
			handleCrash("clock processing callback");
		}
		m_bReadyToProcess|=l_oBoxAlgorithmContext.isAlgorithmReadyToProcess();
		m_bAlreadyClockActivatedOnce=true;
	}

	if(l_bShouldProcessClock || m_ui64ClockFrequency==0)
	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
#if defined _ScopeTester_
		Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::getClockFrequency)"));
#endif
		try
		{
			// note: 1LL should be left shifted 64 bits but this
			//       would result in an integer over shift (the one
			//       would exit). Thus the left shift of 63 bits
			//       with a factor 2 multiplication after the division
			m_ui64ClockFrequency=m_pBoxAlgorithm->getClockFrequency(l_oBoxAlgorithmContext);
			m_ui64ClockActivationStep=1000*2*(m_ui64ClockFrequency?((1LL<<63)/m_ui64ClockFrequency):0);
		}
		catch (...)
		{
			handleCrash("clock frequency request callback");
		}
	}

	if(m_bReadyToProcess)
	{
		doProcess();
		m_bReadyToProcess=false;
	}

/*-----------------------------------------------*/
/* TODO send this messages with better frequency */
	if(m_oBenchmarkChronoProcessClock.hasNewEstimation())
	{
		log() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "process clock" << LogColor_PopStateBit
			<< "::" << m_pBox->getName() << "> "
			<< "Average computing time is " << ((m_oBenchmarkChronoProcessClock.getAverageStepInDuration()*1000000)>>32) << "us\n";
	}
	if(m_oBenchmarkChronoProcessInput.hasNewEstimation())
	{
		log() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "process input" << LogColor_PopStateBit
			<< "::" << m_pBox->getName() << "> "
			<< "Average computing time is " << ((m_oBenchmarkChronoProcessInput.getAverageStepInDuration()*1000000)>>32) << "us\n";
	}
	if(m_oBenchmarkChronoProcess.hasNewEstimation())
	{
		log() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "process      " << LogColor_PopStateBit
			<< "::" << m_pBox->getName() << "> "
			<< "Average computing time is " << ((m_oBenchmarkChronoProcess.getAverageStepInDuration()*1000000)>>32) << "us\n";
	}
/* TODO Thank you for reading :)                 */
/*-----------------------------------------------*/
}

bool CSimulatedBox::processOpenViBEDataUpdateEvent(::OMKValuedEvent< CTypeChunk >* pEvent)
{
	log() << LogLevel_Debug << "computeParameters::processOpenViBEDataUpdateEvent(" << CString(getName().getCString()) << "|" << m_pBox->getName() << "|" << pEvent->date << ")\n";

	if(!m_bActive) return false;

	m_vInput[pEvent->value.getIoConnectorIndex()].push_back(pEvent->value);

	CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
	{
#if defined _ScopeTester_
		Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::processInput)"));
#endif
		try
		{
			m_oBenchmarkChronoProcessInput.stepIn();
			m_pBoxAlgorithm->processInput(l_oBoxAlgorithmContext, pEvent->value.getIoConnectorIndex());
			m_oBenchmarkChronoProcessInput.stepOut();
		}
		catch (...)
		{
			handleCrash("input processing callback");
		}
	}
	m_bReadyToProcess|=l_oBoxAlgorithmContext.isAlgorithmReadyToProcess();

	return true ;
}

bool CSimulatedBox::processMaskStartEvent(::OMKEvent* pEvent)
{
	log() << LogLevel_Debug << "computeParameters::processMaskStartEvent(" << CString(getName().getCString()) << "|" << m_pBox->getName() << "|" << pEvent->date << ")\n";

	if(!m_bActive) return false;

	m_pBoxAlgorithm=dynamic_cast<IBoxAlgorithm*>(getKernelContext().getPluginManager().createPluginObject(m_pBox->getAlgorithmClassIdentifier()));
	if(!m_pBoxAlgorithm)
	{
		getLogManager() << LogLevel_Error << "Could not create box algorithm with class id " << m_pBox->getAlgorithmClassIdentifier() << "... This box will be deactivated but the whole scenario behavior will probably suffer !\n";
		m_bActive=false;
		return false;
	}

	CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
	{
		{
#if defined _ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::initialize)"));
#endif
			try
			{
				m_pBoxAlgorithm->initialize(l_oBoxAlgorithmContext);
			}
			catch (...)
			{
				handleCrash("initialization callback");
			}
		}

		{
#if defined _ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::getClockFrequency)"));
#endif
			try
			{
				// note: 1LL should be left shifted 64 bits but this
				//       would result in an integer over shift (the one
				//       would exit). Thus the left shift of 63 bits
				//       with a factor 2 multiplication after the division
				m_ui64ClockFrequency=m_pBoxAlgorithm->getClockFrequency(l_oBoxAlgorithmContext);
				m_ui64ClockActivationStep=1000*2*(m_ui64ClockFrequency?((1LL<<63)/m_ui64ClockFrequency):0);
			}
			catch (...)
			{
				handleCrash("clock frequency request callback");
			}
		}
	}

	return true ;
}

bool CSimulatedBox::processMaskStopEvent(::OMKEvent* pEvent)
{
	log() << LogLevel_Debug << "computeParameters::processMaskStopEvent(" << CString(getName().getCString()) << "|" << m_pBox->getName() << "|" << pEvent->date << ")\n";

	if(!m_bActive) return false;

	CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
	{
#if defined _ScopeTester_
		Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::uninitialize)"));
#endif
		try
		{
			m_pBoxAlgorithm->uninitialize(l_oBoxAlgorithmContext);
		}
		catch (...)
		{
			handleCrash("uninitialization callback");
		}
	}

	m_pBoxAlgorithm->release();
	m_pBoxAlgorithm=NULL;

	return true ;
}

void CSimulatedBox::handleCrash(const char* sHintName)
{
	m_ui32CrashCount++;

	log() << LogLevel_Error << "Plugin code caused crash " << m_ui32CrashCount << " time(s)\n";
	log() << LogLevel_Error << "  [name:" << m_pBox->getName() << "]\n";
	log() << LogLevel_Error << "  [identifier:" << m_pBox->getIdentifier() << "]\n";
	log() << LogLevel_Error << "  [algorithm class identifier:" << m_pBox->getAlgorithmClassIdentifier() << "]\n";
	log() << LogLevel_Error << "  [place:" << sHintName << "]\n";

	if(m_ui32CrashCount>=_MaxCrash_)
	{
		log () << LogLevel_Fatal << "  This plugin has been disabled !\n";
		m_bActive=false;
	}
}

void CSimulatedBox::doProcess(void)
{
	log() << LogLevel_Debug << "computeParameters::doProcess(" << CString(getName().getCString()) << "|" << m_pBox->getName() << ")\n";

	if(!m_bActive) return;

	CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
	{
#if defined _ScopeTester_
		Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::process)"));
#endif
		try
		{
			m_oBenchmarkChronoProcess.stepIn();
			m_pBoxAlgorithm->process(l_oBoxAlgorithmContext);
			m_oBenchmarkChronoProcess.stepOut();
		}
		catch (...)
		{
			handleCrash("processing callback");
		}
	}

	// perform output sending
	CIdentifier l_oLinkIdentifier=m_pScenario->getNextLinkIdentifierFromBox(OV_UndefinedIdentifier, m_pBox->getIdentifier());
	while(l_oLinkIdentifier!=OV_UndefinedIdentifier)
	{
		const ILink* l_pLink=m_pScenario->getLinkDetails(l_oLinkIdentifier);
		if(l_pLink)
		{
			CIdentifier l_oTargetBoxIdentifier=l_pLink->getTargetBoxIdentifier();
			uint32 l_ui32TargetBoxInputIndex=l_pLink->getTargetBoxInputIndex();
			::OMK::Name target(l_oTargetBoxIdentifier.toString());

			uint32 l_ui32SourceOutputIndex=l_pLink->getSourceBoxOutputIndex();
			list< CTypeChunk >::iterator i=m_vOutput[l_ui32SourceOutputIndex].begin();
			while(i!=m_vOutput[l_ui32SourceOutputIndex].end())
			{
				i->setIoConnectorIndex(l_ui32TargetBoxInputIndex);
				sendOpenViBEDataUpdateEvent(target, (*i));
				++i;
			}
		}
		l_oLinkIdentifier=m_pScenario->getNextLinkIdentifierFromBox(l_oLinkIdentifier, m_pBox->getIdentifier());
	}

	// iterators for input and output chunks
	vector<list< CTypeChunk > >::iterator i;
	list< CTypeChunk >::iterator j;

	// perform input cleaning
	i=m_vInput.begin();
	while(i!=m_vInput.end())
	{
		j=i->begin();
		while(j!=i->end())
		{
			if(j->m_bDeprecated)
			{
				j=i->erase(j);
			}
			else
			{
				++j;
			}
		}
		++i;
	}

	// flushes sent output chunks
	i=m_vOutput.begin();
	while(i!=m_vOutput.end())
	{
		i->resize(0);
		++i;
	}

	// discards waiting output chunks
	j=m_vCurrentOutput.begin();
	while(j!=m_vCurrentOutput.end())
	{
		// checks buffer size
		if(j->getBuffer().getSize())
		{
			// the buffer has been (partially ?) filled but not sent
			CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
			l_oBoxAlgorithmContext.getPlayerContext()->getLogManager() << LogLevel_Warning << "Output buffer filled but not marked as ready to send\n"; // $$$ should use log
			j->getBuffer().setSize(0, true);
		}

		++j;
	}
}

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// - --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

uint64 CSimulatedBox::getCurrentTime(void) const
{
	if(m_ui64ClockFrequency==0)
	{
		uint64 l_ui64CurrentDate=(((OpenViBE::uint64)this->getSimulatedDate())<<32);
		return l_ui64CurrentDate/1000;
	}
	return m_ui64LastClockActivationDate/1000;
}

CString CSimulatedBox::getOVName(void) const
{
	return m_pBox->getName();
}

const IScenario& CSimulatedBox::getScenario(void) const
{
	return *m_pScenario;
}

uint32 CSimulatedBox::getInputChunkCount(
	const uint32 ui32InputIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	return m_vInput[ui32InputIndex].size();
}

boolean CSimulatedBox::getInputChunk(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex,
	uint64& rStartTime,
	uint64& rEndTime,
	uint64& rChunkSize,
	const uint8*& rpChunkBuffer) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		return false;
	}

	const CTypeChunk& l_rChunk=_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex);
	rStartTime=l_rChunk.getStartTime();
	rEndTime=l_rChunk.getEndTime();
	rChunkSize=l_rChunk.getBuffer().getSize();
	rpChunkBuffer=l_rChunk.getBuffer().getDirectPointer();
	return true;
}

const IMemoryBuffer* CSimulatedBox::getInputChunk(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		return false;
	}

	return &_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex).getBuffer();
}

uint64 CSimulatedBox::getInputChunkStartTime(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		return false;
	}

	const CTypeChunk& l_rChunk=_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex);
	return l_rChunk.getStartTime();
}

uint64 CSimulatedBox::getInputChunkEndTime(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		return false;
	}

	const CTypeChunk& l_rChunk=_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex);
	return l_rChunk.getEndTime();
}

boolean CSimulatedBox::markInputAsDeprecated(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex)
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		return false;
	}
	_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex).m_bDeprecated=true;
	return true;
}

uint64 CSimulatedBox::getOutputChunkSize(
	const uint32 ui32OutputIndex) const
{
	if(ui32OutputIndex>=m_vCurrentOutput.size())
	{
		return false;
	}
	return _my_get_(m_vCurrentOutput, ui32OutputIndex).getBuffer().getSize();
}

boolean CSimulatedBox::setOutputChunkSize(
	const uint32 ui32OutputIndex,
	const uint64 ui64Size,
	const boolean bDiscard)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}
	return _my_get_(m_vCurrentOutput, ui32OutputIndex).getBuffer().setSize(ui64Size, bDiscard);
}

uint8* CSimulatedBox::getOutputChunkBuffer(
	const uint32 ui32OutputIndex)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}
	return _my_get_(m_vCurrentOutput, ui32OutputIndex).getBuffer().getDirectPointer();
}

boolean CSimulatedBox::appendOutputChunkData(
	const OpenViBE::uint32 ui32OutputIndex,
	const OpenViBE::uint8* pBuffer,
	const OpenViBE::uint64 ui64BufferSize)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}
	return _my_get_(m_vCurrentOutput, ui32OutputIndex).getBuffer().appendOutputChunkData(pBuffer, ui64BufferSize);
}

IMemoryBuffer* CSimulatedBox::getOutputChunk(
	const uint32 ui32OutputIndex)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}
	return &_my_get_(m_vCurrentOutput, ui32OutputIndex).getBuffer();
}

boolean CSimulatedBox::markOutputAsReadyToSend(
	const uint32 ui32OutputIndex,
	const uint64 ui64StartTime,
	const uint64 ui64EndTime)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}

	// sets start and end time
	_my_get_(m_vCurrentOutput, ui32OutputIndex).setStartTime(ui64StartTime);
	_my_get_(m_vCurrentOutput, ui32OutputIndex).setEndTime(ui64EndTime);

	// copies chunk
	m_vOutput[ui32OutputIndex].push_back(_my_get_(m_vCurrentOutput, ui32OutputIndex));

	// resets chunk size
	_my_get_(m_vCurrentOutput, ui32OutputIndex).getBuffer().setSize(0, true);

	return true;
}

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// - --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

::GtkWidget* CSimulatedBox::create3DWidget(const CString& rName)
{
	//there is a single 3D widget per simulated box
	if(m_pGtkOgreWidget == NULL)
	{
		m_pGtkOgreWidget = gtk_ogre_new(rName.toASCIIString(), this);
	}
	return GTK_WIDGET(m_pGtkOgreWidget);
}

boolean CSimulatedBox::setBackgroundColor(float32 f32ColorRed, float32 f32ColorGreen, float32 f32ColorBlue)
{
	return m_pGtkOgreWidget->setBackgroundColor(f32ColorRed, f32ColorGreen, f32ColorBlue);
}

boolean CSimulatedBox::setCameraToEncompassObjects()
{
	return m_pGtkOgreWidget->setCameraToEncompassObjects();
}

CIdentifier CSimulatedBox::createObject(const CString& rObjectFileName)
{
	return m_pGtkOgreWidget->createObject(rObjectFileName);
}

CIdentifier CSimulatedBox::createObject(const EStandard3DObject eStandard3DObject)
{
	return m_pGtkOgreWidget->createObject(eStandard3DObject);
}

boolean CSimulatedBox::removeObject(const CIdentifier& rIdentifier)
{
	return m_pGtkOgreWidget->removeObject(rIdentifier);
}

boolean CSimulatedBox::setObjectScale(const CIdentifier& rIdentifier, float32 f32ScaleX, float32 f32ScaleY, float32 f32ScaleZ)
{
	return m_pGtkOgreWidget->setObjectScale(rIdentifier, f32ScaleX, f32ScaleY, f32ScaleZ);
}

boolean CSimulatedBox::setObjectScale(const CIdentifier& rIdentifier, float32 f32Scale)
{
	return m_pGtkOgreWidget->setObjectScale(rIdentifier, f32Scale);
}

boolean CSimulatedBox::setObjectPosition(const CIdentifier& rIdentifier, float32 f32PositionX, float32 f32PositionY, float32 f32PositionZ)
{
	return m_pGtkOgreWidget->setObjectPosition(rIdentifier, f32PositionX, f32PositionY, f32PositionZ);
}

boolean CSimulatedBox::setObjectOrientation(const CIdentifier& rIdentifier, float32 f32OrientationX, float32 f32OrientationY,
	float32 f32OrientationZ, float32 f32OrientationW)
{
	return m_pGtkOgreWidget->setObjectOrientation(rIdentifier, f32OrientationX, f32OrientationY, f32OrientationZ, f32OrientationW);
}

boolean CSimulatedBox::setObjectColor(const CIdentifier& rIdentifier, float32 f32ColorRed, float32 f32ColorGreen, float32 f32ColorBlue)
{
	return m_pGtkOgreWidget->setObjectColor(rIdentifier, f32ColorRed, f32ColorGreen, f32ColorBlue);
}

boolean CSimulatedBox::setObjectTransparency(const CIdentifier& rIdentifier, float32 f32Transparency)
{
	return m_pGtkOgreWidget->setObjectTransparency(rIdentifier, f32Transparency);
}

boolean CSimulatedBox::setObjectVertexColorArray(const CIdentifier& rIdentifier, const uint32 ui32VertexColorCount, const float32* pVertexColorArray)
{
	return m_pGtkOgreWidget->setObjectVertexColorArray(rIdentifier, ui32VertexColorCount, pVertexColorArray);
}

boolean CSimulatedBox::getObjectVertexCount(const CIdentifier& rIdentifier, uint32& ui32VertexCount) const
{
	return m_pGtkOgreWidget->getObjectVertexCount(rIdentifier, ui32VertexCount);
}

boolean CSimulatedBox::getObjectVertexPositionArray( const OpenViBE::CIdentifier& rIdentifier,	float32* pVertexPositionArray) const
{
	return m_pGtkOgreWidget->getObjectVertexPositionArray(rIdentifier, pVertexPositionArray);
}
