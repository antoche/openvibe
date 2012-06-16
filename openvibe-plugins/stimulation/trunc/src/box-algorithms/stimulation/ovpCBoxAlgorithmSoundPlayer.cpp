#include "ovpCBoxAlgorithmSoundPlayer.h"

#if defined OVP_OS_Windows
 #include <windows.h>
 #include <mmsystem.h>
#endif
#if defined OVP_OS_Linux
 #include <unistd.h>
#endif

#include <string>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Stimulation;

using namespace std;

#define boolean OpenViBE::boolean

boolean CBoxAlgorithmSoundPlayer::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pStreamDecoder->initialize();

	for(uint32 i=0; i<l_rStaticBoxContext.getSettingCount(); i+=2)
	{
		uint64 l_ui64StimulationIdentifier=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), i);
		CString l_sSoundFilename=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), i+1);

		m_vSoundInfo[l_ui64StimulationIdentifier].push_back(l_sSoundFilename);
	}
	return true;
}

boolean CBoxAlgorithmSoundPlayer::uninitialize(void)
{
	m_pStreamDecoder->uninitialize();
	getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);

	return true;
}

boolean CBoxAlgorithmSoundPlayer::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmSoundPlayer::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	std::map < uint64, std::vector < CString > >::const_iterator it;
	std::vector < CString >::const_iterator it2;
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		TParameterHandler < const IMemoryBuffer* > l_ipMemoryBuffer(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
		l_ipMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		m_pStreamDecoder->process();
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			TParameterHandler < IStimulationSet* > l_opStimulationSet(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
			for(uint32 j=0; j<l_opStimulationSet->getStimulationCount(); j++)
			{
				it=m_vSoundInfo.find(l_opStimulationSet->getStimulationIdentifier(j));
				if(it!=m_vSoundInfo.end())
				{
					for(it2=it->second.begin(); it2!=it->second.end(); it2++)
					{
#if defined OVP_OS_Windows
						::sndPlaySound(it2->toASCIIString(), SND_NOSTOP | SND_ASYNC );
#elif defined OVP_OS_Linux
						string l_sCommand;
						l_sCommand+="cat ";
						l_sCommand+=it2->toASCIIString();
						l_sCommand+=" > /dev/dsp &";
						int l_iResult=::system(l_sCommand.c_str());
#else
						getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Sound player not yet implemented for this OS\n";
#endif
					}
				}
			}
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
