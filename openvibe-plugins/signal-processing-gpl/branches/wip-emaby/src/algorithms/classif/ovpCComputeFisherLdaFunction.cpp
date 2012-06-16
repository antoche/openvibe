#include "ovpCComputeFisherLdaFunction.h"

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

boolean CComputeFisherLdaFunction::initialize(void)
{
	ip_pMatrixFirstClass.initialize(getInputParameter(OVP_Algorithm_ComputeFisherLdaFunction_InputParameterId_MatrixFirstClass));
	ip_pMatrixSecondClass.initialize(getInputParameter(OVP_Algorithm_ComputeFisherLdaFunction_InputParameterId_MatrixSecondClass));

	op_pMatrixGlobalMean.initialize(getOutputParameter(OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalMean));
	op_pMatrixGlobalCovariance.initialize(getOutputParameter(OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalCovariance));
	op_pMatrixGlobalProbability.initialize(getOutputParameter(OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalProbability));

	return true;
}

boolean CComputeFisherLdaFunction::uninitialize(void)
{
	ip_pMatrixFirstClass.uninitialize();
	ip_pMatrixSecondClass.uninitialize();

	op_pMatrixGlobalMean.uninitialize();
	op_pMatrixGlobalCovariance.uninitialize();
	op_pMatrixGlobalProbability.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CComputeFisherLdaFunction::process(void)
{
	IMatrix* l_pInputMatrixFirstClass = ip_pMatrixFirstClass;
	IMatrix* l_pInputMatrixSecondClass = ip_pMatrixSecondClass;

	IMatrix* l_pOutputMatrixGlobalMean = op_pMatrixGlobalMean;
	IMatrix* l_pOutputMatrixGlobalCovariance = op_pMatrixGlobalCovariance;
	IMatrix* l_pOutputMatrixGlobalProbability = op_pMatrixGlobalProbability;

	if(isInputTriggerActive(OVP_Algorithm_ComputeFisherLdaFunction_InputTriggerId_Initialize))
	{
	}

	if(isInputTriggerActive(OVP_Algorithm_ComputeFisherLdaFunction_InputTriggerId_ComputeFunction))
	{
		// input vars
		float64* l_pInputFirstClass = l_pInputMatrixFirstClass->getBuffer();
		float64* l_pInputSecondClass = l_pInputMatrixSecondClass->getBuffer();

		// dimension of input signal biuffer
		uint32 l_ui32InputMatrixFirstClassDimensionNbItems=ip_pMatrixFirstClass->getDimensionSize(0);
		uint32 l_ui32InputMatrixFirstClassDimensionSizeEpoch=ip_pMatrixFirstClass->getDimensionSize(1);

		uint32 l_ui32InputMatrixSecondClassDimensionNbItems=ip_pMatrixSecondClass->getDimensionSize(0);
		uint32 l_ui32InputMatrixSecondClassDimensionSizeEpoch=ip_pMatrixSecondClass->getDimensionSize(1);

		// output vars
		l_pOutputMatrixGlobalMean->setDimensionCount(2);
		l_pOutputMatrixGlobalMean->setDimensionSize(0, 2);
		l_pOutputMatrixGlobalMean->setDimensionSize(1, l_ui32InputMatrixFirstClassDimensionSizeEpoch);
		float64 * l_pOutputGlobalMean = l_pOutputMatrixGlobalMean->getBuffer();

		l_pOutputMatrixGlobalCovariance->setDimensionCount(2);
		l_pOutputMatrixGlobalCovariance->setDimensionSize(0, l_ui32InputMatrixFirstClassDimensionSizeEpoch);
		l_pOutputMatrixGlobalCovariance->setDimensionSize(1, l_ui32InputMatrixFirstClassDimensionSizeEpoch);
		float64 * l_pOutputGlobalCovariance = l_pOutputMatrixGlobalCovariance->getBuffer();

		l_pOutputMatrixGlobalProbability->setDimensionCount(2);
		l_pOutputMatrixGlobalProbability->setDimensionSize(0, 2);
		l_pOutputMatrixGlobalProbability->setDimensionSize(1, 1);
		float64 * l_pOutputGlobalProbability = l_pOutputMatrixGlobalProbability->getBuffer();

		mat l_matInputFirstClass(l_ui32InputMatrixFirstClassDimensionNbItems, l_ui32InputMatrixFirstClassDimensionSizeEpoch);
		this->getLogManager() << LogLevel_Debug << "Epoch P300\n";
		for (uint64 i=0;  i < l_ui32InputMatrixFirstClassDimensionNbItems; i++)
		{
			for(uint64 j=0 ; j<l_ui32InputMatrixFirstClassDimensionSizeEpoch; j++)
			{
				l_matInputFirstClass(i, j)=  l_pInputFirstClass[j+i*l_ui32InputMatrixFirstClassDimensionSizeEpoch];
				this->getLogManager() << LogLevel_Debug << l_matInputFirstClass(i, j) << "  ";
			}
			this->getLogManager() << LogLevel_Debug << "\n";
		}
		this->getLogManager() << LogLevel_Debug << "Epoch Non P300\n";
		mat l_matInputSecondClass(l_ui32InputMatrixSecondClassDimensionNbItems, l_ui32InputMatrixSecondClassDimensionSizeEpoch);
		for (uint64 i=0;  i < l_ui32InputMatrixSecondClassDimensionNbItems; i++)
		{
			for(uint64 j=0 ; j< l_ui32InputMatrixSecondClassDimensionSizeEpoch; j++)
			{
				l_matInputSecondClass(i, j) =  l_pInputSecondClass[j+i*l_ui32InputMatrixSecondClassDimensionSizeEpoch];
				this->getLogManager() << LogLevel_Debug << l_matInputSecondClass(i, j) << "  ";
			}
			this->getLogManager() << LogLevel_Debug << "\n";
		}

		//Compute global mean
		for(uint64 j=0 ; j<l_ui32InputMatrixFirstClassDimensionSizeEpoch; j++)
		{
			l_pOutputGlobalMean[j] = mean(l_matInputFirstClass.get_col(j));
		}
		for(uint64 j=0 ; j<l_ui32InputMatrixSecondClassDimensionSizeEpoch; j++)
		{
			l_pOutputGlobalMean[j+l_ui32InputMatrixSecondClassDimensionSizeEpoch] = mean(l_matInputSecondClass.get_col(j));
		}
		// Data correction
		mat l_matGlobalMean(1,l_ui32InputMatrixSecondClassDimensionSizeEpoch);

		for(uint64 j=0 ; j<l_ui32InputMatrixFirstClassDimensionSizeEpoch; j++)
		{
			l_matGlobalMean(0,j) = (l_ui32InputMatrixFirstClassDimensionNbItems * mean(l_matInputFirstClass.get_col(j)) + l_ui32InputMatrixSecondClassDimensionNbItems* mean(l_matInputSecondClass.get_col(j)))/ (l_ui32InputMatrixFirstClassDimensionNbItems+ l_ui32InputMatrixSecondClassDimensionNbItems);
		}
		for (uint64 i=0;  i < l_ui32InputMatrixFirstClassDimensionNbItems; i++)
		{
			for(uint64 j=0 ; j<l_ui32InputMatrixFirstClassDimensionSizeEpoch; j++)
			{
				float64 l_f64tmpValue = l_matInputFirstClass(i, j);
				l_matInputFirstClass(i,j) = l_f64tmpValue - l_matGlobalMean(0,j);
			}
		}
		for (uint64 i=0;  i < l_ui32InputMatrixSecondClassDimensionNbItems; i++)
		{
			for(uint64 j=0 ; j<l_ui32InputMatrixSecondClassDimensionSizeEpoch; j++)
			{
				float64 l_f64tmpValue = l_matInputSecondClass(i, j);
				l_matInputSecondClass(i,j) = l_f64tmpValue - l_matGlobalMean(0,j);
			}
		}

		//Compute global covariance
		mat l_matOutputGlobalCovariance(l_ui32InputMatrixFirstClassDimensionSizeEpoch, l_ui32InputMatrixFirstClassDimensionSizeEpoch);
		mat l_matOutputGlobalCovarianceInv(l_ui32InputMatrixFirstClassDimensionSizeEpoch, l_ui32InputMatrixFirstClassDimensionSizeEpoch);

		//Cov class 1
		mat l_matOutputClass1Covariance(l_ui32InputMatrixFirstClassDimensionSizeEpoch, l_ui32InputMatrixFirstClassDimensionSizeEpoch);
		l_matOutputClass1Covariance = cov(l_matInputFirstClass, false);

		l_matOutputClass1Covariance *= l_ui32InputMatrixFirstClassDimensionNbItems;
		l_matOutputClass1Covariance /= (l_ui32InputMatrixFirstClassDimensionNbItems + l_ui32InputMatrixSecondClassDimensionNbItems);

		//Cov class2
		mat l_matOutputClass2Covariance(l_ui32InputMatrixSecondClassDimensionSizeEpoch, l_ui32InputMatrixSecondClassDimensionSizeEpoch);
		l_matOutputClass2Covariance = cov(l_matInputSecondClass, false);

		l_matOutputClass2Covariance *= l_ui32InputMatrixSecondClassDimensionNbItems;
		l_matOutputClass2Covariance /= (l_ui32InputMatrixFirstClassDimensionNbItems + l_ui32InputMatrixSecondClassDimensionNbItems);

		l_matOutputGlobalCovariance = l_matOutputClass1Covariance + l_matOutputClass2Covariance;

		l_matOutputGlobalCovarianceInv = inv(l_matOutputGlobalCovariance);

		for (uint64 i=0;  i < l_ui32InputMatrixFirstClassDimensionSizeEpoch; i++)
		{
			for(uint64 j=0 ; j<l_ui32InputMatrixFirstClassDimensionSizeEpoch; j++)
			{
				l_pOutputGlobalCovariance[i*l_ui32InputMatrixFirstClassDimensionSizeEpoch+j] =  l_matOutputGlobalCovarianceInv(i,j);
			}
		}

		l_pOutputGlobalProbability[0] = (float64)l_ui32InputMatrixFirstClassDimensionNbItems/(l_ui32InputMatrixFirstClassDimensionNbItems+l_ui32InputMatrixSecondClassDimensionNbItems);
		l_pOutputGlobalProbability[1] =  (float64)l_ui32InputMatrixSecondClassDimensionNbItems/(l_ui32InputMatrixFirstClassDimensionNbItems+l_ui32InputMatrixSecondClassDimensionNbItems);
	}

	return true;
}
