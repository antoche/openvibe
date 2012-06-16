#ifndef __OpenViBEPlugins_Algorithm_XMLScenarioExporter_H__
#define __OpenViBEPlugins_Algorithm_XMLScenarioExporter_H__

#include "../../../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <openvibe-toolkit/ovtk_all.h>

#include <xml/IWriter.h>

#define OVP_ClassId_Algorithm_XMLScenarioExporter                                        OpenViBE::CIdentifier(0x53693531, 0xB136CF3F)
#define OVP_ClassId_Algorithm_XMLScenarioExporterDesc                                    OpenViBE::CIdentifier(0x9709C9FA, 0xF126F74E)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CAlgorithmXMLScenarioExporter : public OpenViBEToolkit::CAlgorithmScenarioExporter, public XML::IWriterCallback
		{
		public:

			CAlgorithmXMLScenarioExporter(void);
			virtual ~CAlgorithmXMLScenarioExporter(void);

			virtual OpenViBE::boolean exportStart(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier);
			virtual OpenViBE::boolean exportIdentifier(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CIdentifier& rValue);
			virtual OpenViBE::boolean exportString(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CString& rValue);
			virtual OpenViBE::boolean exportUInteger(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::uint64 ui64Value);
			virtual OpenViBE::boolean exportStop(OpenViBE::IMemoryBuffer& rMemoryBuffer);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmScenarioExporter, OVP_ClassId_Algorithm_XMLScenarioExporter)

		protected:

			virtual void write(const char* sString); // XML::IWriterCallback

			XML::IWriter* m_pWriter;
			OpenViBE::IMemoryBuffer* m_pMemoryBuffer;
		};

		class CAlgorithmXMLScenarioExporterDesc : public OpenViBEToolkit::CAlgorithmScenarioExporterDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("XML Scenario exporter"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("A sample XML scenario exporter"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This scenario exporter uses simple XML format to output the scenario"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/XML Scenario"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			// virtual OpenViBE::CString getFileExtension(void) const       { return OpenViBE::CString("xml;XML"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_XMLScenarioExporter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CAlgorithmXMLScenarioExporter(); }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmScenarioExporterDesc, OVP_ClassId_Algorithm_XMLScenarioExporterDesc)
		};
	};
};

#endif // __OpenViBEPlugins_Algorithm_XMLScenarioExporter_H__
