#include "ovpCDetectingMinMax.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;
using namespace std;
// ________________________________________________________________________________________________________________
//

boolean CDetectingMinMax::initialize(void)
{
	ip_pSignalMatrix.initialize(getInputParameter(OVP_Algorithm_DetectingMinMax_InputParameterId_SignalMatrix));
	ip_ui64SamplingFrequency.initialize(getInputParameter(OVP_Algorithm_DetectingMinMax_InputParameterId_SamplingFrequency));
	ip_f64TimeWindowStart.initialize(getInputParameter(OVP_Algorithm_DetectingMinMax_InputParameterId_TimeWindowStart));
	ip_f64TimeWindowEnd.initialize(getInputParameter(OVP_Algorithm_DetectingMinMax_InputParameterId_TimeWindowEnd));

	op_pSignalMatrix.initialize(getOutputParameter(OVP_Algorithm_DetectingMinMax_OutputParameterId_SignalMatrix));

	return true;
}

boolean CDetectingMinMax::uninitialize(void)
{
	op_pSignalMatrix.uninitialize();

	ip_f64TimeWindowEnd.uninitialize();
	ip_f64TimeWindowStart.uninitialize();
	ip_ui64SamplingFrequency.uninitialize();
	ip_pSignalMatrix.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CDetectingMinMax::process(void)
{
	// signal input vars
	IMatrix* l_pSignalInputMatrix=ip_pSignalMatrix;
	float64* l_pSignalInput = l_pSignalInputMatrix->getBuffer();

	// signal output vars
	IMatrix* l_pSignalOutputMatrix=op_pSignalMatrix;
	l_pSignalOutputMatrix->setDimensionCount(2);
	l_pSignalOutputMatrix->setDimensionSize(0,1);
	l_pSignalOutputMatrix->setDimensionSize(1, 1);

	float64* l_pSignalOutput = l_pSignalOutputMatrix->getBuffer();

	if(isInputTriggerActive(OVP_Algorithm_DetectingMinMax_InputTriggerId_Initialize))
	{
	}

	if(isInputTriggerActive(OVP_Algorithm_DetectingMinMax_InputTriggerId_DetectsMin))
	{
		// dimension of input signal biuffer
		uint32 l_ui32SignalInputMatrixDimensionNbChannels=ip_pSignalMatrix->getDimensionSize(0);
		uint32 l_ui32SignalInputMatrixDimensionSizeEpoch=ip_pSignalMatrix->getDimensionSize(1);

		// Must be changed
		float64 l_f64MinValue = 1E10;

		for (uint64 i=0;  i < l_ui32SignalInputMatrixDimensionNbChannels; i++)
		{
			for(uint64 j=0 ; j<l_ui32SignalInputMatrixDimensionSizeEpoch; j++)
			{
				if (l_pSignalInput[i*l_ui32SignalInputMatrixDimensionSizeEpoch+j] < l_f64MinValue)
				{
					l_f64MinValue = l_pSignalInput[i*l_ui32SignalInputMatrixDimensionSizeEpoch+j];
				}
			}
		}
		l_pSignalOutput[0] =  l_f64MinValue;
	}

	if(isInputTriggerActive(OVP_Algorithm_DetectingMinMax_InputTriggerId_DetectsMax))
	{
		// dimension of input signal biuffer
		uint32 l_ui32SignalInputMatrixDimensionNbChannels=ip_pSignalMatrix->getDimensionSize(0);
		uint32 l_ui32SignalInputMatrixDimensionSizeEpoch=ip_pSignalMatrix->getDimensionSize(1);

		// Must be changed
		float64 l_f64MaxValue = -1E10;

		for (uint64 i=0;  i < l_ui32SignalInputMatrixDimensionNbChannels; i++)
		{
			uint64 l_ui64Start=static_cast<uint64>(floor(ip_f64TimeWindowStart/1000.*ip_ui64SamplingFrequency));
			uint64 l_ui64Stop=static_cast<uint64>(floor(ip_f64TimeWindowEnd/1000.*ip_ui64SamplingFrequency));
			for(uint64 j=l_ui64Start; j<l_ui64Stop; j++)
			{
				if (l_pSignalInput[i*l_ui32SignalInputMatrixDimensionSizeEpoch+j] > l_f64MaxValue)
				{
					l_f64MaxValue = l_pSignalInput[i*l_ui32SignalInputMatrixDimensionSizeEpoch+j];
				}
			}
		}
		l_pSignalOutput[0] =  l_f64MaxValue;
	}

	return true;
}
