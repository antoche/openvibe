#include "ovpCAlgorithmUnivariateStatistics.h"

#include <cmath>
#include <iostream>
#include <algorithm>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

// ________________________________________________________________________________________________________________
//

boolean CAlgoUnivariateStatistic::initialize(void)
{
	ip_pMatrix.initialize(getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_Matrix));
	op_pProcessedMatrix_Mean.initialize(getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Mean));
	op_pProcessedMatrix_Variance.initialize(getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Var));
	op_pProcessedMatrix_Range.initialize(getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Range));
	op_pProcessedMatrix_Median.initialize(getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Med));
	op_pProcessedMatrix_IQR.initialize(getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_IQR));
	op_pProcessedMatrix_Percentile.initialize(getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Percent));

	ip_bStatisticMeanActive.initialize(this->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_MeanActive));
	ip_bStatisticVarianceActive.initialize(this->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_VarActive));
	ip_bStatisticRangeActive.initialize(this->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_RangeActive));
	ip_bStatisticMedianActive.initialize(this->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_MedActive));
	ip_bStatisticIQRActive.initialize(this->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_IQRActive));
	ip_bStatisticPercentileActive.initialize(this->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_PercentActive));

	ip_ui64PercentileValue.initialize(this->getInputParameter(OVP_Algorithm_UnivariateStatistic_InputParameterId_PercentValue));
	op_fCompression.initialize(this->getOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Compression));

	return true;
}

