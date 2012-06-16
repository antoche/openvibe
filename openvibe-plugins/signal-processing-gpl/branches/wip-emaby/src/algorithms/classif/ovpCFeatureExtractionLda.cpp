#include "ovpCFeatureExtractionLda.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;
using namespace itpp;
using namespace std;
// ________________________________________________________________________________________________________________
//

boolean CFeatureExtractionLda::initialize(void)
{
	ip_pSignalEpochTable.initialize(getInputParameter(OVP_Algorithm_FeatureExtractionLda_InputParameterId_EpochTable));
	ip_pClass1SamplesNumber.initialize(getInputParameter(OVP_Algorithm_FeatureExtractionLda_InputParameterId_Class1SamplesNumber));

	op_pMatrixFirstClass.initialize(getOutputParameter(OVP_Algorithm_FeatureExtractionLda_OutputParameterId_MatrixFirstClass));
	op_pMatrixSecondClass.initialize(getOutputParameter(OVP_Algorithm_FeatureExtractionLda_OutputParameterId_MatrixSecondClass));

	m_uint64IndexTainingSet = 0;
	m_bFirstTime = true;

	return true;
}

boolean CFeatureExtractionLda::uninitialize(void)
{
	op_pMatrixSecondClass.uninitialize();
	op_pMatrixFirstClass.uninitialize();

	ip_pClass1SamplesNumber.uninitialize();
	ip_pSignalEpochTable.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CFeatureExtractionLda::process(void)
{
	IMatrix* l_pInputClass1SamplesNumber = ip_pClass1SamplesNumber;
	IMatrix* l_pInputSignalEpochTable = ip_pSignalEpochTable;
	IMatrix* l_pMatrixFirstClass = op_pMatrixFirstClass;
	IMatrix* l_pMatrixSecondClass = op_pMatrixSecondClass;

	float64* l_pFirstClass = NULL;
	float64* l_pSecondClass = NULL;

	if(isInputTriggerActive(OVP_Algorithm_FeatureExtractionLda_InputTriggerId_Initialize))
	{
		m_bFirstTime = true;
		m_uint64IndexTainingSet = 0;
	}

	if(isInputTriggerActive(OVP_Algorithm_FeatureExtractionLda_InputTriggerId_ExtractFeature))
	{
		// input vars
		float64* l_pSignalEpochTable = l_pInputSignalEpochTable->getBuffer();
		float64* l_pClass1SamplesNumber = l_pInputClass1SamplesNumber->getBuffer();

		// dimension of input signal epochs
		uint32 l_ui32InputSignalEpochTableDimensionNbUtterances=ip_pSignalEpochTable->getDimensionSize(0);
		uint32 l_ui32InputSignalEpochTableDimensionSizeEpoch=ip_pSignalEpochTable->getDimensionSize(1);
		uint32 l_ui32InputClass1SamplesNumberDimensionColumn=ip_pClass1SamplesNumber->getDimensionSize(1);

		// output vars
		if (m_bFirstTime)
		{
			l_pMatrixFirstClass->setDimensionCount(2);
			l_pMatrixFirstClass->setDimensionSize(0, l_ui32InputClass1SamplesNumberDimensionColumn);
			l_pMatrixFirstClass->setDimensionSize(1, l_ui32InputSignalEpochTableDimensionSizeEpoch);

			l_pMatrixSecondClass->setDimensionCount(2);
			l_pMatrixSecondClass->setDimensionSize(0, (l_ui32InputSignalEpochTableDimensionNbUtterances-1)*l_ui32InputClass1SamplesNumberDimensionColumn);
			l_pMatrixSecondClass->setDimensionSize(1, l_ui32InputSignalEpochTableDimensionSizeEpoch);

			m_bFirstTime = false;
		}

		l_pFirstClass = l_pMatrixFirstClass->getBuffer();
		l_pSecondClass = l_pMatrixSecondClass->getBuffer();

		uint32 l_ui32TmpIndex = 0;
		for (uint64 i=0; i < l_ui32InputSignalEpochTableDimensionNbUtterances; i++)
		{
			if (i==l_pClass1SamplesNumber[m_uint64IndexTainingSet]-1)
			{
				for (uint64 k=0; k<l_ui32InputSignalEpochTableDimensionSizeEpoch; k++)
				{
					l_pFirstClass[k+m_uint64IndexTainingSet*l_ui32InputSignalEpochTableDimensionSizeEpoch] = l_pSignalEpochTable[k+i*l_ui32InputSignalEpochTableDimensionSizeEpoch];
				}
			}
			else
			{
				for (uint64 k=0; k<l_ui32InputSignalEpochTableDimensionSizeEpoch; k++)
				{
					l_pSecondClass[k+l_ui32TmpIndex*l_ui32InputSignalEpochTableDimensionSizeEpoch+m_uint64IndexTainingSet*(l_ui32InputSignalEpochTableDimensionNbUtterances-1)*l_ui32InputSignalEpochTableDimensionSizeEpoch] = l_pSignalEpochTable[k+i*l_ui32InputSignalEpochTableDimensionSizeEpoch];
				}
				l_ui32TmpIndex++;
			}
		}
		m_uint64IndexTainingSet++;
	}

	return true;
}
