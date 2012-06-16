#ifndef __SamplePlugin_CScenarioExporterSVG_H__
#define __SamplePlugin_CScenarioExporterSVG_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>

#include <xml/IWriter.h>

#include <iostream>
#include <fstream>

namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CScenarioExporterSVG
			: public OpenViBE::Plugins::IScenarioExporter
			, virtual public XML::IWriterCallback
		{
		public:

			CScenarioExporterSVG(void);

			virtual void release(void);

			virtual void write(const char* sString);

			virtual void exportBox(const OpenViBE::Kernel::IBox& rBox);
			virtual void exportLink(const OpenViBE::Kernel::ILink& rLink);

			virtual OpenViBE::boolean doExport(OpenViBE::Kernel::IScenarioExporterContext& rScenarioExporterContext);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IScenarioExporter, OVP_ClassId_ScenarioExporterSVG)

		protected:

			XML::IWriter* m_pWriter;
			std::ofstream m_oFile;
		};

		class CScenarioExporterSVGDesc : public OpenViBE::Plugins::IScenarioExporterDesc
		{
		public:
			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("SVG Scenario exporter"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("SVG scenario exporter"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This scenario exporter allows to get a vector image"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getFileExtension(void) const       { return OpenViBE::CString("svg;SVG"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_ScenarioExporterSVG; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CScenarioExporterSVG(); }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IScenarioExporterDesc, OVP_ClassId_ScenarioExporterSVGDesc)
		};
	};
};

#endif // __SamplePlugin_CScenarioExporterSVG_H__
