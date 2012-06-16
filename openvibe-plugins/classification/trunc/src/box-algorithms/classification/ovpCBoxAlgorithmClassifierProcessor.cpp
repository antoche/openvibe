#include "ovpCBoxAlgorithmClassifierProcessor.h"

#include <fstream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Classification;
using namespace std;

boolean CBoxAlgorithmClassifierProcessor::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	CIdentifier l_oClassifierAlgorithmClassIdentifier;
	CString l_sClassifierAlgorithmClassIdentifier;
	l_rStaticBoxContext.getSettingValue(0, l_sClassifierAlgorithmClassIdentifier);
	l_oClassifierAlgorithmClassIdentifier=this->getTypeManager().getEnumerationEntryValueFromName(OVTK_TypeId_ClassificationAlgorithm, l_sClassifierAlgorithmClassIdentifier);

	if(l_oClassifierAlgorithmClassIdentifier==OV_UndefinedIdentifier)
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Unknown classifier algorithm [" << l_sClassifierAlgorithmClassIdentifier << "]\n";
		return false;
	}

	CString l_sConfigurationFilename;
	l_rStaticBoxContext.getSettingValue(1, l_sConfigurationFilename);

	for(uint32 i=2; i<l_rStaticBoxContext.getSettingCount(); i++)
	{
		CString l_sStimulationName;
		l_rStaticBoxContext.getSettingValue(i, l_sStimulationName);
		m_vStimulation[i-2]=this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, l_sStimulationName);
	}

	m_pFeaturesDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamDecoder));
	m_pLabelsEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pClassificationStateEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	m_pClassifier=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(l_oClassifierAlgorithmClassIdentifier));

	m_pFeaturesDecoder->initialize();
	m_pLabelsEncoder->initialize();
	m_pClassificationStateEncoder->initialize();
	m_pClassifier->initialize();

	m_pClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVector)->setReferenceTarget(m_pFeaturesDecoder->getOutputParameter(OVP_GD_Algorithm_FeatureVectorStreamDecoder_OutputParameterId_Matrix));
	m_pClassificationStateEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ClassificationValues));

	TParameterHandler < IMemoryBuffer* > ip_pClassificationConfiguration(m_pClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_Configuration));
	IMemoryBuffer* l_pConfigurationFile=ip_pClassificationConfiguration;
	ifstream l_oFile(l_sConfigurationFilename.toASCIIString(), ios::binary);
	if(l_oFile.is_open())
	{
		size_t l_iFileLen;
		l_oFile.seekg(0, ios::end);
		l_iFileLen=l_oFile.tellg();
		l_oFile.seekg(0, ios::beg);
		l_pConfigurationFile->setSize(l_iFileLen, true);
		l_oFile.read((char*)l_pConfigurationFile->getDirectPointer(), l_iFileLen);
		l_oFile.close();
		m_pClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_LoadConfiguration);
	}
	else
	{
		this->getLogManager() << LogLevel_Warning << "Could not load configuration from file [" << l_sConfigurationFilename << "]\n";
	}

	m_bOutputHeaderSent=false;
	return true;
}

boolean CBoxAlgorithmClassifierProcessor::uninitialize(void)
{
	m_pClassifier->uninitialize();
	m_pClassificationStateEncoder->uninitialize();
	m_pLabelsEncoder->uninitialize();
	m_pFeaturesDecoder->uninitialize();

	this->getAlgorithmManager().releaseAlgorithm(*m_pClassifier);
	this->getAlgorithmManager().releaseAlgorithm(*m_pClassificationStateEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_pLabelsEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_pFeaturesDecoder);

	return true;
}

boolean CBoxAlgorithmClassifierProcessor::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmClassifierProcessor::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		uint64 l_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(0, i);
		uint64 l_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(0, i);

		TParameterHandler < const IMemoryBuffer* > ip_pFeatureVectorMemoryBuffer(m_pFeaturesDecoder->getInputParameter(OVP_GD_Algorithm_FeatureVectorStreamDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler < IMemoryBuffer* > op_pLabelsMemoryBuffer(m_pLabelsEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		TParameterHandler < IMemoryBuffer* > op_pClassificationStateMemoryBuffer(m_pClassificationStateEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

		TParameterHandler < IStimulationSet* > ip_pLabelsStimulationSet(m_pLabelsEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
		TParameterHandler < float64 > op_f64ClassificationStateClass(m_pClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Class));

		ip_pFeatureVectorMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		op_pLabelsMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
		op_pClassificationStateMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(1);

		m_pFeaturesDecoder->process();
		if(m_pFeaturesDecoder->isOutputTriggerActive(OVP_GD_Algorithm_FeatureVectorStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_bOutputHeaderSent=false;
		}
		if(m_pFeaturesDecoder->isOutputTriggerActive(OVP_GD_Algorithm_FeatureVectorStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			if(m_pClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_Classify))
			{
				if(!m_bOutputHeaderSent)
				{
					m_pLabelsEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
					m_pClassificationStateEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
					l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64StartTime);
					l_rDynamicBoxContext.markOutputAsReadyToSend(1, l_ui64StartTime, l_ui64StartTime);
					m_bOutputHeaderSent=true;
				}

				ip_pLabelsStimulationSet->setStimulationCount(1);
				ip_pLabelsStimulationSet->setStimulationIdentifier(0, m_vStimulation[op_f64ClassificationStateClass]);
				ip_pLabelsStimulationSet->setStimulationDate(0, l_ui64EndTime);
				ip_pLabelsStimulationSet->setStimulationDuration(0, 0);

				m_pLabelsEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
				m_pClassificationStateEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
				l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64EndTime);
				l_rDynamicBoxContext.markOutputAsReadyToSend(1, l_ui64StartTime, l_ui64EndTime);
			}
		}
		if(m_pFeaturesDecoder->isOutputTriggerActive(OVP_GD_Algorithm_FeatureVectorStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pLabelsEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeEnd);
			m_pClassificationStateEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64EndTime);
			l_rDynamicBoxContext.markOutputAsReadyToSend(1, l_ui64StartTime, l_ui64EndTime);
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
