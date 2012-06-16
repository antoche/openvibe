#include "ovpCBoxAlgorithmBrainampFileReader.h"
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

uint64 CBoxAlgorithmBrainampFileReader::getClockFrequency(void)
{
	// Brainamp file reader parameters
	TParameterHandler < CString* > ip_sFilename(m_pBrainampFileReader->getInputParameter(OVP_Algorithm_BrainampFileReader_InputParameterId_Filename));
	TParameterHandler < float64 > ip_f64EpochDuration(m_pBrainampFileReader->getInputParameter(OVP_Algorithm_BrainampFileReader_InputParameterId_EpochDuration));
	TParameterHandler < uint64 > ip_ui64SeekTime(m_pBrainampFileReader->getInputParameter(OVP_Algorithm_BrainampFileReader_InputParameterId_SeekTime));
	TParameterHandler < uint64 > op_ui64CurrentStartTime(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_CurrentStartTime));
	TParameterHandler < uint64 > op_ui64CurrentEndTime(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_CurrentEndTime));
	TParameterHandler < uint64 > op_ui64SamplingRate(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_SamplingRate));
	TParameterHandler < IMatrix* > op_pSignalMatrix(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_SignalMatrix));
	TParameterHandler < IStimulationSet* > op_pStimulations(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_Stimulations));

	return (uint64)((1LL<<32)/ip_f64EpochDuration);
}

boolean CBoxAlgorithmBrainampFileReader::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	// IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	// Creates algorithms
	m_pBrainampFileReader=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_BrainampFileReader));
	m_pExperimentInformationStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_ExperimentInformationStreamEncoder));
	m_pSignalStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStimulationStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));

	m_pBrainampFileReader->initialize();
	m_pExperimentInformationStreamEncoder->initialize();
	m_pSignalStreamEncoder->initialize();
	m_pStimulationStreamEncoder->initialize();

	// Brainamp file reader parameters
	TParameterHandler < CString* > ip_sFilename(m_pBrainampFileReader->getInputParameter(OVP_Algorithm_BrainampFileReader_InputParameterId_Filename));
	TParameterHandler < float64 > ip_f64EpochDuration(m_pBrainampFileReader->getInputParameter(OVP_Algorithm_BrainampFileReader_InputParameterId_EpochDuration));
	TParameterHandler < uint64 > ip_ui64SeekTime(m_pBrainampFileReader->getInputParameter(OVP_Algorithm_BrainampFileReader_InputParameterId_SeekTime));
	TParameterHandler < uint64 > op_ui64CurrentStartTime(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_CurrentStartTime));
	TParameterHandler < uint64 > op_ui64CurrentEndTime(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_CurrentEndTime));
	TParameterHandler < uint64 > op_ui64SamplingRate(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_SamplingRate));
	TParameterHandler < IMatrix* > op_pSignalMatrix(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_SignalMatrix));
	TParameterHandler < IStimulationSet* > op_pStimulations(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_Stimulations));

	// Signal stream encoder parameters
	TParameterHandler < uint64 > ip_ui64SamplingRate(m_pSignalStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));
	TParameterHandler < IMatrix* > ip_pSignalMatrix(m_pSignalStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));

	// Stimulation stream encoder parameters
	TParameterHandler < IStimulationSet* > ip_pStimulations(m_pStimulationStreamEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));

	// Connect parameters together
	ip_ui64SamplingRate.setReferenceTarget(op_ui64SamplingRate);
	ip_pSignalMatrix.setReferenceTarget(op_pSignalMatrix);
	ip_pStimulations.setReferenceTarget(op_pStimulations);

	// Configures settings according to box
	CString l_sSettingValue;

	l_rStaticBoxContext.getSettingValue(0, l_sSettingValue);
	*ip_sFilename=l_sSettingValue;

	l_rStaticBoxContext.getSettingValue(1, l_sSettingValue);
	ip_f64EpochDuration=atof(l_sSettingValue.toASCIIString());

	m_bHeaderSent=false;

	return true;
}

