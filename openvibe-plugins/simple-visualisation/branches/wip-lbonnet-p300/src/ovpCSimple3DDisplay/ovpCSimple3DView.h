#ifndef __SimpleVisualisationPlugin_CSimple3DView_H__
#define __SimpleVisualisationPlugin_CSimple3DView_H__

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <gtk/gtk.h>

#include "ovpCSimple3DDatabase.h"

#include <vector>
#include <string>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		/**
		 * This class contains everything necessary to setup a GTK window and display
		 * a 3D viewer
		 */
		class CSimple3DView : public CSignalDisplayDrawable
		{
		public:
			CSimple3DView(
				CSimple3DDatabase& rSimple3DDatabase);

			virtual ~CSimple3DView();

			/** \name CSignalDisplayDrawable implementation */
			//@{
			/**
			 * Initializes the window.
			 */
			virtual void init();
			/**
			 * Invalidates the window's content and tells it to redraw itself.
			 */
			virtual void redraw();
			//@}

			/**
			 * \brief Get pointer to toolbar
			 * \param[out] pToolbar pointer to toolbar
			 */
			void getToolbar(
				GtkWidget*& pToolbarWidget);

			void createVisualObjectCB();
			void deleteVisualObjectCB();
			void setVisualObjectPositionCB();
			void setVisualObjectOrientationCB();
			void setVisualObjectScaleCB();
			void setVisualObjectColorCB();
			void repositionCameraCB();

		private:
/*
			void refreshObjectComboBoxes();
			void refreshObjectComboBox(
				GtkComboBox*& pComboBox,
				guint left_attach,
				guint right_attach,
				guint top_attach,
				guint bottom_attach);
*/
			//create a text combo box at the placeholder's location (if a combo box is created within Builder, critical
			//warnings are triggered at interface loading time because there is no way to tell Builder to use a text combo box,
			//and Gtk looks for a non existent TreeModel to use with the combo box
			//TODO : move this to a higher level class?
			::GtkComboBox* createTextComboBox(
				const gchar* pParentTableName,
				const std::vector<std::string>& rComboBoxEntries);

			gint showDialog(
			::GtkTable* pTable,
			const gchar* pTitle);

			static gboolean redrawCB(
				::GtkWidget* pWidget,
				::GdkEventExpose* pEvent,
				gpointer data);

			static gboolean sizeAllocateCB(
				::GtkWidget* pWidget,
				::GtkAllocation* pAllocation,
				gpointer data);

			/**
			 * Resizes the widget's drawing area.
			 */
			void resize(
				OpenViBE::uint32 ui32Width,
				OpenViBE::uint32 ui32Height);

			//! The database that contains the information to use to draw the signals
			CSimple3DDatabase& m_rSimple3DDatabase;

			::GtkBuilder* m_pBuilderInterface;

			::GtkTable* m_pCreateObjectTable;
			::GtkEntry* m_pCreateObjectName;
			::GtkRadioButton* m_pCreateObjectStandardRadioButton;
			::GtkComboBox* m_pStandardObjectComboBox;
			::GtkEntry* m_pCustomObjectEntry;

			::GtkTable* m_pDeleteObjectTable;
			::GtkEntry* m_pDeleteObjectName;

			::GtkTable* m_pSetPositionTable;
			::GtkEntry* m_pPositionObjectName;
			::GtkEntry* m_pXPositionEntry, *m_pYPositionEntry, *m_pZPositionEntry;

			::GtkTable* m_pSetScaleTable;
			::GtkEntry* m_pScaleObjectName;
			::GtkEntry* m_pXScaleEntry, *m_pYScaleEntry, *m_pZScaleEntry;

			::GtkTable* m_pSetColorTable;
			::GtkEntry* m_pColorObjectName;
			::GtkRadioButton* m_pSetMaterialColorRadioButton;
			::GtkEntry* m_pRedEntry, *m_pGreenEntry, *m_pBlueEntry, *m_pTransparencyEntry;
		};
	}
}

#endif //__SimpleVisualisationPlugin_CSimple3DView_H__
