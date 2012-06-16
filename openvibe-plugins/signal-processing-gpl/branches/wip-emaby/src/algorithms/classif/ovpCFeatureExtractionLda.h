#ifndef __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Filter_CFeatureExtractionLda_H__
#define __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Filter_CFeatureExtractionLda_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <itpp/itbase.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CFeatureExtractionLda : virtual public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_FeatureExtractionLda);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pSignalEpochTable;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pClass1SamplesNumber;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrixFirstClass;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrixSecondClass;
			OpenViBE::uint64 m_uint64IndexTainingSet;
			OpenViBE::float64 * m_pOutputFirstClass;
			OpenViBE::float64 * m_pOutputSecondClass;
			OpenViBE::boolean m_bFirstTime;
		};

		class CFeatureExtractionLdaDesc : virtual public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Features Extractor for Lda training"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Algorithm/Classification"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_FeatureExtractionLda; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CFeatureExtractionLda(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmProto) const
			{
				rAlgorithmProto.addInputParameter (OVP_Algorithm_FeatureExtractionLda_InputParameterId_EpochTable,          "Epoch table",            OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_FeatureExtractionLda_InputParameterId_Class1SamplesNumber, "Class 1 samples number", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_FeatureExtractionLda_OutputParameterId_MatrixFirstClass,   "Matrix first class",     OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_FeatureExtractionLda_OutputParameterId_MatrixSecondClass,  "Matrix second class",    OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_FeatureExtractionLda_InputTriggerId_Initialize,            "Initialize");
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_FeatureExtractionLda_InputTriggerId_ExtractFeature,        "Extract features");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_FeatureExtractionLdaDesc);
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Filter_CFeatureExtractionLda_H__
