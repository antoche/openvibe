#include "ovpCBoxAlgorithmStreamedMatrixSwitch.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Streaming;

using namespace OpenViBEToolkit;

using namespace std;

boolean CBoxAlgorithmStreamedMatrixSwitch::initialize(void)
{
	// Getting the settings to build the map Stim code / output index
	for(uint32 i = 0; i < this->getStaticBoxContext().getSettingCount(); i++)
	{
		uint64 l_oStimCode  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(),i);
		if(!m_mStimulationOutputIndexMap.insert(make_pair(l_oStimCode, i)).second)
		{
			this->getLogManager() << LogLevel_Warning << "The stimulation code ["<<this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation,l_oStimCode) << "] for the output ["<< i << "] is already used by a previous output.\n";
		}
		else
		{
			this->getLogManager() << LogLevel_Trace << "The stimulation code ["<<this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation,l_oStimCode) << "] is registered for the output ["<< i << "]\n";
		}
	}
	// At start, no output is active.
	m_i32ActiveOutputIndex = -1;

	// Stimulation stream decoder
	m_oStimulationDecoder.initialize(*this);
	m_ui64LastStimulationInputChunkEndTime = 0;

	//initializing the decoder depending on the input type.
	CIdentifier l_oTypeIdentifier;
	this->getStaticBoxContext().getInputType(1,l_oTypeIdentifier);

	if(l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
	{
		m_pStreamDecoder = new TStreamedMatrixDecoder < CBoxAlgorithmStreamedMatrixSwitch >(*this);
		//m_pStreamEncoder = new TStreamedMatrixEncoder < CBoxAlgorithmStreamedMatrixSwitch >(*this);
	}
	if(l_oTypeIdentifier == OV_TypeId_Signal)
	{
		m_pStreamDecoder = new TSignalDecoder < CBoxAlgorithmStreamedMatrixSwitch >(*this);
		//m_pStreamEncoder = new TSignalEncoder < CBoxAlgorithmStreamedMatrixSwitch >(*this);
	}
	if(l_oTypeIdentifier == OV_TypeId_Spectrum)
	{
		m_pStreamDecoder = new TSpectrumDecoder < CBoxAlgorithmStreamedMatrixSwitch >(*this);
		//m_pStreamEncoder = new TSpectrumEncoder < CBoxAlgorithmStreamedMatrixSwitch >(*this);
	}
	if(l_oTypeIdentifier == OV_TypeId_FeatureVector)
	{
		m_pStreamDecoder = new TFeatureVectorDecoder < CBoxAlgorithmStreamedMatrixSwitch >(*this);
		//m_pStreamEncoder = new TFeatureVectorEncoder < CBoxAlgorithmStreamedMatrixSwitch >(*this);
	}
	if(l_oTypeIdentifier == OV_TypeId_ChannelLocalisation)
	{
		m_pStreamDecoder = new TChannelLocalisationDecoder < CBoxAlgorithmStreamedMatrixSwitch >(*this);
		//m_pStreamEncoder = new TChannelLocalisationEncoder < CBoxAlgorithmStreamedMatrixSwitch >(*this);
	}

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmStreamedMatrixSwitch::uninitialize(void)
{
	m_oStimulationDecoder.uninitialize();
	m_pStreamDecoder->uninitialize();
	delete m_pStreamDecoder;
	//m_pStreamEncoder->uninitialize();
	//delete m_pStreamEncoder;

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmStreamedMatrixSwitch::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmStreamedMatrixSwitch::process(void)
{

	// the static box context describes the box inputs, outputs, settings structures
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	// the dynamic box context describes the current state of the box inputs and outputs (i.e. the chunks)
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	uint64 l_ui64StartTime=0;
	uint64 l_ui64EndTime=0;
	uint64 l_ui64ChunkSize=0;
	const uint8* l_pChunkBuffer=NULL;

	//iterate over all chunk on input 0 (Stimulation)
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		m_oStimulationDecoder.decode(0,i);

		if(m_oStimulationDecoder.isHeaderReceived() || m_oStimulationDecoder.isEndReceived())
		{
			// nothing
		}
		if(m_oStimulationDecoder.isBufferReceived())
		{
			// we update the active output index and time if needed
			OpenViBE::IStimulationSet * l_pStimSet = m_oStimulationDecoder.getOutputStimulationSet();
			for(uint32 stim_index = 0 ; stim_index < l_pStimSet->getStimulationCount(); stim_index++)
			{
				if(m_mStimulationOutputIndexMap.find(l_pStimSet->getStimulationIdentifier(stim_index)) != m_mStimulationOutputIndexMap.end())
				{
					m_i32ActiveOutputIndex = m_mStimulationOutputIndexMap[l_pStimSet->getStimulationIdentifier(stim_index)];
					this->getLogManager() << LogLevel_Trace << "Switching with ["<<this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation,l_pStimSet->getStimulationIdentifier(stim_index)) << "] to output ["<< m_i32ActiveOutputIndex << "].\n";
				}
			}
			m_ui64LastStimulationInputChunkEndTime = l_rDynamicBoxContext.getInputChunkEndTime(0,i);
		}
	}

	for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(1); j++)
	{
		//We decode the chunk but we don't automatically mark it as deprecated, as we may need to keep it.
		m_pStreamDecoder->decode(1,j,false);
		{
			l_rDynamicBoxContext.getInputChunk(1, j, l_ui64StartTime, l_ui64EndTime, l_ui64ChunkSize, l_pChunkBuffer);
			if(m_pStreamDecoder->isHeaderReceived() || m_pStreamDecoder->isEndReceived())
			{
				for(uint32 j = 0; j < l_rStaticBoxContext.getOutputCount(); j++)
				{
					l_rDynamicBoxContext.appendOutputChunkData(j, l_pChunkBuffer, l_ui64ChunkSize);
					l_rDynamicBoxContext.markOutputAsReadyToSend(j, l_ui64StartTime, l_ui64EndTime);
				}
				l_rDynamicBoxContext.markInputAsDeprecated(1,j);
			}
			if(m_pStreamDecoder->isBufferReceived())
			{
				if(m_i32ActiveOutputIndex == -1)
				{
					// we drop every chunk when no output is activated
					l_rDynamicBoxContext.markInputAsDeprecated(1,j);
				}
				else
				{
					if(l_ui64StartTime < m_ui64LastStimulationInputChunkEndTime)
					{
						// the input chunk is in the good time range (we are sure that no stim has been received to change the active output)
						l_rDynamicBoxContext.appendOutputChunkData(m_i32ActiveOutputIndex, l_pChunkBuffer, l_ui64ChunkSize);
						l_rDynamicBoxContext.markOutputAsReadyToSend(m_i32ActiveOutputIndex, l_ui64StartTime, l_ui64EndTime);
						l_rDynamicBoxContext.markInputAsDeprecated(1,j);
					}
					// else : we keep the input chunk, no mark as deprecated !
				}
			}
		}
	}

	return true;
}
