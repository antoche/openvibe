#include "ovpCApplyTemporalFilter.h"

#include <math.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;
using namespace itpp;
using namespace std;
// ________________________________________________________________________________________________________________
//

void ComputeFilterInitialCondition(vec b, vec a, vec &zi)
{
	int na, nb,  j, i;
	na = length(a);
	nb = length(b);

	//--------------------------------------
	// use sparse matrix to solve system of linear equations for initial conditions
	// zi are the steady-state states of the filter b(z)/a(z) in the state-space 
	//implementation of the 'filter' command.

	mat eye_1;
	eye_1 = eye(na-1);

	mat eye_2;
	eye_2 = eye(na-2);

	vec a1(na-1);
	vec zeros_1(na-2);
	zeros_1 = zeros(na-2);
	vec b1(na-1);
	vec a2(na-1);

	for(j=1 ; j<na; j++)
	{
		a1[j-1] = - a[j];
		b1[j-1] =   b[j];
		a2[j-1] =   a[j];
	}

	mat mat_conc_01;
	mat mat_zeros_1;

	mat_zeros_1 = zeros(na-2,1);	
	mat_conc_01 = concat_vertical(eye_2,transpose(mat_zeros_1));
	
	mat mat_a1;
	mat_a1 =zeros(na-1,1);
	mat_a1.set_col (0, a1);

	mat mat_conc_02;
	mat_conc_02 = concat_horizontal(mat_a1,mat_conc_01);
	
	mat mat_Num;
	mat_Num = eye_1 - mat_conc_02;


	vec vec_Denom(na-1);
	for(i=0 ; i<na-1 ; i++)
	{
		vec_Denom[i] = b1[i] - (a2[i] * b[0]);
	}

	zi = inv(mat_Num) * vec_Denom;
}









void IIR_Filter (vec b, vec a, vec data, vec V0, vec &dataFiltered, vec &Vf)
{
	int i,j,i_V0;
	double Sum_a_tmp,Sum_b_tmp;
	double Sum_tmp_Vf;
	double V0_tmp;
	int na, nb, dataSize;
	na = length(a);
	nb = length(b);
	dataSize = length(data);
		
	if(dataSize<nb)
	{
		for (i=0;i<dataSize;i++)
		{
			Sum_b_tmp = 0.0;
			for (j=0;j<=i;j++)
			{
				Sum_b_tmp = Sum_b_tmp + (b[j] * data[i-j]);
			}	


			Sum_a_tmp = 0.0;
	
			for (j=0;j<=i;j++)
			{
				Sum_a_tmp = Sum_a_tmp + (a[j] * dataFiltered[i-j]);
			}
			dataFiltered[i] = Sum_b_tmp - Sum_a_tmp + V0[i];
		}

		for (i=0;i<(nb-1);i++)
		{
			Sum_tmp_Vf=0.0;
			V0_tmp = 0.0;
			for (j=0;j<(nb-1);j++)
			{
				if ((i+j)<(nb-1))
				{
					if ((dataSize-1-j) >= 0)
					{
						Sum_tmp_Vf = Sum_tmp_Vf + (b[i+j+1] * data[dataSize-1-j]) - (a[i+j+1]*dataFiltered[dataSize-1-j]);
						i_V0 = i+j+1;
					}
					if ((dataSize-1-j) < 0)
					{
						V0_tmp = V0[i_V0];
					}
				}
			}
			Vf[i] = Sum_tmp_Vf +  V0_tmp;
		}
	}
	else
	{
		for (i=0;i<nb-1;i++)
		{
			Sum_b_tmp = 0.0;
			for (j=0;j<=i;j++)
			{
				Sum_b_tmp = Sum_b_tmp + (b[j] * data[i-j]);
			}	
			Sum_a_tmp = 0.0;
	
			for (j=0;j<=i;j++)
			{
				Sum_a_tmp = Sum_a_tmp + (a[j] * dataFiltered[i-j]);
			}
			dataFiltered[i] = Sum_b_tmp - Sum_a_tmp + V0[i];	
		}
	
	
		for (i=nb-1;i<dataSize;i++)
		{
			Sum_b_tmp = 0.0;
			for (j=0;j<nb;j++)
			{
				Sum_b_tmp = Sum_b_tmp + (b[j] * data[i-j]);
			}
			Sum_a_tmp = 0.0;
			for (j=0;j<nb;j++)
			{
				Sum_a_tmp = Sum_a_tmp + (a[j] * dataFiltered[i-j]);
			}	
			dataFiltered[i] = Sum_b_tmp - Sum_a_tmp;
		}
	
		for (i=0;i<nb-1;i++)
		{	
			Sum_tmp_Vf=0.0;
			for (j=i;j<nb-1;j++)
			{
				Sum_tmp_Vf = Sum_tmp_Vf + (b[j+1] * data[dataSize-1-j+i]) - (a[j+1] * dataFiltered[dataSize-1-j+i]);
			}
			Vf[i] = Sum_tmp_Vf;
		}
	}


}




