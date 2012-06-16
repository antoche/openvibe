#include "ovpCSimpleDSP.h"

#include <system/Memory.h>

#include <iostream>
#include <sstream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace std;

CSimpleDSP::CSimpleDSP(void)
	:m_pStreamEncoder(NULL)
	,m_pEquationParser(NULL)
	,m_ui64EquationType(OP_USERDEF)
	,m_f64SpecialEquationParameter(0)
{
}

boolean CSimpleDSP::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	uint32 i;

	m_ppVariable=new float64*[l_rStaticBoxContext.getInputCount()];
	if(!m_ppVariable)
	{
		return false;
	}

	CString l_sEquation=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_pEquationParser=new CEquationParser(*this, m_ppVariable, l_rStaticBoxContext.getInputCount());
	if(!m_pEquationParser->compileEquation(l_sEquation.toASCIIString()))
	{
		return false;
	}
	m_ui64EquationType=m_pEquationParser->getTreeCategory();
	m_f64SpecialEquationParameter=m_pEquationParser->getTreeParameter();

	CIdentifier l_oStreamType;
	l_rStaticBoxContext.getOutputType(0, l_oStreamType);
	if(!this->getTypeManager().isDerivedFromStream(l_oStreamType, OV_TypeId_StreamedMatrix))
	{
		return false;
	}
	if(l_oStreamType==OV_TypeId_StreamedMatrix)
	{
		m_pStreamEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
		m_pStreamEncoder->initialize();
		for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
		{
			IAlgorithmProxy* l_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
			l_pStreamDecoder->initialize();
			m_vStreamDecoder.push_back(l_pStreamDecoder);
		}
	}
	else if(l_oStreamType==OV_TypeId_FeatureVector)
	{
		m_pStreamEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
		m_pStreamEncoder->initialize();
		for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
		{
			IAlgorithmProxy* l_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamDecoder));
			l_pStreamDecoder->initialize();
			m_vStreamDecoder.push_back(l_pStreamDecoder);
		}
	}
	else if(l_oStreamType==OV_TypeId_Signal)
	{
		m_pStreamEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
		m_pStreamEncoder->initialize();
		for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
		{
			IAlgorithmProxy* l_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
			l_pStreamDecoder->initialize();
			TParameterHandler<uint64> ip_ui64SamplingRate(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));
			TParameterHandler<uint64> op_ui64SamplingRate(l_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
			ip_ui64SamplingRate.setReferenceTarget(op_ui64SamplingRate);
			m_vStreamDecoder.push_back(l_pStreamDecoder);
		}
	}
	else if(l_oStreamType==OV_TypeId_Spectrum)
	{
		m_pStreamEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
		m_pStreamEncoder->initialize();
		for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
		{
			IAlgorithmProxy* l_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
			l_pStreamDecoder->initialize();
			TParameterHandler<IMatrix*> op_ui64MinMaxFrequencyBands(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands));
			TParameterHandler<IMatrix*> ip_ui64MinMaxFrequencyBands(l_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));
			ip_ui64MinMaxFrequencyBands.setReferenceTarget(op_ui64MinMaxFrequencyBands);
			m_vStreamDecoder.push_back(l_pStreamDecoder);
		}
	}
	else
	{
		this->getLogManager() << LogLevel_ImportantWarning << "The type [name=" << this->getTypeManager().getTypeName(l_oStreamType) << ":id=" << l_oStreamType << "] is not yet implemented\n";
		return false;
	}

	m_bCheckChunkDates=this->getConfigurationManager().expandAsBoolean("${Plugin_SignalProcessing_SimpleDSP_CheckChunkDates}", true);
	this->getLogManager() << LogLevel_Trace << (m_bCheckChunkDates?"Checking chunk dates...":"Not checking chunk dates !") << "\n";

	return true;
}

boolean CSimpleDSP::uninitialize(void)
{
	std::vector < IAlgorithmProxy* >::iterator it;
	for(it=m_vStreamDecoder.begin(); it!=m_vStreamDecoder.end(); it++)
	{
		(*it)->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(**it);
	}
	m_vStreamDecoder.clear();

	if(m_pStreamEncoder)
	{
		m_pStreamEncoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
		m_pStreamEncoder=NULL;
	}

	delete m_pEquationParser;
	m_pEquationParser=NULL;

	delete [] m_ppVariable;
	m_ppVariable=NULL;

	return true;
}

