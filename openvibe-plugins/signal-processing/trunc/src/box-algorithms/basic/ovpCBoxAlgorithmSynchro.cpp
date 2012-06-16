#include "ovpCBoxAlgorithmSynchro.h"

#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

boolean CBoxAlgorithmSynchro::initialize(void)
{
	m_oCInputChannel.initialize(this);
	
	m_oCOutputChannel.initialize(this);

	m_bStimulationReceivedStart=false;

	return true;
}

boolean CBoxAlgorithmSynchro::uninitialize(void)
{
	m_oCInputChannel.uninitialize();
	
	m_oCOutputChannel.uninitialize();

	return true;
}

boolean CBoxAlgorithmSynchro::processInput(uint32 ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmSynchro::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	if(m_oCInputChannel.isWorking())
	{
		// process stimulations
		for(uint32 index=0, nb=m_oCInputChannel.getNbOfStimulationBuffers(); index < nb; index++)
		{	
			OpenViBE::uint64 l_u64StartTimestamp, l_u64EndTimestamp; 
			OpenViBE::IStimulationSet* l_pIStimulationSet = m_oCInputChannel.getStimulation(l_u64StartTimestamp, l_u64EndTimestamp, index);

			if(!l_pIStimulationSet)
				break;

			m_oCOutputChannel.sendStimulation(l_pIStimulationSet, l_u64StartTimestamp, l_u64EndTimestamp);
		}
		// process signal
		for(uint32 index=0, nb=m_oCInputChannel.getNbOfSignalBuffers(); index < nb; index++)
		{	
			OpenViBE::uint64 l_u64StartTimestamp, l_u64EndTimestamp; 
			OpenViBE::CMatrix* l_pCMatrix = m_oCInputChannel.getSignal(l_u64StartTimestamp, l_u64EndTimestamp, index++);

			if(!l_pCMatrix)
				break;

			m_oCOutputChannel.sendSignal(l_pCMatrix, l_u64StartTimestamp, l_u64EndTimestamp);
		}
	}
	else if(m_oCInputChannel.hasSynchro())
	{	
		m_oCOutputChannel.processSynchroSignal(m_oCInputChannel.getStimulationPosition(), m_oCInputChannel.getSignalPosition());
		m_oCInputChannel.startWorking();
	}
	else if(m_oCInputChannel.hasHeader())
		m_oCInputChannel.waitForSynchro();
	else
	{	
		if(m_oCInputChannel.waitForSignalHeader())
		{
			m_oCOutputChannel.sendHeader(m_oCInputChannel.getSamplingRate(), m_oCInputChannel.getMatrixPtr());
		}
	}

	return true;
}
