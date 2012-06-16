#include "ovpCEncoderAlgorithmTest.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

boolean CEncoderAlgorithmTest::initialize(void)
{
	m_pStreamEncoder[0]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ExperimentInformationStreamEncoder));
	m_pStreamEncoder[1]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_FeatureVectorStreamEncoder));
	m_pStreamEncoder[2]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoder[3]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SpectrumStreamEncoder));
	m_pStreamEncoder[4]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_StimulationStreamEncoder));
	m_pStreamEncoder[5]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	m_pStreamEncoder[6]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoder));

	for(uint32 i=0; i<7; i++)
	{
		m_pStreamEncoder[i]->initialize();
		op_pMemoryBuffer[i].initialize(m_pStreamEncoder[i]->getOutputParameter(OVP_Algorithm_EBMLStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	}

	m_pMatrix1=new CMatrix();
	m_pMatrix1->setDimensionCount(2);
	m_pMatrix1->setDimensionSize(0, 16);
	m_pMatrix1->setDimensionSize(1, 16);
	OpenViBEToolkit::Tools::Matrix::clearContent(*m_pMatrix1);

	m_pMatrix2=new CMatrix();
	m_pMatrix2->setDimensionCount(2);
	m_pMatrix2->setDimensionSize(0, 16);
	m_pMatrix2->setDimensionSize(1, 2);
	OpenViBEToolkit::Tools::Matrix::clearContent(*m_pMatrix2);

	m_pMatrix3=new CMatrix();
	m_pMatrix3->setDimensionCount(2);
	m_pMatrix3->setDimensionSize(0, 4);
	m_pMatrix3->setDimensionLabel(0, 0, "C3");
	m_pMatrix3->setDimensionLabel(0, 1, "Cz");
	m_pMatrix3->setDimensionLabel(0, 2, "C4");
	m_pMatrix3->setDimensionLabel(0, 3, "Pz");
	m_pMatrix3->setDimensionSize(1, 3);
	m_pMatrix3->setDimensionLabel(1, 0, "x");
	m_pMatrix3->setDimensionLabel(1, 1, "y");
	m_pMatrix3->setDimensionLabel(1, 2, "z");
	OpenViBEToolkit::Tools::Matrix::clearContent(*m_pMatrix3);

	m_pStimulationSet=new CStimulationSet();

	uint64 m_ui64SamplingRate=16;

	m_pStreamEncoder[1]->getInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setValue(&m_pMatrix1);
	m_pStreamEncoder[2]->getInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setValue(&m_pMatrix1);
	m_pStreamEncoder[2]->getInputParameter(OVP_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setValue(&m_ui64SamplingRate);
	m_pStreamEncoder[3]->getInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setValue(&m_pMatrix1);
	m_pStreamEncoder[3]->getInputParameter(OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands)->setValue(&m_pMatrix2);
	m_pStreamEncoder[4]->getInputParameter(OVP_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet)->setValue(&m_pStimulationSet);
	m_pStreamEncoder[5]->getInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setValue(&m_pMatrix2);
	m_pStreamEncoder[6]->getInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setValue(&m_pMatrix3);

	m_bHasSentHeader=false;
	m_ui64StartTime=0;
	m_ui64EndTime=0;

	return true;
}

boolean CEncoderAlgorithmTest::uininitialize(void)
{
	delete m_pStimulationSet;
	delete m_pMatrix2;
	delete m_pMatrix1;

	for(uint32 i=0; i<7; i++)
	{
		op_pMemoryBuffer[i].uninitialize();
		m_pStreamEncoder[i]->uninitialize();
		getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder[i]);
		m_pStreamEncoder[i]=NULL;
	}

	return true;
}

boolean CEncoderAlgorithmTest::processClock(IMessageClock& rMessageClock)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CEncoderAlgorithmTest::process(void)
{
	IBoxIO& l_rDynamicBoxContext=getDynamicBoxContext();
	IBox& l_rStaticBoxContext=getStaticBoxContext();
	IPlayerContext& l_rPlayerContext=getPlayerContext();

	if(!m_bHasSentHeader)
	{
		m_ui64StartTime=0;
		m_ui64EndTime=0;
		for(uint32 i=0; i<l_rStaticBoxContext.getOutputCount(); i++)
		{
			op_pMemoryBuffer[i]=l_rDynamicBoxContext.getOutputChunk(i);
			m_pStreamEncoder[i]->process(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeHeader);
		}
		m_bHasSentHeader=true;
	}
	else
	{
		for(uint32 i=0; i<l_rStaticBoxContext.getOutputCount(); i++)
		{
			op_pMemoryBuffer[i]=l_rDynamicBoxContext.getOutputChunk(i);
			m_pStreamEncoder[i]->process(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeBuffer);
		}
	}

	for(uint32 i=0; i<l_rStaticBoxContext.getOutputCount(); i++)
	{
		l_rDynamicBoxContext.markOutputAsReadyToSend(i, m_ui64StartTime, m_ui64EndTime);
	}

	m_ui64StartTime=m_ui64EndTime;
	m_ui64EndTime=l_rPlayerContext.getCurrentTime();

	return true;
}
