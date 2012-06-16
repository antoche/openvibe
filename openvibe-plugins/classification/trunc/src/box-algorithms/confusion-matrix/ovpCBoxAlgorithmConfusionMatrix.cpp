#include "ovpCBoxAlgorithmConfusionMatrix.h"

#include "../../algorithms/confusion-matrix/ovpCAlgorithmConfusionMatrix.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Classification;
using namespace std;

#define uint32 OpenViBE::uint32
#define uint64 OpenViBE::uint64

boolean CBoxAlgorithmConfusionMatrix::initialize(void)
{
	//CLASSIFIER RESULTS DECODER
	m_pClassifierStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pClassifierStimulationDecoder->initialize();
	//IO for the classifier stim decoder
	ip_pClassifierMemoryBufferToDecode.initialize(m_pClassifierStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pClassifierStimulationSetDecoded.initialize(m_pClassifierStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	//TARGETS DECODER
	m_pTargetStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pTargetStimulationDecoder->initialize();
	//IO for the targets stim decoder
	ip_pTargetMemoryBufferToDecode.initialize(m_pTargetStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pTargetStimulationSetDecoded.initialize(m_pTargetStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	//CONFUSION MATRIX ALGORITHM
	m_pConfusionMatrixAlgorithm = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ConfusionMatrix));
	m_pConfusionMatrixAlgorithm->initialize();
	//IO for the confusion matrix algorithm
	ip_pTargetStimulationSet.initialize(m_pConfusionMatrixAlgorithm->getInputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_TargetStimulationSet));
	ip_pClassifierStimulationSet.initialize(m_pConfusionMatrixAlgorithm->getInputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_ClassifierStimulationSet));
	ip_bPercentages.initialize(m_pConfusionMatrixAlgorithm->getInputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_Percentage));
	ip_bSums.initialize(m_pConfusionMatrixAlgorithm->getInputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_Sums));
	op_pConfusionMatrix.initialize(m_pConfusionMatrixAlgorithm->getOutputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_OutputParameterId_ConfusionMatrix));

	//MATRIX ENCODER
	m_pConfusionMatrixEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	m_pConfusionMatrixEncoder->initialize();

	CString l_sPercentageSetting;
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(0,l_sPercentageSetting);
	ip_bPercentages = this->getConfigurationManager().expandAsBoolean(l_sPercentageSetting);

	CString l_sSumsSetting;
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(1,l_sSumsSetting);
	ip_bSums = this->getConfigurationManager().expandAsBoolean(l_sSumsSetting);

	m_ui32ClassCount=getBoxAlgorithmContext()->getStaticBoxContext()->getSettingCount() - FIRST_CLASS_SETTING_INDEX;
	vector < uint64 > l_vClassCodes;
	l_vClassCodes.resize(m_ui32ClassCount);
	for(uint32 i = 0; i< m_ui32ClassCount; i++)
	{
		CString l_sClassValue;
		getStaticBoxContext().getSettingValue(i+FIRST_CLASS_SETTING_INDEX, l_sClassValue); // classes are settings from 2 to n
		l_vClassCodes[i] =(uint64)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), i+FIRST_CLASS_SETTING_INDEX);
	}
	// verification...
	for(uint32 i = 0; i< m_ui32ClassCount; i++)
	{
		for(uint32 j = i+1; j< m_ui32ClassCount; j++)
		{
			if(l_vClassCodes[i] == l_vClassCodes[j])
			{
				CString l_sClassValue;
				getStaticBoxContext().getSettingValue(i+FIRST_CLASS_SETTING_INDEX, l_sClassValue);
				getLogManager() << LogLevel_Error << "You must use unique classes to compute a confusion matrix. Class "<<i+1<<" and "<<j+1<< " are the same ("<<l_sClassValue.toASCIIString()<<").\n";
				return false;
			}
		}
	}

	ip_pClassesCodes.initialize(m_pConfusionMatrixAlgorithm->getInputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_ClassCodes));

	for(uint32 i = 0 ; i<l_vClassCodes.size(); i++)
	{
		ip_pClassesCodes->appendStimulation(l_vClassCodes[i],0,0);
	}

	// setting reference targets
	m_pConfusionMatrixAlgorithm->getInputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_ClassifierStimulationSet)->setReferenceTarget(m_pClassifierStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
	m_pConfusionMatrixAlgorithm->getInputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_TargetStimulationSet)->setReferenceTarget(m_pTargetStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	m_pConfusionMatrixEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pConfusionMatrixAlgorithm->getOutputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_OutputParameterId_ConfusionMatrix));

	return true;
}

