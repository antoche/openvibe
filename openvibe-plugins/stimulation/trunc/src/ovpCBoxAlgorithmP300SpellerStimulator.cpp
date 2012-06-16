#include "ovpCBoxAlgorithmP300SpellerStimulator.h"

#include <list>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Stimulation;

#define _LOG_(lm, x) { lm << x; }
#define _OPTIONAL_LOG_(lm, x) /* _LOG_(lm, x); */

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

	class _AutoCast_
	{
	public:
		_AutoCast_(IBox& rBox, IConfigurationManager& rConfigurationManager, const uint32 ui32Index) : m_rConfigurationManager(rConfigurationManager) { rBox.getSettingValue(ui32Index, m_sSettingValue); }
		operator uint64 (void) { return m_rConfigurationManager.expandAsUInteger(m_sSettingValue); }
		operator int64 (void) { return m_rConfigurationManager.expandAsInteger(m_sSettingValue); }
		operator float64 (void) { return m_rConfigurationManager.expandAsFloat(m_sSettingValue); }
		operator boolean (void) { return m_rConfigurationManager.expandAsBoolean(m_sSettingValue); }
		operator const CString (void) { return m_sSettingValue; }
	protected:
		IConfigurationManager& m_rConfigurationManager;
		CString m_sSettingValue;
	};
};

uint64 CBoxAlgorithmP300SpellerStimulator::getClockFrequency(void)
{
	return 128LL<<32;
}

boolean CBoxAlgorithmP300SpellerStimulator::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	m_pStimulationDecoder=NULL;
	m_pStimulationEncoder=NULL;

	m_ui64StartStimulation       =this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, _AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 0));
	m_ui64RowStimulationBase     =this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, _AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 1));
	m_ui64ColumnStimulationBase  =this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, _AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 2));

	m_ui64RowCount               =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 3);
	m_ui64ColumnCount            =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 4);

	if(m_ui64RowCount==0 || m_ui64ColumnCount==0)
	{
		_LOG_(this->getLogManager(), LogLevel_ImportantWarning << "This stimulator should at least have 1 row and 1 column (got " << m_ui64RowCount << " and " << m_ui64ColumnCount << "\n");
		return false;
	}

	if(m_ui64RowCount!=m_ui64ColumnCount)
	{
		_LOG_(this->getLogManager(), LogLevel_ImportantWarning << "This stimulator should have the same number of row(s) and columns(s) (got " << m_ui64RowCount << " and " << m_ui64ColumnCount << "\n");
		return false;
	}

	m_ui64RepetitionCountInTrial =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 5);
	m_ui64TrialCount             =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 6);
	m_ui64FlashDuration          =((float64)_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 7))*(1LL<<32);
	m_ui64NoFlashDuration        =((float64)_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 8))*(1LL<<32);
	m_ui64InterRepetitionDuration=((float64)_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 9))*(1LL<<32);
	m_ui64InterTrialDuration     =((float64)_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 10))*(1LL<<32);

	m_bAvoidNeighborFlashing     =_AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 11);

	if(m_ui64InterRepetitionDuration<(10LL<<32)/1000)
	{
		_LOG_(this->getLogManager(), LogLevel_Warning << "Inter repetition duration should not be less than 10 ms\n");
		m_ui64InterRepetitionDuration=(10LL<<32)/1000;
	}

	if(m_ui64InterTrialDuration<(10LL<<32)/1000)
	{
		_LOG_(this->getLogManager(), LogLevel_Warning << "Inter trial duration should not be less than 10 ms\n");
		m_ui64InterTrialDuration=(10LL<<32)/1000;
	}

	if(m_bAvoidNeighborFlashing)
	{
		_LOG_(this->getLogManager(), LogLevel_ImportantWarning << "Avoid neighbor flashing setting is not considered yet\n");
	}

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------

	m_pStimulationEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pStimulationEncoder->initialize();

	m_pStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pStimulationDecoder->initialize();

	m_ui64LastTime=0;
	m_bHeaderSent=false;
	m_bStartReceived=false;

	m_ui32LastState=State_None;
	m_ui64TrialStartTime=m_ui64InterTrialDuration;

	m_ui64FlashCountInRepetition=m_ui64RowCount+m_ui64ColumnCount;
	m_ui64RepetitionDuration=m_ui64FlashCountInRepetition*(m_ui64FlashDuration+m_ui64NoFlashDuration);
	m_ui64TrialDuration=m_ui64RepetitionCountInTrial*(m_ui64RepetitionDuration+m_ui64InterRepetitionDuration);
	m_ui64TrialIndex=1;

	this->generate_sequence();
	return true;
}

