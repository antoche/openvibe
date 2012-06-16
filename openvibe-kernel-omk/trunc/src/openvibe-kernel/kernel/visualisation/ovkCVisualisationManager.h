#ifndef __OpenViBEKernel_Kernel_Visualisation_CVisualisationManager_H__
#define __OpenViBEKernel_Kernel_Visualisation_CVisualisationManager_H__

#include "../ovkTKernelObject.h"
#include "../ovkIGtkOVCustomHandler.h"

#include <map>

class COgreVisualisation;
typedef struct _GtkWidget GtkWidget;

namespace OpenViBE
{
	namespace Kernel
	{
		class CVisualisationManager : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IVisualisationManager>, public IGtkOVCustomHandler
		{
		public:

			CVisualisationManager(
				const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual ~CVisualisationManager();

			/**
			 * \brief Initialize 3D context and primary render window
			 * \return True if 3d context was successfully initialized, false otherwise
			 */
			virtual OpenViBE::boolean initialize3DContext(void);

			/**
			 * \brief Create a resource group
			 * \param [out] rResourceGroupIdentifier Identifier of resource group created
			 * \param [in] rResourceGroupName Resource group name
			 * \return True if resource group could be created, false otherwise
			 */
			virtual OpenViBE::boolean createResourceGroup(
				OpenViBE::CIdentifier& rResourceGroupIdentifier,
				const OpenViBE::CString& rResourceGroupName);

			/**
			 * \brief Add a resource location
			 * \remarks Resources can't be added once Resource Group has been initialized
			 * \param rResourceGroupIdentifier Resource group identifier
			 * \param rResourceName Resource name
			 * \param eResourceType Resource type
			 * \param bRecursive Resources are added recursively from location passed in parameter if true
			 * \return True if resource location could be added, false otherwise
			 */
			virtual OpenViBE::boolean addResourceLocation(
				const OpenViBE::CIdentifier& rResourceGroupIdentifier,
				const OpenViBE::CString& rResourceName,
				OpenViBE::Kernel::EResourceType rResourceType,
				OpenViBE::boolean bRecursive);

			/**
			 * \brief Initialize resource group
			 * \remarks Each call to this method increments the group's internal reference counter.
			 * When it is set to one (upon first call to this method) the group is actually initialized, but isn't upon
			 * subsequent calls. It has to be destroyed (its ref count set back to 0) and recreated to be reinitialized.
			 * \param rResourceGroupIdentifier Resource group identifier
			 * \return True if resource group could be initialized (or was initialized already), false if an error occured
			 */
			virtual OpenViBE::boolean initializeResourceGroup(
				const OpenViBE::CIdentifier& rResourceGroupIdentifier);

			/**
			 * \brief Destroy resource group
			 * \remarks Each call to this method decrements the group's internal reference counter.
			 * Only when this counter reaches 0 is the group actually destroyed.
			 * \param rResourceGroupIdentifier Resource group identifier
			 * \return True if resource group could be destroyed (or was initialized already), false if an error occured
			 */
			virtual OpenViBE::boolean destroyResourceGroup(
				const OpenViBE::CIdentifier& rResourceGroupIdentifier);

			COgreVisualisation* getOgreVisualisation();

			///IGtkOVCustom interface implementation for primary render window realization
			OpenViBE::boolean handleRealizeEvent(
				GtkWidget* pOVCustomWidget);

			virtual OpenViBE::boolean createVisualisationTree(
				OpenViBE::CIdentifier& rVisualisationTreeIdentifier);
			virtual OpenViBE::boolean releaseVisualisationTree(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier);
			virtual OpenViBE::Kernel::IVisualisationTree& getVisualisationTree(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier);
			virtual OpenViBE::boolean enumerateVisualisationTrees(
				OpenViBE::Kernel::IVisualisationManager::IVisualisationTreeEnum& rCallBack) const;

			virtual OpenViBE::boolean setToolbar(
				const CIdentifier& rVisualisationTreeIdentifier,
				const CIdentifier& rBoxIdentifier,
				::GtkWidget* pToolbar);
			virtual OpenViBE::boolean setWidget(
				const CIdentifier& rVisualisationTreeIdentifier,
				const CIdentifier& rBoxIdentifier,
				::GtkWidget* pTopmostWidget);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IVisualisationManager>, OVK_ClassId_Kernel_Visualisation_VisualisationManager);

		protected:

			virtual OpenViBE::CIdentifier getUnusedIdentifier(void) const;

		protected:

			/// Map of visualisation trees (one per scenario, storing visualisation widgets arrangement in space)
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::IVisualisationTree*> m_vVisualisationTree;
			/// Widget containing primary render window, created at startup and destroyed upon shutdown
			::GtkWidget* m_pPrimaryRenderWindowWidget;
			/// The one and only ogre visualisation objet, created at startup and destroyed upon shutdown
			COgreVisualisation* m_pOgreVisualisation;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Visualisation_CVisualisationManager_H__
