#include "ovpCNaiveBayesApplyFunction.h"

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

boolean CNaiveBayesApplyFunction::initialize(void)
{
	ip_pMatrixSignal.initialize(getInputParameter(OVP_Algorithm_NaiveBayesApplyFunction_InputParameterId_MatrixSignal));
	ip_pMatrixMean.initialize(getInputParameter(OVP_Algorithm_NaiveBayesApplyFunction_InputParameterId_MatrixMean));
	ip_pMatrixVariance.initialize(getInputParameter(OVP_Algorithm_NaiveBayesApplyFunction_InputParameterId_MatrixVariance));
	ip_pMatrixLogTerm.initialize(getInputParameter(OVP_Algorithm_NaiveBayesApplyFunction_InputParameterId_LogTerm));

	op_pMatrixFunctionalNB.initialize(getOutputParameter(OVP_Algorithm_NaiveBayesApplyFunction_OutputParameterId_MatrixClassFunctional));
	
	return true;
}

boolean CNaiveBayesApplyFunction::uninitialize(void)
{
	op_pMatrixFunctionalNB.uninitialize();

	ip_pMatrixSignal.uninitialize();
	ip_pMatrixMean.uninitialize();
	ip_pMatrixVariance.uninitialize();
	ip_pMatrixLogTerm.uninitialize();
	
	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CNaiveBayesApplyFunction::process(void)
{
	IMatrix* l_pOutputMatrixFunctionalNB = op_pMatrixFunctionalNB;
	float64 l_f64DiffSigMeanClass1,l_f64DiffSigMeanClass2;
	float64 l_f64SumClass1, l_f64SqrClass1, l_f64NBFunctionalClass1;
	float64 l_f64SumClass2, l_f64SqrClass2, l_f64NBFunctionalClass2;
	float64 *l_pBufferData, *l_pBufferMean, *l_pBufferVariance, *l_pBufferLogTerm, *l_pOutputFunctionalNB ;

	if(isInputTriggerActive(OVP_Algorithm_NaiveBayesApplyFunction_InputTriggerId_Initialize))
	{
	}

	if(isInputTriggerActive(OVP_Algorithm_NaiveBayesApplyFunction_InputTriggerId_Apply))
	{
		uint32 l_ui32InputMatrixSignalDimensionCount=ip_pMatrixSignal->getDimensionCount();
		uint32 l_ui32NbChannel, l_ui32NbSample, l_ui32Offset;
		
		if (l_ui32InputMatrixSignalDimensionCount == 2)
		{
			l_ui32NbChannel = ip_pMatrixSignal->getDimensionSize(0);
			l_ui32NbSample = ip_pMatrixSignal->getDimensionSize(1);			
			l_pBufferData = ip_pMatrixSignal->getBuffer();
					
			l_pBufferMean = ip_pMatrixMean->getBuffer();
			l_pBufferVariance = ip_pMatrixVariance->getBuffer();
			l_pBufferLogTerm = ip_pMatrixLogTerm->getBuffer();
			
			
			
			//this->getLogManager()<<LogLevel_Info<<" ip_pMatrixSignal->getDimensionSize(1): "<<ip_pMatrixSignal->getDimensionSize(1)<<"\n";
			
			//this->getLogManager()<<LogLevel_Info<<" ip_pMatrixMean->getDimensionSize(1): "<<ip_pMatrixMean->getDimensionSize(1)<<"\n";
						
			//this->getLogManager()<<LogLevel_Info<<" ip_pMatrixVariance->getDimensionSize(1): "<<ip_pMatrixVariance->getDimensionSize(1)<<"\n";
			
			//this->getLogManager()<<LogLevel_Info<<" ip_pMatrixLogTerm->getDimensionSize(1): "<<ip_pMatrixLogTerm->getDimensionSize(1)<<"\n";
			
			
			
			// output var
			l_pOutputMatrixFunctionalNB->setDimensionCount(2);
			l_pOutputMatrixFunctionalNB->setDimensionSize(0, 1);
			l_pOutputMatrixFunctionalNB->setDimensionSize(1, 1);
			l_pOutputFunctionalNB = l_pOutputMatrixFunctionalNB->getBuffer();
			
			l_f64SumClass1=0.0;
			for (uint32 i=0; i<l_ui32NbChannel; i++)
			{
				for (uint32 j=0; j<l_ui32NbSample; j++)
				{						
					l_f64DiffSigMeanClass1 = l_pBufferData[(i*l_ui32NbSample)+j] - l_pBufferMean[(i*l_ui32NbSample)+j];
					l_f64SqrClass1 = l_f64DiffSigMeanClass1 * l_f64DiffSigMeanClass1;
					l_f64SqrClass1 /= l_pBufferVariance[(i*l_ui32NbSample)+j];
					l_f64SumClass1 += l_f64SqrClass1;
				}
			}
			l_f64NBFunctionalClass1 = l_pBufferLogTerm[0]-(0.5*l_f64SumClass1);
			
			l_f64SumClass2=0.0;
			l_ui32Offset = l_ui32NbChannel*l_ui32NbSample;
			for (uint32 i=0; i<l_ui32NbChannel; i++)
			{
				for (uint32 j=0; j<l_ui32NbSample; j++)
				{
					l_f64DiffSigMeanClass2 = l_pBufferData[(i*l_ui32NbSample)+j] - l_pBufferMean[l_ui32Offset+(i*l_ui32NbSample)+j];
					l_f64SqrClass2 = l_f64DiffSigMeanClass2 * l_f64DiffSigMeanClass2;
					l_f64SqrClass2 /= l_pBufferVariance[l_ui32Offset+(i*l_ui32NbSample)+j];
					l_f64SumClass2 += l_f64SqrClass2;
				}
			}
			l_f64NBFunctionalClass2 = l_pBufferLogTerm[1]-(0.5*l_f64SumClass2);
			
			//this->getLogManager()<<LogLevel_Info<<" l_f64NBFunctionalClass1: "<<l_f64NBFunctionalClass1<<" l_f64NBFunctionalClass2: "<<l_f64NBFunctionalClass2<<"\n";
			
			l_pOutputFunctionalNB[0] = l_f64NBFunctionalClass1;
			l_pOutputFunctionalNB[1] = l_f64NBFunctionalClass2;
		}
		else
		{
			this->getLogManager()<<LogLevel_Error<<"Bad number of dimension count : "<<l_ui32InputMatrixSignalDimensionCount<<" . Must be 2.\n";
		}
	}
	return true;
}
