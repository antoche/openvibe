#ifndef __OpenViBEPlugins_Algorithm_ConfusionMatrix_H__
#define __OpenViBEPlugins_Algorithm_ConfusionMatrix_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#define OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_Sums                     OpenViBE::CIdentifier(0x75502E8E, 0x05D838EE)
#define OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_Percentage               OpenViBE::CIdentifier(0x7E504E8E, 0x058858EE)
#define OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_TargetStimulationSet     OpenViBE::CIdentifier(0x7E504E8F, 0x058858EF)
#define OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_ClassifierStimulationSet OpenViBE::CIdentifier(0x45220B61, 0x13FD7491)
#define OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_ClassCodes               OpenViBE::CIdentifier(0x67780C91, 0x2A556C51)
#define OVP_Algorithm_ConfusionMatrixAlgorithm_OutputParameterId_ConfusionMatrix         OpenViBE::CIdentifier(0x67780C91, 0x2A556C51)

#define OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_ResetTarget                OpenViBE::CIdentifier(0x4D390BDA, 0x6A180667)
#define OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_ResetClassifier            OpenViBE::CIdentifier(0x3C132C38, 0x557D2503)
#define OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_FeedTarget                 OpenViBE::CIdentifier(0x6B1E76B3, 0x06741B21)
#define OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_FeedClassifier             OpenViBE::CIdentifier(0x3EFC64B8, 0x5ACC3125)
#define OVP_Algorithm_ConfusionMatrixAlgorithm_OutputTriggerId_ConfusionPerformed        OpenViBE::CIdentifier(0x790C2277, 0x3D041A63)

#define OVP_ClassId_Algorithm_ConfusionMatrix                                            OpenViBE::CIdentifier(0x699F416B, 0x3BAE4324)
#define OVP_ClassId_Algorithm_ConfusionMatrixDesc                                        OpenViBE::CIdentifier(0x4CDD225D, 0x6C9A59DB)

#include <map>
#include <vector>
#include <sstream>

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CAlgorithmConfusionMatrix : virtual public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_ConfusionMatrix);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bPercentages;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bSums;

			// input TARGET
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pTargetStimulationSet;
			// deduced timeline:
			std::map<OpenViBE::uint64,OpenViBE::uint64> m_mTargetsTimeLine;

			// input CLASSIFIER
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pClassifierStimulationSet;

			//CONFUSION MATRIX computing
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pClassesCodes;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pConfusionMatrix;

			OpenViBE::CMatrix m_oConfusionMatrix; // the values, not percentage
			std::map < OpenViBE::uint64, OpenViBE::uint32 > m_mapClassificationAttemptCountPerClass;

		private:

			OpenViBE::boolean isClass(OpenViBE::uint64 StimulationIdentifier);
			OpenViBE::uint32 getClassIndex(OpenViBE::uint64 StimulationIdentifier);
		};

		class CAlgorithmConfusionMatrixDesc : virtual public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Confusion Matrix Algorithm"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Laurent Bonnet"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Make a confusion matrix out of classification results coming from one classifier."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Classification"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ConfusionMatrix; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Classification::CAlgorithmConfusionMatrix; }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputParameter  (OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_TargetStimulationSet,     "Targets",                OpenViBE::Kernel::ParameterType_StimulationSet);
				rAlgorithmPrototype.addInputParameter  (OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_ClassifierStimulationSet, "Classification results", OpenViBE::Kernel::ParameterType_StimulationSet);
				rAlgorithmPrototype.addInputParameter  (OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_ClassCodes,               "Class codes",            OpenViBE::Kernel::ParameterType_StimulationSet);
				rAlgorithmPrototype.addInputParameter  (OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_Percentage,               "Percentage",             OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmPrototype.addInputParameter  (OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_Sums,                     "Sums",                   OpenViBE::Kernel::ParameterType_Boolean);

				rAlgorithmPrototype.addOutputParameter (OVP_Algorithm_ConfusionMatrixAlgorithm_OutputParameterId_ConfusionMatrix,           "Confusion matrix",     OpenViBE::Kernel::ParameterType_Matrix);

				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_ResetTarget,                   "Reset Target");
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_ResetClassifier,               "Reset Classifier");
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_FeedTarget,                    "Feed Target");
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_FeedClassifier,                "Feed Classifier");

				rAlgorithmPrototype.addOutputTrigger  (OVP_Algorithm_ConfusionMatrixAlgorithm_OutputTriggerId_ConfusionPerformed,           "Confusion computing performed");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_ConfusionMatrixDesc);
		};
	};
};

#endif // __OpenViBEPlugins_Algorithm_ConfusionMatrix_H__
