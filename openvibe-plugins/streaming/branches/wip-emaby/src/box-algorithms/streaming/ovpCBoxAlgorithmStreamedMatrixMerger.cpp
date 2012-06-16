#include "ovpCBoxAlgorithmStreamedMatrixMerger.h"

#include <system/Memory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Streaming;





boolean CBoxAlgorithmStreamedMatrixMerger::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		IAlgorithmProxy* l_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
		l_pStreamDecoder->initialize();
		m_vStreamDecoder.push_back(l_pStreamDecoder);
	}

	m_pStreamEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	m_pStreamEncoder->initialize();

	return true;
}



boolean CBoxAlgorithmStreamedMatrixMerger::uninitialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	m_pStreamEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		IAlgorithmProxy* l_pStreamDecoder=m_vStreamDecoder[i];
		l_pStreamDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*l_pStreamDecoder);
	}
	m_vStreamDecoder.clear();

	return true;
}

boolean CBoxAlgorithmStreamedMatrixMerger::processInput(uint32 ui32InputIndex)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IDynamicBoxContext& l_rDynamicBoxContext=this->getDynamicBoxContext();

	if(l_rDynamicBoxContext.getInputChunkCount(0) == 0)
	{
		return true;
	}

	uint64 l_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(0, 0);
	uint64 l_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(0, 0);
	for(uint32 i=1; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		if(l_rDynamicBoxContext.getInputChunkCount(i)==0)
		{
			return true;
		}
		if(l_ui64StartTime!=l_rDynamicBoxContext.getInputChunkStartTime(i, 0)) { return false; }
		if(l_ui64EndTime!=l_rDynamicBoxContext.getInputChunkEndTime(i, 0)) { return false; }
	}
  
  
  
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}


boolean CBoxAlgorithmStreamedMatrixMerger::process(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	
	uint32 i, j, k;
	uint32 l_ui32RowsCountPerSentBlock=0;
	uint32 l_ui32ColumnsCount=0;
	uint32 l_ui32HeaderCount=0;
	uint32 l_ui32BufferCount=0;
	uint32 l_ui32EndCount=0;
	
	this->getLogManager() << LogLevel_Info << "l_rStaticBoxContext.getInputCount() " << l_rStaticBoxContext.getInputCount() << " --\n";

	
	for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(m_vStreamDecoder[i]->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler < IMatrix* > op_pMatrix(m_vStreamDecoder[i]->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));

		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(i, 0);

		m_vStreamDecoder[i]->process();
		if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			l_ui32HeaderCount++;
			if(i==0)
			{
				l_ui32RowsCountPerSentBlock=op_pMatrix->getDimensionSize(1);
				l_ui32ColumnsCount=op_pMatrix->getDimensionSize(0);
			}
			else
			{
				if(l_ui32RowsCountPerSentBlock!=op_pMatrix->getDimensionSize(1)) { return false; }
				l_ui32ColumnsCount+=op_pMatrix->getDimensionSize(0);
			}
		}
		
			this->getLogManager() << LogLevel_Info << "l_ui32ColumnsCount " << l_ui32ColumnsCount << " --\n";
			this->getLogManager() << LogLevel_Info << "l_ui32RowsCountPerSentBlock " << l_ui32RowsCountPerSentBlock << " --\n";

		if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			l_ui32BufferCount++;
			this->getLogManager() << LogLevel_Info << "l_ui32BufferCount " << l_ui32BufferCount << " --\n";

		}
		if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			l_ui32EndCount++;	
			this->getLogManager() << LogLevel_Info << "l_ui32EndCount " << l_ui32EndCount << " --\n";

		}


		l_rDynamicBoxContext.markInputAsDeprecated(i, 0);
	}

	if(l_ui32HeaderCount && l_ui32HeaderCount!=l_rStaticBoxContext.getInputCount()) { return false; }
	if(l_ui32BufferCount && l_ui32BufferCount!=l_rStaticBoxContext.getInputCount()) { return false; }
	if(l_ui32EndCount && l_ui32EndCount!=l_rStaticBoxContext.getInputCount()) { return false; }

	TParameterHandler < IMatrix* > ip_pMatrix(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
	TParameterHandler < IMemoryBuffer* > op_pMemoryBuffer(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);

	if(l_ui32HeaderCount)
	{
		k=0;
		ip_pMatrix->setDimensionCount(2);
		ip_pMatrix->setDimensionSize(0, l_ui32ColumnsCount);
		ip_pMatrix->setDimensionSize(1, l_ui32RowsCountPerSentBlock);
		for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
		{
			TParameterHandler < IMatrix* > op_pMatrix(m_vStreamDecoder[i]->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
			for(j=0; j<op_pMatrix->getDimensionSize(0); j++, k++)
			{
				ip_pMatrix->setDimensionLabel(0, k, op_pMatrix->getDimensionLabel(0, j));
			}
		}
		m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
	}

	l_ui32RowsCountPerSentBlock=ip_pMatrix->getDimensionSize(1);

	if(l_ui32BufferCount)
	{
		k=0;
		for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
		{
			TParameterHandler < IMatrix* > op_pMatrix(m_vStreamDecoder[i]->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
			for(j=0; j<op_pMatrix->getDimensionSize(0); j++, k++)
			{
				System::Memory::copy(ip_pMatrix->getBuffer() + k*l_ui32RowsCountPerSentBlock, op_pMatrix->getBuffer() + j*l_ui32RowsCountPerSentBlock, l_ui32RowsCountPerSentBlock*sizeof(float64));
			}
		this->getLogManager() << LogLevel_Info << "op_pMatrix->getDimensionSize(0) " << op_pMatrix->getDimensionSize(0) << " --\n";

		}
		m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
	}

	if(l_ui32EndCount)
	{
		m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
	}

	if(l_ui32HeaderCount!=0 || l_ui32BufferCount!=0 || l_ui32EndCount!=0)
	{
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, 0), l_rDynamicBoxContext.getInputChunkEndTime(0, 0));
	}

	return true;
}



