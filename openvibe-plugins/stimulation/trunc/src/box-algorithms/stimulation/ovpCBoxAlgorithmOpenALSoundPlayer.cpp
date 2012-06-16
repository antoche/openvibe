#include "ovpCBoxAlgorithmOpenALSoundPlayer.h"

#if defined TARGET_HAS_ThirdPartyOpenAL

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Stimulation;

using namespace std;

#define boolean OpenViBE::boolean

#define BUFFER_SIZE 32768
#define UNIQUE_SOURCE 1

boolean CBoxAlgorithmOpenALSoundPlayer::initialize(void)
{

	m_pStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pStreamDecoder->initialize();

	m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pStreamEncoder->initialize();
	ip_pStimulationSet.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	op_pMemoryBuffer.initialize(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));


	m_ui64PlayTrigger = FSettingValueAutoCast(*this->getBoxAlgorithmContext(),0);
	m_ui64StopTrigger = FSettingValueAutoCast(*this->getBoxAlgorithmContext(),1);
	m_sFileName = FSettingValueAutoCast(*this->getBoxAlgorithmContext(),2);
	m_bLoop = FSettingValueAutoCast(*this->getBoxAlgorithmContext(),3);

	m_ui64LastOutputChunkDate = -1;
	m_bStartOfSoundSent = false;
	m_bEndOfSoundSent = false;

	if(alutInit(NULL,NULL) != AL_TRUE)
	{
		if(alutGetError () == ALUT_ERROR_INVALID_OPERATION)
		{
			this->getLogManager() << LogLevel_Trace << "ALUT already initialized.\n";
		}
		else
		{
			this->getLogManager() << LogLevel_Error << "ALUT initialization returned a bad status.\n";
			this->getLogManager() << LogLevel_Error << "ALUT ERROR:\n"<<alutGetErrorString(alutGetError ())<<"\n";
			return false;
		}
	}

	m_iFileFormat = FILE_FORMAT_UNSUPPORTED;

	string l_sFile((const char *)m_sFileName);
	if(l_sFile.find(".wav") !=string::npos)
	{
		m_iFileFormat = FILE_FORMAT_WAV;
	}
	if(l_sFile.find(".ogg") !=string::npos)
	{
		m_iFileFormat = FILE_FORMAT_OGG;
	}
	return openSoundFile();
}