void Filtfilt (vec b, vec a, vec data, vec &dataFiltered)
{
	int na, nb, dataSize, j,  LengthEdgeTransients;
	na = length(a);
	nb = length(b);
	dataSize = length(data);
	LengthEdgeTransients = 3*(nb - 1 );

	vec x_b(dataSize + (2*LengthEdgeTransients));
	vec y_b(dataSize + (2*LengthEdgeTransients));
	vec y_b_2(dataSize + (2*LengthEdgeTransients));
	vec y_c(dataSize + (2*LengthEdgeTransients));
	vec y_c_2(dataSize + (2*LengthEdgeTransients));

	x_b = zeros(dataSize+ (2*LengthEdgeTransients));
	y_b = zeros(dataSize+ (2*LengthEdgeTransients));
	y_b_2 = zeros(dataSize+ (2*LengthEdgeTransients));
	y_c = zeros(dataSize+ (2*LengthEdgeTransients));
	y_c_2 = zeros(dataSize+ (2*LengthEdgeTransients));
	
	
	vec zi(na-1);	
	ComputeFilterInitialCondition(b,a, zi);

	for(j=0 ; j<LengthEdgeTransients; j++)
	{
		x_b [j]	 = (2*data[0]) - data[LengthEdgeTransients-j];	
	}
	for(j=0 ; j<dataSize; j++)
	{
		x_b [j+LengthEdgeTransients] = data[j];
	}

	for(j=0 ; j<LengthEdgeTransients ; j++)
	{
		x_b [j+LengthEdgeTransients+dataSize]	 = (2*data[dataSize-1]) - data[dataSize - j - 2];		
	}

	vec zi_Chan(na-1);
	for(j=0 ; j<na-1; j++)
	{
			zi_Chan[j] = zi[j]*x_b[0];
	}

	vec vec_FinalStates(na-1);

	IIR_Filter (b, a, x_b, zi_Chan,y_b,vec_FinalStates);

	for(j=0 ; j<dataSize + (2*LengthEdgeTransients); j++)
	{
		y_c[j] = y_b [(dataSize + (2*LengthEdgeTransients)) -1 -j];
	}

	vec zi_Chan_2(na-1);

	for(j=0 ; j< na-1; j++)
	{
		zi_Chan_2[j] = zi[j]*y_c[0];				
	}
	
	IIR_Filter (b, a, y_c, zi_Chan_2,y_b_2,vec_FinalStates);

		
	for(j=0 ; j<dataSize + (2*LengthEdgeTransients); j++)
	{
		y_c_2[j] = y_b_2 [(dataSize + (2*LengthEdgeTransients)) -1 -j];
	}

	
	for(j=0 ; j<dataSize; j++)
	{
		dataFiltered[j]	 = y_c_2[j+LengthEdgeTransients];	
	}
}




