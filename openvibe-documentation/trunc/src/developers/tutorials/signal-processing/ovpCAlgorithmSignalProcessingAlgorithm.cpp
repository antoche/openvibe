#include "ovpCAlgorithmSignalProcessingAlgorithm.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;

boolean CAlgorithmSignalProcessingAlgorithm::initialize(void)
{
	ip_pMatrix.initialize(this->getInputParameter(OVP_Algorithm_SignalProcessingAlgorithm_InputParameterId_Matrix));
	op_pMatrix.initialize(this->getOutputParameter(OVP_Algorithm_SignalProcessingAlgorithm_OutputParameterId_Matrix));
	return true;
}

boolean CAlgorithmSignalProcessingAlgorithm::uninitialize(void)
{
	op_pMatrix.uninitialize();
	ip_pMatrix.uninitialize();
	return true;
}

boolean CAlgorithmSignalProcessingAlgorithm::process(void)
{
	IMatrix* l_pInputMatrix=ip_pMatrix;
	IMatrix* l_pOutputMatrix=op_pMatrix;

	if(this->isInputTriggerActive(OVP_Algorithm_SignalProcessingAlgorithm_InputTriggerId_Initialize))
	{
		OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pOutputMatrix, *l_pInputMatrix);
	}

	if(this->isInputTriggerActive(OVP_Algorithm_SignalProcessingAlgorithm_InputTriggerId_Process))
	{
		OpenViBEToolkit::Tools::Matrix::copyContent(*l_pOutputMatrix, *l_pInputMatrix);
		for(uint32 i=0; i<l_pInputMatrix->getDimensionSize(0); i++)
		{
			l_pOutputMatrix->getBuffer()[i*l_pInputMatrix->getDimensionSize(1)]=0;
		}

		this->activateOutputTrigger(OVP_Algorithm_SignalProcessingAlgorithm_OutputTriggerId_ProcessDone, true);
	}

	return true;
}
