#ifndef __OpenViBE_Kernel_Visualisation_IVisualisationWidget_H__
#define __OpenViBE_Kernel_Visualisation_IVisualisationWidget_H__

#include "../scenario/ovIAttributable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		\brief This enum lists the different types of IVisualisationWidget supported by the platform
		*/
		enum EVisualisationWidgetType
		{
			EVisualisationWidget_Undefined, /**< Undefined widget (empty slot in an IVisualisationTree) */
			EVisualisationWidget_VisualisationWindow, /**< Top-level IVisualisationWidget container */
			EVisualisationWidget_VisualisationPanel, /**< Notebook tab containing IVisualisationWidget objects */
			EVisualisationWidget_VisualisationBox, /**< Visualisation plugin */ 
			EVisualisationWidget_VerticalSplit, /**< Split widget that divides its client area vertically in two */
			EVisualisationWidget_HorizontalSplit /**< Split widget that divides its client area horizontally in two */
		};

		/**
		 * \class IVisualisationWidget
		 * \author Vincent Delannoy (INRIA/IRISA)
		 * \date 2007-11
		 * \brief Interface of visualisation widgets that are handled by an IVisualisationTree

				These objects are stored in an IVisualisationTree object as they are being created and modified
				to suit the graphical needs of a scenario.
		 */
		class OV_API IVisualisationWidget : public OpenViBE::Kernel::IAttributable
		{
		public:

			/**
				\brief Initializes the widget
				\param rIdentifier [in] identifier of the widget
				\param rName [in] name of the widget (optional)
				\param oType [in] type of the widget
				\param rParentIdentifier [in] parent widget identifier (OV_Undefined for top-level widgets)
				\param rBoxIdentifier [in] if widget type is EVisualisationWidget_VisualisationBox, identifier of corresponding IBox
				\param ui32NbChildren [in] number of children of this widget (none for a visualisation box, 1 for a visualisation panel, 2 for split widgets, variable number for windows)
				\return True if widget was successfully initialized, false otherwise
			*/
			virtual OpenViBE::boolean	initialize(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::CString& rName,
				OpenViBE::Kernel::EVisualisationWidgetType oType,
				const OpenViBE::CIdentifier& rParentIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				OpenViBE::uint32 ui32NbChildren)=0;

			/**
				\brief Returns the identifier of the widget
				\return Widget identifier
			*/
			virtual OpenViBE::CIdentifier	getIdentifier(void) const=0;

			/**
				\brief Returns the name of the widget
				\return Widget name
			*/
			virtual const OpenViBE::CString& getName(void) const=0;

			/**
				\brief Sets the name of the widget				
				\param rName [in] name to give to the widget
			*/
			virtual void setName(const CString& rName)=0;

			/**
				\brief Returns the type of the widget
				\return Widget type
			*/
			virtual EVisualisationWidgetType getType(void) const=0;

			/**
				\brief Returns the identifier of the widget's parent (if any)
				\return Widget's parent identifier if any, OV_Undefined otherwise
			*/
			virtual OpenViBE::CIdentifier	getParentIdentifier(void) const=0;
			/**
				\brief Sets the identifier of the widget's parent				
				\param rParentIdentifier [in] identifier of the widget's parent
			*/
			virtual void setParentIdentifier(
				const OpenViBE::CIdentifier& rParentIdentifier)=0;

			/**
				\brief Returns the identifier of the IBox associated to this widget.

				This only applies to widgets of type EVisualisationWidget_VisualisationBox.
				\return Identifier of IBox associated to this widget
			*/
			virtual OpenViBE::CIdentifier getBoxIdentifier(void) const=0;

			/**
				\brief Returns the number of children of this widget
				\return Number of child widgets
			*/
			virtual OpenViBE::uint32 getNbChildren(void) const=0;

			/**
				\brief Returns the index of a given child
				\param rIdentifier [in] identifier of a child widget
				\param ui32Index [out] index at which the child widget is stored
				\return True if the child was found, false otherwise
			*/
			virtual OpenViBE::boolean getChildIndex(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32Index) const=0;

			/**
				\brief Adds a child to a widget

				Only useful for top-level widgets (EVisualisationWidget_VisualisationWindow) since the number
				of tabs their notebook may contain is unknown a priori. The child is added after existing children.
				\param rChildIdentifier [in] identifier of child to be added to widget
				\return True if child was successfully added
			*/
			virtual OpenViBE::boolean addChild(
				const OpenViBE::CIdentifier& rChildIdentifier)=0;

			/**
				\brief Removes a child from a widget				
				\param rChildIdentifier [in] identifier of child to be removed to the widget
				\return True if the child was successfully removed
			*/
			virtual OpenViBE::boolean removeChild(
				const OpenViBE::CIdentifier& rChildIdentifier)=0;

			/**
				\brief Returns the identifier of a given child				
				\param ui32ChildIndex [in] index of child whose identifier is to be retrieved
				\param rChildIdentifier [out] identifier of child
				\return True if child identifier was successfully returned, false otherwise
			*/
			virtual OpenViBE::boolean getChildIdentifier(
				OpenViBE::uint32 ui32ChildIndex,
				OpenViBE::CIdentifier& rChildIdentifier) const=0;

			/**
				\brief Sets the identifier of a child

				\param ui32ChildIndex [in] index of child whose identifier is to be set
				\param rChildIdentifier [in] identifier of the child to be added to the widget
				\return True if the child was successfully set
			*/
			virtual OpenViBE::boolean setChildIdentifier(
				OpenViBE::uint32 ui32ChildIndex,
				const OpenViBE::CIdentifier& rChildIdentifier)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Visualisation_VisualisationWidget)
		};
	};
};

#endif //__OpenViBE_Kernel_Visualisation_IVisualisationWidget_H__
