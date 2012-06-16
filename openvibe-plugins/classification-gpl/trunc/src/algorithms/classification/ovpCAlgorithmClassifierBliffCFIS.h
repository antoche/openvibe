#ifndef __OpenViBEPlugins_Algorithm_ClassifierBliffCFIS_H__
#define __OpenViBEPlugins_Algorithm_ClassifierBliffCFIS_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#if defined TARGET_HAS_ThirdPartyBLiFF

#define OVP_ClassId_Algorithm_ClassifierBliffCFIS                                        OpenViBE::CIdentifier(0x595077F4, 0xB3BC4FA8)
#define OVP_ClassId_Algorithm_ClassifierBliffCFISDesc                                    OpenViBE::CIdentifier(0xFFB66F6B, 0x2C959C3A)

#define OVP_TypeId_CFISMode                                                              OpenViBE::CIdentifier(0x2397E74F, 0xCAC8F95C)
#define OVP_TypeId_CFISMode_SimpleGaussian                                               OpenViBE::CIdentifier(0x1EE19D4D, 0x9834754A)
#define OVP_TypeId_CFISMode_TwoSidedGaussian                                             OpenViBE::CIdentifier(0xE227217C, 0x2ECB6AE9)

#define OVP_TypeId_CFISOutputMode                                                        OpenViBE::CIdentifier(0xA1C204DB, 0x5351B110)
#define OVP_TypeId_CFISOutputMode_ClassMembership                                        OpenViBE::CIdentifier(0x97D7C635, 0xA2200BD5)
#define OVP_TypeId_CFISOutputMode_RuleFulfillment                                        OpenViBE::CIdentifier(0x12D38A5E, 0x2DBEA482)

#define OVP_Algorithm_CAlgorithmClassifierCFIS_InputParameterId_Radius                   OpenViBE::CIdentifier(0x67961B8D, 0xABAD78BC)
#define OVP_Algorithm_CAlgorithmClassifierCFIS_InputParameterId_Mode                     OpenViBE::CIdentifier(0x5EE293DA, 0x4C15B1C6)
#define OVP_Algorithm_CAlgorithmClassifierCFIS_InputParameterId_OutputMode               OpenViBE::CIdentifier(0x11469872, 0x43C4B059)

namespace OpenViBEPlugins
{
	namespace Local
	{
		class CAlgorithmClassifierBliffCFIS : public OpenViBEToolkit::CAlgorithmClassifier
		{
		public:

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean train(const OpenViBEToolkit::IFeatureVectorSet& rFeatureVectorSet);
			virtual OpenViBE::boolean classify(const OpenViBEToolkit::IFeatureVector& rFeatureVector, OpenViBE::float64& rf64Class, OpenViBEToolkit::IVector& rClassificationValues);

			virtual OpenViBE::boolean saveConfiguration(OpenViBE::IMemoryBuffer& rMemoryBuffer);
			virtual OpenViBE::boolean loadConfiguration(const OpenViBE::IMemoryBuffer& rMemoryBuffer);

			_IsDerivedFromClass_Final_(CAlgorithmClassifier, OVP_ClassId_Algorithm_ClassifierBliffCFIS);

			OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > ip_f64Radius;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64Mode;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64OutputMode;

			OpenViBE::CMemoryBuffer m_oConfiguration;
		};

		class CAlgorithmClassifierBliffCFISDesc : public OpenViBEToolkit::CAlgorithmClassifierDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("BLiFF CFIS classifier - GPL"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard / Fabien Lotte"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA / INSA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ClassifierBliffCFIS; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Local::CAlgorithmClassifierBliffCFIS; }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CAlgorithmClassifierDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_CAlgorithmClassifierCFIS_InputParameterId_Radius,     "Radius",      OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_CAlgorithmClassifierCFIS_InputParameterId_Mode,       "Mode",        OpenViBE::Kernel::ParameterType_Enumeration, OVP_TypeId_CFISMode);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_CAlgorithmClassifierCFIS_InputParameterId_OutputMode, "Output Mode", OpenViBE::Kernel::ParameterType_Enumeration, OVP_TypeId_CFISOutputMode);

				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmClassifierDesc, OVP_ClassId_Algorithm_ClassifierBliffCFISDesc);
		};
	};
};

#endif // TARGET_HAS_ThirdPartyBLiFF

#endif // __OpenViBEPlugins_Algorithm_ClassifierBliffCFIS_H__
