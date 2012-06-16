#include "ovpCBoxAlgorithmLevelMeasure.h"

#include "../../algorithms/simple-visualisation/ovpCAlgorithmLevelMeasure.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

boolean CBoxAlgorithmLevelMeasure::initialize(void)
{
	// CString l_sSettingValue;
	// getStaticBoxContext().getSettingValue(0, l_sSettingValue);
	// ...

	m_pMatrix=new CMatrix();

	m_pStreamedMatrixDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
	m_pLevelMeasure=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_LevelMeasure));

	m_pStreamedMatrixDecoder->initialize();
	m_pLevelMeasure->initialize();

	ip_pStreamedMatrixDecoderMemoryBuffer.initialize(m_pStreamedMatrixDecoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pStreamedMatrixDecoderMatrix.initialize(m_pStreamedMatrixDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));

	ip_pLevelMeasureMatrix.initialize(m_pLevelMeasure->getInputParameter(OVP_Algorithm_LevelMeasure_InputParameterId_Matrix));
	op_pLevelMeasureMainWidget.initialize(m_pLevelMeasure->getOutputParameter(OVP_Algorithm_LevelMeasure_OutputParameterId_MainWidget));
	op_pLevelMeasureToolbarWidget.initialize(m_pLevelMeasure->getOutputParameter(OVP_Algorithm_LevelMeasure_OutputParameterId_ToolbarWidget));

	op_pStreamedMatrixDecoderMatrix.setReferenceTarget(m_pMatrix);
	ip_pLevelMeasureMatrix.setReferenceTarget(m_pMatrix);

	return true;
}

boolean CBoxAlgorithmLevelMeasure::uninitialize(void)
{
	op_pLevelMeasureToolbarWidget.uninitialize();
	op_pLevelMeasureMainWidget.uninitialize();
	ip_pLevelMeasureMatrix.uninitialize();

	op_pStreamedMatrixDecoderMatrix.uninitialize();
	ip_pStreamedMatrixDecoderMemoryBuffer.uninitialize();

	m_pLevelMeasure->uninitialize();
	m_pStreamedMatrixDecoder->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_pLevelMeasure);
	getAlgorithmManager().releaseAlgorithm(*m_pStreamedMatrixDecoder);

	delete m_pMatrix;
	m_pMatrix=NULL;

	return true;
}

boolean CBoxAlgorithmLevelMeasure::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmLevelMeasure::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		ip_pStreamedMatrixDecoderMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		m_pStreamedMatrixDecoder->process();
		if(m_pStreamedMatrixDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_pLevelMeasure->process(OVP_Algorithm_LevelMeasure_InputTriggerId_Reset);
			getVisualisationContext().setWidget(op_pLevelMeasureMainWidget);
			getVisualisationContext().setToolbar(op_pLevelMeasureToolbarWidget);
		}

		if(m_pStreamedMatrixDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{

// ----- >8 ------------------------------------------------------------------------------------------------------------------------------------------------------
// should be done in a processing box !

			float64 l_f64Sum=0;

			{
				float64* l_pMatrixBuffer=m_pMatrix->getBuffer();
				uint32 l_ui32ElementCount=m_pMatrix->getBufferElementCount();
				while(l_ui32ElementCount--)
				{
					l_f64Sum+=*l_pMatrixBuffer;
					l_pMatrixBuffer++;
				}
			}

			{
				float64 l_f64Factor=(l_f64Sum!=0?1./l_f64Sum:0.5);
				float64* l_pMatrixBuffer=m_pMatrix->getBuffer();
				uint32 l_ui32ElementCount=m_pMatrix->getBufferElementCount();
				while(l_ui32ElementCount--)
				{
					*l_pMatrixBuffer*=l_f64Factor;
					l_pMatrixBuffer++;
				}
			}

// ----- >8 ------------------------------------------------------------------------------------------------------------------------------------------------------

			m_pLevelMeasure->process(OVP_Algorithm_LevelMeasure_InputTriggerId_Refresh);
		}

		if(m_pStreamedMatrixDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
		}

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
