#ifndef __SamplePlugin_CScenarioExporterXML_H__
#define __SamplePlugin_CScenarioExporterXML_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>

#include <xml/IWriter.h>

#include <iostream>
#include <fstream>

namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CScenarioExporterXML
			: public OpenViBE::Plugins::IScenarioExporter
			, virtual public XML::IWriterCallback
		{
		public:

			CScenarioExporterXML(void);

			virtual void release(void);

			virtual void write(const char* sString);

			virtual void exportAttribute(const OpenViBE::CIdentifier& rAttributeIdentifier, const OpenViBE::CString& rAttributeValue);
			virtual void exportBox(const OpenViBE::Kernel::IBox& rBox);
			virtual void exportLink(const OpenViBE::Kernel::ILink& rLink);

			virtual OpenViBE::boolean doExport(OpenViBE::Kernel::IScenarioExporterContext& rScenarioExporterContext);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IScenarioExporter, OVP_ClassId_ScenarioExporterXML)

		protected:

			XML::IWriter* m_pWriter;
			std::ofstream m_oFile;
		};

		class CScenarioExporterXMLDesc : public OpenViBE::Plugins::IScenarioExporterDesc
		{
		public:
			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("XML Scenario exporter"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("A sample XML scenario exporter"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This scenario exporter uses simple XML format to output the scenario"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getFileExtension(void) const       { return OpenViBE::CString("xml;XML"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_ScenarioExporterXML; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CScenarioExporterXML(); }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IScenarioExporterDesc, OVP_ClassId_ScenarioExporterXMLDesc)
		};
	};
};

#endif // __SamplePlugin_CScenarioExporterXML_H__
