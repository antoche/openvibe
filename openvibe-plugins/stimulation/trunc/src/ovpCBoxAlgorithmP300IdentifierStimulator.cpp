#include "ovpCBoxAlgorithmP300IdentifierStimulator.h"

#include <list>
#include <cstdlib>
#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Stimulation;

#define _LOG_(lm, x) { lm << x; }
#define _OPTIONAL_LOG_(lm, x)  /*_LOG_(lm, x);*/

namespace
{
	enum
	{
		State_None,
		State_Flash,
		State_NoFlash,
		State_RepetitionRest,
		State_TrialRest,
	};

	CString state_to_string(uint32 ui32State)
	{
		switch(ui32State)
		{
			case State_None: return "State_None";
			case State_Flash: return "State_Flash";
			case State_NoFlash: return "State_NoFlash";
			case State_RepetitionRest: return "State_RepetitionRest";
			case State_TrialRest: return "State_TrialRest";
			default:
				break;
		}
		return "unknown";
	}
};

uint64 CBoxAlgorithmP300IdentifierStimulator::getClockFrequency(void)
{
	return 128LL << 32;
}

boolean CBoxAlgorithmP300IdentifierStimulator::initialize(void)
{
	//get values of the configure windows for all settings
	
	m_ui64StartStimulation    =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_ui64StimulationBase     =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	m_ui64ImagesCount         =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

	//the number of images must be different to 0
	if(m_ui64ImagesCount==0)
	{
		_LOG_(this->getLogManager(), LogLevel_ImportantWarning << "This stimulator should at least have 1 Image (got " << m_ui64ImagesCount <<  ")\n");
		return false;
	}

	m_f64PercentRepetitionTarget=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);

	//the percent of Repetition contains the target must be between 0% and 100%
	if(m_f64PercentRepetitionTarget>100. )
	{
		_LOG_(this->getLogManager(), LogLevel_Warning << "The percent of repetition contains Target, should not be more than 100% \n");
		m_f64PercentRepetitionTarget=100.;
	}

	if(m_f64PercentRepetitionTarget<0.)
	{
		_LOG_(this->getLogManager(), LogLevel_Warning << "The percent of repetition contains Target, should not be less than 0% \n");
		m_f64PercentRepetitionTarget=0.;
	}

	m_ui64RepetitionCountInTrial =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);
	m_ui64TrialCount             =FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5);
	m_ui64FlashDuration          =float64(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 6))*(1LL << 32);
	m_ui64NoFlashDuration        =float64(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 7))*(1LL << 32);
	m_ui64InterRepetitionDuration=float64(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 8))*(1LL << 32);
	m_ui64InterTrialDuration     =float64(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 9))*(1LL << 32);

	//-----------------------------------------------------------------------------------------------------------------------------------------

	m_pStimulationEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pStimulationEncoder->initialize();

	m_pStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pStimulationDecoder->initialize();

	m_pStimulationTargetDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pStimulationTargetDecoder->initialize();

	//initialized all variables
	m_bHeaderSent=false;
	m_ui32LastState=State_None;
	
	m_ui64FlashCountInRepetition=m_ui64ImagesCount;
	m_ui64FlashCountInRepetitionWithoutTarget=m_ui64FlashCountInRepetition-1;
	m_ui64RepetitionDuration=m_ui64FlashCountInRepetition*(m_ui64FlashDuration+m_ui64NoFlashDuration);
	m_ui64RepetitionDurationWithoutTarget=m_ui64FlashCountInRepetitionWithoutTarget*(m_ui64FlashDuration+m_ui64NoFlashDuration);
	
	m_pRepetitionTarget=new uint64[m_ui64RepetitionCountInTrial];

	reset();

	return true;
}

boolean CBoxAlgorithmP300IdentifierStimulator::reset(void)
{	
	m_ui64LastTime=0;
	m_bStartReceived=false;
	m_ui64TrialStartTime=m_ui64InterTrialDuration;

	m_i64TargetNumber=0;
	m_ui64TrialIndex=1;
	//generate the first trial variables
	this->generate_trial_vars();
	return true;
}

boolean CBoxAlgorithmP300IdentifierStimulator::uninitialize(void)
{
	m_pStimulationDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationDecoder);

	m_pStimulationTargetDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationTargetDecoder);

	m_pStimulationEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationEncoder);
	return true;
}

