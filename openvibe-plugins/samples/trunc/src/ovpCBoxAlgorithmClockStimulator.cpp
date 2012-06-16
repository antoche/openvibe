#include "ovpCBoxAlgorithmClockStimulator.h"
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;

uint64 CBoxAlgorithmClockStimulator::getClockFrequency(void)
{
	return (1LL<<32)*32;
}

boolean CBoxAlgorithmClockStimulator::initialize(void)
{
	CString l_sSettingValue;

	float64 l_f64InterstimulationInterval=0;
	getStaticBoxContext().getSettingValue(0, l_sSettingValue);
	l_f64InterstimulationInterval=::atof(l_sSettingValue.toASCIIString());
	m_ui64InterstimulationInterval=(uint64)(l_f64InterstimulationInterval*(1LL<<32));

	getStaticBoxContext().getSettingValue(1, l_sSettingValue);
	m_ui64StimulationId=getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, l_sSettingValue);

	m_ui64LastStimulationDate=0;
	m_ui64LastEndTime=0;

	m_pStimulationEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pStimulationEncoder->initialize();

	return true;
}

boolean CBoxAlgorithmClockStimulator::uninitialize(void)
{
	m_pStimulationEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationEncoder);

	return true;
}

boolean CBoxAlgorithmClockStimulator::processClock(IMessageClock& rMessageClock)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmClockStimulator::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	TParameterHandler < IStimulationSet* > ip_pStimulationSet(m_pStimulationEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	TParameterHandler < IMemoryBuffer* > op_pMemoryBuffer(m_pStimulationEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	uint64 l_ui64CurrentTime=getPlayerContext().getCurrentTime();

	op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);

	ip_pStimulationSet->setStimulationCount(0);
	while(m_ui64LastStimulationDate+m_ui64InterstimulationInterval<l_ui64CurrentTime)
	{
		m_ui64LastStimulationDate+=m_ui64InterstimulationInterval;
		ip_pStimulationSet->appendStimulation(m_ui64StimulationId, m_ui64LastStimulationDate, 0);
	}

	if(m_ui64LastEndTime==0)
	{
		m_pStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastEndTime, m_ui64LastEndTime);
	}

	m_pStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
	l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastEndTime, l_ui64CurrentTime);

	m_ui64LastEndTime=l_ui64CurrentTime;

	return true;
}
