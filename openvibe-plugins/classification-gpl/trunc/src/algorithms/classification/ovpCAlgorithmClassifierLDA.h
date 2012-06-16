#ifndef __OpenViBEPlugins_Algorithm_ClassifierLDA_H__
#define __OpenViBEPlugins_Algorithm_ClassifierLDA_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <xml/IWriter.h>
#include <xml/IReader.h>

#include <stack>

#if defined TARGET_HAS_ThirdPartyITPP

#include <itpp/itbase.h>

#define OVP_ClassId_Algorithm_ClassifierLDA                                        OpenViBE::CIdentifier(0xD7183FC7, 0xBD74F298)
#define OVP_ClassId_Algorithm_ClassifierLDADesc                                    OpenViBE::CIdentifier(0xD42D544A, 0x7A28DDB1)

namespace OpenViBEPlugins
{
	namespace Local
	{
		class CAlgorithmClassifierLDA : public OpenViBEToolkit::CAlgorithmClassifier, public XML::IWriterCallback, public XML::IReaderCallback
		{
		public:

			virtual OpenViBE::boolean train(const OpenViBEToolkit::IFeatureVectorSet& rFeatureVectorSet);
			virtual OpenViBE::boolean classify(const OpenViBEToolkit::IFeatureVector& rFeatureVector, OpenViBE::float64& rf64Class, OpenViBEToolkit::IVector& rClassificationValues);

			virtual OpenViBE::boolean saveConfiguration(OpenViBE::IMemoryBuffer& rMemoryBuffer);
			virtual OpenViBE::boolean loadConfiguration(const OpenViBE::IMemoryBuffer& rMemoryBuffer);

			_IsDerivedFromClass_Final_(CAlgorithmClassifier, OVP_ClassId_Algorithm_ClassifierLDA);

		protected:

			virtual void write(const char* sString); // XML IWriterCallback

			virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount); // XML IReaderCallback
			virtual void processChildData(const char* sData); // XML IReaderCallback
			virtual void closeChild(void); // XML ReaderCallback

			std::stack<OpenViBE::CString> m_vNode;

			OpenViBE::float64 m_f64Class1;
			OpenViBE::float64 m_f64Class2;

			OpenViBE::CMemoryBuffer m_oConfiguration;
			itpp::vec m_oCoefficients;
		};

		class CAlgorithmClassifierLDADesc : public OpenViBEToolkit::CAlgorithmClassifierDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("LDA classifier - GPL"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard / Fabien Lotte"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA / INSA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ClassifierLDA; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Local::CAlgorithmClassifierLDA; }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CAlgorithmClassifierDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmClassifierDesc, OVP_ClassId_Algorithm_ClassifierLDADesc);
		};
	};
};

#endif // TARGET_HAS_ThirdPartyITPP

#endif // __OpenViBEPlugins_Algorithm_ClassifierLDA_H__