boolean CBoxAlgorithmP300IdentifierStimulator::processInput(OpenViBE::uint32 ui32Index)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		if(!m_bStartReceived)
		{
			TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(m_pStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
			TParameterHandler < IStimulationSet* > op_pStimulationSet(m_pStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
			ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
			m_pStimulationDecoder->process();

			if(m_pStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
			}

			if(m_pStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				for(uint32 j=0; j<op_pStimulationSet->getStimulationCount(); j++)
				{
					if(op_pStimulationSet->getStimulationIdentifier(j) == m_ui64StartStimulation)
					{
						_LOG_(this->getLogManager(), LogLevel_Trace << "Start\n");
						m_ui64TrialStartTime=op_pStimulationSet->getStimulationDate(j)+m_ui64InterTrialDuration;
						m_bStartReceived=true;
					}
				}
			}

			if(m_pStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
			}
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(1); i++)
	{
		if(m_bStartReceived)
		{
			TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(m_pStimulationTargetDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
			TParameterHandler < IStimulationSet* > op_pStimulationSet(m_pStimulationTargetDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
			ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(1, i);
			m_pStimulationTargetDecoder->process();

			if(m_pStimulationTargetDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
			}

			if(m_pStimulationTargetDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				for(uint32 j=0; j<op_pStimulationSet->getStimulationCount(); j++)
				{
					uint64 l_ui64StimulationIndex=op_pStimulationSet->getStimulationIdentifier(j)-m_ui64StimulationBase;
					if(l_ui64StimulationIndex<m_ui64ImagesCount)
					{
						m_i64TargetNumber=(int64)l_ui64StimulationIndex;
						_LOG_(this->getLogManager(), LogLevel_Trace << "Choosen number of the targets " << m_i64TargetNumber << "\n");
					}
				}
			}

			if(m_pStimulationTargetDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
			}
		}

		l_rDynamicBoxContext.markInputAsDeprecated(1, i);
	}
	return true;
}

boolean CBoxAlgorithmP300IdentifierStimulator::processClock(OpenViBE::CMessageClock & rMessageClock)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmP300IdentifierStimulator::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	uint32 l_ui32State=State_NoFlash;
	uint64 l_ui64CurrentTime=this->getPlayerContext().getCurrentTime();
	uint64 l_ui64FlashIndex=(uint64)-1;

	CStimulationSet l_oStimulationSet;

	//if start stimulation is receive
	if(m_bStartReceived)
	{

		//case of inter-trial
		if(l_ui64CurrentTime<m_ui64TrialStartTime)
		{
			l_ui32State=State_TrialRest;
		}

		//case of in-trial
		else
		{
			uint64 l_ui64CurrentTimeInTrial=l_ui64CurrentTime-m_ui64TrialStartTime;

			//case of the current time is out of the trial time
			if(l_ui64CurrentTimeInTrial >= m_ui64TrialDuration)
			{
				//has next trial
				if(m_ui64TrialCount==0 || m_ui64TrialIndex<m_ui64TrialCount)
				{
					m_ui64TrialStartTime=l_ui64CurrentTime+m_ui64InterTrialDuration;
					l_ui32State=State_TrialRest;
					l_ui64FlashIndex=(uint64)-1;
					m_ui64TrialIndex++;
					generate_trial_vars();
				}
				//it was the last trial
				else
				{
					l_ui32State=State_None;
					reset();
				}
			}
			else
			{
				int64 l_i64CurrentTimeInRepetition=getCurrentTimeInRepetition(l_ui64CurrentTimeInTrial);

				//case of the current time is out of the repetition time
				if(l_i64CurrentTimeInRepetition < 0 || (uint64)l_i64CurrentTimeInRepetition >= (m_bRepetitionWithoutTarget?m_ui64RepetitionDurationWithoutTarget:m_ui64RepetitionDuration))
				{
					l_ui32State=State_RepetitionRest;
					l_ui64FlashIndex=(uint64)-1;
				}
				else
				{
					l_ui64FlashIndex =l_i64CurrentTimeInRepetition/(m_ui64FlashDuration+m_ui64NoFlashDuration);

					//case of the current time isn't out of the flash time
					if(l_i64CurrentTimeInRepetition%(m_ui64FlashDuration+m_ui64NoFlashDuration)<m_ui64FlashDuration)
					{
						l_ui32State=State_Flash;
					}
					else
					{
						l_ui32State=State_NoFlash;
					}
				}
			}
		}

		//case of the state changed
		if(l_ui32State!=m_ui32LastState)
		{
			//trigger send about the old state
			switch(m_ui32LastState)
			{
				//case of the older state was a flash
				case State_Flash:
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_VisualStimulationStop, l_ui64CurrentTime, 0);
					_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_VisualStimulationStop\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex  << "\n");
					break;
				//case of the older state was a no-flash
				case State_NoFlash:
					break;

				//case of the older state was a inter-repetition
				case State_RepetitionRest:
					if(l_ui32State!=State_TrialRest && l_ui32State!=State_None)
					{
						l_oStimulationSet.appendStimulation(OVTK_StimulationId_SegmentStart, l_ui64CurrentTime, 0);
						_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_SegmentStart\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					}
					break;
				//case of the older state was inter-Trial
				case State_TrialRest:
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_RestStop, l_ui64CurrentTime, 0);
					_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_RestStop\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_TrialStart, l_ui64CurrentTime, 0);
					_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_TrialStart\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_SegmentStart, l_ui64CurrentTime, 0);
					_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_SegmentStart\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					break;
				//case of the older state was a None state
				case State_None:
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_ExperimentStart, l_ui64CurrentTime, 0);
					_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_ExperimentStart\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					break;

				default:
					break;
			}

			//trigger and operation about the new state
			switch(l_ui32State)
			{
				//case of the new state is a flash
				case State_Flash:
					l_oStimulationSet.appendStimulation(m_ui64StimulationBase+m_vImages[l_ui64FlashIndex], l_ui64CurrentTime, 0);
					_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_LabelId(x)\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_VisualStimulationStart, l_ui64CurrentTime, 0);
					_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_VisualStimulationStart\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					break;
				//case of the new state is a no-flash
				case State_NoFlash:
					break;
				//case of the new state is a inter-repetition
				case State_RepetitionRest:
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_SegmentStop, l_ui64CurrentTime, 0);
					_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_SegmentStop\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					this->generate_sequence();
					break;
				//case of the new state is a inter-trial
				case State_TrialRest:
					m_i64TargetNumber=-1;
					if(m_ui32LastState!=State_None)
					{
						if(m_ui32LastState!=State_RepetitionRest)
						{
							l_oStimulationSet.appendStimulation(OVTK_StimulationId_SegmentStop, l_ui64CurrentTime, 0);
							_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_SegmentStop\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
						}
						l_oStimulationSet.appendStimulation(OVTK_StimulationId_TrialStop, l_ui64CurrentTime, 0);
						_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_TrialStop\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					}
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_RestStart, l_ui64CurrentTime, 0);
					_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_RestStart\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					break;
				//case of the new state is a none state
				case State_None:
					if(m_ui32LastState!=State_RepetitionRest)
					{
						l_oStimulationSet.appendStimulation(OVTK_StimulationId_SegmentStop, l_ui64CurrentTime, 0);
						_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_SegmentStop\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					}
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_TrialStop, l_ui64CurrentTime, 0);
					_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_TrialStop\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_ExperimentStop, l_ui64CurrentTime, 0);
					_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_ExperimentStop\n\t; Trial index:" << m_ui64TrialIndex << " Repetition index: " << m_ui64RepetitionIndex << "\n");
					break;

				default:
					break;
			}

			m_ui32LastState=l_ui32State;
		}
