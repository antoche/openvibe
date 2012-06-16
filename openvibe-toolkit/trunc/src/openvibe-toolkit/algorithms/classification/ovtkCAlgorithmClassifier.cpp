#include "ovtkCAlgorithmClassifier.h"
#include "ovtkCFeatureVector.hpp"
#include "ovtkCFeatureVectorSet.hpp"
#include "ovtkCVector.hpp"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEToolkit;

boolean CAlgorithmClassifier::process(void)
{
	TParameterHandler < IMatrix* > ip_pFeatureVector(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVector));
	TParameterHandler < IMemoryBuffer* > ip_pConfiguration(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_Configuration));
	TParameterHandler < float64 > op_pClass(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Class));
	TParameterHandler < IMatrix* > op_pClassificationValues(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ClassificationValues));

	TParameterHandler < IMatrix* > ip_pFeatureVectorSet(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVectorSet));
	TParameterHandler < IMemoryBuffer* > op_pConfiguration(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration));

	if(this->isInputTriggerActive(OVTK_Algorithm_Classifier_InputTriggerId_Train))
	{
		IMatrix* l_pFeatureVectorSet=ip_pFeatureVectorSet;
		if(!l_pFeatureVectorSet)
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Feature vector set matrix is NULL\n";
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
		}
		else
		{
			CFeatureVectorSet l_oFeatureVectorSetAdapter(*l_pFeatureVectorSet);
			if(this->train(l_oFeatureVectorSetAdapter))
			{
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Success, true);
			}
			else
			{
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
			}
		}
	}

	if(this->isInputTriggerActive(OVTK_Algorithm_Classifier_InputTriggerId_Classify))
	{
		IMatrix* l_pFeatureVector=ip_pFeatureVector;
		float64 l_f64Class=0;
		IMatrix* l_pClassificationValues=op_pClassificationValues;
		if(!l_pFeatureVector || !l_pClassificationValues)
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Either feature vector matrix is NULL or classification values matrix is NULL\n";
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
		}
		else
		{
			CFeatureVector l_oFeatureVectorAdapter(*l_pFeatureVector);
			CVector l_oClassificationValuesAdapter(*l_pClassificationValues);

			if(this->classify(l_oFeatureVectorAdapter, l_f64Class, l_oClassificationValuesAdapter))
			{
				op_pClass=l_f64Class;

				/* --------------------------------------------------------------------------
				this->getLogManager() << LogLevel_Trace << "Classified feature vector [ ";
				for(i=0; i<l_oFeatureVectorAdapter.getSize(); i++) this->getLogManager() << l_oFeatureVectorAdapter[i] << " ";
				this->getLogManager() << "] with class " << l_f64Class << " and status [ ";
				for(i=0; i<l_oClassificationValuesAdapter.getSize(); i++) this->getLogManager() << l_oClassificationValuesAdapter[i] << " ";
				this->getLogManager() << "]\n";
				-------------------------------------------------------------------------- */

				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Success, true);
			}
			else
			{
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
			}
		}
	}

	if(this->isInputTriggerActive(OVTK_Algorithm_Classifier_InputTriggerId_SaveConfiguration))
	{
		IMemoryBuffer* l_pConfiguration=op_pConfiguration;
		if(!l_pConfiguration)
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Configuration memory buffer is NULL\n";
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
		}
		else
		{
			l_pConfiguration->setSize(0, true);
			if(this->saveConfiguration(*l_pConfiguration))
			{
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Success, true);
			}
			else
			{
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
			}
		}
	}

	if(this->isInputTriggerActive(OVTK_Algorithm_Classifier_InputTriggerId_LoadConfiguration))
	{
		IMemoryBuffer* l_pConfiguration=ip_pConfiguration;
		if(!l_pConfiguration)
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Configuration memory buffer is NULL\n";
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
		}
		else
		{
			if(this->loadConfiguration(*l_pConfiguration))
			{
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Success, true);
			}
			else
			{
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
			}
		}
	}

	return true;
}