boolean CApplyTemporalFilter::initialize(void)
{
	ip_pSignalMatrix.initialize(getInputParameter(OVP_Algorithm_ApplyTemporalFilter_InputParameterId_SignalMatrix));
	ip_pFilterCoefficientsMatrix.initialize(getInputParameter(OVP_Algorithm_ApplyTemporalFilter_InputParameterId_FilterCoefficientsMatrix));
	op_pSignalMatrix.initialize(getOutputParameter(OVP_Algorithm_ApplyTemporalFilter_OutputParameterId_FilteredSignalMatrix));

	return true;
}

boolean CApplyTemporalFilter::uninitialize(void)
{
	op_pSignalMatrix.uninitialize();
	ip_pFilterCoefficientsMatrix.uninitialize();
	ip_pSignalMatrix.uninitialize();

	return true;
}

// 	
//

boolean CApplyTemporalFilter::process(void)
{
	IMatrix* l_pSignalInputMatrix=ip_pSignalMatrix;
	IMatrix* l_pOutputMatrix=op_pSignalMatrix;

	if(isInputTriggerActive(OVP_Algorithm_ApplyTemporalFilter_InputTriggerId_Initialize))
	{
		m_bFlagInitialize  = true;

		OpenViBEToolkit::Tools::MatrixManipulation::copyDescription(*l_pOutputMatrix, *l_pSignalInputMatrix);
		OpenViBEToolkit::Tools::MatrixManipulation::clearContent(*l_pOutputMatrix);

		// dimension of input coef (numerator, denominator) filter
		uint32 l_ui32FilterCoefficientsNumeratorDimensionSize=ip_pFilterCoefficientsMatrix->getDimensionSize(1);
		uint32 l_ui32FilterCoefficientsDenominatorDimensionSize=ip_pFilterCoefficientsMatrix->getDimensionSize(0);

		//coef filters vars
		IMatrix* l_pFilterCoefficientsInputMatrix=ip_pFilterCoefficientsMatrix;
		float64* l_pFilterCoefficientsInput =l_pFilterCoefficientsInputMatrix->getBuffer();

		m_vecDenomCoefFilter = zeros(l_ui32FilterCoefficientsDenominatorDimensionSize);
		m_vecNumCoefFilter= zeros(l_ui32FilterCoefficientsNumeratorDimensionSize);


		for (uint32 i=0; i<l_ui32FilterCoefficientsNumeratorDimensionSize;i++)
		{
			m_vecNumCoefFilter[i] = l_pFilterCoefficientsInput[i];
		}


		for (uint32 i=0; i<l_ui32FilterCoefficientsDenominatorDimensionSize;i++)
		{
			m_vecDenomCoefFilter[i] = l_pFilterCoefficientsInput[ l_ui32FilterCoefficientsNumeratorDimensionSize + i];
		}
		
	}

	if(isInputTriggerActive(OVP_Algorithm_ApplyTemporalFilter_InputTriggerId_ApplyFilter))
	{
		// signal input vars
		float64* l_pSignalInput = l_pSignalInputMatrix->getBuffer();

		// dimension of input signal buffer
		uint32 l_ui32SignalInputMatrixDimensionCount=ip_pSignalMatrix->getDimensionCount();
		uint32 l_ui32SignalInputMatrixDimensionNbChannels=ip_pSignalMatrix->getDimensionSize(0);
		uint32 l_ui32SignalInputMatrixDimensionSizeEpoch=ip_pSignalMatrix->getDimensionSize(1);

		// signal output vars
		l_pOutputMatrix->setDimensionCount(l_ui32SignalInputMatrixDimensionCount);
		for (uint32 i=0 ; i<l_ui32SignalInputMatrixDimensionCount; i++)
		{
			l_pOutputMatrix->setDimensionSize(i,ip_pSignalMatrix->getDimensionSize(i));
		}
		float64* l_pFilteredSignalMatrix= l_pOutputMatrix->getBuffer();

		vec y(l_ui32SignalInputMatrixDimensionSizeEpoch);		
		vec x(l_ui32SignalInputMatrixDimensionSizeEpoch);
		x = zeros(l_ui32SignalInputMatrixDimensionSizeEpoch);
			

		for (uint64 i=0;  i < l_ui32SignalInputMatrixDimensionNbChannels; i++)
		{
			for(uint64 j=0 ; j<l_ui32SignalInputMatrixDimensionSizeEpoch; j++)
			{
				x[j] =  (double)l_pSignalInput[i*l_ui32SignalInputMatrixDimensionSizeEpoch+j];
			}

			// --- Modif Manu
			Filtfilt (m_vecNumCoefFilter, m_vecDenomCoefFilter, x, y);
			// --- Fin Modif Manu

			for(uint64 k=0 ; k<l_ui32SignalInputMatrixDimensionSizeEpoch ; k++)
			{
				l_pFilteredSignalMatrix[i*l_ui32SignalInputMatrixDimensionSizeEpoch+k] = y[k];
			}
		}
	}

	if(isInputTriggerActive(OVP_Algorithm_ApplyTemporalFilter_InputTriggerId_ApplyFilterWithHistoric))
	{
		// signal input vars
		float64* l_pSignalInput = l_pSignalInputMatrix->getBuffer();

		// dimension of input signal biuffer
		uint32 l_ui32SignalInputMatrixDimensionCount=ip_pSignalMatrix->getDimensionCount();
		uint32 l_ui32SignalInputMatrixDimensionNbChannels=ip_pSignalMatrix->getDimensionSize(0);
		uint32 l_ui32SignalInputMatrixDimensionSizeEpoch=ip_pSignalMatrix->getDimensionSize(1);
		
		// historic buffers
		if (m_bFlagInitialize)
		{	
			// --- Modif Manu
			vec Zi(ip_pFilterCoefficientsMatrix->getDimensionSize(0)-1);
			Zi = zeros(ip_pFilterCoefficientsMatrix->getDimensionSize(0)-1);
			ComputeFilterInitialCondition(m_vecNumCoefFilter,m_vecDenomCoefFilter, Zi);

			m_oCurrentStates.resize(l_ui32SignalInputMatrixDimensionNbChannels);
			for(size_t i=0 ; i<l_ui32SignalInputMatrixDimensionNbChannels ; i++)
			{			
				m_oCurrentStates[i] = Zi*(double)l_pSignalInput[i*l_ui32SignalInputMatrixDimensionSizeEpoch];
			}
			// --- Fin Modif Manu

			m_bFlagInitialize = false;
		}

		// signal output vars
		l_pOutputMatrix->setDimensionCount(l_ui32SignalInputMatrixDimensionCount);
		for (uint32 i=0 ; i<l_ui32SignalInputMatrixDimensionCount; i++)
		{
			l_pOutputMatrix->setDimensionSize(i,ip_pSignalMatrix->getDimensionSize(i));
		}
		float64* l_pFilteredSignalMatrix= l_pOutputMatrix->getBuffer();

		vec x(l_ui32SignalInputMatrixDimensionSizeEpoch);
		x = zeros(l_ui32SignalInputMatrixDimensionSizeEpoch);
		
		vec y(l_ui32SignalInputMatrixDimensionSizeEpoch);
		//y = zeros(l_ui32SignalInputMatrixDimensionSizeEpoch);

		for (uint64 i=0;  i < l_ui32SignalInputMatrixDimensionNbChannels; i++)
		{
			for(uint64 j=0 ; j<l_ui32SignalInputMatrixDimensionSizeEpoch; j++)
			{
				x[j] =  (double)l_pSignalInput[i*l_ui32SignalInputMatrixDimensionSizeEpoch+j];
			}
			// --- Modif Manu
			y = zeros(l_ui32SignalInputMatrixDimensionSizeEpoch);
			IIR_Filter (m_vecNumCoefFilter,m_vecDenomCoefFilter, x, m_oCurrentStates[i],y,m_oCurrentStates[i]);
			// --- Fin Modif Manu

			for(uint64 k=0 ; k<l_ui32SignalInputMatrixDimensionSizeEpoch ; k++)
			{
				l_pFilteredSignalMatrix[i*l_ui32SignalInputMatrixDimensionSizeEpoch+k] = y[k];
			}
		}
	}

	return true;
}