#if 0
		_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Info << "State:" << state_to_string(l_ui32State) << " - flash index:" << l_ui64FlashIndex << " - repetition index:" << l_ui64RepetitionIndex << " - trial index:" << m_ui64TrialIndex << "\n");
#endif
	}

	TParameterHandler < IStimulationSet* > ip_pStimulationSet(m_pStimulationEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	TParameterHandler < IMemoryBuffer* > op_pMemoryBuffer(m_pStimulationEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	ip_pStimulationSet=&l_oStimulationSet;
	op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
	if(!m_bHeaderSent)
	{
		m_pStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastTime, l_ui64CurrentTime);
	}
	if(m_ui64LastTime!=l_ui64CurrentTime)
	{
		m_pStimulationEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastTime, l_ui64CurrentTime);
	}
	m_ui64LastTime=l_ui64CurrentTime;
	m_bHeaderSent=true;
	return true;
}



void CBoxAlgorithmP300IdentifierStimulator::generate_sequence(void)
{
	if(m_ui64RepetitionIndex<m_ui64RepetitionCountInTrial)
	{
		_LOG_(this->getLogManager(), LogLevel_Trace << "generate_sequence Repetition: " << m_ui64RepetitionIndex  << " Target: " << m_i64TargetNumber << "\n");
		if(m_ui64RepetitionIndex>0)
		{
			m_bRepetitionWithoutTarget=m_pRepetitionTarget[m_ui64RepetitionIndex]==m_pRepetitionTarget[m_ui64RepetitionIndex-1];
		}
		else
		{
			m_bRepetitionWithoutTarget=(m_pRepetitionTarget[0]==0);
		}
		m_ui64RepetitionIndex++;
		std::vector < uint64 > l_vImages;
		m_vImages.clear();
		for(uint32 i=0;i<m_ui64ImagesCount;i++){
			l_vImages.push_back(i);
		}
		_LOG_(this->getLogManager(), LogLevel_Trace << "Number target: " << " Repetition without target: " << m_bRepetitionWithoutTarget << "\n");
		if(m_bRepetitionWithoutTarget&&m_i64TargetNumber!=-1){
			l_vImages.erase(l_vImages.begin()+m_i64TargetNumber);
		}
		while(l_vImages.size()>0){
			int j=rand()%l_vImages.size();
			m_vImages.push_back(l_vImages[j]);
			l_vImages.erase(l_vImages.begin()+j);
		}
	}
}


