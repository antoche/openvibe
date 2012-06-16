#include "ovpCNaiveBayesApplyBoxAlgorithm.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <climits>
#include <cfloat>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;
using namespace std;

#define CSTE_DIV 500

boolean CNaiveBayesApplyBoxAlgorithm::initialize(void)
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

	// Compute Naive Bayes
	m_pNaiveBayesApplyFunction=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_NaiveBayesApplyFunction));
	m_pNaiveBayesApplyFunction->initialize();

	// Get NaiveBayesApply parameters (from Training)
	getStaticBoxContext().getSettingValue(0, m_sLoadTrainingDataFilename);

	float64* l_pBufferMatrix;

	// Read from file
	IMatrix *l_oMeanClassFeaturesMatrix=&m_oMeanClassFeaturesMatrix;
	IMatrix *l_oVarianceClassFeaturesMatrix=&m_oVarianceClassFeaturesMatrix;
	IMatrix *l_oLogTermVector=&m_oLogTermVector;

	ifstream l_oFile(m_sLoadTrainingDataFilename.toASCIIString(), ios::binary);
	if(l_oFile.is_open())
	{
		// Number of classes
		l_oFile.read((char *)&m_ui32NbClasses, sizeof(uint32));
		this -> getLogManager() << LogLevel_Trace << "Nb classes : " << m_ui32NbClasses << "\n";

		// Number of features
		l_oFile.read((char *)&m_ui32NbFeatures, sizeof(uint32));
		this -> getLogManager() << LogLevel_Trace << "Nb features : " << m_ui32NbFeatures << "\n";

		l_oMeanClassFeaturesMatrix->setDimensionCount(2);
		l_oVarianceClassFeaturesMatrix->setDimensionCount(2);
		l_oLogTermVector->setDimensionCount(2);

		l_oMeanClassFeaturesMatrix->setDimensionSize(0, m_ui32NbClasses);
		l_oVarianceClassFeaturesMatrix->setDimensionSize(0, m_ui32NbClasses);
		l_oLogTermVector->setDimensionSize(0, m_ui32NbClasses);

		l_oMeanClassFeaturesMatrix->setDimensionSize(1, m_ui32NbFeatures);
		l_oVarianceClassFeaturesMatrix->setDimensionSize(1, m_ui32NbFeatures);
		l_oLogTermVector->setDimensionSize(1, 1);

		for (uint32 i=0; i<m_ui32NbClasses; i++)
		{
			// Mean Matrix
			l_pBufferMatrix = l_oMeanClassFeaturesMatrix->getBuffer();
			l_oFile.read((char *)(l_pBufferMatrix+(i*m_ui32NbFeatures)), m_ui32NbFeatures*sizeof(float64));

			// Variance Matrix
			l_pBufferMatrix = l_oVarianceClassFeaturesMatrix->getBuffer();
			l_oFile.read((char *)(l_pBufferMatrix+(i*m_ui32NbFeatures)), m_ui32NbFeatures*sizeof(float64));

			// Log  Term Matrix (constant)
			l_pBufferMatrix = l_oLogTermVector->getBuffer();
			l_oFile.read((char *)(l_pBufferMatrix+i), sizeof(float64));
		}

		l_oFile.close();

		this->getLogManager() << LogLevel_Info << "File " << m_sLoadTrainingDataFilename << " loaded\n";
	}
	else
	{
		this->getLogManager() << LogLevel_Error << "Could not load training data from file [" << m_sLoadTrainingDataFilename << "]\n";
	}

	m_pNaiveBayesApplyFunction->getInputParameter(OVP_Algorithm_NaiveBayesApplyFunction_InputParameterId_MatrixMean)->setValue(&l_oMeanClassFeaturesMatrix);

	m_pNaiveBayesApplyFunction->getInputParameter(OVP_Algorithm_NaiveBayesApplyFunction_InputParameterId_MatrixVariance)->setValue(&l_oVarianceClassFeaturesMatrix);

	m_pNaiveBayesApplyFunction->getInputParameter(OVP_Algorithm_NaiveBayesApplyFunction_InputParameterId_LogTerm)->setValue(&l_oLogTermVector);

	m_oOutputFunctionalNB.setDimensionCount(1);
	m_oOutputFunctionalNB.setDimensionSize(0, 2);
	IMatrix *l_oOutputFunctionalNB=&m_oOutputFunctionalNB;
	m_pNaiveBayesApplyFunction->getOutputParameter(OVP_Algorithm_NaiveBayesApplyFunction_OutputParameterId_MatrixClassFunctional)->setValue(&l_oOutputFunctionalNB);

	m_oEncodeDecisionMatrix.setDimensionCount(1);
	m_oEncodeDecisionMatrix.setDimensionSize(0, 1);
	IMatrix *l_oEncodeDecisionMatrix=&m_oEncodeDecisionMatrix;

	m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setValue(&l_oEncodeDecisionMatrix);

	m_ui64LastStartTime=0;
	m_ui64LastEndTime=0;
	m_ui64NbEntriesDecoded = 0;
	m_bHasSentHeader=false;
	m_bFirstTime = true;
	m_pf64AllEntriesClassFunctionalNB = NULL;

	return true;
}