boolean CBoxAlgorithmP300SpellerStimulator::uninitialize(void)
{
	if(m_pStimulationDecoder)
	{
		m_pStimulationDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationDecoder);
		m_pStimulationDecoder=NULL;
	}

	if(m_pStimulationEncoder)
	{
		m_pStimulationEncoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationEncoder);
		m_pStimulationEncoder=NULL;
	}

	return true;
}

boolean CBoxAlgorithmP300SpellerStimulator::processInput(uint32 ui32InputIndex)
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

	return true;
}

boolean CBoxAlgorithmP300SpellerStimulator::processClock(IMessageClock& rMessageClock)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmP300SpellerStimulator::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	uint32 l_ui32State=State_NoFlash;
	uint64 l_ui64CurrentTime=this->getPlayerContext().getCurrentTime();
	uint64 l_ui64FlashIndex=(uint64)-1;
	uint64 l_ui64RepetitionIndex=(uint64)-1;

	CStimulationSet l_oStimulationSet;

	if(m_bStartReceived)
	{
		if(l_ui64CurrentTime<m_ui64TrialStartTime)
		{
			l_ui32State=State_TrialRest;
		}
		else
		{
			uint64 l_ui64CurrentTimeInTrial     =l_ui64CurrentTime-m_ui64TrialStartTime;
			uint64 l_ui64CurrentTimeInRepetition=l_ui64CurrentTimeInTrial%(m_ui64RepetitionDuration+m_ui64InterRepetitionDuration);
			uint64 l_ui64RepritionIndexInTrial  =l_ui64CurrentTimeInTrial/(m_ui64RepetitionDuration+m_ui64InterRepetitionDuration);
			uint64 l_ui64FlashIndexInRepetition =l_ui64CurrentTimeInRepetition/(m_ui64FlashDuration+m_ui64NoFlashDuration);

			l_ui64FlashIndex=l_ui64FlashIndexInRepetition;
			l_ui64RepetitionIndex=l_ui64RepritionIndexInTrial;

			if(l_ui64CurrentTimeInTrial >= m_ui64TrialDuration)
			{
				if(m_ui64TrialCount==0 || m_ui64TrialIndex<m_ui64TrialCount)
				{
					m_ui64TrialStartTime=l_ui64CurrentTime+m_ui64InterTrialDuration;
					l_ui32State=State_TrialRest;
					l_ui64FlashIndex=(uint64)-1;
					l_ui64RepetitionIndex=(uint64)-1;
					m_ui64TrialIndex++;
				}
				else
				{
					l_ui32State=State_None;
				}
			}
			else
			{
				if(l_ui64CurrentTimeInRepetition >= m_ui64RepetitionDuration)
				{
					l_ui32State=State_RepetitionRest;
					l_ui64FlashIndex=(uint64)-1;
				}
				else
				{
					if(l_ui64CurrentTimeInRepetition%(m_ui64FlashDuration+m_ui64NoFlashDuration)<m_ui64FlashDuration)
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

		if(l_ui32State!=m_ui32LastState)
		{
			boolean l_bRow=((l_ui64FlashIndex&1)==1?true:false);
			long l_iRow=l_bRow?m_vRow[l_ui64FlashIndex>>1]:-1;
			long l_iColumn=l_bRow?-1:m_vColumn[l_ui64FlashIndex>>1];

			switch(m_ui32LastState)
			{
				case State_Flash:
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_VisualStimulationStop, l_ui64CurrentTime, 0);
					_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_VisualStimulationStop\n");
					break;

				case State_NoFlash:
					break;

				case State_RepetitionRest:
					if(l_ui32State!=State_TrialRest && l_ui32State!=State_None)
					{
						l_oStimulationSet.appendStimulation(OVTK_StimulationId_SegmentStart, l_ui64CurrentTime, 0);
						_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_SegmentStart\n");
					}
					break;

				case State_TrialRest:
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_RestStop, l_ui64CurrentTime, 0);
					_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_RestStop\n");
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_TrialStart, l_ui64CurrentTime, 0);
					_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_TrialStart\n");
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_SegmentStart, l_ui64CurrentTime, 0);
					_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_SegmentStart\n");
					break;

				case State_None:
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_ExperimentStart, l_ui64CurrentTime, 0);
					_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_ExperimentStart\n");
					break;

				default:
					break;
			}

			switch(l_ui32State)
			{
				case State_Flash:
					l_oStimulationSet.appendStimulation(l_bRow?m_ui64RowStimulationBase+l_iRow:m_ui64ColumnStimulationBase+l_iColumn, l_ui64CurrentTime, 0);
					_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_LabelId(x)\n");
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_VisualStimulationStart, l_ui64CurrentTime, 0);
					_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_VisualStimulationStart\n");
					break;

				case State_NoFlash:
					break;

				case State_RepetitionRest:
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_SegmentStop, l_ui64CurrentTime, 0);
					_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_SegmentStop\n");
					this->generate_sequence();
					break;

				case State_TrialRest:
					if(m_ui32LastState!=State_None)
					{
						if(m_ui32LastState!=State_RepetitionRest)
						{
							l_oStimulationSet.appendStimulation(OVTK_StimulationId_SegmentStop, l_ui64CurrentTime, 0);
							_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_SegmentStop\n");
						}
						l_oStimulationSet.appendStimulation(OVTK_StimulationId_TrialStop, l_ui64CurrentTime, 0);
						_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_TrialStop\n");
					}
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_RestStart, l_ui64CurrentTime, 0);
					_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_RestStart\n");
					break;

				case State_None:
					if(m_ui32LastState!=State_RepetitionRest)
					{
						l_oStimulationSet.appendStimulation(OVTK_StimulationId_SegmentStop, l_ui64CurrentTime, 0);
						_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_SegmentStop\n");
					}
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_TrialStop, l_ui64CurrentTime, 0);
					_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_TrialStop\n");
					l_oStimulationSet.appendStimulation(OVTK_StimulationId_ExperimentStop, l_ui64CurrentTime, 0);
					_OPTIONAL_LOG_(this->getLogManager(), LogLevel_Trace << "sends OVTK_StimulationId_ExperimentStop\n");
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

void CBoxAlgorithmP300SpellerStimulator::generate_sequence(void)
{
	uint32 i,j;

	std::vector < uint32 > l_vRow;
	m_vRow.clear();
	for(i=0; i<m_ui64RowCount; i++)
	{
		l_vRow.push_back(i);
	}
	for(i=0; i<m_ui64RowCount; i++)
	{
		j=rand()%l_vRow.size();
		m_vRow[i]=l_vRow[j];
		l_vRow.erase(l_vRow.begin()+j);
	}

	std::vector < uint32 > l_vColumn;
	m_vColumn.clear();
	for(i=0; i<m_ui64ColumnCount; i++)
	{
		l_vColumn.push_back(i);
	}
	for(i=0; i<m_ui64ColumnCount; i++)
	{
		j=rand()%l_vColumn.size();
		m_vColumn[i]=l_vColumn[j];
		l_vColumn.erase(l_vColumn.begin()+j);
	}
}
