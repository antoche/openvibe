#ifndef __SamplePlugin_CScenarioImporterXML_H__
#define __SamplePlugin_CScenarioImporterXML_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>

#include <xml/IReader.h>

#include <map>
#include <stack>
#include <string>
#include <vector>

namespace OpenViBEPlugins
{
	namespace Samples
	{
		#define _default_and_copy_constructor_1_(c,m1)                      c(void) { } c(const c& r) : m1(r.m1) { }
		#define _default_and_copy_constructor_2_(c,m1,m2)                   c(void) { } c(const c& r) : m1(r.m1), m2(r.m2) { }
		#define _default_and_copy_constructor_3_(c,m1,m2,m3)                c(void) { } c(const c& r) : m1(r.m1), m2(r.m2), m3(r.m3) { }
		#define _default_and_copy_constructor_4_(c,m1,m2,m3,m4)             c(void) { } c(const c& r) : m1(r.m1), m2(r.m2), m3(r.m3), m4(r.m4) { }
		#define _default_and_copy_constructor_5_(c,m1,m2,m3,m4,m5)          c(void) { } c(const c& r) : m1(r.m1), m2(r.m2), m3(r.m3), m4(r.m4), m5(r.m5) { }
		#define _default_and_copy_constructor_6_(c,m1,m2,m3,m4,m5,m6)       c(void) { } c(const c& r) : m1(r.m1), m2(r.m2), m3(r.m3), m4(r.m4), m5(r.m5), m6(r.m6) { }
		#define _default_and_copy_constructor_7_(c,m1,m2,m3,m4,m5,m6,m7)    c(void) { } c(const c& r) : m1(r.m1), m2(r.m2), m3(r.m3), m4(r.m4), m5(r.m5), m6(r.m6), m7(r.m7) { }
		#define _default_and_copy_constructor_8_(c,m1,m2,m3,m4,m5,m6,m7,m8) c(void) { } c(const c& r) : m1(r.m1), m2(r.m2), m3(r.m3), m4(r.m4), m5(r.m5), m6(r.m6), m7(r.m7), m8(r.m8) { }

		typedef struct _SInput
		{
			_default_and_copy_constructor_2_(_SInput, m_sTypeIdentifier, m_sName);
			std::string m_sTypeIdentifier;
			std::string m_sName;
		} SInput;
		typedef struct _SOutput
		{
			_default_and_copy_constructor_2_(_SOutput, m_sTypeIdentifier, m_sName);
			std::string m_sTypeIdentifier;
			std::string m_sName;
		} SOutput;
		typedef struct _SSetting
		{
			_default_and_copy_constructor_4_(_SSetting, m_sTypeIdentifier, m_sName, m_sDefaultValue, m_sValue);
			std::string m_sTypeIdentifier;
			std::string m_sName;
			std::string m_sDefaultValue;
			std::string m_sValue;
		} SSetting;
		typedef struct _SAttribute
		{
			_default_and_copy_constructor_2_(_SAttribute, m_sIdentifier, m_sValue);
			std::string m_sIdentifier;
			std::string m_sValue;
		} SAttribute;
		typedef struct _SBox
		{
			_default_and_copy_constructor_7_(_SBox, m_sIdentifier, m_sAlgorithmClassIdentifier, m_sName, m_vInput, m_vOutput, m_vSetting, m_vAttribute);
			std::string m_sIdentifier;
			std::string m_sAlgorithmClassIdentifier;
			std::string m_sName;
			std::vector<SInput> m_vInput;
			std::vector<SOutput> m_vOutput;
			std::vector<SSetting> m_vSetting;
			std::vector<SAttribute> m_vAttribute;
		} SBox;
		typedef struct _SLinkSource
		{
			_default_and_copy_constructor_2_(_SLinkSource, m_sBoxIdentifier, m_sBoxOutputIndex);
			std::string m_sBoxIdentifier;
			std::string m_sBoxOutputIndex;
		} SLinkSource;
		typedef struct _SLinkTarget
		{
			_default_and_copy_constructor_2_(_SLinkTarget, m_sBoxIdentifier, m_sBoxInputIndex);
			std::string m_sBoxIdentifier;
			std::string m_sBoxInputIndex;
		} SLinkTarget;
		typedef struct _SLink
		{
			_default_and_copy_constructor_4_(_SLink, m_sIdentifier, m_oLinkSource, m_oLinkTarget, m_vAttribute);
			std::string m_sIdentifier;
			SLinkSource m_oLinkSource;
			SLinkTarget m_oLinkTarget;
			std::vector<SAttribute> m_vAttribute;
		} SLink;
		typedef struct _SVisualisationWidget
		{
			_default_and_copy_constructor_8_(_SVisualisationWidget, m_sIdentifier, m_sName, m_sType, m_sParentIdentifier, m_sIndex, m_sBoxIdentifier, m_sNbChildren, m_vAttribute);
			std::string m_sIdentifier;
			std::string m_sName;
			std::string m_sType;
			std::string m_sParentIdentifier;
			std::string m_sIndex;
			std::string m_sBoxIdentifier;
			std::string m_sNbChildren;
			std::vector<SAttribute> m_vAttribute;
		} SVisualisationWidget;
		typedef struct _SScenario
		{
			_default_and_copy_constructor_4_(_SScenario, m_vBox, m_vLink, m_vAttribute, m_vVisualisationWidget);
			std::vector<SBox> m_vBox;
			std::vector<SLink> m_vLink;
			std::vector<SAttribute> m_vAttribute;
			std::vector<SVisualisationWidget> m_vVisualisationWidget;
		} SScenario;

		#undef _default_and_copy_constructor_1_
		#undef _default_and_copy_constructor_2_
		#undef _default_and_copy_constructor_3_
		#undef _default_and_copy_constructor_4_
		#undef _default_and_copy_constructor_5_
		#undef _default_and_copy_constructor_6_
		#undef _default_and_copy_constructor_7_

		class CScenarioImporterXML
			: public OpenViBE::Plugins::IScenarioImporter
			, virtual public XML::IReaderCallback
		{
		public:

			CScenarioImporterXML(void);

			virtual void release(void);

			virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount);
			virtual void processChildData(const char* sData);
			virtual void closeChild(void);

			virtual OpenViBE::boolean doImport(OpenViBE::Kernel::IScenarioImporterContext& rScenarioImporterContext);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IScenarioImporter, OVP_ClassId_ScenarioImporterXML)

		protected:

			std::map<OpenViBE::CIdentifier, OpenViBE::CIdentifier> m_vBoxIdMapping;
			std::map<OpenViBE::CIdentifier, OpenViBE::CIdentifier> m_vLinkIdMapping;
			std::map<OpenViBE::CIdentifier, OpenViBE::CIdentifier> m_vVisualisationWidgetIdMapping;
			std::stack<std::string> m_vNodes;
			OpenViBE::uint32 m_ui32Status;
			OpenViBE::boolean m_bIsOpenViBEScenario;
			SScenario m_oScenario;
		};

		class CScenarioImporterXMLDesc : public OpenViBE::Plugins::IScenarioImporterDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("XML Scenario importer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("A sample XML scenario importer"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This scenario importer uses simple XML format to input the scenario"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getFileExtension(void) const       { return OpenViBE::CString("xml;XML"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_ScenarioImporterXML; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CScenarioImporterXML(); }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IScenarioImporterDesc, OVP_ClassId_ScenarioImporterXMLDesc)
		};
	};
};

#endif // __SamplePlugin_CScenarioImporterXML_H__