boolean CNaiveBayesApplyBoxAlgorithm::uninitialize(void)
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

	m_pNaiveBayesApplyFunction->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_pNaiveBayesApplyFunction);

	for (uint32 i_ent=0; i_ent<m_ui64NbEntries; i_ent++)
	{
		free(m_pf64AllEntriesClassFunctionalNB[i_ent]);
	}
	free(m_pf64AllEntriesClassFunctionalNB);

	free(m_pui32IndexClass2Max);

	return true;
}

boolean CNaiveBayesApplyBoxAlgorithm::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CNaiveBayesApplyBoxAlgorithm::process(void)
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
			}

			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				if(m_bFirstTime)
				{
					TParameterHandler < const IMatrix* > l_opMatrix(m_vStreamDecoder[i]->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));

					if ((l_opMatrix->getDimensionCount() == 2) && (m_ui32NbFeatures == (l_opMatrix->getDimensionSize(0)*l_opMatrix->getDimensionSize(1))))
					{
					}
					else
					{
						this->getLogManager()<<LogLevel_Error<<"Bad dimension of data or input training data : expected " << m_ui32NbFeatures << " features and input matrix size is " << l_opMatrix->getBufferElementCount() << "\n";
					}

					m_pf64AllEntriesClassFunctionalNB = (float64 **)calloc(m_ui64NbEntries, sizeof(float64*));
					if (m_pf64AllEntriesClassFunctionalNB == NULL)
					{
						this->getLogManager()<<LogLevel_Error<<"Memory allocation for storing all entries functionnal NB.\n";
					}
					for (uint32 i_ent=0; i_ent<m_ui64NbEntries; i_ent++)
					{
						m_pf64AllEntriesClassFunctionalNB[i_ent] = (float64 *)calloc(2, sizeof(float64));
						if (m_pf64AllEntriesClassFunctionalNB[i_ent] == NULL)
						{
							this->getLogManager()<<LogLevel_Error<<"Memory allocation for storing all functionnal NB.\n";
						}
					}

					m_pui32IndexClass2Max = (uint32 *)calloc(m_ui64NbEntries, sizeof(uint32));
					if (m_pui32IndexClass2Max == NULL)
					{
						this->getLogManager()<<LogLevel_Error<<"Memory allocation for storing index of maximum.\n";
					}

					m_bFirstTime = false;
				}
				m_pNaiveBayesApplyFunction->getInputParameter(OVP_Algorithm_NaiveBayesApplyFunction_InputParameterId_MatrixSignal)->setReferenceTarget(m_vStreamDecoder[i]->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));

				m_pNaiveBayesApplyFunction->process(OVP_Algorithm_NaiveBayesApplyFunction_InputTriggerId_Apply);

				uint32 l_ui32MatDimCount = m_oOutputFunctionalNB.getDimensionCount();
				float64 *l_f64FunctionalNB= m_oOutputFunctionalNB.getBuffer();

				for (uint32 i_class=0; i_class<l_ui32MatDimCount; i_class++)
				{
					m_pf64AllEntriesClassFunctionalNB[i][i_class] = l_f64FunctionalNB[i_class];
				}

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
		// Decision
		float64 *l_pf64EncodeMatrix, l_f64MaxAbsFunctional=0.0, l_f64SumExpFunctional1and2, l_f64DiffMin;
		// uint32 l_ui32IndMin;

		l_pf64EncodeMatrix = m_oEncodeDecisionMatrix.getBuffer();

		m_ui32Class2MaxNumber = 0;
		l_f64DiffMin = DBL_MAX;

		for (uint32 i_ent=0; i_ent<m_ui64NbEntries; i_ent++)
		{
			//this->getLogManager()<<LogLevel_Info<<"ClassFunctionalNB class 1 : "<<m_pf64AllEntriesClassFunctionalNB[i_ent][0]<<"ClassFunctionalNB class 2 : "<<m_pf64AllEntriesClassFunctionalNB[i_ent][1]<<"\n";
			for (uint32 i_class=0; i_class<2; i_class++)
			{
				if (fabs(m_pf64AllEntriesClassFunctionalNB[i_ent][i_class]) > l_f64MaxAbsFunctional)
				{
					l_f64MaxAbsFunctional = fabs(m_pf64AllEntriesClassFunctionalNB[i_ent][i_class]);
				}
			}

#if 0
			if ((m_pf64AllEntriesClassFunctionalNB[i_ent][1] - m_pf64AllEntriesClassFunctionalNB[i_ent][0]) > 0.0)
			{
				m_pui32IndexClass2Max[m_ui32Class2MaxNumber] = i_ent;
				m_ui32Class2MaxNumber++;
			}
			else if ((m_pf64AllEntriesClassFunctionalNB[i_ent][0] - m_pf64AllEntriesClassFunctionalNB[i_ent][1]) < l_f64DiffMin)
			{
				l_f64DiffMin = m_pf64AllEntriesClassFunctionalNB[i_ent][0] - m_pf64AllEntriesClassFunctionalNB[i_ent][1];
				l_ui32IndMin = i_ent;
			}
#endif
		}

