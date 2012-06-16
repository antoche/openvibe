#include "ovpCApplyFisherLdaFunction.h"

#include <cmath>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;
using namespace itpp;
using namespace std;
// ________________________________________________________________________________________________________________
//

boolean CApplyFisherLdaFunction::initialize(void)
{
	ip_pMatrixSignal.initialize(getInputParameter(OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixSignal));
	ip_pMatrixGlobalMean.initialize(getInputParameter(OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalMean));
	ip_pMatrixGlobalCovariance.initialize(getInputParameter(OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalCovariance));
	ip_pMatrixGlobalProbability.initialize(getInputParameter(OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalProbability));

	op_pFisherLdaDecision.initialize(getOutputParameter(OVP_Algorithm_ApplyFisherLdaFunction_OutputParameterId_FisherLdaDecision));



	return true;
}

boolean CApplyFisherLdaFunction::uninitialize(void)
{
	op_pFisherLdaDecision.uninitialize();

	ip_pMatrixGlobalProbability.uninitialize();
	ip_pMatrixGlobalCovariance.uninitialize();
	ip_pMatrixGlobalMean.uninitialize();
	ip_pMatrixSignal.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CApplyFisherLdaFunction::process(void)
{
	IMatrix* l_pInputMatrixSignal = ip_pMatrixSignal;
	IMatrix* l_pInputMatrixGlobalMean = ip_pMatrixGlobalMean;
	IMatrix* l_pInputMatrixGlobalCovariance = ip_pMatrixGlobalCovariance;
	IMatrix* l_pInputMatrixGlobalProbability = ip_pMatrixGlobalProbability;

	IMatrix* l_pOutputMatrixFisherLdaDecision = op_pFisherLdaDecision;

	if(isInputTriggerActive(OVP_Algorithm_ApplyFisherLdaFunction_InputTriggerId_Initialize))
	{
	}

	if(isInputTriggerActive(OVP_Algorithm_ApplyFisherLdaFunction_InputTriggerId_ApplyFunction))
	{
		// inputs var
		// signal
		uint32 l_ui32InputMatrixSignalDimensionNbUtterances=ip_pMatrixSignal->getDimensionSize(0);
		uint32 l_ui32InputMatrixSignalDimensionSizeEpoch=ip_pMatrixSignal->getDimensionSize(1);

		float64 * l_pInputSignal = l_pInputMatrixSignal->getBuffer();
		mat l_matInputSignal(l_ui32InputMatrixSignalDimensionNbUtterances, l_ui32InputMatrixSignalDimensionSizeEpoch);
		for (uint64 i=0;  i <l_ui32InputMatrixSignalDimensionNbUtterances; i++)
		{
			for(uint64 j=0 ; j<l_ui32InputMatrixSignalDimensionSizeEpoch; j++)
			{
				l_matInputSignal(i, j) =  l_pInputSignal[i*l_ui32InputMatrixSignalDimensionSizeEpoch+j];
			}
		}
		//mean
		uint32 l_ui32InputMatrixGlobalMeanDimensionNbClass=ip_pMatrixGlobalMean->getDimensionSize(0);
		uint32 l_ui32InputMatrixGlobalMeanDimensionSizeFeatures=ip_pMatrixGlobalMean->getDimensionSize(1);

		float64 * l_pInputGlobalMean = l_pInputMatrixGlobalMean->getBuffer();
		mat l_matInputGlobalMean(l_ui32InputMatrixGlobalMeanDimensionNbClass, l_ui32InputMatrixGlobalMeanDimensionSizeFeatures);
		for (uint64 i=0;  i < l_ui32InputMatrixGlobalMeanDimensionNbClass; i++)
		{
			for(uint64 j=0 ; j<l_ui32InputMatrixGlobalMeanDimensionSizeFeatures; j++)
			{
				l_matInputGlobalMean(i, j) =  l_pInputGlobalMean[i*l_ui32InputMatrixGlobalMeanDimensionSizeFeatures+j];
			}
		}

		//cov
		uint32 l_ui32InputMatrixGlobalCovarianceDimensionNbLine=ip_pMatrixGlobalCovariance->getDimensionSize(0);
		uint32 l_ui32InputMatrixGlobalCovarianceDimensionNbColumn=ip_pMatrixGlobalCovariance->getDimensionSize(1);

		mat l_matIntputGlobalCovariance(l_ui32InputMatrixGlobalCovarianceDimensionNbLine, l_ui32InputMatrixGlobalCovarianceDimensionNbColumn);

		float64 * l_pInputGlobalCovariance = l_pInputMatrixGlobalCovariance->getBuffer();
		for (uint64 i=0;  i < l_ui32InputMatrixGlobalCovarianceDimensionNbLine; i++)
		{
			for(uint64 j=0 ; j<l_ui32InputMatrixGlobalCovarianceDimensionNbColumn; j++)
			{
				l_matIntputGlobalCovariance(i, j) =  l_pInputGlobalCovariance[i*l_ui32InputMatrixGlobalCovarianceDimensionNbColumn+j];
			}
		}

		//proba
		float64 * l_pInputGlobalProbability = l_pInputMatrixGlobalProbability->getBuffer();
		mat l_matInputProbability(ip_pMatrixGlobalProbability->getDimensionSize(0), ip_pMatrixGlobalProbability->getDimensionSize(1));
		for (uint64 i=0;  i < ip_pMatrixGlobalProbability->getDimensionSize(0); i++)
		{
			for(uint64 j=0 ; j<ip_pMatrixGlobalProbability->getDimensionSize(1); j++)
			{
				l_matInputProbability(i, j) =  l_pInputGlobalProbability[i*ip_pMatrixGlobalProbability->getDimensionSize(1)+j];
			}
		}

		// output var
		l_pOutputMatrixFisherLdaDecision->setDimensionCount(2);
		l_pOutputMatrixFisherLdaDecision->setDimensionSize(0, 1);
		l_pOutputMatrixFisherLdaDecision->setDimensionSize(1, 1);
		float64 * l_pOutputFisherLdaDecision = l_pOutputMatrixFisherLdaDecision->getBuffer();

		float64 l_pOutputFisherLdaDecisionClass1, l_pOutputFisherLdaDecisionClass2, l_pOutputFisherLdaValueMax;
		l_pOutputFisherLdaValueMax = -100000000;
		l_pOutputFisherLdaDecision[0] = 0;
		//processing
		// common part : mu(i,:)*inv(C)
		mat l_matMeanByInvCov(l_ui32InputMatrixGlobalMeanDimensionNbClass, l_ui32InputMatrixGlobalCovarianceDimensionNbColumn);
		l_matMeanByInvCov = operator*(l_matInputGlobalMean, l_matIntputGlobalCovariance);

		//-0.5*mutab(i,:)*inv(C)*mutab(i,:)'
		mat  l_matMeanByInvCovByTransMean(l_ui32InputMatrixGlobalMeanDimensionNbClass, 1);
		l_matMeanByInvCovByTransMean = operator*(l_matMeanByInvCov, transpose(l_matInputGlobalMean));
		l_matMeanByInvCovByTransMean *= -0.5;

		for (uint32 i=0; i< ip_pMatrixSignal->getDimensionSize(0); i++)
		{
			// mutab(i,:)*inv(C)*x'
			mat  l_matMeanByInvCovByTransX(l_ui32InputMatrixGlobalMeanDimensionNbClass, 1);
			mat l_matTransposeRowInputSignal(ip_pMatrixSignal->getDimensionSize(1), 1);
			for(uint64 j=0 ; j<ip_pMatrixSignal->getDimensionSize(1); j++)
			{
				l_matTransposeRowInputSignal(j,0) = l_matInputSignal(i, j);
			}
			l_matMeanByInvCovByTransX = operator*(l_matMeanByInvCov, l_matTransposeRowInputSignal);

			//f(i) = mutab(i,:)*inv(C)*x'-0.5*mutab(i,:)*inv(C)*mutab(i,:)'+log(p(i));

			l_pOutputFisherLdaDecisionClass1 = l_matMeanByInvCovByTransX(0,0) + l_matMeanByInvCovByTransMean(0,0) + l_matInputProbability(0,0);
			l_pOutputFisherLdaDecisionClass2 = l_matMeanByInvCovByTransX(1,0) + l_matMeanByInvCovByTransMean(1,0) + l_matInputProbability(1,0);

			this->getLogManager() << LogLevel_Trace << " l_pOutputFisherLdaDecisionClass1= '" << l_pOutputFisherLdaDecisionClass1 << "\n";
			this->getLogManager() << LogLevel_Trace << " l_pOutputFisherLdaDecisionClass2= '" << l_pOutputFisherLdaDecisionClass2 << "\n";
			if (l_pOutputFisherLdaDecisionClass1-l_pOutputFisherLdaDecisionClass2>l_pOutputFisherLdaValueMax)
			{
				l_pOutputFisherLdaValueMax =l_pOutputFisherLdaDecisionClass1-l_pOutputFisherLdaDecisionClass2;
				l_pOutputFisherLdaDecision[0] = i+1;
			}
		}

		this->getLogManager() << LogLevel_Trace << "l_pOutputFisherLdaDecision = " << l_pOutputFisherLdaDecision[0] << "\n";
		this->getLogManager() << LogLevel_Trace << "l_pOutputFisherLdaValueMax= '" << l_pOutputFisherLdaValueMax << "\n";
	}

	return true;
}
