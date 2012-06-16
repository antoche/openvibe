#ifndef __OpenViBEKernel_Kernel_Visualisation_CVisualisationManager_H__
#define __OpenViBEKernel_Kernel_Visualisation_CVisualisationManager_H__

#include "../ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CVisualisationManager : virtual public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IVisualisationManager>
		{
		public:

			CVisualisationManager(
				const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual OpenViBE::boolean createVisualisationTree(
				OpenViBE::CIdentifier& rVisualisationTreeIdentifier);
			virtual OpenViBE::boolean releaseVisualisationTree(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier);
			virtual OpenViBE::Kernel::IVisualisationTree& getVisualisationTree(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier);
			virtual OpenViBE::boolean enumerateVisualisationTrees(
				OpenViBE::Kernel::IVisualisationManager::IVisualisationTreeEnum& rCallBack) const;
			virtual OpenViBE::boolean setWidgets(
				const CIdentifier& rVisualisationTreeIdentifier,
				const CString& rVisualisationBoxName,
				::GtkWidget* pWidget,
				::GtkWidget* pToolbarWidget);
			
			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IVisualisationManager>, OVK_ClassId_Kernel_Visualisation_VisualisationManager);

		protected:
			virtual OpenViBE::CIdentifier getUnusedIdentifier(void) const;

		protected:
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::IVisualisationTree*> m_vVisualisationTree;			
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Visualisation_CVisualisationManager_H__
