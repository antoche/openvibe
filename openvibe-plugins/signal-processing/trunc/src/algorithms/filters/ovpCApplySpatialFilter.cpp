#if 0
#include "ovpCApplySpatialFilter.h"
#include <cmath>
#include <system/Memory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace std;
// ________________________________________________________________________________________________________________
//

boolean CApplySpatialFilter::initialize(void)
{
	m_oSignalIntputMatrixHandle.initialize(getInputParameter(OVP_Algorithm_ApplySpatialFilter_InputParameterId_SignalMatrix));
	m_oFilterCoefficientsInputMatrixHandle.initialize(getInputParameter(OVP_Algorithm_ApplySpatialFilter_InputParameterId_FilterCoefficientsMatrix));
	m_oSignalOutputMatrixHandle.initialize(getOutputParameter(OVP_Algorithm_ApplySpatialFilter_OutputParameterId_FilteredSignalMatrix));
	m_bActive=true;

	return true;
}

boolean CApplySpatialFilter::uninitialize(void)
{
	m_oSignalIntputMatrixHandle.uninitialize();
	m_oFilterCoefficientsInputMatrixHandle.uninitialize();
	m_oSignalOutputMatrixHandle.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CApplySpatialFilter::process(void)
{
	if(!m_bActive)
	{
		return false;
	}

	IMatrix* l_pFilterCoefficientsInputMatrix = m_oFilterCoefficientsInputMatrixHandle;
	IMatrix* l_pSignalInputMatrix = m_oSignalIntputMatrixHandle;
	IMatrix* l_pSignalOutputMatrix = m_oSignalOutputMatrixHandle;

	// dimensions of the filter coefficients matrix
	uint32 l_ui32FilterCoefficientsLineDimensionSize=m_oFilterCoefficientsInputMatrixHandle->getDimensionSize(0);
	uint32 l_ui32FilterCoefficientsColumnDimensionSize=m_oFilterCoefficientsInputMatrixHandle->getDimensionSize(1);
	// dimensions of the input buffer of signal
	uint32 l_ui32SignalInputMatrixDimensionCount=m_oSignalIntputMatrixHandle->getDimensionCount();
	uint32 l_ui32SignalInputLineDimensionSize=m_oSignalIntputMatrixHandle->getDimensionSize(0);
	uint32 l_ui32SignalInputColumnDimensionSize=m_oSignalIntputMatrixHandle->getDimensionSize(1);

	if(l_ui32FilterCoefficientsColumnDimensionSize != l_ui32SignalInputLineDimensionSize)
	{
		getLogManager() << LogLevel_ImportantWarning  << "Bad matrix size - number of column of filter coefficients = " << l_ui32FilterCoefficientsColumnDimensionSize << " while Number of Line of Signal = " << l_ui32SignalInputLineDimensionSize << "\n";
		m_bActive=false;
		return false;
	}

	// Input vars
	float64* l_pSignalInput = l_pSignalInputMatrix->getBuffer();
	float64* l_pFilterCoefficientsInput = l_pFilterCoefficientsInputMatrix->getBuffer();

	// signal output vars
	l_pSignalOutputMatrix->setDimensionCount(l_ui32SignalInputMatrixDimensionCount);
	l_pSignalOutputMatrix->setDimensionSize(0,l_ui32FilterCoefficientsLineDimensionSize);
	l_pSignalOutputMatrix->setDimensionSize(1,l_ui32SignalInputColumnDimensionSize);
	//~ l_pOutputMatrix->setDimensionLabel(indexDimension, indexSize, label);
	float64* l_pSignalOutput = l_pSignalOutputMatrix->getBuffer();

	if(isInputTriggerActive(OVP_Algorithm_ApplySpatialFilter_InputTriggerId_Initialize))
	{
		for (uint64 i=0;  i < l_ui32SignalInputColumnDimensionSize; i++)
		{
			for(uint64 j=0 ; j<l_ui32FilterCoefficientsLineDimensionSize; j++)
			{
				l_pSignalOutput[i*l_ui32FilterCoefficientsLineDimensionSize+j]=0;
			}
		}
	}

	if(isInputTriggerActive(OVP_Algorithm_ApplySpatialFilter_InputTriggerId_ApplyFilter))
	{
		for (uint64 i=0;  i < l_ui32SignalInputColumnDimensionSize; i++)
		{
			for(uint64 j=0 ; j<l_ui32FilterCoefficientsLineDimensionSize; j++)
			{
				l_pSignalOutput[i*l_ui32FilterCoefficientsLineDimensionSize+j]=0;
			}
		}

		for (uint64 i=0;  i < l_ui32FilterCoefficientsLineDimensionSize; i++)
		{
			for(uint64 j=0 ; j<l_ui32SignalInputColumnDimensionSize; j++)
			{
				for (uint64 k=0; k< l_ui32FilterCoefficientsColumnDimensionSize; k++)
				{
					l_pSignalOutput[i*l_ui32SignalInputColumnDimensionSize+j] = l_pFilterCoefficientsInput[k+l_ui32FilterCoefficientsColumnDimensionSize*i]*l_pSignalInput[j+k*l_ui32SignalInputColumnDimensionSize]+l_pSignalOutput[i*l_ui32SignalInputColumnDimensionSize+j];
				}
			}
		}

	}

	return true;
}
#endif
