#ifndef __OpenViBE_Plugins_IScenarioExporterDesc_H__
#define __OpenViBE_Plugins_IScenarioExporterDesc_H__

#include "ovIPluginObjectDesc.h"

namespace OpenViBE
{
	namespace Plugins
	{
		class OV_API IScenarioExporterDesc : public OpenViBE::Plugins::IPluginObjectDesc
		{
		public:

			virtual OpenViBE::CString getFileExtension(void) const=0;

			_IsDerivedFromClass_(OpenViBE::Plugins::IPluginObjectDesc, OV_ClassId_Plugins_ScenarioExporterDesc);
		};
	};
};

#endif // __OpenViBE_Plugins_IScenarioExporterDesc_H__
