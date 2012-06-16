#ifndef __OpenViBEDesigner_CPlayerVisualisation_H__
#define __OpenViBEDesigner_CPlayerVisualisation_H__

#include <string>
#include <vector>
#include <map>
#include <gtk/gtk.h>

namespace OpenViBEDesigner
{	
	class CPlayerVisualisation : public OpenViBE::Kernel::ITreeViewCB
	{
	public:		
		CPlayerVisualisation(
			const OpenViBE::Kernel::IKernelContext& rKernelContext, 
			const OpenViBE::Kernel::IScenario& rScenario,
			OpenViBE::Kernel::IVisualisationTree& rVisualisationTree);

		virtual ~CPlayerVisualisation();
	
		void init();
		void release();
	
		//ITreeViewCB callbacks overloading
		::GtkWidget* loadTreeWidget(
			OpenViBE::Kernel::IVisualisationWidget* pWidget);
		OpenViBE::boolean	setWidgets(
			const OpenViBE::CString& rVisualisationBoxName, 
			::GtkWidget* pWidget,
			::GtkWidget* pToolbarWidget);

		void showTopLevelWindows();
		void hideTopLevelWindows();

	protected:										

		static gboolean configure_event_cb(
			::GtkWidget* widget, 
			::GdkEventConfigure* event,
			gpointer user_data);
		static gboolean widget_expose_event_cb(
			::GtkWidget* widget,
			::GdkEventExpose* event,
			gpointer user_data);			
		void resizeCB(
			::GtkContainer* container);		
		
		//callbacks for DND
		static void	drag_data_get_from_widget_cb(
			::GtkWidget* pSrcWidget, 
			::GdkDragContext* pDC, 
			::GtkSelectionData* pSelectionData,
			guint uiInfo,
			guint uiTime,
			gpointer pData);
		static void	drag_data_received_in_widget_cb(
			::GtkWidget* pDstWidget, 
			::GdkDragContext*,
			gint,
			gint,
			::GtkSelectionData* pSelectionData,
			guint,
			guint,
			gpointer pData);

		//callback for toolbar
		static void toolbar_button_toggled_cb(
			::GtkToggleButton* pButton, 
			gpointer user_data);
		OpenViBE::boolean toggleToolbarCB(::GtkToggleButton* pButton);
		static gboolean toolbar_delete_event_cb(
			::GtkWidget *widget,
			::GdkEvent  *event,
      gpointer   user_data);
		OpenViBE::boolean deleteToolbarCB(::GtkWidget* pWidget);

		const OpenViBE::Kernel::IKernelContext&	m_rKernelContext;
		const OpenViBE::Kernel::IScenario& m_rScenario;
		OpenViBE::Kernel::IVisualisationTree& m_rVisualisationTree;				
		std::map < OpenViBE::CIdentifier, ::GtkWindow* > m_mVisualisationWindow;
		std::map < ::GtkWidget*, OpenViBE::CIdentifier > m_mVisualisationWidget;
		std::map < ::GtkToggleButton*, ::GtkWidget* > m_mVisualisationWidgetToolbar;
		::GtkToggleButton* m_pActiveToolbarButton; //pointer to button of active toolbar, if any
	};
};

#endif // __OpenViBEDesigner_CPlayerVisualisation_H__
