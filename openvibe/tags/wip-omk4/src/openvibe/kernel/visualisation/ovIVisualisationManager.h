#ifndef __OpenViBE_Kernel_Visualisation_IVisualisationManager_H__
#define __OpenViBE_Kernel_Visualisation_IVisualisationManager_H__

#include "../ovIKernelObject.h"

typedef struct _GtkWidget GtkWidget;

namespace OpenViBE
{
	namespace Kernel
	{
		/**
			\class IVisualisationManager
			\author Vincent Delannoy (INRIA/IRISA)
			\date 2007-11
			\brief The VisualisationManager handles IVisualisationTree objects

			It maintains a list of IVisualisationTree objects, each of which is associated to a scenario. This
			manager is used both at scenario creation time (to load or create IVisualisationTree objects), and when the 
			scenario is being run (to forward IVisualisationWidget pointers to the related IVisualisationTree).
		*/
		class OV_API IVisualisationManager : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
				\brief An interface used to iteratively notify its creator of the existence of IVisualisationTree objects
			*/
			class OV_API IVisualisationTreeEnum
			{
			public:
				virtual ~IVisualisationTreeEnum(void) { }
				/**
					\brief Callback method called iteratively as the IVisualisationManager goes through a set of IVisualisationTree objects					
					\param rVisualisationTreeIdentifier [in] identifier of an IVisualisationTree object
					\param rVisualisationTree [in] corresponding IVisualisationTree object
					\return True if IVisualisationTree object was successfully registered, false otherwise
				*/
				virtual OpenViBE::boolean callback(
					const OpenViBE::CIdentifier& rVisualisationTreeIdentifier,
					const OpenViBE::Kernel::IVisualisationTree& rVisualisationTree)=0;
			};

			/**
				\brief Creates an IVisualisationTree object.
				\param rVisualisationTreeIdentifier [out] identifier of the IVisualisationTree object created by this method
				\return True if object was successfully created, false otherwise
			*/
			virtual OpenViBE::boolean createVisualisationTree(
				OpenViBE::CIdentifier& rVisualisationTreeIdentifier)=0;
			/**
				\brief Releases an IVisualisationTree object.
				\param rVisualisationTreeIdentifier [in] identifier of the IVisualisationTree object to be released
				\return True if object was successfully released, false otherwise
			*/
			virtual OpenViBE::boolean releaseVisualisationTree(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier)=0;
			/**
				\brief Looks for an IVisualisationTree object.
				\param rVisualisationTreeIdentifier [in] identifier of the IVisualisationTree object to be returned
				\return Reference on IVisualisationTree looked for, OV_Undefined otherwise
			*/
			virtual OpenViBE::Kernel::IVisualisationTree& getVisualisationTree(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier)=0;
			/**
				\brief Enumerates IVisualisationTree objects registered in this manager.
				\param rCallback [in] IVisualisationTreeEnum object to be notified for each IVisualisationTree registered in this manager.
				\return True if IVisualisationTree objects were successfully enumerated, false otherwise
			*/
			virtual OpenViBE::boolean enumerateVisualisationTrees(
				OpenViBE::Kernel::IVisualisationManager::IVisualisationTreeEnum& rCallBack) const=0;
			/**
				\brief Sets the GtkWidgets corresponding to a visualisation plugin's main window and toolbar
				
				This method is to be called by visualisation plugins as they are being initialized. It lets them forward pointers 
				to their main window and toolbar to the scenario's IVisualisationTree.
				\param rVisualisationTreeIdentifier [in] identifier of the IVisualisationTree to which the pointers are to be forwarded
				\param rVisualisationBoxName [in] name of IVisualisationWidget associated to the pointers
				\param pWidget [in] pointer to the main window of the widget
				\param pToolbarWidget [in] pointer to the toolbar of the widget
				\return True if pointers were successfully forwarded to IVisualisationTree, false otherwise
			*/
			virtual OpenViBE::boolean setWidgets(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifer,
				const OpenViBE::CString& rVisualisationBoxName,
				::GtkWidget* pWidget,
				::GtkWidget* pToolbarWidget)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Visualisation_VisualisationManager)
		};
	};
};

#endif // __OpenViBE_Kernel_Visualisation_IVisualisationManager_H__