boolean CBoxAlgorithmBrainampFileReader::uninitialize(void)
{
	m_pBrainampFileReader->process(OVP_Algorithm_BrainampFileReader_InputTriggerId_Close);

	m_pBrainampFileReader->uninitialize();
	m_pStimulationStreamEncoder->uninitialize();
	m_pSignalStreamEncoder->uninitialize();
	m_pExperimentInformationStreamEncoder->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_pBrainampFileReader);
	getAlgorithmManager().releaseAlgorithm(*m_pStimulationStreamEncoder);
	getAlgorithmManager().releaseAlgorithm(*m_pSignalStreamEncoder);
	getAlgorithmManager().releaseAlgorithm(*m_pExperimentInformationStreamEncoder);

	return true;
}

boolean CBoxAlgorithmBrainampFileReader::processClock(IMessageClock& rMessageClock)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmBrainampFileReader::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	// ...

	// l_rStaticBoxContext.getInputCount();
	// l_rStaticBoxContext.getOutputCount();
	// l_rStaticBoxContext.getSettingCount();

	// l_rDynamicBoxContext.getInputChunkCount()
	// l_rDynamicBoxContext.getInputChunk(i, )
	// l_rDynamicBoxContext.getOutputChunk(i, )

	// Brainamp file reader parameters
	TParameterHandler < CString* > ip_sFilename(m_pBrainampFileReader->getInputParameter(OVP_Algorithm_BrainampFileReader_InputParameterId_Filename));
	TParameterHandler < float64 > ip_f64EpochDuration(m_pBrainampFileReader->getInputParameter(OVP_Algorithm_BrainampFileReader_InputParameterId_EpochDuration));
	TParameterHandler < uint64 > ip_ui64SeekTime(m_pBrainampFileReader->getInputParameter(OVP_Algorithm_BrainampFileReader_InputParameterId_SeekTime));
	TParameterHandler < uint64 > op_ui64CurrentStartTime(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_CurrentStartTime));
	TParameterHandler < uint64 > op_ui64CurrentEndTime(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_CurrentEndTime));
	TParameterHandler < uint64 > op_ui64SamplingRate(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_SamplingRate));
	TParameterHandler < IMatrix* > op_pSignalMatrix(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_SignalMatrix));
	TParameterHandler < IStimulationSet* > op_pStimulations(m_pBrainampFileReader->getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_Stimulations));

	// Signal stream encoder parameters
	TParameterHandler < IMemoryBuffer* > op_pSignalMemoryBuffer(m_pSignalStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	// Stimulation stream encoder parameters
	TParameterHandler < IMemoryBuffer* > op_pStimulationMemoryBuffer(m_pStimulationStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	// Experiment information
	TParameterHandler < IMemoryBuffer* > op_pExperimentInformationMemoryBuffer(m_pExperimentInformationStreamEncoder->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	// Connects parameters to memory buffer
	op_pExperimentInformationMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
	op_pSignalMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(1);
	op_pStimulationMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(2);

	if(!m_bHeaderSent)
	{
		// Opens file
		m_pBrainampFileReader->process(OVP_Algorithm_BrainampFileReader_InputTriggerId_Open);

		// Produces header
		m_pExperimentInformationStreamEncoder->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeHeader);
		m_pStimulationStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
		m_pSignalStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);

		// Sends header
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, 0, 0);
		l_rDynamicBoxContext.markOutputAsReadyToSend(1, 0, 0);
		l_rDynamicBoxContext.markOutputAsReadyToSend(2, 0, 0);

		// Turn flag off
		m_bHeaderSent=true;
	}

	m_pBrainampFileReader->process(OVP_Algorithm_BrainampFileReader_InputTriggerId_Next);
	if(m_pBrainampFileReader->isOutputTriggerActive(OVP_Algorithm_BrainampFileReader_OutputTriggerId_DataProduced))
	{
		// Produces buffer
		m_pExperimentInformationStreamEncoder->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeBuffer);
		m_pStimulationStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
		m_pSignalStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);

		// Sends buffer
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, op_ui64CurrentStartTime, op_ui64CurrentEndTime);
		l_rDynamicBoxContext.markOutputAsReadyToSend(1, op_ui64CurrentStartTime, op_ui64CurrentEndTime);
		l_rDynamicBoxContext.markOutputAsReadyToSend(2, op_ui64CurrentStartTime, op_ui64CurrentEndTime);
	}

	return true;
}
