#ifndef __OpenViBEPlugins_Algorithm_ClassifierBliffLDA_H__
#define __OpenViBEPlugins_Algorithm_ClassifierBliffLDA_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#if defined TARGET_HAS_ThirdPartyBLiFF

#define OVP_ClassId_Algorithm_ClassifierBliffLDA                                        OpenViBE::CIdentifier(0xC200B3CC, 0x19E64CBD)
#define OVP_ClassId_Algorithm_ClassifierBliffLDADesc                                    OpenViBE::CIdentifier(0xBA291D66, 0x2CF290F7)

namespace OpenViBEPlugins
{
	namespace Local
	{
		class CAlgorithmClassifierBliffLDA : public OpenViBEToolkit::CAlgorithmClassifier
		{
		public:

			virtual OpenViBE::boolean train(const OpenViBEToolkit::IFeatureVectorSet& rFeatureVectorSet);
			virtual OpenViBE::boolean classify(const OpenViBEToolkit::IFeatureVector& rFeatureVector, OpenViBE::float64& rf64Class, OpenViBEToolkit::IVector& rClassificationValues);

			virtual OpenViBE::boolean saveConfiguration(OpenViBE::IMemoryBuffer& rMemoryBuffer);
			virtual OpenViBE::boolean loadConfiguration(const OpenViBE::IMemoryBuffer& rMemoryBuffer);

			_IsDerivedFromClass_Final_(CAlgorithmClassifier, OVP_ClassId_Algorithm_ClassifierBliffLDA);

			OpenViBE::CMemoryBuffer m_oConfiguration;
		};

		class CAlgorithmClassifierBliffLDADesc : public OpenViBEToolkit::CAlgorithmClassifierDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("BLiFF LDA classifier - GPL"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard / Fabien Lotte"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA / INSA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ClassifierBliffLDA; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Local::CAlgorithmClassifierBliffLDA; }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CAlgorithmClassifierDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmClassifierDesc, OVP_ClassId_Algorithm_ClassifierBliffLDADesc);
		};
	};
};

#endif // TARGET_HAS_ThirdPartyBLiFF

#endif // __OpenViBEPlugins_Algorithm_ClassifierBliffLDA_H__