boolean CBoxAlgorithmConfusionMatrix::uninitialize(void)
{
	//IO for the classifier results
	op_pClassifierStimulationSetDecoded.uninitialize();
	ip_pClassifierMemoryBufferToDecode.uninitialize();

	//IO for the targets
	op_pTargetStimulationSetDecoded.uninitialize();
	ip_pTargetMemoryBufferToDecode.uninitialize();

	//IO for the confusion matrix algorithm
	op_pConfusionMatrix.uninitialize();
	ip_pClassifierStimulationSet.uninitialize();
	ip_pTargetStimulationSet.uninitialize();
	ip_pClassesCodes.uninitialize();
	ip_bPercentages.uninitialize();

	//CLASSIFIER RESULTS DECODER
	m_pClassifierStimulationDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*(m_pClassifierStimulationDecoder));

	//TARGETS DECODER
	m_pTargetStimulationDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pTargetStimulationDecoder);

	//CONFUSION MATRIX
	m_pConfusionMatrixAlgorithm->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pConfusionMatrixAlgorithm);

	//MATRIX ENCODER
	m_pConfusionMatrixEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pConfusionMatrixEncoder);

	return true;
}

boolean CBoxAlgorithmConfusionMatrix::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmConfusionMatrix::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	//Input 0: Targets
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle(m_pConfusionMatrixEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		l_oOutputMemoryBufferHandle=l_rDynamicBoxContext.getOutputChunk(0);

		ip_pTargetMemoryBufferToDecode=l_rDynamicBoxContext.getInputChunk(0, i);
		m_pTargetStimulationDecoder->process();

		if(m_pTargetStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_pConfusionMatrixAlgorithm->process(OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_ResetTarget);
			m_pConfusionMatrixEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
			m_ui64CurrentProcessingTimeLimit = 0;
		}

		if(m_pTargetStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			uint64 l_ui64ChunkEndTime = l_rDynamicBoxContext.getInputChunkEndTime(0, i);
			m_ui64CurrentProcessingTimeLimit = (l_ui64ChunkEndTime>m_ui64CurrentProcessingTimeLimit?l_ui64ChunkEndTime:m_ui64CurrentProcessingTimeLimit);

			m_pConfusionMatrixAlgorithm->process(OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_FeedTarget);

		}

		if(m_pTargetStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pConfusionMatrixEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	//Input 1: Classifier results
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(1); i++)
	{
		TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle(m_pConfusionMatrixEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		l_oOutputMemoryBufferHandle=l_rDynamicBoxContext.getOutputChunk(0);

		uint64 l_ui64ChunkEndTime = l_rDynamicBoxContext.getInputChunkEndTime(1,i);
		if(l_ui64ChunkEndTime <= m_ui64CurrentProcessingTimeLimit)
		{
			ip_pClassifierMemoryBufferToDecode=l_rDynamicBoxContext.getInputChunk(1, i);
			m_pClassifierStimulationDecoder->process();

			if(m_pClassifierStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
				m_pConfusionMatrixAlgorithm->process(OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_ResetClassifier);
			}

			if(m_pClassifierStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				m_pConfusionMatrixAlgorithm->process(OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_FeedClassifier);
				if(m_pConfusionMatrixAlgorithm->isOutputTriggerActive(OVP_Algorithm_ConfusionMatrixAlgorithm_OutputTriggerId_ConfusionPerformed))
				{
					m_pConfusionMatrixEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
					l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(1, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
				}
			}

			if(m_pClassifierStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
				m_pConfusionMatrixEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
				l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(1, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
			}

			l_rDynamicBoxContext.markInputAsDeprecated(1, i);
		}
	}

	return true;
}
