#ifndef __OpenViBE_All_H__
#define __OpenViBE_All_H__

//___________________________________________________________________//
//                                                                   //
// Basic includes                                                    //
//___________________________________________________________________//
//                                                                   //

#include "ov_types.h"
#include "ov_defines.h"

#include "ovCIdentifier.h"
#include "ovCString.h"
#include "ovCNameValuePairList.h"

#include "ovIMatrix.h"
#include "ovCMatrix.h"

#include "ovIStimulationSet.h"
#include "ovCStimulationSet.h"

#include "ovIMemoryBuffer.h"
#include "ovCMemoryBuffer.h"

#include "ovIObject.h"
#include "ovIObjectVisitor.h"
#include "ovIKernelLoader.h"
#include "ovCKernelLoader.h"

//___________________________________________________________________//
//                                                                   //
// Kernel includes                                                   //
//___________________________________________________________________//
//                                                                   //

#include "kernel/ovIKernelContext.h"
#include "kernel/ovIKernelDesc.h"
#include "kernel/ovIKernelObject.h"
#include "kernel/ovIKernelObjectFactory.h"

#include "kernel/ovITypeManager.h"
#include "kernel/ovIParameter.h"
#include "kernel/ovIConfigurable.h"
#include "kernel/ovIObjectVisitorContext.h"
#include "kernel/ovTParameterHandler.h"

#include "kernel/algorithm/ovIAlgorithmContext.h"
#include "kernel/algorithm/ovIAlgorithmManager.h"
#include "kernel/algorithm/ovIAlgorithmProto.h"
#include "kernel/algorithm/ovIAlgorithmProxy.h"

#include "kernel/configuration/ovIConfigurationManager.h"

#include "kernel/log/ovILogManager.h"
#include "kernel/log/ovILogListener.h"

#include "kernel/player/ovIBoxAlgorithmContext.h"
#include "kernel/player/ovIMessage.h"
#include "kernel/player/ovIMessageClock.h"
#include "kernel/player/ovIMessageEvent.h"
#include "kernel/player/ovIMessageSignal.h"
#include "kernel/player/ovIPlayer.h"
#include "kernel/player/ovIPlayerContext.h"
#include "kernel/player/ovIPlayerManager.h"

#include "kernel/plugins/ovIPluginManager.h"
#include "kernel/plugins/ovIPluginModule.h"
#include "kernel/plugins/ovIPluginModuleContext.h"

#include "kernel/scenario/ovIAttributable.h"
#include "kernel/scenario/ovIBox.h"
#include "kernel/scenario/ovIBoxIO.h"
#include "kernel/scenario/ovIBoxListenerContext.h"
#include "kernel/scenario/ovIBoxProto.h"
#include "kernel/scenario/ovIComment.h"
#include "kernel/scenario/ovILink.h"
#include "kernel/scenario/ovIProcessingUnit.h"
#include "kernel/scenario/ovIScenario.h"
#include "kernel/scenario/ovIScenarioExporterContext.h"
#include "kernel/scenario/ovIScenarioImporterContext.h"
#include "kernel/scenario/ovIScenarioManager.h"

#include "kernel/visualisation/ovIVisualisationContext.h"
#include "kernel/visualisation/ovIVisualisationTree.h"
#include "kernel/visualisation/ovIVisualisationManager.h"
#include "kernel/visualisation/ovIVisualisationWidget.h"

//___________________________________________________________________//
//                                                                   //
// Plugins includes                                                  //
//___________________________________________________________________//
//                                                                   //

#include "plugins/ovIPluginObject.h"
#include "plugins/ovIPluginObjectDesc.h"

#include "plugins/ovIAlgorithm.h"
#include "plugins/ovIAlgorithmDesc.h"

#include "plugins/ovIBoxAlgorithm.h"
#include "plugins/ovIBoxAlgorithmDesc.h"

#include "plugins/ovIScenarioImporter.h"
#include "plugins/ovIScenarioImporterDesc.h"

#include "plugins/ovIScenarioExporter.h"
#include "plugins/ovIScenarioExporterDesc.h"

namespace OpenViBE
{
	namespace Plugins
	{
		// Backward compatibility
		typedef OpenViBE::Kernel::IBox IStaticBoxContext;
		typedef OpenViBE::Kernel::IBoxIO IDynamicBoxContext;
	};
};

//___________________________________________________________________//
//                                                                   //
// Plugins includes                                                  //
//___________________________________________________________________//
//                                                                   //

#define OVP_Declare_Begin() \
	static std::vector<OpenViBE::Plugins::IPluginObjectDesc*> g_descriptors; \
	static std::vector<OpenViBE::Plugins::IPluginObjectDesc*>::iterator l_itDescriptors; \
	extern "C" \
	{ \
		OVP_API OpenViBE::boolean onInitialize(const OpenViBE::Kernel::IPluginModuleContext& rPluginModuleContext) \
		{

#define OVP_Declare_New(Class) \
			g_descriptors.push_back(new Class);

#define OVP_Declare_End() \
			return true; \
		} \
		OVP_API OpenViBE::boolean onUninitialize(const OpenViBE::Kernel::IPluginModuleContext& rPluginModuleContext) \
		{ \
			for(l_itDescriptors=g_descriptors.begin(); l_itDescriptors!=g_descriptors.end(); l_itDescriptors++) \
				delete *l_itDescriptors; \
			g_descriptors.clear(); \
			return true; \
		} \
		OVP_API OpenViBE::boolean onGetPluginObjectDescription(const OpenViBE::Kernel::IPluginModuleContext& rPluginModuleContext, OpenViBE::uint32 ui32Index, OpenViBE::Plugins::IPluginObjectDesc*& rpPluginObjectDescription) \
		{ \
			if(ui32Index>=g_descriptors.size()) \
			{ \
				rpPluginObjectDescription=NULL; \
				return false; \
			} \
			rpPluginObjectDescription=g_descriptors[ui32Index]; \
			return true; \
		} \
	}

#endif // __OpenViBE_All_H__