boolean CAlgoUnivariateStatistic::uninitialize(void)
{
	op_fCompression.uninitialize();
	ip_ui64PercentileValue.uninitialize();

	ip_bStatisticMeanActive.uninitialize();
	ip_bStatisticVarianceActive.uninitialize();
	ip_bStatisticRangeActive.uninitialize();
	ip_bStatisticMedianActive.uninitialize();
	ip_bStatisticIQRActive.uninitialize();
	ip_bStatisticPercentileActive.uninitialize();

	op_pProcessedMatrix_Mean.uninitialize();
	op_pProcessedMatrix_Variance.uninitialize();
	op_pProcessedMatrix_Range.uninitialize();
	op_pProcessedMatrix_Median.uninitialize();
	op_pProcessedMatrix_IQR.uninitialize();
	op_pProcessedMatrix_Percentile.uninitialize();
	ip_pMatrix.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CAlgoUnivariateStatistic::process(void)
{
	IMatrix* l_pInputMatrix=ip_pMatrix;
	IMatrix* l_pOutputMatrix_Mean=op_pProcessedMatrix_Mean;
	IMatrix* l_pOutputMatrix_Variance=op_pProcessedMatrix_Variance;
	IMatrix* l_pOutputMatrix_Range=op_pProcessedMatrix_Range;
	IMatrix* l_pOutputMatrix_Median=op_pProcessedMatrix_Median;
	IMatrix* l_pOutputMatrix_IQR=op_pProcessedMatrix_IQR;
	IMatrix* l_pOutputMatrix_Percentile=op_pProcessedMatrix_Percentile;

	if(this->isInputTriggerActive(OVP_Algorithm_UnivariateStatistic_InputTriggerId_Initialize))
	{
		this->getLogManager() << LogLevel_Debug << "input : " << l_pInputMatrix->getDimensionCount() << " : " << l_pInputMatrix->getDimensionSize(0) << "*" << l_pInputMatrix->getDimensionSize(1) << "\n";

		//initialize matrix output
		if(!setMatrixDimension(l_pOutputMatrix_Mean,l_pInputMatrix)) { return false; }
		if(!setMatrixDimension(l_pOutputMatrix_Variance,l_pInputMatrix)) { return false; }
		if(!setMatrixDimension(l_pOutputMatrix_Range,l_pInputMatrix)) { return false; }
		if(!setMatrixDimension(l_pOutputMatrix_Median,l_pInputMatrix)) { return false; }
		if(!setMatrixDimension(l_pOutputMatrix_IQR,l_pInputMatrix)) { return false; }
		if(!setMatrixDimension(l_pOutputMatrix_Percentile,l_pInputMatrix)) { return false; }

		//inform about the compression on sampling rate due to this operation N->1 :=: fq->fq/N
		op_fCompression=1/float64(l_pInputMatrix->getDimensionSize(1));

		//percentile value
		m_ui64PercentileValue=ip_ui64PercentileValue;
		//select operation to do (avoid unuseful calculus)
		m_bSumActive=ip_bStatisticMeanActive || ip_bStatisticVarianceActive;
		m_bSqaresumActive=ip_bStatisticVarianceActive;
		m_bSortActive=ip_bStatisticRangeActive || ip_bStatisticMedianActive || ip_bStatisticIQRActive || ip_bStatisticPercentileActive;

		if(m_bSumActive)
		{
			OpenViBEToolkit::Tools::Matrix::copyDescription(m_oSumMatrix, *l_pInputMatrix);
			m_oSumMatrix.setDimensionSize(1,1);
		}

		if(m_bSqaresumActive)
		{
			OpenViBEToolkit::Tools::Matrix::copyDescription(m_oSumMatrix2, *l_pInputMatrix);
			m_oSumMatrix2.setDimensionSize(1,1);
		}

		if(m_bSortActive)
		{
			OpenViBEToolkit::Tools::Matrix::copyDescription(m_oSortMatrix, *l_pInputMatrix);
		}
	}

	if(this->isInputTriggerActive(OVP_Algorithm_UnivariateStatistic_InputTriggerId_Process))
	{
		///make necessary operations
		//dimension
		float64 l_fS=float64(l_pInputMatrix->getDimensionSize(1));
		//sum, sum square, sort
		std::vector<float64> vect(l_pInputMatrix->getDimensionSize(1));
		for(uint32 i=0; i<l_pInputMatrix->getDimensionSize(0);i++)
		{
			if(m_bSortActive)
			{
				//copy fonctionne pas car le buffer n'est pas unidirectionnel...
				for(uint32 j = 0; j < l_pInputMatrix->getDimensionSize(1); j++)
				{
					vect[j]=l_pInputMatrix->getBuffer()[i*l_pInputMatrix->getDimensionSize(1)+j];
				}
				std::sort(vect.begin(),vect.end());
			}

			float64 l_fY=0, l_fY2=0;
			for(uint32 j = 0; j < l_pInputMatrix->getDimensionSize(1); j++)
			{
				float64 l_fX=l_pInputMatrix->getBuffer()[i*l_pInputMatrix->getDimensionSize(1)+j];
				if(m_bSumActive)
				{
					l_fY+=l_fX;
				}
				if(m_bSqaresumActive)
				{
					l_fY2+=l_fX*l_fX;
				}
				if(m_bSortActive)
				{
					m_oSortMatrix.getBuffer()[i*m_oSortMatrix.getDimensionSize(1)+j]=vect.at(j);
				}
			}

			if(m_bSumActive)
			{
				m_oSumMatrix.getBuffer()[i*m_oSumMatrix.getDimensionSize(1)]=l_fY;
			}
			if(m_bSqaresumActive)
			{
				m_oSumMatrix2.getBuffer()[i*m_oSumMatrix2.getDimensionSize(1)]=l_fY2;
			}
		}

		///make statistics operations...
		if(ip_bStatisticMeanActive)
		{
			for(uint32 i=0; i<l_pOutputMatrix_Mean->getDimensionSize(0);i++)
			{
				l_pOutputMatrix_Mean->getBuffer()[i*l_pOutputMatrix_Mean->getDimensionSize(1)]=m_oSumMatrix.getBuffer()[i*m_oSumMatrix.getDimensionSize(1)]/l_pInputMatrix->getDimensionSize(1);
			}
		}
		if(ip_bStatisticVarianceActive)
		{
			for(uint32 i = 0; i < l_pOutputMatrix_Variance->getDimensionSize(0); i++)
			{
				float64 l_fY=m_oSumMatrix.getBuffer()[i*m_oSumMatrix.getDimensionSize(1)],
				        l_fY2=m_oSumMatrix2.getBuffer()[i*m_oSumMatrix2.getDimensionSize(1)];
				l_pOutputMatrix_Variance->getBuffer()[i*l_pOutputMatrix_Variance->getDimensionSize(1)]=l_fY2/l_fS-l_fY*l_fY/(l_fS*l_fS);
			}
		}
		if(ip_bStatisticRangeActive)
		{
			for(uint32 i = 0; i < l_pOutputMatrix_Range->getDimensionSize(0); i++)
			{
				float64 l_min=m_oSortMatrix.getBuffer()[i*m_oSortMatrix.getDimensionSize(1)+0],
				        l_max=m_oSortMatrix.getBuffer()[(i+1)*m_oSortMatrix.getDimensionSize(1)-1];
				l_pOutputMatrix_Range->getBuffer()[i*l_pOutputMatrix_Range->getDimensionSize(1)]=l_max-l_min;
			}
		}
		if(ip_bStatisticMedianActive)
		{
			for(uint32 i = 0; i < l_pOutputMatrix_Median->getDimensionSize(0); i++)
			{
				l_pOutputMatrix_Median->getBuffer()[i*l_pOutputMatrix_Median->getDimensionSize(1)]=
				(m_oSortMatrix.getDimensionSize(1)%2) ?
				    m_oSortMatrix.getBuffer()[i*m_oSortMatrix.getDimensionSize(1)+m_oSortMatrix.getDimensionSize(1)/2+1-1] :
				   (m_oSortMatrix.getBuffer()[i*m_oSortMatrix.getDimensionSize(1)+m_oSortMatrix.getDimensionSize(1)/2-1]+m_oSortMatrix.getBuffer()[i*m_oSortMatrix.getDimensionSize(1)+m_oSortMatrix.getDimensionSize(1)/2+1-1])/2 ;
			}
		}
		if(ip_bStatisticIQRActive)
		{
			for(uint32 i = 0; i < l_pOutputMatrix_IQR->getDimensionSize(0); i++)
			{
				float64 l_flow=0,l_fup=0;
				uint32 l_uireste=m_oSortMatrix.getDimensionSize(1)%4, l_uinb=4-l_uireste;
				for(uint32 k=0; k<l_uinb;k++)
				{
					l_flow+=m_oSortMatrix.getBuffer()[i*m_oSortMatrix.getDimensionSize(1)+m_oSortMatrix.getDimensionSize(1)/4-(l_uinb-1)+k-1];
				}

				l_flow/=l_uinb;

				for(uint32 k=0; k<l_uinb;k++)
				{
					l_fup+=m_oSortMatrix.getBuffer()[i*m_oSortMatrix.getDimensionSize(1)+m_oSortMatrix.getDimensionSize(1)-m_oSortMatrix.getDimensionSize(1)/4-1+k-1];
				}

				l_fup/=l_uinb;
				l_pOutputMatrix_IQR->getBuffer()[i*l_pOutputMatrix_IQR->getDimensionSize(1)]=l_fup-l_flow;
			}
		}
		if(ip_bStatisticPercentileActive)
		{
			uint64 l_ui64Value=m_ui64PercentileValue;
			for(uint32 i = 0; i <l_pOutputMatrix_Percentile->getDimensionSize(0); i++)
			{
				l_pOutputMatrix_Percentile->getBuffer()[i*l_pOutputMatrix_Percentile->getDimensionSize(1)]=m_oSortMatrix.getBuffer()[i*m_oSortMatrix.getDimensionSize(1)+std::max(int(0),int(m_oSortMatrix.getDimensionSize(1)*l_ui64Value/100-1))];
			}
		}

		this->activateOutputTrigger(OVP_Algorithm_UnivariateStatistic_OutputTriggerId_ProcessDone, true);
	}

	return true;
}

boolean CAlgoUnivariateStatistic::setMatrixDimension(IMatrix* m_pMatrix, IMatrix* m_pReferenceMatrix)
{
	if(m_pReferenceMatrix->getDimensionCount()<2)
	{
		this->getLogManager() << LogLevel_Warning << "Input matrix doesn't respect basic criteria (2 Dimensions)\n";
		return false;
	}
	m_pMatrix->setDimensionCount(m_pReferenceMatrix->getDimensionCount());
	m_pMatrix->setDimensionSize(0, m_pReferenceMatrix->getDimensionSize(0));
	m_pMatrix->setDimensionSize(1, 1);
	OpenViBEToolkit::Tools::Matrix::clearContent(*m_pMatrix);
	return true;
}
