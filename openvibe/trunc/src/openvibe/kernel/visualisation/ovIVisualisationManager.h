#ifndef __OpenViBE_Kernel_Visualisation_IVisualisationManager_H__
#define __OpenViBE_Kernel_Visualisation_IVisualisationManager_H__

#include "../ovIKernelObject.h"

#include "ovIVisualisationContext.h"

typedef struct _GtkWidget GtkWidget;

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IVisualisationManager
		 * \author Vincent Delannoy (INRIA/IRISA)
		 * \date 2007-11
		 * \brief The VisualisationManager handles IVisualisationTree objects
		 * It maintains a list of IVisualisationTree objects, each of which is associated to a scenario. This
		 * manager is used both at scenario creation time (to load or create IVisualisationTree objects), and when the
		 * scenario is being run (to forward IVisualisationWidget pointers to the related IVisualisationTree).
		 */
		class OV_API IVisualisationManager : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief An interface used to iteratively notify its creator of the existence of IVisualisationTree objects
			 */
			class OV_API IVisualisationTreeEnum
			{
			public:
				virtual ~IVisualisationTreeEnum(void) { }
				/**
				 * \brief Callback method called iteratively as the IVisualisationManager goes through a set of IVisualisationTree objects
				 * \param rVisualisationTreeIdentifier [in] identifier of an IVisualisationTree object
				 * \param rVisualisationTree [in] corresponding IVisualisationTree object
				 * \return True if IVisualisationTree object was successfully registered, false otherwise
				 */
				virtual OpenViBE::boolean callback(
					const OpenViBE::CIdentifier& rVisualisationTreeIdentifier,
					const OpenViBE::Kernel::IVisualisationTree& rVisualisationTree)=0;
			};

			/**
			 * \brief Initialize 3D context and primary render window
			 * \return True if 3d context was successfully initialized, false otherwise
			 */
			virtual OpenViBE::boolean initialize3DContext(void)=0;

			/**
			 * \brief Create a resource group
			 * \param [out] rResourceGroupIdentifier Identifier of resource group created
			 * \param [in] rResourceGroupName Resource group name
			 * \return True if resource group could be created, false otherwise
			 */
			virtual OpenViBE::boolean createResourceGroup(
				OpenViBE::CIdentifier& rResourceGroupIdentifier,
				const OpenViBE::CString& rResourceGroupName)=0;

			/**
			 * \brief Add a resource location
			 * \remark Resources can't be added once Resource Group has been initialized
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
				OpenViBE::boolean bRecursive)=0;

			/**
			 * \brief Initialize resource group
			 * \remark Each call to this method increments the group's internal reference counter.
			 * When it is set to one (upon first call to this method) the group is actually initialized, but isn't upon
			 * subsequent calls. It has to be destroyed (its ref count set back to 0) and recreated to be reinitialized.
			 * \param rResourceGroupIdentifier Resource group identifier
			 * \return True if resource group could be initialized (or was initialized already), false if an error occured
			 */
			virtual OpenViBE::boolean initializeResourceGroup(
				const OpenViBE::CIdentifier& rResourceGroupIdentifier)=0;

			/**
			 * \brief Destroy resource group
			 * \remark Each call to this method decrements the group's internal reference counter.
			 * Only when this counter reaches 0 is the group actually destroyed.
			 * \param rResourceGroupIdentifier Resource group identifier
			 * \return True if resource group could be destroyed (or was initialized already), false if an error occured
			 */
			virtual OpenViBE::boolean destroyResourceGroup(
				const OpenViBE::CIdentifier& rResourceGroupIdentifier)=0;

			/**
			 * \brief Creates an IVisualisationTree object.
			 * \param rVisualisationTreeIdentifier [out] identifier of the IVisualisationTree object created by this method
			 * \return True if object was successfully created, false otherwise
			 */
			virtual OpenViBE::boolean createVisualisationTree(
				OpenViBE::CIdentifier& rVisualisationTreeIdentifier)=0;
			/**
			 * \brief Releases an IVisualisationTree object.
			 * \param rVisualisationTreeIdentifier [in] identifier of the IVisualisationTree object to be released
			 * \return True if object was successfully released, false otherwise
			 */
			virtual OpenViBE::boolean releaseVisualisationTree(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier)=0;
			/**
			 * \brief Looks for an IVisualisationTree object.
			 * \param rVisualisationTreeIdentifier [in] identifier of the IVisualisationTree object to be returned
			 * \return Reference on IVisualisationTree looked for, OV_Undefined otherwise
			 */
			virtual OpenViBE::Kernel::IVisualisationTree& getVisualisationTree(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier)=0;
			/**
			 * \brief Enumerates IVisualisationTree objects registered in this manager.
			 * \param rCallback [in] IVisualisationTreeEnum object to be notified for each IVisualisationTree registered in this manager.
			 * \return True if IVisualisationTree objects were successfully enumerated, false otherwise
			 */
			virtual OpenViBE::boolean enumerateVisualisationTrees(
				OpenViBE::Kernel::IVisualisationManager::IVisualisationTreeEnum& rCallBack) const=0;

			/**
			 * \brief Set the toolbar of a visualisation plugin.
			 * This method is to be called by visualisation plugins as they are being initialized. It lets them send
			 * a pointer to their toolbar (if they have one) to the scenario's IVisualisationTree.
			 * \param rVisualisationTreeIdentifier [in] identifier of IVisualisationTree to which the toolbar pointer is to be forwarded
			 * \param rBoxIdentifier [in] Identifier of IBox whose toolbar pointer is being set
			 * \param pToolbarWidget [in] pointer to the toolbar of the widget
			 * \return True if pointer was successfully forwarded to IVisualisationTree, false otherwise
			 */
			virtual OpenViBE::boolean setToolbar(
				const CIdentifier& rVisualisationTreeIdentifier,
				const CIdentifier& rBoxIdentifier,
				::GtkWidget* pToolbar)=0;

			/**
			 * \brief Set the topmost widget of a visualisation plugin.
			 * This method is to be called by visualisation plugins as they are being initialized. It lets them send
			 * a pointer to their topmost widget to the scenario's IVisualisationTree.
			 * \param rVisualisationTreeIdentifier [in] identifier of IVisualisationTree to which the toolbar pointer is to be forwarded
			 * \param rBoxIdentifier [in] Identifier of IBox whose topmost widget pointer is being set
			 * \param pWidget [in] pointer to the main window of the widget
			 * \return True if pointer was successfully forwarded to IVisualisationTree, false otherwise
			 */
			virtual OpenViBE::boolean setWidget(
				const CIdentifier& rVisualisationTreeIdentifier,
				const CIdentifier& rBoxIdentifier,
				::GtkWidget* pTopmostWidget)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Visualisation_VisualisationManager)
		};
	};
};

#endif // __OpenViBE_Kernel_Visualisation_IVisualisationManager_H__
