#ifndef __OpenViBEToolkit_Algorithm_ClassifierTrainer_H__
#define __OpenViBEToolkit_Algorithm_ClassifierTrainer_H__

#include "../ovtkTAlgorithm.h"
#include "../../ovtkIFeatureVectorSet.h"

#define OVTK_ClassId_Algorithm_ClassifierTrainer                             OpenViBE::CIdentifier(0x5ABC21E6, 0x3C3D349B)
#define OVTK_ClassId_Algorithm_ClassifierTrainerDesc                         OpenViBE::CIdentifier(0x56DD6401, 0x07033341)
#define OVTK_Algorithm_ClassifierTrainer_InputParameterId_FeatureVectorSet   OpenViBE::CIdentifier(0x27C05927, 0x5DE9103A)
#define OVTK_Algorithm_ClassifierTrainer_OutputParameterId_Configuration     OpenViBE::CIdentifier(0x30590936, 0x61CE5971)
#define OVTK_Algorithm_ClassifierTrainer_InputTriggerId_Train                OpenViBE::CIdentifier(0x34684752, 0x78A46DE2)
#define OVTK_Algorithm_ClassifierTrainer_InputTriggerId_SaveConfiguration    OpenViBE::CIdentifier(0x79750528, 0x6CC85FC1)
#define OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Success             OpenViBE::CIdentifier(0x7B8C0EFF, 0x26224D6B)
#define OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed              OpenViBE::CIdentifier(0x31B97C83, 0x59015D0E)

namespace OpenViBEToolkit
{
	class OVTK_API CAlgorithmClassifierTrainer : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
	{
	public:

		virtual void release(void) { delete this; }

		virtual OpenViBE::boolean process(void);
		virtual OpenViBE::boolean train(const OpenViBEToolkit::IFeatureVectorSet& rFeatureVectorSet)=0;
		virtual OpenViBE::boolean saveConfiguration(OpenViBE::IMemoryBuffer& rMemoryBuffer)=0;

		_IsDerivedFromClass_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVTK_ClassId_Algorithm_ClassifierTrainer);

	protected:
	};

	class OVTK_API CAlgorithmClassifierTrainerDesc : public OpenViBE::Plugins::IAlgorithmDesc
	{
	public:

		virtual OpenViBE::boolean getAlgorithmPrototype(
			OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
		{
			rAlgorithmPrototype.addInputParameter (OVTK_Algorithm_ClassifierTrainer_InputParameterId_FeatureVectorSet, "Feature vector set", OpenViBE::Kernel::ParameterType_Matrix);
			rAlgorithmPrototype.addOutputParameter(OVTK_Algorithm_ClassifierTrainer_OutputParameterId_Configuration,   "Configuration",      OpenViBE::Kernel::ParameterType_MemoryBuffer);
			rAlgorithmPrototype.addInputTrigger   (OVTK_Algorithm_ClassifierTrainer_InputTriggerId_Train,              "Train");
			rAlgorithmPrototype.addInputTrigger   (OVTK_Algorithm_ClassifierTrainer_InputTriggerId_SaveConfiguration,  "Save configuration");
			rAlgorithmPrototype.addOutputTrigger  (OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Success,           "Success");
			rAlgorithmPrototype.addOutputTrigger  (OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed,            "Failed");
			return true;
		}

		_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithmDesc, OVTK_ClassId_Algorithm_ClassifierTrainerDesc);
	};
};

#endif // __OpenViBEToolkit_Algorithm_ClassifierTrainer_H__