void CBoxAlgorithmP300IdentifierStimulator::generate_trial_vars(void)
{
	_LOG_(this->getLogManager(), LogLevel_Trace << "generate_trial_vars " << "\n");
	uint64 l_ui64NumberTargetInTrial=0;
	for(uint32 i=0;i<m_ui64RepetitionCountInTrial;i++)
	{
		uint32 random=rand();
		//increment the chance to display the target. we would'nt like to have no Target display in a trial.
		float64 l_f64PercentRepetitionTargetInc=m_f64PercentRepetitionTarget+(100-m_f64PercentRepetitionTarget)/(m_ui64RepetitionCountInTrial-1)*i;
		float64 l_f64PercentRepetitionTarget=(l_ui64NumberTargetInTrial<1)?l_f64PercentRepetitionTargetInc:m_f64PercentRepetitionTarget;

		if(random%100<l_f64PercentRepetitionTarget)
		{
			l_ui64NumberTargetInTrial++;
		}
		m_pRepetitionTarget[i]=l_ui64NumberTargetInTrial;
	}
	m_ui64TrialDuration=l_ui64NumberTargetInTrial*(m_ui64RepetitionDuration+m_ui64InterRepetitionDuration)+(m_ui64RepetitionCountInTrial-l_ui64NumberTargetInTrial)*(m_ui64RepetitionDurationWithoutTarget+m_ui64InterRepetitionDuration);
	m_ui64RepetitionIndex=0;
	m_i64TargetNumber=-1;
}

int64 CBoxAlgorithmP300IdentifierStimulator::getCurrentTimeInRepetition(uint64 ui64CurrentTimeInTrial)
{
	if(m_ui64RepetitionIndex==0)
	{
		return -1;
	}
	int64 l_i64CurrentTimeInRepetition=ui64CurrentTimeInTrial-m_ui64RepetitionDuration*m_pRepetitionTarget[m_ui64RepetitionIndex-1]
	                                                         -m_ui64RepetitionDurationWithoutTarget*(m_ui64RepetitionIndex-m_pRepetitionTarget[m_ui64RepetitionIndex-1])
	                                                         -m_ui64InterRepetitionDuration*m_ui64RepetitionIndex;
	//case of the current time in Repetition is out of the current Repetition time
	if(l_i64CurrentTimeInRepetition>0)
	{
		generate_sequence();
		return getCurrentTimeInRepetition(ui64CurrentTimeInTrial);
	}
	return l_i64CurrentTimeInRepetition+m_ui64InterRepetitionDuration+(m_bRepetitionWithoutTarget?m_ui64RepetitionDurationWithoutTarget:m_ui64RepetitionDuration);
}
