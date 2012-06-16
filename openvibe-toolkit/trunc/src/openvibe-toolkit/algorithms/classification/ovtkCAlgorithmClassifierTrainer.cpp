#include "ovtkCAlgorithmClassifierTrainer.h"
#include "ovtkCFeatureVectorSet.hpp"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEToolkit;

boolean CAlgorithmClassifierTrainer::process(void)
{
	TParameterHandler < IMatrix* > ip_pFeatureVectorSet(this->getInputParameter(OVTK_Algorithm_ClassifierTrainer_InputParameterId_FeatureVectorSet));
	TParameterHandler < IMemoryBuffer* > op_pConfiguration(this->getOutputParameter(OVTK_Algorithm_ClassifierTrainer_OutputParameterId_Configuration));

	if(this->isInputTriggerActive(OVTK_Algorithm_ClassifierTrainer_InputTriggerId_Train))
	{
		IMatrix* l_pFeatureVectorSet=ip_pFeatureVectorSet;
		if(!l_pFeatureVectorSet)
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Feature vector set matrix is NULL\n";
			this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed, true);
		}
		else
		{
			CFeatureVectorSet l_oFeatureVectorSetAdapter(*l_pFeatureVectorSet);
			if(this->train(l_oFeatureVectorSetAdapter))
			{
				this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Success, true);
			}
			else
			{
				this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed, true);
			}
		}
	}

	if(this->isInputTriggerActive(OVTK_Algorithm_ClassifierTrainer_InputTriggerId_SaveConfiguration))
	{
		IMemoryBuffer* l_pConfiguration=op_pConfiguration;
		if(!l_pConfiguration)
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Configuration memory buffer is NULL\n";
			this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed, true);
		}
		else
		{
			l_pConfiguration->setSize(0, true);
			if(this->saveConfiguration(*l_pConfiguration))
			{
				this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Success, true);
			}
			else
			{
				this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed, true);
			}
		}
	}

	return true;
}
