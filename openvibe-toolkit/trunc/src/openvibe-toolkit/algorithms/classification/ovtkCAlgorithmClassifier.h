#ifndef __OpenViBEToolkit_Algorithm_Classifier_H__
#define __OpenViBEToolkit_Algorithm_Classifier_H__

#include "../ovtkTAlgorithm.h"
#include "../../ovtkIVector.h"
#include "../../ovtkIFeatureVector.h"
#include "../../ovtkIFeatureVectorSet.h"

#define OVTK_ClassId_Algorithm_Classifier                                OpenViBE::CIdentifier(0x3B910935, 0xE4DBACC4)
#define OVTK_ClassId_Algorithm_ClassifierDesc                            OpenViBE::CIdentifier(0xFDB84F2F, 0x2F5C510D)

#define OVTK_Algorithm_Classifier_InputParameterId_FeatureVector         OpenViBE::CIdentifier(0x6D69BF98, 0x1EB9EE66)
#define OVTK_Algorithm_Classifier_InputParameterId_FeatureVectorSet      OpenViBE::CIdentifier(0x27C05927, 0x5DE9103A)
#define OVTK_Algorithm_Classifier_InputParameterId_Configuration         OpenViBE::CIdentifier(0xA705428E, 0x5BB1CADD)

#define OVTK_Algorithm_Classifier_OutputParameterId_Class                OpenViBE::CIdentifier(0x8A39A7EA, 0xF2EE45C4)
#define OVTK_Algorithm_Classifier_OutputParameterId_ClassificationValues OpenViBE::CIdentifier(0xDA77D7E4, 0x766B48EA)
#define OVTK_Algorithm_Classifier_OutputParameterId_Configuration        OpenViBE::CIdentifier(0x30590936, 0x61CE5971)

#define OVTK_Algorithm_Classifier_InputTriggerId_Train                   OpenViBE::CIdentifier(0x34684752, 0x78A46DE2)
#define OVTK_Algorithm_Classifier_InputTriggerId_Classify                OpenViBE::CIdentifier(0x843A87D8, 0x566E85A1)
#define OVTK_Algorithm_Classifier_InputTriggerId_SaveConfiguration       OpenViBE::CIdentifier(0x79750528, 0x6CC85FC1)
#define OVTK_Algorithm_Classifier_InputTriggerId_LoadConfiguration       OpenViBE::CIdentifier(0xF346BBE0, 0xADAFC735)

#define OVTK_Algorithm_Classifier_OutputTriggerId_Success                OpenViBE::CIdentifier(0x24FAB755, 0x78868782)
#define OVTK_Algorithm_Classifier_OutputTriggerId_Failed                 OpenViBE::CIdentifier(0x6E72B255, 0x317FAA04)


namespace OpenViBEToolkit
{
	class OVTK_API CAlgorithmClassifier : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
	{
	public:

		virtual void release(void) { delete this; }

		virtual OpenViBE::boolean process(void);

		virtual OpenViBE::boolean train(const OpenViBEToolkit::IFeatureVectorSet& rFeatureVectorSet)=0;
		virtual OpenViBE::boolean classify(const OpenViBEToolkit::IFeatureVector& rFeatureVector, OpenViBE::float64& rf64Class, OpenViBEToolkit::IVector& rClassificationValue)=0;

		virtual OpenViBE::boolean saveConfiguration(OpenViBE::IMemoryBuffer& rMemoryBuffer)=0;
		virtual OpenViBE::boolean loadConfiguration(const OpenViBE::IMemoryBuffer& rMemoryBuffer)=0;

		_IsDerivedFromClass_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVTK_ClassId_Algorithm_Classifier);
	};

	class OVTK_API CAlgorithmClassifierDesc : public OpenViBE::Plugins::IAlgorithmDesc
	{
	public:

		virtual OpenViBE::boolean getAlgorithmPrototype(
			OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
		{
			rAlgorithmPrototype.addInputParameter (OVTK_Algorithm_Classifier_InputParameterId_FeatureVector,         "Feature vector",        OpenViBE::Kernel::ParameterType_Matrix);
			rAlgorithmPrototype.addInputParameter (OVTK_Algorithm_Classifier_InputParameterId_FeatureVectorSet,      "Feature vector set",    OpenViBE::Kernel::ParameterType_Matrix);
			rAlgorithmPrototype.addInputParameter (OVTK_Algorithm_Classifier_InputParameterId_Configuration,         "Configuration",         OpenViBE::Kernel::ParameterType_MemoryBuffer);

			rAlgorithmPrototype.addOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Class,                "Class",                 OpenViBE::Kernel::ParameterType_Float);
			rAlgorithmPrototype.addOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ClassificationValues, "Classification values", OpenViBE::Kernel::ParameterType_Matrix);
			rAlgorithmPrototype.addOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration,        "Configuration",         OpenViBE::Kernel::ParameterType_MemoryBuffer);

			rAlgorithmPrototype.addInputTrigger   (OVTK_Algorithm_Classifier_InputTriggerId_Train,                   "Train");
			rAlgorithmPrototype.addInputTrigger   (OVTK_Algorithm_Classifier_InputTriggerId_Classify,                "Classify");
			rAlgorithmPrototype.addInputTrigger   (OVTK_Algorithm_Classifier_InputTriggerId_LoadConfiguration,       "Load configuration");
			rAlgorithmPrototype.addInputTrigger   (OVTK_Algorithm_Classifier_InputTriggerId_SaveConfiguration,       "Save configuration");

			rAlgorithmPrototype.addOutputTrigger  (OVTK_Algorithm_Classifier_OutputTriggerId_Success,                "Success");
			rAlgorithmPrototype.addOutputTrigger  (OVTK_Algorithm_Classifier_OutputTriggerId_Failed,                 "Failed");


			return true;
		}

		_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithmDesc, OVTK_ClassId_Algorithm_ClassifierDesc);
	};
};

#endif // __OpenViBEToolkit_Algorithm_Classifier_H__