boolean CBoxAlgorithmOpenALSoundPlayer::uninitialize(void)
{
	m_pStreamDecoder->uninitialize();
	getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
	m_pStreamEncoder->uninitialize();
	getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	
	boolean l_bStatus = stopSound();



#if UNIQUE_SOURCE
	alDeleteSources(1, &m_uiSourceHandle);
#endif
	alDeleteBuffers(1, &m_uiSoundBufferHandle);


	if(alutExit() != AL_TRUE)
	{
		if(alutGetError () == ALUT_ERROR_INVALID_OPERATION)
		{
			this->getLogManager() << LogLevel_Trace << "ALUT already exited.\n";
		}
		else
		{
			this->getLogManager() << LogLevel_Error << "ALUT uninitialization returned a bad status.\n";
			this->getLogManager() << LogLevel_Error << "ALUT ERROR:\n"<<alutGetErrorString(alutGetError ())<<"\n";
			return false;
		}
	}

	return l_bStatus;

}
boolean CBoxAlgorithmOpenALSoundPlayer::processClock(OpenViBE::CMessageClock& rMessageClock)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmOpenALSoundPlayer::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
	
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		TParameterHandler < const IMemoryBuffer* > l_ipMemoryBuffer(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
		l_ipMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		m_pStreamDecoder->process();

		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
			m_ui64LastOutputChunkDate = l_rDynamicBoxContext.getInputChunkEndTime(0, i);
		}

		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			TParameterHandler < IStimulationSet* > l_opStimulationSet(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
			for(uint32 j=0; j<l_opStimulationSet->getStimulationCount(); j++)
			{
				if(l_opStimulationSet->getStimulationIdentifier(j) == m_ui64PlayTrigger)
				{
					playSound();
					m_bEndOfSoundSent = false;
					m_bStartOfSoundSent = false;
				}
				if(l_opStimulationSet->getStimulationIdentifier(j) == m_ui64StopTrigger)
				{
					stopSound();
				}
			}
		}

		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeEnd);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
			m_ui64LastOutputChunkDate = l_rDynamicBoxContext.getInputChunkEndTime(0, i);
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);

	}



	ALint l_uiStatus;
	alGetSourcei(m_uiSourceHandle, AL_SOURCE_STATE, &l_uiStatus);
	// CASE : the sound has stopped, and we need to send the stimulation
	if(l_uiStatus == AL_STOPPED && !m_bEndOfSoundSent)
	{
		ip_pStimulationSet->clear();
		ip_pStimulationSet->appendStimulation(
			m_ui64StopTrigger,
			this->getPlayerContext().getCurrentTime(),
			0);
		m_pStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastOutputChunkDate, this->getPlayerContext().getCurrentTime());
		m_bEndOfSoundSent = true;
	}
	// CASE : the sound has started playing, and we need to send the stimulation
	if(l_uiStatus == AL_PLAYING && !m_bStartOfSoundSent)
	{
		ip_pStimulationSet->clear();
		ip_pStimulationSet->appendStimulation(
			m_ui64PlayTrigger,
			this->getPlayerContext().getCurrentTime(),
			0);
		m_pStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastOutputChunkDate, this->getPlayerContext().getCurrentTime());
		m_bStartOfSoundSent = true;
	}
	// CASE : the sound has stopped (or is playing), but there is no need to send anything but empty set (the box is "idle" stop or in a middle of a sound)
	if((l_uiStatus == AL_STOPPED && m_bEndOfSoundSent) || l_uiStatus == AL_PLAYING && m_bStartOfSoundSent || l_uiStatus == AL_INITIAL)
	{
		ip_pStimulationSet->clear();
		m_pStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastOutputChunkDate, this->getPlayerContext().getCurrentTime());
	}

	m_ui64LastOutputChunkDate = this->getPlayerContext().getCurrentTime();

	return true;
}