#if 0
		if (m_ui32Class2MaxNumber == 0)
		{
			this->getLogManager()<<LogLevel_Warning<<"No entry selected\n";
			l_pf64EncodeMatrix[0] = l_ui32IndMin+1;
		}
		else if (m_ui32Class2MaxNumber == 1)
		{
			l_pf64EncodeMatrix[0] = m_pui32IndexClass2Max[m_ui32Class2MaxNumber-1]+1;
		}
		else if (m_ui32Class2MaxNumber > 1)
		{
			for (uint32 i_ent=0; i_ent<m_ui64NbEntries; i_ent++)
			{
				for (uint32 i_class=0; i_class<2; i_class++)
				{
					m_pf64AllEntriesClassFunctionalNB[i_ent][i_class] = exp(m_pf64AllEntriesClassFunctionalNB[i_ent][i_class] / (float64)CSTE_DIV);
				}

				l_f64SumExpFunctional1and2 = (m_pf64AllEntriesClassFunctionalNB[i_ent][0] + m_pf64AllEntriesClassFunctionalNB[i_ent][1]);

				m_pf64AllEntriesClassFunctionalNB[i_ent][0] = m_pf64AllEntriesClassFunctionalNB[i_ent][0] / l_f64SumExpFunctional1and2;
				m_pf64AllEntriesClassFunctionalNB[i_ent][1] = m_pf64AllEntriesClassFunctionalNB[i_ent][1] / l_f64SumExpFunctional1and2;
			}

			float64 l_f64value=0;
			uint32 l_ui32Index;
			for (uint32 i_max=0; i_max<m_ui32Class2MaxNumber; i_max++)
			{
				if (m_pf64AllEntriesClassFunctionalNB[m_pui32IndexClass2Max[i_max]][1] > l_f64value)
				{
					l_f64value = m_pf64AllEntriesClassFunctionalNB[m_pui32IndexClass2Max[i_max]][1];
					l_ui32Index = m_pui32IndexClass2Max[i_max]+1;
				}
			}

			l_pf64EncodeMatrix[0] = (float64)l_ui32Index;
		}
#endif

		float64 l_f64value=0;
		uint32 l_ui32Index=0;
		for (uint32 i_ent=0; i_ent<m_ui64NbEntries; i_ent++)
		{
			for (uint32 i_class=0; i_class<2; i_class++)
			{
				m_pf64AllEntriesClassFunctionalNB[i_ent][i_class] = exp(m_pf64AllEntriesClassFunctionalNB[i_ent][i_class] / (float64)CSTE_DIV);
			}

			l_f64SumExpFunctional1and2 = (m_pf64AllEntriesClassFunctionalNB[i_ent][0] + m_pf64AllEntriesClassFunctionalNB[i_ent][1]);

			m_pf64AllEntriesClassFunctionalNB[i_ent][0] = m_pf64AllEntriesClassFunctionalNB[i_ent][0] / l_f64SumExpFunctional1and2;
			m_pf64AllEntriesClassFunctionalNB[i_ent][1] = m_pf64AllEntriesClassFunctionalNB[i_ent][1] / l_f64SumExpFunctional1and2;

			if (m_pf64AllEntriesClassFunctionalNB[i_ent][1] > l_f64value)
			{
				l_f64value = m_pf64AllEntriesClassFunctionalNB[i_ent][1];
				l_ui32Index = i_ent+1;
			}
		}

	    l_pf64EncodeMatrix[0] = (float64)l_ui32Index;

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
		m_ui64NbEntriesDecoded=0;
	}

	return true;
}
