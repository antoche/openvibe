#ifndef __OpenViBE_Plugins_IScenarioImporterDesc_H__
#define __OpenViBE_Plugins_IScenarioImporterDesc_H__

#include "ovIPluginObjectDesc.h"

namespace OpenViBE
{
	namespace Plugins
	{
		class OV_API IScenarioImporterDesc : public OpenViBE::Plugins::IPluginObjectDesc
		{
		public:

			virtual OpenViBE::CString getFileExtension(void) const=0;

			_IsDerivedFromClass_(OpenViBE::Plugins::IPluginObjectDesc, OV_ClassId_Plugins_ScenarioImporterDesc);
		};
	};
};

#endif // __OpenViBE_Plugins_IScenarioImporterDesc_H__