boolean CBoxAlgorithmOpenALSoundPlayer::openSoundFile()
{
	switch(m_iFileFormat)
	{
		case FILE_FORMAT_WAV:
		{
			this->getLogManager() << LogLevel_Trace << "Buffering WAV file (this step may take some times for long files).\n";
			m_uiSoundBufferHandle = alutCreateBufferFromFile(m_sFileName);
			this->getLogManager() << LogLevel_Trace << "WAV file buffered.\n";
			if(m_uiSoundBufferHandle == AL_NONE)
			{
				this->getLogManager() << LogLevel_Error << "ALUT can't create buffer from file "<<m_sFileName<<"\n";
				this->getLogManager() << LogLevel_Error << "ALUT ERROR:\n"<<alutGetErrorString(alutGetError ())<<"\n";
				return false;
			}
			break;
		}
		case FILE_FORMAT_OGG:
		{
			// On windows using fopen+ov_open can lead to failure, as stated in the vorbis official documentation:
			//http://xiph.org/vorbis/doc/vorbisfile/ov_open.html
			// using ov_fopen instead.
			//m_oOggVorbisStream.File = fopen((const char *)m_sFileName, "rb");
			//if (m_oOggVorbisStream.File == NULL)
			//{
			//	this->getLogManager() << LogLevel_Error << "Can't open file "<<m_sFileName<<": IO error\n.";
			//	return false;
			//}
			
#if defined OVP_OS_Windows
			if(ov_fopen(const_cast<char*>(m_sFileName.toASCIIString()), &m_oOggVorbisStream.Stream) < 0)
#elif defined OVP_OS_Linux
			if((m_oOggVorbisStream.File = fopen((const char *)m_sFileName, "rb")) == NULL)
			{
				this->getLogManager() << LogLevel_Error << "Can't open file "<<m_sFileName<<": IO error\n.";
				return false;
			}
			if(ov_open(m_oOggVorbisStream.File, &(m_oOggVorbisStream.Stream), NULL, 0) < 0)
#else
#error "Please port this code"
#endif
			{
				this->getLogManager() << LogLevel_Error << "Can't open file "<<m_sFileName<<": OGG VORBIS stream error\n";
				return false;
			}
			
			vorbis_info* l_pInfos = ov_info(&m_oOggVorbisStream.Stream, -1);
			m_oOggVorbisStream.Format     = l_pInfos->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
			m_oOggVorbisStream.SampleRate = l_pInfos->rate;
			
			//Now we fill the raw buffer (good for small piece of sound... use buffering for big files)
			this->getLogManager() << LogLevel_Trace << "Buffering OGG file (this step may take some times for long files).\n";
			int32 l_iBytesRead;
			int32 bitStream;
			char l_pBuffer[BUFFER_SIZE];
			do {
				// Read up to a buffer's worth of decoded sound data
				l_iBytesRead = ov_read(&m_oOggVorbisStream.Stream, l_pBuffer, BUFFER_SIZE, 0, 2, 1, &bitStream);
				// Append to end of buffer
				m_vRawOggBufferFromFile.insert(m_vRawOggBufferFromFile.end(), l_pBuffer, l_pBuffer + l_iBytesRead);
			} while (l_iBytesRead > 0);
			this->getLogManager() << LogLevel_Trace << "OGG file buffered.\n";
			
			//we have decoded all the file. we drop the decoder (file is closed for us).
			ov_clear(&m_oOggVorbisStream.Stream);

			//create empty buffer
			alGenBuffers(1, &m_uiSoundBufferHandle);
			//fill it with raw data
			alBufferData(m_uiSoundBufferHandle, m_oOggVorbisStream.Format, &m_vRawOggBufferFromFile[0], static_cast < ALsizei > (m_vRawOggBufferFromFile.size()), m_oOggVorbisStream.SampleRate);

			break;
		}
		default:
		{
			this->getLogManager() << LogLevel_Error << "Unsupported file format. Please use only WAV or OGG files.\n";
			return false;
		}
	}

#if UNIQUE_SOURCE
	alGenSources(1, &m_uiSourceHandle);
	alSourcei (m_uiSourceHandle, AL_BUFFER, m_uiSoundBufferHandle);
	alSourcei (m_uiSourceHandle, AL_LOOPING, (m_bLoop?AL_TRUE:AL_FALSE));
#endif
	return true;
}
boolean CBoxAlgorithmOpenALSoundPlayer::playSound()
{
	switch(m_iFileFormat)
	{
		case FILE_FORMAT_WAV:
		case FILE_FORMAT_OGG:
		{
#if UNIQUE_SOURCE
			ALint l_uiStatus;
			alGetSourcei(m_uiSourceHandle, AL_SOURCE_STATE, &l_uiStatus);
			if(l_uiStatus == AL_PLAYING)
			{
				// we start back again
				alSourceStop(m_uiSourceHandle);
			}
			alSourcePlay(m_uiSourceHandle);
#else
			ALuint l_uiSource;
			alGenSources(1, &l_uiSource);
			m_vOpenALSources.push_back(l_uiSource);
			alSourcei (l_uiSource, AL_BUFFER, m_uiSoundBufferHandle);
			alSourcei (l_uiSource, AL_LOOPING, (m_bLoop?AL_TRUE:AL_FALSE));
			alSourcePlay(l_uiSource);
#endif
			break;
		}
		default:
		{
			this->getLogManager() << LogLevel_Error << "Unsupported file format. Please use only WAV or OGG files.\n";
			return false;
		}
	}
	return true;
}
boolean CBoxAlgorithmOpenALSoundPlayer::stopSound()
{
	switch(m_iFileFormat)
	{
		case FILE_FORMAT_WAV:
		case FILE_FORMAT_OGG:
		{
			
#if UNIQUE_SOURCE
			alSourceStop(m_uiSourceHandle);
#else
			for(uint32 i = 0;i<m_vOpenALSources.size();i++)
			{
				//stop all sources
				alSourceStop(m_vOpenALSources[i]);
				alDeleteSources(1, &m_vOpenALSources[i]);
			}
			m_vOpenALSources.clear();
#endif
			break;
		}
		default:
		{
			this->getLogManager() << LogLevel_Error << "Unsupported file format. Please use only WAV or OGG files.\n";
			return false;
		}
	}

	return true;
}


#endif //TARGET_HAS_ThirdPartyOpenAL
