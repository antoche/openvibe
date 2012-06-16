#include "ovpCBoxAlgorithmVRPNButtonClient.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::VRPN;

#define boolean OpenViBE::boolean

namespace
{
	void VRPN_CALLBACK vrpn_button_cb(void* pUserData, const vrpn_BUTTONCB b)
	{
		OpenViBEPlugins::VRPN::CBoxAlgorithmVRPNButtonClient* l_pBox = static_cast< OpenViBEPlugins::VRPN::CBoxAlgorithmVRPNButtonClient* > (pUserData);

		(l_pBox->m_vButtonList).push_back(std::pair<OpenViBE::uint32, boolean>(b.button, b.state ? true : false));
	}
}

uint64 CBoxAlgorithmVRPNButtonClient::getClockFrequency(void)
{
	return 128LL<<32; // the box clock frequency
}

boolean CBoxAlgorithmVRPNButtonClient::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	for(uint32 i=0; i<l_rStaticBoxContext.getOutputCount(); i++)
	{
		IAlgorithmProxy* l_pStreamEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
		l_pStreamEncoder->initialize();
		CStimulationSet* l_pStimulationSet=new CStimulationSet();
		TParameterHandler < IStimulationSet* > ip_pStimulationSet(l_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
		ip_pStimulationSet=l_pStimulationSet;
		m_vStreamEncoder.push_back(l_pStreamEncoder);
		m_vStimulationSet.push_back(l_pStimulationSet);

		m_vStimulationIdentifierOn.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), i*2+1));
		m_vStimulationIdentifierOff.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), i*2+2));
	}

	CString l_sPeripheralName=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	m_pVRPNButtonRemote=new vrpn_Button_Remote(l_sPeripheralName.toASCIIString());
	m_pVRPNButtonRemote->register_change_handler((void*)(this), vrpn_button_cb);

	m_ui64LastChunkEndTime=uint64(-1);

	return true;
}

boolean CBoxAlgorithmVRPNButtonClient::uninitialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	delete m_pVRPNButtonRemote;
	m_pVRPNButtonRemote=NULL;

	for(uint32 i=0; i<l_rStaticBoxContext.getOutputCount(); i++)
	{
		delete m_vStimulationSet[i];
		m_vStreamEncoder[i]->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_vStreamEncoder[i]);
	}
	m_vStimulationSet.clear();
	m_vStreamEncoder.clear();
	m_vStimulationIdentifierOn.clear();
	m_vStimulationIdentifierOff.clear();

	return true;
}

boolean CBoxAlgorithmVRPNButtonClient::processClock(IMessageClock& rMessageClock)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}


boolean CBoxAlgorithmVRPNButtonClient::process(void)
{

	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	uint32 i;

	// Clears pending stimulations
	for(i=0; i<l_rStaticBoxContext.getOutputCount(); i++)
	{
		m_vStimulationSet[i]->clear();
	}

	// Refreshes VRPN device
	m_bGotStimulation=false;
	m_pVRPNButtonRemote->mainloop();

	while (m_vButtonList.size() > 0)
	{
		std::pair<OpenViBE::uint32, boolean> button_state = m_vButtonList.front();
		m_vButtonList.pop_front();

		setButton(button_state.first, button_state.second);
	}

	// Encodes streams
	for(i=0; i<l_rStaticBoxContext.getOutputCount(); i++)
	{
		TParameterHandler < IMemoryBuffer* > op_pMemoryBuffer(m_vStreamEncoder[i]->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(i);
		if(m_ui64LastChunkEndTime==uint64(-1))
		{
			m_vStreamEncoder[i]->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
			l_rDynamicBoxContext.markOutputAsReadyToSend(i, 0, 0);
		}
		else
		{
			if(m_bGotStimulation)
			{
				m_vStreamEncoder[i]->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
			}
			l_rDynamicBoxContext.markOutputAsReadyToSend(i, m_ui64LastChunkEndTime, this->getPlayerContext().getCurrentTime());
		}
	}

	// Updates timings
	m_ui64LastChunkEndTime=this->getPlayerContext().getCurrentTime();

	return true;
}

void CBoxAlgorithmVRPNButtonClient::setButton(uint32 ui32ButtonIndex, boolean bPressed)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	if(ui32ButtonIndex>=l_rStaticBoxContext.getOutputCount())
	{
		this->getLogManager() << LogLevel_Warning << "Ignored button " << ui32ButtonIndex+1 << " with state " << CString(bPressed?"pressed":"released") << "...\n";
	}
	else
	{
		this->getLogManager() << LogLevel_Trace << "Changed button " << ui32ButtonIndex+1 << " with state " << CString(bPressed?"pressed":"released") << "...\n";
		if(bPressed)
		{
			m_vStimulationSet[ui32ButtonIndex]->appendStimulation(m_vStimulationIdentifierOn[ui32ButtonIndex], this->getPlayerContext().getCurrentTime(), 0);
		}
		else
		{
			m_vStimulationSet[ui32ButtonIndex]->appendStimulation(m_vStimulationIdentifierOff[ui32ButtonIndex], this->getPlayerContext().getCurrentTime(), 0);
		}
		m_bGotStimulation=true;
	}
}
