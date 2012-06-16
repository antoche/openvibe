#include "ovpCTest.h"

#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;
using namespace OpenViBEToolkit;
using namespace std;

CTest::CTest(void)
	:m_oSignalReaderCallbackProxy(*this
		,&CTest::setChannelCount
		,&CTest::setChannelName
		,&CTest::setSampleCountPerBuffer
		,&CTest::setSamplingRate
		,&CTest::setSampleBuffer)
	,m_oStimulationReaderCallbackProxy(*this
		,&CTest::setStimulationCount
		,&CTest::setStimulation)
	,m_pSignalReaderCallback(createBoxAlgorithmSignalInputReaderCallback(m_oSignalReaderCallbackProxy))
	,m_pStimulationReaderCallback(createBoxAlgorithmStimulationInputReaderCallback(m_oStimulationReaderCallbackProxy))
	,m_oSignalReader(*m_pSignalReaderCallback)
	,m_oStimulationReader(*m_pStimulationReaderCallback)
	,m_ui64SignalLatestSampleTime(0)
{
}

CTest::~CTest(void)
{
}

void CTest::release(void)
{
	delete this;
}

boolean CTest::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CTest::process(void)
{
	// IBox* l_pStaticBoxContext=getBoxAlgorithmContext()->getStaticBoxContext();
	IBoxIO* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	uint64 l_ui64StartTime=0;
	uint64 l_ui64EndTime=0;
	uint64 l_ui64ChunkSize=0;
	const uint8* l_pChunkBuffer=NULL;

	for(uint32 i=0; i<l_pDynamicBoxContext->getInputChunkCount(0); i++) // signal input
	{
		l_pDynamicBoxContext->getInputChunk(0, i, l_ui64StartTime, l_ui64EndTime, l_ui64ChunkSize, l_pChunkBuffer);
		m_oSignalReader.processData(l_pChunkBuffer, l_ui64ChunkSize);
		l_pDynamicBoxContext->markInputAsDeprecated(0, i);
		m_ui64SignalLatestSampleTime=l_ui64EndTime;
	}

	for(uint32 j=0; j<l_pDynamicBoxContext->getInputChunkCount(1); j++) // stimulations input
	{
		l_pDynamicBoxContext->getInputChunk(1, j, l_ui64StartTime, l_ui64EndTime, l_ui64ChunkSize, l_pChunkBuffer);
		if(l_ui64EndTime<m_ui64SignalLatestSampleTime)
		{
			m_oStimulationReader.processData(l_pChunkBuffer, l_ui64ChunkSize);
			l_pDynamicBoxContext->markInputAsDeprecated(1, j);
		}
		else
		{
		}
	}

	return true;
}

CIdentifier CTest::getStimulationIdentifierTrialStart(void)
{
	return 1;
}

CIdentifier CTest::getStimulationIdentifierTrialEnd(void)
{
	return 2;
}

CIdentifier CTest::getStimulationIdentifierTrialLabelRangeStart(void)
{
	return 3;
}

CIdentifier CTest::getStimulationIdentifierTrialLabelRangeEnd(void)
{
	return 4;
}

CIdentifier CTest::getStimulationIdentifierTrain(void)
{
	return 5;
}

boolean CTest::train(ISignalTrialSet& rTrialSet)
{
	return true;
}
