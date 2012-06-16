#ifndef __OpenViBEPlugins_Algorithm_ClassifierNULL_H__
#define __OpenViBEPlugins_Algorithm_ClassifierNULL_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#define OVP_ClassId_Algorithm_ClassifierNULL                                        OpenViBE::CIdentifier(0x043D09AB, 0xCB5E4859)
#define OVP_ClassId_Algorithm_ClassifierNULLDesc                                    OpenViBE::CIdentifier(0x3B365233, 0x812C47DD)

#define OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter1                    OpenViBE::CIdentifier(0x6DA99952, 0x7E72C143)
#define OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter2                    OpenViBE::CIdentifier(0xEAC5694A, 0x56CFEF02)
#define OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter3                    OpenViBE::CIdentifier(0x72F6222D, 0x375BAE2C)

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CAlgorithmClassifierNULL : public OpenViBEToolkit::CAlgorithmClassifier
		{
		public:

			CAlgorithmClassifierNULL(void);

			virtual OpenViBE::boolean initialize(void);

			virtual OpenViBE::boolean train(const OpenViBEToolkit::IFeatureVectorSet& rFeatureVectorSet);
			virtual OpenViBE::boolean classify(const OpenViBEToolkit::IFeatureVector& rFeatureVector, OpenViBE::float64& rf64Class, OpenViBEToolkit::IVector& rClassificationValues);

			virtual OpenViBE::boolean saveConfiguration(OpenViBE::IMemoryBuffer& rMemoryBuffer);
			virtual OpenViBE::boolean loadConfiguration(const OpenViBE::IMemoryBuffer& rMemoryBuffer);

			_IsDerivedFromClass_Final_(CAlgorithmClassifier, OVP_ClassId_Algorithm_ClassifierNULL);

		protected:
		};

		class CAlgorithmClassifierNULLDesc : public OpenViBEToolkit::CAlgorithmClassifierDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("NULL Classifier (does nothing)"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ClassifierNULL; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Classification::CAlgorithmClassifierNULL; }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CAlgorithmClassifierDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter1, "Parameter 1", OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter2, "Parameter 2", OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter3, "Parameter 3", OpenViBE::Kernel::ParameterType_Enumeration, OV_TypeId_Stimulation);
				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmClassifierDesc, OVP_ClassId_Algorithm_ClassifierNULLDesc);
		};
	};
};

#endif // __OpenViBEPlugins_Algorithm_ClassifierNULL_H__
