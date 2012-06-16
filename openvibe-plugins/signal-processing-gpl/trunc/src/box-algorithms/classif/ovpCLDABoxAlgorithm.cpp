#include "ovpCLDABoxAlgorithm.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;
using namespace std;

boolean CLDABoxAlgorithm::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	m_ui64NbEntries = getStaticBoxContext().getInputCount();

	m_vStreamDecoder.resize(getStaticBoxContext().getInputCount());
	m_vStreamDecoderEndTime.resize(getStaticBoxContext().getInputCount());
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder[i]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
		m_vStreamDecoder[i]->initialize();
		m_vStreamDecoderEndTime[i] = 0;
	}
	m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	m_pStreamEncoder->initialize();

	// Extract features
	m_pFeatureExtractionLda=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_FeatureExtractionLda));
	m_pFeatureExtractionLda->initialize();
	// Train Fisher Lda
	m_pComputeFisherLdaFunction=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ComputeFisherLdaFunction));
	m_pComputeFisherLdaFunction->initialize();
	// Compute Fisher Lda
	m_pApplyFisherLdaFunction=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ApplyFisherLdaFunction));
	m_pApplyFisherLdaFunction->initialize();

	// Get parameters
	CString l_oNbTrainingSamples;
	CString l_oParameters;
	CString l_oLoadTrainingParms;
	CString l_oSaveTrainingData;
	CString l_oLoadTrainingData;

	// Get LDA parameters
	getStaticBoxContext().getSettingValue(2, l_oLoadTrainingParms);
	m_bLoadTrainingParms = (l_oLoadTrainingParms == CString("true"));
	if (m_bLoadTrainingParms)
	{
		getStaticBoxContext().getSettingValue(3, m_sLoadTrainingParmsFilename);
	}
	else
	{
		getStaticBoxContext().getSettingValue(0, l_oNbTrainingSamples);
		getStaticBoxContext().getSettingValue(1, l_oParameters);
	}

	getStaticBoxContext().getSettingValue(4, l_oSaveTrainingData);
	m_bSaveTrainingData = (l_oSaveTrainingData == CString("true"));
	if (m_bSaveTrainingData)
	{
		getStaticBoxContext().getSettingValue(5, m_sSaveTrainingDataFilename);
	}
	getStaticBoxContext().getSettingValue(6, l_oLoadTrainingData);
	m_bLoadTrainingData = (l_oLoadTrainingData == CString("true"));
	if (m_bLoadTrainingData)
	{
		getStaticBoxContext().getSettingValue(7, m_sLoadTrainingDataFilename);
	}

	if (m_bLoadTrainingParms)
	{
		// Get from file
		ifstream l_oFileTrainingParms(m_sLoadTrainingParmsFilename.toASCIIString(), ifstream::in);
		if (l_oFileTrainingParms.is_open())
		{
			uint32 l_ui32Value;

			l_oFileTrainingParms >> l_ui32Value;
			m_ui64NbTrainingSamples = (uint64)l_ui32Value;
			for (uint32 i=0; i<m_ui64NbTrainingSamples; i++)
			{
				l_oFileTrainingParms >> l_ui32Value;
				m_oCoefficients.push_back((uint64)l_ui32Value);
			}

			l_oFileTrainingParms.close();
		}
		else
		{
			this->getLogManager() << LogLevel_Error << "Can't open parameter file " << m_sLoadTrainingParmsFilename << " .\n";
		}
	}
	else
	{
		// Get from parameter box
		m_ui64NbTrainingSamples=atoi(l_oNbTrainingSamples);
		istringstream l_oStreamString((const char*)l_oParameters);
		uint64 l_uint64CurrentValue;
		while(l_oStreamString>>l_uint64CurrentValue)
		{
			m_oCoefficients.push_back(l_uint64CurrentValue);
		}
	}

	IMatrix* l_pClass1SamplesNumbersInputMatrix = &m_oClass1SamplesNumbersInputMatrix;
	l_pClass1SamplesNumbersInputMatrix->setDimensionCount(2);
	l_pClass1SamplesNumbersInputMatrix->setDimensionSize(0,1);
	l_pClass1SamplesNumbersInputMatrix->setDimensionSize(1, m_ui64NbTrainingSamples);
	float64* l_pClass1SamplesNumbersInput = l_pClass1SamplesNumbersInputMatrix->getBuffer();
	for (uint32 i = 0; i<m_oCoefficients.size(); i++)
	{
		l_pClass1SamplesNumbersInput[i] = m_oCoefficients[i];
	}

	m_pFeatureExtractionLda->getInputParameter(OVP_Algorithm_FeatureExtractionLda_InputParameterId_Class1SamplesNumber)->setValue(&l_pClass1SamplesNumbersInputMatrix);

	m_pComputeFisherLdaFunction->getInputParameter(OVP_Algorithm_ComputeFisherLdaFunction_InputParameterId_MatrixFirstClass)->setReferenceTarget(m_pFeatureExtractionLda->getOutputParameter(OVP_Algorithm_FeatureExtractionLda_OutputParameterId_MatrixFirstClass));
	m_pComputeFisherLdaFunction->getInputParameter(OVP_Algorithm_ComputeFisherLdaFunction_InputParameterId_MatrixSecondClass)->setReferenceTarget(m_pFeatureExtractionLda->getOutputParameter(OVP_Algorithm_FeatureExtractionLda_OutputParameterId_MatrixSecondClass));

	m_pApplyFisherLdaFunction->getInputParameter(OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalMean)->setReferenceTarget(m_pComputeFisherLdaFunction->getOutputParameter(OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalMean));
	m_pApplyFisherLdaFunction->getInputParameter(OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalCovariance)->setReferenceTarget(m_pComputeFisherLdaFunction->getOutputParameter(OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalCovariance));
	m_pApplyFisherLdaFunction->getInputParameter(OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalProbability)->setReferenceTarget(m_pComputeFisherLdaFunction->getOutputParameter(OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalProbability));

	m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setReferenceTarget(m_pApplyFisherLdaFunction->getOutputParameter(OVP_Algorithm_ApplyFisherLdaFunction_OutputParameterId_FisherLdaDecision));

	m_ui64LastStartTime=0;
	m_ui64LastEndTime=0;

	m_ui64TrainingIndex = 0;
	m_ui64NbEntriesDecoded = 0;
	m_bHasSentHeader=false;
	m_bFirstTime = true;

	m_bNeedTraining = true;
	m_bNeedApply = false;

	if (m_bLoadTrainingData)
	{
		uint32 l_ui32MatDimCount, l_ui32MatDimSize, l_ui32MatElementCount;
		float64* l_pBufferMatrix;

		// Read from file
		TParameterHandler < IMatrix* > op_pLoadDataGLobalMean(m_pApplyFisherLdaFunction->getInputParameter(OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalMean));
		IMatrix* l_pLoadDataGlobalMean=op_pLoadDataGLobalMean;

		TParameterHandler < IMatrix* > op_pLoadDataGLobalCovariance(m_pApplyFisherLdaFunction->getInputParameter(OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalCovariance));
		IMatrix* l_pLoadDataGlobalCovariance=op_pLoadDataGLobalCovariance;

		TParameterHandler < IMatrix* > op_pLoadDataGLobalProbability(m_pApplyFisherLdaFunction->getInputParameter(OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalProbability));
		IMatrix* l_pLoadDataGlobalProbability=op_pLoadDataGLobalProbability;

		ifstream l_oFile(m_sLoadTrainingDataFilename.toASCIIString(), ios::binary);
		if(l_oFile.is_open())
		{
			// Global Mean Matrix

			// Read matrix dimensions
			l_oFile.read((char *)&l_ui32MatDimCount, sizeof(uint32));
			l_pLoadDataGlobalMean->setDimensionCount(l_ui32MatDimCount);

			l_ui32MatElementCount = 1;
			for (uint32 i=0; i<l_ui32MatDimCount; i++)
			{
				l_oFile.read((char *)&l_ui32MatDimSize, sizeof(uint32));
				l_pLoadDataGlobalMean->setDimensionSize(i, l_ui32MatDimSize);
				l_ui32MatElementCount *= l_ui32MatDimSize;
			}

			// Read matrix content
			l_pBufferMatrix = l_pLoadDataGlobalMean->getBuffer();
			l_oFile.read((char *)l_pBufferMatrix, l_ui32MatElementCount*sizeof(float64));

			// Global Covariance Matrix

			// Read matrix dimensions
			l_oFile.read((char *)&l_ui32MatDimCount, sizeof(uint32));
			l_pLoadDataGlobalCovariance->setDimensionCount(l_ui32MatDimCount);

			l_ui32MatElementCount = 1;
			for (uint32 i=0; i<l_ui32MatDimCount; i++)
			{
				l_oFile.read((char *)&l_ui32MatDimSize, sizeof(uint32));
				l_pLoadDataGlobalCovariance->setDimensionSize(i, l_ui32MatDimSize);
				l_ui32MatElementCount *= l_ui32MatDimSize;
			}

			// Read matrix content
			l_pBufferMatrix = l_pLoadDataGlobalCovariance->getBuffer();
			l_oFile.read((char *)l_pBufferMatrix, l_ui32MatElementCount*sizeof(float64));

			// Global Probability Matrix

			// Read matrix dimensions
			l_oFile.read((char *)&l_ui32MatDimCount, sizeof(uint32));
			l_pLoadDataGlobalProbability->setDimensionCount(l_ui32MatDimCount);

			l_ui32MatElementCount = 1;
			for (uint32 i=0; i<l_ui32MatDimCount; i++)
			{
				l_oFile.read((char *)&l_ui32MatDimSize, sizeof(uint32));
				l_pLoadDataGlobalProbability->setDimensionSize(i, l_ui32MatDimSize);
				l_ui32MatElementCount *= l_ui32MatDimSize;
			}

			// Read matrix content
			l_pBufferMatrix = l_pLoadDataGlobalProbability->getBuffer();
			l_oFile.read((char *)l_pBufferMatrix, l_ui32MatElementCount*sizeof(float64));

			l_oFile.close();

			this->getLogManager() << LogLevel_Debug << "File " << m_sLoadTrainingDataFilename << " loaded\n";
		}
		else
		{
			this->getLogManager() << LogLevel_Error << "Could not load training data from file [" << m_sLoadTrainingDataFilename << "]\n";
		}

		m_bNeedTraining = false;
		m_bNeedApply = true;
	}

	return true;
}