boolean CSimpleDSP::processInput(uint32 ui32InputIndex)
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
		if(m_bCheckChunkDates)
		{
			boolean l_bValidDates=true;
			if(l_ui64StartTime!=l_rDynamicBoxContext.getInputChunkStartTime(i, 0)) { l_bValidDates=false; }
			if(l_ui64EndTime!=l_rDynamicBoxContext.getInputChunkEndTime(i, 0)) { l_bValidDates=false; }
			if(!l_bValidDates)
			{
				this->getLogManager() << LogLevel_Warning << "Chunk dates mismatch, check stream structure or if turn " << CString("Plugin_SignalProcessing_SimpleDSP_CheckChunkDates") << " to " << false << " in your configuration file (" << this->getConfigurationManager().expand("${CustomConfiguration}") << ")\n";
				return l_bValidDates;
			}
		}
	}

	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CSimpleDSP::process(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IDynamicBoxContext& l_rDynamicBoxContext=this->getDynamicBoxContext();

	uint32 i;
	uint32 l_ui32HeaderCount=0;
	uint32 l_ui32BufferCount=0;
	uint32 l_ui32EndCount=0;

	TParameterHandler < IMatrix* > ip_pMatrix(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
	TParameterHandler < IMemoryBuffer* > op_pMemoryBuffer(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	m_vMatrix.clear();

	op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
	for(i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(m_vStreamDecoder[i]->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler < IMatrix* > op_pMatrix(m_vStreamDecoder[i]->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(i, 0);
		m_vStreamDecoder[i]->process();
		if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			if(i!=0)
			{
				if(m_vMatrix[0]->getBufferElementCount() != op_pMatrix->getBufferElementCount())
				{
					this->getLogManager() << LogLevel_Warning << "Matrix dimension mismatch\n";
					return false;
				}
			}
			l_ui32HeaderCount++;
		}
		if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			l_ui32BufferCount++;
		}
		if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			l_ui32EndCount++;
		}
		m_vMatrix.push_back(op_pMatrix);
		l_rDynamicBoxContext.markInputAsDeprecated(i, 0);
	}

	if((l_ui32HeaderCount && l_ui32HeaderCount!=l_rStaticBoxContext.getInputCount())
	|| (l_ui32BufferCount && l_ui32BufferCount!=l_rStaticBoxContext.getInputCount())
	|| (l_ui32EndCount && l_ui32EndCount!=l_rStaticBoxContext.getInputCount()))
	{
		this->getLogManager() << LogLevel_Warning << "Stream structure mismatch\n";
		return false;
	}

	if(l_ui32HeaderCount)
	{
		OpenViBEToolkit::Tools::Matrix::copyDescription(*ip_pMatrix, *m_vMatrix[0]);
		m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
	}
	if(l_ui32BufferCount)
	{
		this->evaluate();
		m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
	}
	if(l_ui32EndCount)
	{
		m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
	}

	if(l_ui32HeaderCount || l_ui32BufferCount || l_ui32EndCount)
	{
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, 0), l_rDynamicBoxContext.getInputChunkEndTime(0, 0));
	}

	return true;
}

void CSimpleDSP::evaluate(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		m_ppVariable[i]=m_vMatrix[i]->getBuffer();
	}

	TParameterHandler < IMatrix* > ip_pMatrix(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
	float64* l_pBuffer=ip_pMatrix->getBuffer();
	float64* l_pBufferEnd=ip_pMatrix->getBuffer() + ip_pMatrix->getBufferElementCount();

	while(l_pBuffer!=l_pBufferEnd)
	{
		*l_pBuffer=m_pEquationParser->executeEquation();

		for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
		{
			m_ppVariable[i]++;
		}

		l_pBuffer++;
	}

#if 0
	switch(m_ui64EquationType)
	{
		//The equation is not a special one, we have to execute the whole stack of function calls
		case OP_USERDEF:
			//for every samples
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_f64Variable = pBuffer[i];
				m_pMatrixBuffer[i] = m_pEquationParser->executeEquation();
			}
			break;

		case OP_X2:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = pBuffer[i]*pBuffer[i];
			}
			break;

		case OP_NONE:
			System::Memory::copy(m_pMatrixBuffer, pBuffer, m_ui64MatrixBufferSize*sizeof(float64));
			break;

		case OP_ABS:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = abs(pBuffer[i]);
			}
			break;
		case OP_ACOS:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = acos(pBuffer[i]);
			}
			break;

		case OP_ASIN:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = asin(pBuffer[i]);
			}
			break;

		case OP_ATAN:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = atan(pBuffer[i]);
			}
			break;

		case OP_CEIL:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = ceil(pBuffer[i]);
			}
			break;

		case OP_COS:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = cos(pBuffer[i]);
			}
			break;

		case OP_EXP:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = exp(pBuffer[i]);
			}
			break;

		case OP_FLOOR:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = floor(pBuffer[i]);
			}
			break;

		case OP_LOG:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = log(pBuffer[i]);
			}
			break;

		case OP_LOG10:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = log10(pBuffer[i]);
			}
			break;

		case OP_SIN:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = sin(pBuffer[i]);
			}
			break;

		case OP_SQRT:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = sqrt(pBuffer[i]);
			}
			break;

		case OP_TAN:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = tan(pBuffer[i]);
			}
			break;

		case OP_ADD:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = pBuffer[i] + m_f64SpecialEquationParameter;
			}
			break;

		case OP_MUL:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = pBuffer[i] * m_f64SpecialEquationParameter;
			}
			break;

		case OP_DIV:
			for(uint64 i=0 ; i<m_ui64MatrixBufferSize ; i++)
			{
				m_pMatrixBuffer[i] = pBuffer[i] / m_f64SpecialEquationParameter;
			}
			break;
	}
#endif
}