boolean CLDABoxAlgorithm::uninitialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	m_pStreamEncoder->uninitialize();
	getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder[i]->uninitialize();
		getAlgorithmManager().releaseAlgorithm(*m_vStreamDecoder[i]);
	}
	m_vStreamDecoder.clear();

	m_pFeatureExtractionLda->uninitialize();
	m_pComputeFisherLdaFunction->uninitialize();
	m_pApplyFisherLdaFunction->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_pFeatureExtractionLda);
	getAlgorithmManager().releaseAlgorithm(*m_pComputeFisherLdaFunction);
	getAlgorithmManager().releaseAlgorithm(*m_pApplyFisherLdaFunction);

	return true;
}

boolean CLDABoxAlgorithm::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CLDABoxAlgorithm::process(void)
{
	IBoxIO& l_rDynamicBoxContext=getDynamicBoxContext();
	IBox& l_rStaticBoxContext=getStaticBoxContext();

	TParameterHandler < IMemoryBuffer* > l_oOutputMemoryBufferHandle(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	uint64 l_ui64ReadChunkMinEndTime=0xffffffffffffffffll;
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			TParameterHandler < const IMemoryBuffer* > l_ipMemoryBuffer(m_vStreamDecoder[i]->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
			l_ipMemoryBuffer=l_rDynamicBoxContext.getInputChunk(i, j);
			m_vStreamDecoder[i]->process();

			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
				m_pFeatureExtractionLda->process(OVP_Algorithm_FeatureExtractionLda_InputTriggerId_Initialize);
			}

			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				m_oSignalIntputMatrixHandle = m_vStreamDecoder[i]->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix);
				IMatrix* l_pSignalInputMatrix = m_oSignalIntputMatrixHandle;
				float64* l_pSignalInput = l_pSignalInputMatrix->getBuffer();
				// dimensions of the input buffer of signal
				uint32 l_ui32SignalInputColumnDimensionSize=m_oSignalIntputMatrixHandle->getDimensionSize(1);

				if(m_bFirstTime)
				{
					m_pEpochTable = new CMatrix[m_ui64NbEntries*l_ui32SignalInputColumnDimensionSize];
					m_pEpochTable->setDimensionCount(2);
					m_pEpochTable->setDimensionSize(0,m_ui64NbEntries);
					m_pEpochTable->setDimensionSize(1,l_ui32SignalInputColumnDimensionSize);
					m_pf64EpochTable = m_pEpochTable->getBuffer();
					m_bFirstTime = false;
				}
				for (uint32 k=0; k< l_ui32SignalInputColumnDimensionSize; k++)
				{
					m_pf64EpochTable[k+i*l_ui32SignalInputColumnDimensionSize] = l_pSignalInput[k];
				}
//				this->getLogManager()<<LogLevel_Warning<<"epoch size="<<l_ui32SignalInputColumnDimensionSize<<"\n";

				m_ui64NbEntriesDecoded++;

			}

			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
			}

			m_vStreamDecoderEndTime[i]=l_rDynamicBoxContext.getInputChunkEndTime(i, j);
			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}

		if(l_ui64ReadChunkMinEndTime > m_vStreamDecoderEndTime[i])
		{
			l_ui64ReadChunkMinEndTime=m_vStreamDecoderEndTime[i];
		}

	}

	if (m_ui64NbEntriesDecoded == m_ui64NbEntries)
	{
		// Training
		if (m_bNeedTraining)
		{
			if (m_ui64TrainingIndex < m_ui64NbTrainingSamples)
			{
				this->getLogManager() << LogLevel_Debug << "m_ui64TrainingIndex = " <<m_ui64TrainingIndex+1 << " / " << m_ui64NbTrainingSamples << "\n";
				m_pFeatureExtractionLda->getInputParameter(OVP_Algorithm_FeatureExtractionLda_InputParameterId_EpochTable)->setValue(&m_pEpochTable);
				m_pFeatureExtractionLda->process(OVP_Algorithm_FeatureExtractionLda_InputTriggerId_ExtractFeature);

				m_ui64TrainingIndex++;
			}
			if (m_ui64TrainingIndex == m_ui64NbTrainingSamples)
			{
				// Compute
				m_pComputeFisherLdaFunction->process(OVP_Algorithm_ComputeFisherLdaFunction_InputTriggerId_ComputeFunction);
				this->getLogManager() << LogLevel_Debug << "LDA process done\n";

				if (m_bSaveTrainingData)
				{
					uint32 l_ui32MatDimCount, l_ui32MatDimSize, l_ui32MatElementCount;

					// Save to file
					TParameterHandler < IMatrix* > op_pSaveDataGLobalMean(m_pComputeFisherLdaFunction->getOutputParameter(OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalMean));
					const IMatrix* l_pSaveDataGlobalMean=op_pSaveDataGLobalMean;

					TParameterHandler < IMatrix* > op_pSaveDataGLobalCovariance(m_pComputeFisherLdaFunction->getOutputParameter(OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalCovariance));
					const IMatrix* l_pSaveDataGlobalCovariance=op_pSaveDataGLobalCovariance;

					TParameterHandler < IMatrix* > op_pSaveDataGLobalProbability(m_pComputeFisherLdaFunction->getOutputParameter(OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalProbability));
					const IMatrix* l_pSaveDataGlobalProbability=op_pSaveDataGLobalProbability;

					ofstream l_oFile(m_sSaveTrainingDataFilename.toASCIIString(), ios::binary);
					if(l_oFile.is_open())
					{
						// Global Mean Matrix

						// Write matrix dimensions
						l_ui32MatDimCount = l_pSaveDataGlobalMean->getDimensionCount();
						l_oFile.write((char*)&l_ui32MatDimCount, sizeof(uint32));

						l_ui32MatElementCount = 1;
						for (uint32 i=0; i<l_ui32MatDimCount; i++)
						{
							l_ui32MatDimSize = l_pSaveDataGlobalMean->getDimensionSize(i);
							l_ui32MatElementCount *= l_ui32MatDimSize;
							l_oFile.write((char*)&l_ui32MatDimSize, sizeof(uint32));
						}

						// Write matrix content
						l_oFile.write((char*)l_pSaveDataGlobalMean->getBuffer(), l_ui32MatElementCount*sizeof(float64));

						// Global Covariance Matrix

						// Write matrix dimensions
						l_ui32MatDimCount = l_pSaveDataGlobalCovariance->getDimensionCount();
						l_oFile.write((char*)&l_ui32MatDimCount, sizeof(uint32));

						l_ui32MatElementCount = 1;
						for (uint32 i=0; i<l_ui32MatDimCount; i++)
						{
							l_ui32MatDimSize = l_pSaveDataGlobalCovariance->getDimensionSize(i);
							l_ui32MatElementCount *= l_ui32MatDimSize;
							l_oFile.write((char*)&l_ui32MatDimSize, sizeof(uint32));
						}

						// Write matrix content
						l_oFile.write((char*)l_pSaveDataGlobalCovariance->getBuffer(), l_ui32MatElementCount*sizeof(float64));

						// Global Probability Matrix

						// Write matrix dimensions
						l_ui32MatDimCount = l_pSaveDataGlobalProbability->getDimensionCount();
						l_oFile.write((char*)&l_ui32MatDimCount, sizeof(uint32));

						l_ui32MatElementCount = 1;
						for (uint32 i=0; i<l_ui32MatDimCount; i++)
						{
							l_ui32MatDimSize = l_pSaveDataGlobalProbability->getDimensionSize(i);
							l_ui32MatElementCount *= l_ui32MatDimSize;
							l_oFile.write((char*)&l_ui32MatDimSize, sizeof(uint32));
						}

						// Write matrix content
						l_oFile.write((char*)l_pSaveDataGlobalProbability->getBuffer(), l_ui32MatElementCount*sizeof(float64));
						l_oFile.close();

						this->getLogManager() << LogLevel_Debug << "File " << m_sSaveTrainingDataFilename << " saved\n";
					}
					else
					{
						this->getLogManager() << LogLevel_Error << "Could not save training data to file [" << m_sSaveTrainingDataFilename << "]\n";
					}
				}

			m_bNeedTraining=false;
			m_bNeedApply=true;
			}
		}

		// Apply
		if (m_bNeedApply)
		{
			m_pApplyFisherLdaFunction->getInputParameter(OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixSignal)->setValue(&m_pEpochTable);
			m_pApplyFisherLdaFunction->process(OVP_Algorithm_ApplyFisherLdaFunction_InputTriggerId_ApplyFunction);

			if(!m_bHasSentHeader)
			{
				l_oOutputMemoryBufferHandle=l_rDynamicBoxContext.getOutputChunk(0);
				m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
				l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastEndTime, m_ui64LastEndTime);
				m_bHasSentHeader=true;
			}

			l_oOutputMemoryBufferHandle=l_rDynamicBoxContext.getOutputChunk(0);
			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastStartTime, l_ui64ReadChunkMinEndTime);
			m_ui64LastStartTime=m_ui64LastEndTime;
			m_ui64LastEndTime=l_ui64ReadChunkMinEndTime;
		}
		m_ui64NbEntriesDecoded=0;
	}

	return true;
}
