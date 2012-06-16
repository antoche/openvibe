#include "ovdCInterfacedScenario.h"
#include "ovdCBoxProxy.h"
#include "ovdCLinkProxy.h"
#include "ovdCConnectorEditor.h"
#include "ovdCSettingEditor.h"
#include "ovdCInterfacedObject.h"
#include "ovdTAttributeHandler.h"
#include "ovdCDesignerVisualisation.h"
#include "ovdCPlayerVisualisation.h"

#include <vector>
#include <iostream>

#include <gdk/gdkkeysyms.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEDesigner;
using namespace std;

extern map<uint32, ::GdkColor> g_vColors;

static ::GtkTargetEntry g_vTargetEntry[]= {
	{ "STRING", 0, 0 },
	{ "text/plain", 0, 0 } };

static ::GdkColor colorFromIdentifier(const CIdentifier& rIdentifier)
{
	::GdkColor l_oGdkColor;
	unsigned int l_ui32Value1=0;
	unsigned int l_ui32Value2=0;
	uint64 l_ui64Result=0;

	sscanf(rIdentifier.toString(), "(0x%08X, 0x%08X)", &l_ui32Value1, &l_ui32Value2);
	l_ui64Result+=l_ui32Value1;
	l_ui64Result<<=32;
	l_ui64Result+=l_ui32Value2;

	l_oGdkColor.pixel=(guint16)0;
	l_oGdkColor.red  =(guint16)(((l_ui64Result    )&0xffff)|0x8000);
	l_oGdkColor.green=(guint16)(((l_ui64Result>>16)&0xffff)|0x8000);
	l_oGdkColor.blue =(guint16)(((l_ui64Result>>32)&0xffff)|0x8000);

	return l_oGdkColor;
}

static void scenario_drawing_area_expose_cb(::GtkWidget* pWidget, ::GdkEventExpose* pEvent, gpointer pUserData)
{
	static_cast<CInterfacedScenario*>(pUserData)->scenarioDrawingAreaExposeCB(pEvent);
}
static void scenario_drawing_area_drag_data_received_cb(::GtkWidget* pWidget, ::GdkDragContext* pDragContext, gint iX, gint iY, ::GtkSelectionData* pSelectionData, guint uiInfo, guint uiT, gpointer pUserData)
{
	static_cast<CInterfacedScenario*>(pUserData)->scenarioDrawingAreaDragDataReceivedCB(pDragContext, iX, iY, pSelectionData, uiInfo, uiT);
}
static gboolean scenario_drawing_area_motion_notify_cb(::GtkWidget* pWidget, ::GdkEventMotion* pEvent, gpointer pUserData)
{
	static_cast<CInterfacedScenario*>(pUserData)->scenarioDrawingAreaMotionNotifyCB(pWidget, pEvent);
	return FALSE;
}
static void scenario_drawing_area_button_pressed_cb(::GtkWidget* pWidget, ::GdkEventButton* pEvent, gpointer pUserData)
{
	static_cast<CInterfacedScenario*>(pUserData)->scenarioDrawingAreaButtonPressedCB(pWidget, pEvent);
}
static void scenario_drawing_area_button_released_cb(::GtkWidget* pWidget, ::GdkEventButton* pEvent, gpointer pUserData)
{
	static_cast<CInterfacedScenario*>(pUserData)->scenarioDrawingAreaButtonReleasedCB(pWidget, pEvent);
}
static void scenario_drawing_area_key_press_event_cb(::GtkWidget* pWidget, ::GdkEventKey* pEvent, gpointer pUserData)
{
	static_cast<CInterfacedScenario*>(pUserData)->scenarioDrawingAreaKeyPressEventCB(pWidget, pEvent);
}
static void scenario_drawing_area_key_release_event_cb(::GtkWidget* pWidget, ::GdkEventKey* pEvent, gpointer pUserData)
{
	static_cast<CInterfacedScenario*>(pUserData)->scenarioDrawingAreaKeyReleaseEventCB(pWidget, pEvent);
}

template <void (CInterfacedScenario::*mfpCallback)(void)>
void menuitem_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
{
	std::cout << "menuitem_cb" << std::endl;
	(static_cast<CInterfacedScenario*>(pUserData)->*mfpCallback)();
}

	CInterfacedScenario::CInterfacedScenario(IKernel& rKernel, IScenario& rScenario, CIdentifier& rScenarioIdentifier, ::GtkNotebook& rNotebook, const char* sGUIFilename)
		:m_oScenarioIdentifier(rScenarioIdentifier)
		,m_rKernel(rKernel)
		,m_rScenario(rScenario)
		,m_pPlayer(NULL)
		,m_rNotebook(rNotebook)
		,m_pVisualisationTree(NULL)
		,m_pDesignerVisualisation(NULL)
		,m_bDesignerVisualisationToggled(false)
		,m_pPlayerVisualisation(NULL)
		,m_pGladeDummyScenarioNotebookTitle(NULL)
		,m_pGladeDummyScenarioNotebookClient(NULL)
		,m_pGladeTooltip(NULL)
		,m_pNotebookPageTitle(NULL)
		,m_pNotebookPageContent(NULL)
		,m_pScenarioDrawingArea(NULL)
		,m_pStencilBuffer(NULL)
		,m_bHasFileName(false)
		,m_bHasBeenModified(false)
		,m_bButtonPressed(false)
		,m_bShiftPressed(false)
		,m_bControlPressed(false)
		,m_bAltPressed(false)
		,m_sGUIFilename(sGUIFilename)
		,m_i32ViewOffsetX(0)
		,m_i32ViewOffsetY(0)
		,m_ui32CurrentMode(Mode_None)
	{
		m_pGladeDummyScenarioNotebookTitle=glade_xml_new(m_sGUIFilename.c_str(), "dummy_scenario_notebook_title", NULL);
		m_pGladeDummyScenarioNotebookClient=glade_xml_new(m_sGUIFilename.c_str(), "dummy_scenario_notebook_client", NULL);
		m_pGladeTooltip=glade_xml_new(m_sGUIFilename.c_str(), "openvibe_tooltip", NULL);

		m_pNotebookPageTitle=glade_xml_get_widget(m_pGladeDummyScenarioNotebookTitle, "dummy_scenario_notebook_title");
		m_pNotebookPageContent=glade_xml_get_widget(m_pGladeDummyScenarioNotebookClient, "dummy_scenario_notebook_client");
		gtk_widget_ref(m_pNotebookPageTitle);
		gtk_widget_ref(m_pNotebookPageContent);
		gtk_widget_unparent(m_pNotebookPageTitle);
		gtk_widget_unparent(m_pNotebookPageContent);
		gtk_notebook_append_page(&m_rNotebook, m_pNotebookPageContent, m_pNotebookPageTitle);
		gtk_widget_unref(m_pNotebookPageContent);
		gtk_widget_unref(m_pNotebookPageTitle);

		m_pScenarioDrawingArea=GTK_DRAWING_AREA(glade_xml_get_widget(m_pGladeDummyScenarioNotebookClient, "scenario_drawing_area"));
		gtk_drag_dest_set(GTK_WIDGET(m_pScenarioDrawingArea), GTK_DEST_DEFAULT_ALL, g_vTargetEntry, sizeof(g_vTargetEntry)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
		g_signal_connect(G_OBJECT(m_pScenarioDrawingArea), "expose_event", G_CALLBACK(scenario_drawing_area_expose_cb), this);
		g_signal_connect(G_OBJECT(m_pScenarioDrawingArea), "drag_data_received", G_CALLBACK(scenario_drawing_area_drag_data_received_cb), this);
		g_signal_connect(G_OBJECT(m_pScenarioDrawingArea), "motion_notify_event", G_CALLBACK(scenario_drawing_area_motion_notify_cb), this);
		g_signal_connect(G_OBJECT(m_pScenarioDrawingArea), "button_press_event", G_CALLBACK(scenario_drawing_area_button_pressed_cb), this);
		g_signal_connect(G_OBJECT(m_pScenarioDrawingArea), "button_release_event", G_CALLBACK(scenario_drawing_area_button_released_cb), this);
		g_signal_connect(G_OBJECT(m_pScenarioDrawingArea), "key-press-event", G_CALLBACK(scenario_drawing_area_key_press_event_cb), this);
		g_signal_connect(G_OBJECT(m_pScenarioDrawingArea), "key-release-event", G_CALLBACK(scenario_drawing_area_key_release_event_cb), this);
		//retrieve visualisation tree
		m_oVisualisationTreeIdentifier = m_rScenario.getVisualisationTreeIdentifier();
		m_pVisualisationTree = &m_rKernel.getContext()->getVisualisationManager().getVisualisationTree(m_oVisualisationTreeIdentifier);

		//FIXME : read this info from scenario file!
		//fill tree in case scenario already contains visualisation widgets
		CIdentifier l_oBoxIdentifier = m_rScenario.getNextBoxIdentifier(OV_UndefinedIdentifier);

		while(l_oBoxIdentifier != OV_UndefinedIdentifier)
		{
			const IBox* l_pBox = m_rScenario.getBoxDetails(l_oBoxIdentifier);
			CIdentifier l_oIdentifier = l_pBox->getAlgorithmClassIdentifier();
			const Plugins::IPluginObjectDesc* l_pPOD = m_rKernel.getContext()->getPluginManager().getPluginObjectDescCreating(l_oIdentifier);

			if(l_pPOD && l_pPOD->hasFunctionality(OpenViBE::Kernel::PluginFunctionality_Visualization))
			{
				CIdentifier l_oIdentifier;
				m_pVisualisationTree->addVisualisationWidget(
					l_oIdentifier,
					l_pBox->getName(),
					EVisualisationWidget_VisualisationBox,
					OV_UndefinedIdentifier,
					0,
					l_pBox->getIdentifier(),
					0);
			}

			l_oBoxIdentifier = m_rScenario.getNextBoxIdentifier(l_oBoxIdentifier);
		}

		//create window manager
		m_pDesignerVisualisation = new CDesignerVisualisation(*m_rKernel.getContext(), *m_pVisualisationTree, *this);
		m_pDesignerVisualisation->init(string(sGUIFilename));
	}

	CInterfacedScenario::~CInterfacedScenario(void)
	{
		//delete window manager
		if(m_pDesignerVisualisation)
		{
			delete m_pDesignerVisualisation;
		}

		if(m_pStencilBuffer)
		{
			g_object_unref(m_pStencilBuffer);
		}

		g_object_unref(m_pGladeDummyScenarioNotebookTitle);
		g_object_unref(m_pGladeDummyScenarioNotebookClient);
		g_object_unref(m_pGladeTooltip);

		gtk_notebook_remove_page(
			&m_rNotebook,
			gtk_notebook_page_num(&m_rNotebook, m_pNotebookPageContent));
	}

	boolean CInterfacedScenario::isLocked(void) const
	{
		return m_pPlayer!=NULL?true:false;
	}

	void CInterfacedScenario::redraw(void)
	{
		gdk_window_invalidate_rect(
			GTK_WIDGET(m_pScenarioDrawingArea)->window,
			NULL,
			true);
	}

	void CInterfacedScenario::updateScenarioLabel(void)
	{
		::GtkLabel* l_pTitleLabel=GTK_LABEL(glade_xml_get_widget(m_pGladeDummyScenarioNotebookTitle, "scenario_label"));
		string l_sLabel;
		l_sLabel+=m_bHasBeenModified?"*":"";
		l_sLabel+=" ";
		l_sLabel+=m_bHasFileName?m_sFileName.substr(m_sFileName.rfind('/')+1):"(untitled)";
		l_sLabel+=" ";
		l_sLabel+=m_bHasBeenModified?"*":"";
		gtk_label_set_text(l_pTitleLabel, l_sLabel.c_str());
	}

#define updateStencilIndex(id,stencilgc) { id++; ::GdkColor sc={0, (guint16)((id&0xff0000)>>8), (guint16)(id&0xff00), (guint16)((id&0xff)<<8) }; gdk_gc_set_rgb_fg_color(stencilgc, &sc); }

	void CInterfacedScenario::redrawBox(IBox& rBox)
	{
		::GtkWidget* l_pWidget=GTK_WIDGET(m_pScenarioDrawingArea);
		::GdkGC* l_pStencilGC=gdk_gc_new(GDK_DRAWABLE(m_pStencilBuffer));
		::GdkGC* l_pDrawGC=gdk_gc_new(l_pWidget->window);

		vector<pair<int32, int32> > l_vInputPosition;
		vector<pair<int32, int32> > l_vOutputPosition;

		OpenViBE::uint32 i;
		const int xMargin=5;
		const int yMargin=5;
		const int iCircleMargin=5;
		const int iCircleSize=11;
		const int iCircleSpace=3;

		CBoxProxy l_oBoxProxy(rBox);
		int xSize=l_oBoxProxy.getWidth(GTK_WIDGET(m_pScenarioDrawingArea))+xMargin*2;
		int ySize=l_oBoxProxy.getHeight(GTK_WIDGET(m_pScenarioDrawingArea))+yMargin*2;
		int xStart=l_oBoxProxy.getXCenter()+m_i32ViewOffsetX-(xSize>>1);
		int yStart=l_oBoxProxy.getYCenter()+m_i32ViewOffsetY-(ySize>>1);

		updateStencilIndex(m_ui32InterfacedObjectId, l_pStencilGC);
		gdk_draw_rectangle(
			GDK_DRAWABLE(m_pStencilBuffer),
			l_pStencilGC,
			TRUE,
			xStart, yStart, xSize, ySize);
		m_vInterfacedObject[m_ui32InterfacedObjectId]=CInterfacedObject(rBox.getIdentifier());

		boolean l_bCanCreate=m_rKernel.getContext()->getPluginManager().canCreatePluginObject(rBox.getAlgorithmClassIdentifier());
		gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[m_vCurrentObject[rBox.getIdentifier()]?Color_BoxBackgroundSelected:(!l_bCanCreate?Color_BoxBackgroundMissing:Color_BoxBackground)]);
		gdk_draw_rectangle(
			l_pWidget->window,
			l_pDrawGC,
			TRUE,
			xStart, yStart, xSize, ySize);
		gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[m_vCurrentObject[rBox.getIdentifier()]?Color_BoxBorderSelected:Color_BoxBorder]);
		gdk_draw_rectangle(
			l_pWidget->window,
			l_pDrawGC,
			FALSE,
			xStart, yStart, xSize, ySize);

		int l_iInputOffset=(xSize-rBox.getInputCount()*(iCircleSpace+iCircleSize)-iCircleMargin*2)/2;
		for(i=0; i<rBox.getInputCount(); i++)
		{
			CIdentifier l_oInputIdentifier;
			rBox.getInputType(i, l_oInputIdentifier);
			::GdkColor l_oInputColor=colorFromIdentifier(l_oInputIdentifier);

			::GdkPoint l_vPoint[4];
			l_vPoint[0].x=iCircleSize>>1;
			l_vPoint[0].y=iCircleSize;
			l_vPoint[1].x=0;
			l_vPoint[1].y=0;
			l_vPoint[2].x=iCircleSize-1;
			l_vPoint[2].y=0;
			for(int j=0; j<3; j++)
			{
				l_vPoint[j].x+=xStart+iCircleMargin+i*(iCircleSpace+iCircleSize)+l_iInputOffset;
				l_vPoint[j].y+=yStart-(iCircleSize>>1);
			}

			updateStencilIndex(m_ui32InterfacedObjectId, l_pStencilGC);
			gdk_draw_polygon(
				GDK_DRAWABLE(m_pStencilBuffer),
				l_pStencilGC,
				TRUE,
				l_vPoint,
				3);
			m_vInterfacedObject[m_ui32InterfacedObjectId]=CInterfacedObject(rBox.getIdentifier(), Connector_Input, i);

			gdk_gc_set_rgb_fg_color(l_pDrawGC, &l_oInputColor);
			gdk_draw_polygon(
				l_pWidget->window,
				l_pDrawGC,
				TRUE,
				l_vPoint,
				3);
			gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[Color_BoxInputBorder]);
			gdk_draw_polygon(
				l_pWidget->window,
				l_pDrawGC,
				FALSE,
				l_vPoint,
				3);

			int32 x=xStart+iCircleMargin+i*(iCircleSpace+iCircleSize)+(iCircleSize>>1)-m_i32ViewOffsetX+l_iInputOffset;
			int32 y=yStart-(iCircleSize>>1)-m_i32ViewOffsetY;
			CIdentifier l_oLinkIdentifier=m_rScenario.getNextLinkIdentifierToBoxInput(OV_UndefinedIdentifier, rBox.getIdentifier(), i);
			while(l_oLinkIdentifier!=OV_UndefinedIdentifier)
			{
				ILink* l_pLink=m_rScenario.getLinkDetails(l_oLinkIdentifier);
				if(l_pLink)
				{
					TAttributeHandler l_oAttributeHandler(*l_pLink);

					if(!l_oAttributeHandler.hasAttribute(OVD_AttributeId_XLinkTargetPosition))
						l_oAttributeHandler.addAttribute<int>(OVD_AttributeId_XLinkTargetPosition, x);
					else
						l_oAttributeHandler.setAttributeValue<int>(OVD_AttributeId_XLinkTargetPosition, x);

					if(!l_oAttributeHandler.hasAttribute(OVD_AttributeId_YLinkTargetPosition))
						l_oAttributeHandler.addAttribute<int>(OVD_AttributeId_YLinkTargetPosition, y);
					else
						l_oAttributeHandler.setAttributeValue<int>(OVD_AttributeId_YLinkTargetPosition, y);
				}
				l_oLinkIdentifier=m_rScenario.getNextLinkIdentifierToBoxInput(l_oLinkIdentifier, rBox.getIdentifier(), i);
			}
		}

		int l_iOutputOffset=(xSize-rBox.getOutputCount()*(iCircleSpace+iCircleSize)-iCircleMargin*2)/2;
		for(i=0; i<rBox.getOutputCount(); i++)
		{
			CIdentifier l_oOutputIdentifier;
			rBox.getOutputType(i, l_oOutputIdentifier);
			::GdkColor l_oOutputColor=colorFromIdentifier(l_oOutputIdentifier);

			::GdkPoint l_vPoint[4];
			l_vPoint[0].x=iCircleSize>>1;
			l_vPoint[0].y=iCircleSize;
			l_vPoint[1].x=0;
			l_vPoint[1].y=0;
			l_vPoint[2].x=iCircleSize-1;
			l_vPoint[2].y=0;
			for(int j=0; j<3; j++)
			{
				l_vPoint[j].x+=xStart+iCircleMargin+i*(iCircleSpace+iCircleSize)+l_iOutputOffset;
				l_vPoint[j].y+=yStart-(iCircleSize>>1)+ySize;
			}

			updateStencilIndex(m_ui32InterfacedObjectId, l_pStencilGC);
			gdk_draw_polygon(
				GDK_DRAWABLE(m_pStencilBuffer),
				l_pStencilGC,
				TRUE,
				l_vPoint,
				3);
			m_vInterfacedObject[m_ui32InterfacedObjectId]=CInterfacedObject(rBox.getIdentifier(), Connector_Output, i);

			gdk_gc_set_rgb_fg_color(l_pDrawGC, &l_oOutputColor);
			gdk_draw_polygon(
				l_pWidget->window,
				l_pDrawGC,
				TRUE,
				l_vPoint,
				3);
			gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[Color_BoxOutputBorder]);
			gdk_draw_polygon(
				l_pWidget->window,
				l_pDrawGC,
				FALSE,
				l_vPoint,
				3);

			int32 x=xStart+iCircleMargin+i*(iCircleSpace+iCircleSize)+(iCircleSize>>1)-m_i32ViewOffsetX+l_iOutputOffset;
			int32 y=yStart+ySize+(iCircleSize>>1)+1-m_i32ViewOffsetY;
			CIdentifier l_oLinkIdentifier=m_rScenario.getNextLinkIdentifierFromBoxOutput(OV_UndefinedIdentifier, rBox.getIdentifier(), i);
			while(l_oLinkIdentifier!=OV_UndefinedIdentifier)
			{
				ILink* l_pLink=m_rScenario.getLinkDetails(l_oLinkIdentifier);
				if(l_pLink)
				{
					TAttributeHandler l_oAttributeHandler(*l_pLink);

					if(!l_oAttributeHandler.hasAttribute(OVD_AttributeId_XLinkSourcePosition))
						l_oAttributeHandler.addAttribute<int>(OVD_AttributeId_XLinkSourcePosition, x);
					else
						l_oAttributeHandler.setAttributeValue<int>(OVD_AttributeId_XLinkSourcePosition, x);

					if(!l_oAttributeHandler.hasAttribute(OVD_AttributeId_YLinkSourcePosition))
						l_oAttributeHandler.addAttribute<int>(OVD_AttributeId_YLinkSourcePosition, y);
					else
						l_oAttributeHandler.setAttributeValue<int>(OVD_AttributeId_YLinkSourcePosition, y);
				}
				l_oLinkIdentifier=m_rScenario.getNextLinkIdentifierFromBoxOutput(l_oLinkIdentifier, rBox.getIdentifier(), i);
			}
		}

/*
		::GdkPixbuf* l_pPixbuf=gtk_widget_render_icon(l_pWidget, GTK_STOCK_EXECUTE, GTK_ICON_SIZE_SMALL_TOOLBAR, "openvibe");
		if(l_pPixbuf)
		{
			gdk_draw_pixbuf(l_pWidget->window, l_pDrawGC, l_pPixbuf, 0, 0, 10, 10, 64, 64, GDK_RGB_DITHER_NONE, 0, 0);
			g_object_unref(l_pPixbuf);
		}
*/

		::PangoContext* l_pPangoContext=NULL;
		::PangoLayout* l_pPangoLayout=NULL;
		l_pPangoContext=gtk_widget_get_pango_context(l_pWidget);
		l_pPangoLayout=pango_layout_new(l_pPangoContext);
		pango_layout_set_alignment(l_pPangoLayout, PANGO_ALIGN_CENTER);
		pango_layout_set_text(l_pPangoLayout, l_oBoxProxy.getLabel(), -1);
		gdk_draw_layout(
			l_pWidget->window,
			l_pWidget->style->text_gc[GTK_WIDGET_STATE(l_pWidget)],
			xStart+xMargin, yStart+yMargin, l_pPangoLayout);
		g_object_unref(l_pPangoLayout);

		g_object_unref(l_pDrawGC);
		g_object_unref(l_pStencilGC);

/*
		CLinkPositionSetterEnum l_oLinkPositionSetterInput(Connector_Input, l_vInputPosition);
		CLinkPositionSetterEnum l_oLinkPositionSetterOutput(Connector_Output, l_vOutputPosition);
		rScenario.enumerateLinksToBox(l_oLinkPositionSetterInput, rBox.getIdentifier());
		rScenario.enumerateLinksFromBox(l_oLinkPositionSetterOutput, rBox.getIdentifier());
*/
	}

	void CInterfacedScenario::redrawLink(ILink& rLink)
	{
		::GtkWidget* l_pWidget=GTK_WIDGET(m_pScenarioDrawingArea);
		::GdkGC* l_pStencilGC=gdk_gc_new(GDK_DRAWABLE(m_pStencilBuffer));
		::GdkGC* l_pDrawGC=gdk_gc_new(l_pWidget->window);

		CLinkProxy l_oLinkProxy(rLink);

		updateStencilIndex(m_ui32InterfacedObjectId, l_pStencilGC);
		gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[m_vCurrentObject[rLink.getIdentifier()]?Color_LinkSelected:Color_Link]);
		gdk_draw_line(
			GDK_DRAWABLE(m_pStencilBuffer),
			l_pStencilGC,
			l_oLinkProxy.getXSource()+m_i32ViewOffsetX, l_oLinkProxy.getYSource()+m_i32ViewOffsetY,
			l_oLinkProxy.getXTarget()+m_i32ViewOffsetX, l_oLinkProxy.getYTarget()+m_i32ViewOffsetY);
		gdk_draw_line(
			l_pWidget->window,
			l_pDrawGC,
			l_oLinkProxy.getXSource()+m_i32ViewOffsetX, l_oLinkProxy.getYSource()+m_i32ViewOffsetY,
			l_oLinkProxy.getXTarget()+m_i32ViewOffsetX, l_oLinkProxy.getYTarget()+m_i32ViewOffsetY);
		m_vInterfacedObject[m_ui32InterfacedObjectId]=CInterfacedObject(rLink.getIdentifier(), Connector_Link, 0);

		g_object_unref(l_pDrawGC);
		g_object_unref(l_pStencilGC);
	}

#undef updateStencilIndex

	uint32 CInterfacedScenario::pickInterfacedObject(int x, int y)
	{
		if(!GDK_DRAWABLE(m_pStencilBuffer))
		{
			return 0xffffffff;
		}

		int l_iMaxX;
		int l_iMaxY;
		uint32 l_ui32InterfacedObjectId=0xffffffff;
		gdk_drawable_get_size(GDK_DRAWABLE(m_pStencilBuffer), &l_iMaxX, &l_iMaxY);
		if(x>=0 && y>=0 && x<l_iMaxX && y<l_iMaxY)
		{
			::GdkPixbuf* l_pPixbuf=gdk_pixbuf_get_from_drawable(NULL, GDK_DRAWABLE(m_pStencilBuffer), NULL, x, y, 0, 0, 1, 1);
			l_ui32InterfacedObjectId=0;
			l_ui32InterfacedObjectId+=(gdk_pixbuf_get_pixels(l_pPixbuf)[0]<<16);
			l_ui32InterfacedObjectId+=(gdk_pixbuf_get_pixels(l_pPixbuf)[1]<<8);
			l_ui32InterfacedObjectId+=(gdk_pixbuf_get_pixels(l_pPixbuf)[2]);
			g_object_unref(l_pPixbuf);
		}
		return l_ui32InterfacedObjectId;
	}

	boolean CInterfacedScenario::pickInterfacedObject(int x, int y, int iSizeX, int iSizeY)
	{
		if(!GDK_DRAWABLE(m_pStencilBuffer))
		{
			return false;
		}

		int i,j;
		int l_iMaxX;
		int l_iMaxY;
		gdk_drawable_get_size(GDK_DRAWABLE(m_pStencilBuffer), &l_iMaxX, &l_iMaxY);

		int iStartX=x;
		int iStartY=y;
		int iEndX=x+iSizeX;
		int iEndY=y+iSizeY;

		// crops according to drawing area boundings
		if(iStartX<0) iStartX=0;
		if(iStartY<0) iStartY=0;
		if(iEndX<0) iEndX=0;
		if(iEndY<0) iEndY=0;
		if(iStartX>=l_iMaxX-1) iStartX=l_iMaxX-1;
		if(iStartY>=l_iMaxY-1) iStartY=l_iMaxY-1;
		if(iEndX>=l_iMaxX-1) iEndX=l_iMaxX-1;
		if(iEndY>=l_iMaxY-1) iEndY=l_iMaxY-1;

		// recompute new size
		iSizeX=iEndX-iStartX+1;
		iSizeY=iEndY-iStartY+1;

		::GdkPixbuf* l_pPixbuf=gdk_pixbuf_get_from_drawable(NULL, GDK_DRAWABLE(m_pStencilBuffer), NULL, iStartX, iStartY, 0, 0, iSizeX, iSizeY);
		guchar* l_pPixels=gdk_pixbuf_get_pixels(l_pPixbuf);
		int l_iRowBytesCount=gdk_pixbuf_get_rowstride(l_pPixbuf);
		int l_iChannelCount=gdk_pixbuf_get_n_channels(l_pPixbuf);
		for(j=0; j<iSizeY; j++)
		{
			for(i=0; i<iSizeX; i++)
			{
				uint32 l_ui32InterfacedObjectId=0;
				l_ui32InterfacedObjectId+=(l_pPixels[j*l_iRowBytesCount+i*l_iChannelCount+0]<<16);
				l_ui32InterfacedObjectId+=(l_pPixels[j*l_iRowBytesCount+i*l_iChannelCount+1]<<8);
				l_ui32InterfacedObjectId+=(l_pPixels[j*l_iRowBytesCount+i*l_iChannelCount+2]);
				if(m_vInterfacedObject[l_ui32InterfacedObjectId].m_oIdentifier!=OV_UndefinedIdentifier)
				{
					if(!m_vCurrentObject[m_vInterfacedObject[l_ui32InterfacedObjectId].m_oIdentifier])
					{
						m_vCurrentObject[m_vInterfacedObject[l_ui32InterfacedObjectId].m_oIdentifier]=true;
					}
				}
			}
		}
		g_object_unref(l_pPixbuf);
		return true;
	}

	void CInterfacedScenario::scenarioDrawingAreaExposeCB(::GdkEventExpose* pEvent)
	{
		gint x,y;

		gdk_window_get_size(GTK_WIDGET(m_pScenarioDrawingArea)->window, &x, &y);
		if(m_pStencilBuffer) g_object_unref(m_pStencilBuffer);
		m_pStencilBuffer=gdk_pixmap_new(NULL, x, y, 24);

		::GdkGC* l_pStencilGC=gdk_gc_new(m_pStencilBuffer);
		::GdkColor l_oColor={0, 0, 0, 0};
		gdk_gc_set_rgb_fg_color(l_pStencilGC, &l_oColor);
		gdk_draw_rectangle(
			GDK_DRAWABLE(m_pStencilBuffer),
			l_pStencilGC,
			TRUE,
			0, 0, x, y);
		g_object_unref(l_pStencilGC);

		if(this->isLocked())
		{
			::GdkColor l_oColor;
			l_oColor.pixel=0;
			l_oColor.red  =0x0f00;
			l_oColor.green=0x0f00;
			l_oColor.blue =0x0f00;

			::GdkGC* l_pDrawGC=gdk_gc_new(GTK_WIDGET(m_pScenarioDrawingArea)->window);
			gdk_gc_set_rgb_fg_color(l_pDrawGC, &l_oColor);
			gdk_gc_set_function(l_pDrawGC, GDK_XOR);
			gdk_draw_rectangle(
				GTK_WIDGET(m_pScenarioDrawingArea)->window,
				l_pDrawGC,
				TRUE,
				0, 0, x, y);
			g_object_unref(l_pDrawGC);
		}

		m_ui32InterfacedObjectId=0;
		m_vInterfacedObject.clear();

		CIdentifier l_oBoxIdentifier=m_rScenario.getNextBoxIdentifier(OV_UndefinedIdentifier);
		while(l_oBoxIdentifier!=OV_UndefinedIdentifier)
		{
			redrawBox(*m_rScenario.getBoxDetails(l_oBoxIdentifier));
			l_oBoxIdentifier=m_rScenario.getNextBoxIdentifier(l_oBoxIdentifier);
		}

		CIdentifier l_oLinkIdentifier=m_rScenario.getNextLinkIdentifier(OV_UndefinedIdentifier);
		while(l_oLinkIdentifier!=OV_UndefinedIdentifier)
		{
			redrawLink(*m_rScenario.getLinkDetails(l_oLinkIdentifier));
			l_oLinkIdentifier=m_rScenario.getNextLinkIdentifier(l_oLinkIdentifier);
		}

		if(m_ui32CurrentMode==Mode_Selection || m_ui32CurrentMode==Mode_SelectionAdd)
		{
			int l_iStartX=(int)min(m_f64PressMouseX, m_f64CurrentMouseX);
			int l_iStartY=(int)min(m_f64PressMouseY, m_f64CurrentMouseY);
			int l_iSizeX=(int)max(m_f64PressMouseX-m_f64CurrentMouseX, m_f64CurrentMouseX-m_f64PressMouseX);
			int l_iSizeY=(int)max(m_f64PressMouseY-m_f64CurrentMouseY, m_f64CurrentMouseY-m_f64PressMouseY);

			::GtkWidget* l_pWidget=GTK_WIDGET(m_pScenarioDrawingArea);
			::GdkGC* l_pDrawGC=gdk_gc_new(l_pWidget->window);
			gdk_gc_set_function(l_pDrawGC, GDK_OR);
			gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[Color_SelectionArea]);
			gdk_draw_rectangle(
				l_pWidget->window,
				l_pDrawGC,
				TRUE,
				l_iStartX, l_iStartY, l_iSizeX, l_iSizeY);
			gdk_gc_set_function(l_pDrawGC, GDK_COPY);
			gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[Color_SelectionAreaBorder]);
			gdk_draw_rectangle(
				l_pWidget->window,
				l_pDrawGC,
				FALSE,
				l_iStartX, l_iStartY, l_iSizeX, l_iSizeY);
			g_object_unref(l_pDrawGC);
		}

		if(m_ui32CurrentMode==Mode_Connect)
		{
			::GtkWidget* l_pWidget=GTK_WIDGET(m_pScenarioDrawingArea);
			::GdkGC* l_pDrawGC=gdk_gc_new(l_pWidget->window);

			gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[Color_Link]);
			gdk_draw_line(
				l_pWidget->window,
				l_pDrawGC,
				(int)m_f64PressMouseX, (int)m_f64PressMouseY,
				(int)m_f64CurrentMouseX, (int)m_f64CurrentMouseY);
			g_object_unref(l_pDrawGC);
		}
	}
	void CInterfacedScenario::scenarioDrawingAreaDragDataReceivedCB(::GdkDragContext* pDragContext, gint iX, gint iY, ::GtkSelectionData* pSelectionData, guint uiInfo, guint uiT)
	{
		m_rKernel.getContext()->getLogManager() << LogLevel_Debug << "scenarioDrawingAreaDragDataReceivedCB [" << (const char*)gtk_selection_data_get_text(pSelectionData) << "]\n";

		if(this->isLocked()) return;

		CIdentifier l_oBoxIdentifier;
		CIdentifier l_oBoxAlgorithmClassIdentifier;
		l_oBoxAlgorithmClassIdentifier.fromString((const char*)gtk_selection_data_get_text(pSelectionData));
		if(l_oBoxAlgorithmClassIdentifier!=OV_UndefinedIdentifier)
		{
			m_rScenario.addBox(l_oBoxAlgorithmClassIdentifier, l_oBoxIdentifier);

			IBox* l_pBox = m_rScenario.getBoxDetails(l_oBoxIdentifier);
			CIdentifier l_oId = l_pBox->getAlgorithmClassIdentifier();
			const Plugins::IPluginObjectDesc* l_pPOD = m_rKernel.getContext()->getPluginManager().getPluginObjectDescCreating(l_oId);

			//if a visualisation box was dropped, add it in window manager
			if(l_pPOD && l_pPOD->hasFunctionality(OpenViBE::Kernel::PluginFunctionality_Visualization))
			{
				//generate a unique name so that it can be identified unambiguously
				CString l_oBoxName;
				generateDisplayPluginName(l_pBox, l_oBoxName);
				l_pBox->setName(l_oBoxName);

				//let window manager know about new box
				m_pDesignerVisualisation->onVisualisationBoxAdded(l_pBox);
			}

			CBoxProxy l_oBoxProxy(m_rScenario, l_oBoxIdentifier);
			l_oBoxProxy.setCenter(iX-m_i32ViewOffsetX, iY-m_i32ViewOffsetY);
			m_bHasBeenModified=true;
			updateScenarioLabel();
		}

		m_f64CurrentMouseX=iX;
		m_f64CurrentMouseY=iY;
	}
	void CInterfacedScenario::scenarioDrawingAreaMotionNotifyCB(::GtkWidget* pWidget, ::GdkEventMotion* pEvent)
	{
		m_rKernel.getContext()->getLogManager() << LogLevel_Debug << "scenarioDrawingAreaMotionNotifyCB\n";

		if(this->isLocked()) return;

		::GtkWidget* l_pTooltip=glade_xml_get_widget(m_pGladeTooltip, "openvibe_tooltip");
		gtk_widget_set_name(l_pTooltip, "gtk-tooltips");
		uint32 l_ui32InterfacedObjectId=pickInterfacedObject((int)pEvent->x, (int)pEvent->y);
		CInterfacedObject& l_rObject=m_vInterfacedObject[l_ui32InterfacedObjectId];
		if(l_rObject.m_oIdentifier!=OV_UndefinedIdentifier
		&& l_rObject.m_ui32ConnectorType!=Connector_Link
		&& l_rObject.m_ui32ConnectorType!=Connector_None)
		{
			IBox* l_pBoxDetails=m_rScenario.getBoxDetails(l_rObject.m_oIdentifier);
			if(l_pBoxDetails)
			{
				CString l_sName;
				CString l_sType;
				if(l_rObject.m_ui32ConnectorType==Connector_Input)
				{
					CIdentifier l_oType;
					l_pBoxDetails->getInputName(l_rObject.m_ui32ConnectorIndex, l_sName);
					l_pBoxDetails->getInputType(l_rObject.m_ui32ConnectorIndex, l_oType);
					l_sType=m_rKernel.getContext()->getTypeManager().getTypeName(l_oType);
				}
				if(l_rObject.m_ui32ConnectorType==Connector_Output)
				{
					CIdentifier l_oType;
					l_pBoxDetails->getOutputName(l_rObject.m_ui32ConnectorIndex, l_sName);
					l_pBoxDetails->getOutputType(l_rObject.m_ui32ConnectorIndex, l_oType);
					l_sType=m_rKernel.getContext()->getTypeManager().getTypeName(l_oType);
				}
				l_sType=CString("[")+l_sType+CString("]");
				gtk_label_set_text(GTK_LABEL(glade_xml_get_widget(m_pGladeTooltip, "label_name_content")), l_sName);
				gtk_label_set_text(GTK_LABEL(glade_xml_get_widget(m_pGladeTooltip, "label_type_content")), l_sType);
				gtk_window_move(GTK_WINDOW(l_pTooltip), (gint)pEvent->x_root, (gint)pEvent->y_root+40);
				gtk_widget_show(l_pTooltip);
			}
		}
		else
		{
			gtk_widget_hide(l_pTooltip);
		}

		if(m_ui32CurrentMode!=Mode_None)
		{
			if(m_ui32CurrentMode==Mode_MoveScenario)
			{
				m_i32ViewOffsetX+=(int32)(pEvent->x-m_f64CurrentMouseX);
				m_i32ViewOffsetY+=(int32)(pEvent->y-m_f64CurrentMouseY);
			}
			if(m_ui32CurrentMode==Mode_MoveSelection)
			{
				map<CIdentifier, boolean>::const_iterator i;
				for(i=m_vCurrentObject.begin(); i!=m_vCurrentObject.end(); i++)
				{
					if(i->second && m_rScenario.isBox(i->first))
					{
						CBoxProxy l_oBoxProxy(m_rScenario, i->first);
						l_oBoxProxy.setCenter(
							l_oBoxProxy.getXCenter()+(int32)(pEvent->x-m_f64CurrentMouseX),
							l_oBoxProxy.getYCenter()+(int32)(pEvent->y-m_f64CurrentMouseY));
					}
				}
			}

			this->redraw();
		}
		m_f64CurrentMouseX=pEvent->x;
		m_f64CurrentMouseY=pEvent->y;
	}
	void CInterfacedScenario::scenarioDrawingAreaButtonPressedCB(::GtkWidget* pWidget, ::GdkEventButton* pEvent)
	{
		m_rKernel.getContext()->getLogManager() << LogLevel_Debug << "scenarioDrawingAreaButtonPressedCB\n";

		if(this->isLocked()) return;

		::GtkWidget* l_pTooltip=glade_xml_get_widget(m_pGladeTooltip, "openvibe_tooltip");
		gtk_widget_hide(l_pTooltip);
		gtk_widget_grab_focus(pWidget);

		m_bButtonPressed|=((pEvent->type==GDK_BUTTON_PRESS)&&(pEvent->button==1));
		m_f64PressMouseX=pEvent->x;
		m_f64PressMouseY=pEvent->y;

		uint32 l_ui32InterfacedObjectId=pickInterfacedObject((int)m_f64PressMouseX, (int)m_f64PressMouseY);
		m_oCurrentObject=m_vInterfacedObject[l_ui32InterfacedObjectId];

		switch(pEvent->button)
		{
			case 1:
				switch(pEvent->type)
				{
					case GDK_BUTTON_PRESS:
						if(m_oCurrentObject.m_oIdentifier==OV_UndefinedIdentifier)
						{
							if(m_bShiftPressed)
							{
								m_ui32CurrentMode=Mode_MoveScenario;
							}
							else
							{
								if(m_bControlPressed)
								{
									m_ui32CurrentMode=Mode_SelectionAdd;
								}
								else
								{
									m_ui32CurrentMode=Mode_Selection;
								}
							}
						}
						else
						{
							if(m_oCurrentObject.m_ui32ConnectorType==Connector_Input || m_oCurrentObject.m_ui32ConnectorType==Connector_Output)
							{
								m_ui32CurrentMode=Mode_Connect;
							}
							else
							{
								m_ui32CurrentMode=Mode_MoveSelection;
								if(!m_vCurrentObject[m_oCurrentObject.m_oIdentifier])
								{
									if(!m_bControlPressed)
									{
										m_vCurrentObject.clear();
									}
									m_vCurrentObject[m_oCurrentObject.m_oIdentifier]=true;
								}
							}
						}
						break;
					case GDK_2BUTTON_PRESS:
						if(m_oCurrentObject.m_oIdentifier!=OV_UndefinedIdentifier)
						{
							m_ui32CurrentMode=Mode_EditSettings;
							m_bShiftPressed=false;
							m_bControlPressed=false;
							m_bAltPressed=false;

							if(m_oCurrentObject.m_ui32ConnectorType==Connector_Input || m_oCurrentObject.m_ui32ConnectorType==Connector_Output)
							{
								IBox* l_pBox=m_rScenario.getBoxDetails(m_oCurrentObject.m_oIdentifier);
								if(l_pBox)
								{
									CConnectorEditor l_oConnectorEditor(m_rKernel, *l_pBox, m_oCurrentObject.m_ui32ConnectorType, m_oCurrentObject.m_ui32ConnectorIndex, m_sGUIFilename.c_str());
									l_oConnectorEditor.run();
								}
							}
							else
							{
								IBox* l_pBox=m_rScenario.getBoxDetails(m_oCurrentObject.m_oIdentifier);
								if(l_pBox)
								{
									CSettingEditor l_oSettingEditor(m_rKernel, *l_pBox, m_sGUIFilename.c_str());
									l_oSettingEditor.run();
								}
							}
						}
						break;
					default:
						break;
				}
				break;

			case 2:
				break;

			case 3:
#if 0
				switch(pEvent->type)
				{
					case GDK_BUTTON_PRESS:
						{
							if(m_oCurrentObject.m_oIdentifier!=OV_UndefinedIdentifier)
							{
								IBox* l_pBox=m_rScenario.getBoxDetails(m_oCurrentObject.m_oIdentifier);
								if(l_pBox)
								{
									uint32 i;
									::GtkMenu* l_pMenu=GTK_MENU(gtk_menu_new());

									#define gtk_menu_add_new_image_menu_item(menu, menuitem, icon, label, cb) \
										::GtkImageMenuItem* menuitem=NULL; \
										{ \
											menuitem=GTK_IMAGE_MENU_ITEM(gtk_image_menu_item_new_with_label(label)); \
											gtk_image_menu_item_set_image(menuitem, gtk_image_new_from_stock(icon, GTK_ICON_SIZE_MENU)); \
											gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menuitem)); \
											if((void*)(cb)) g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(cb), this); \
										}
									#define gtk_menu_add_separator_menu_item(menu) \
										{ \
											::GtkSeparatorMenuItem* menuitem=GTK_SEPARATOR_MENU_ITEM(gtk_separator_menu_item_new()); \
											gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(menuitem)); \
										}

									// -------------- INPUTS --------------

									::GtkMenu* l_pMenuInput=GTK_MENU(gtk_menu_new());
									gtk_menu_add_new_image_menu_item(l_pMenu, l_pMenuItemInput, GTK_STOCK_PROPERTIES, "modify inputs", NULL);
									for(i=0; i<l_pBox->getInputCount(); i++)
									{
										CString l_sName;
										CIdentifier l_oType;
										l_pBox->getInputName(i, l_sName);
										l_pBox->getInputType(i, l_oType);
										gtk_menu_add_new_image_menu_item(l_pMenuInput, l_pMenuInputMenuItem, GTK_STOCK_PROPERTIES, l_sName, NULL);

										::GtkMenu* l_pMenuInputMenuAction=GTK_MENU(gtk_menu_new());
										gtk_menu_add_new_image_menu_item(l_pMenuInputMenuAction, l_pMenuInputInputMenuItemEdit, GTK_STOCK_EDIT, "edit...", NULL);
										gtk_menu_add_new_image_menu_item(l_pMenuInputMenuAction, l_pMenuInputInputMenuItemRemove, GTK_STOCK_REMOVE, "remove...", NULL);
										gtk_menu_item_set_submenu(GTK_MENU_ITEM(l_pMenuInputMenuItem), GTK_WIDGET(l_pMenuInputMenuAction));
									}
									gtk_menu_add_separator_menu_item(l_pMenuInput);
									gtk_menu_add_new_image_menu_item(l_pMenuInput, l_pMenuInputMenuItemAdd, GTK_STOCK_ADD, "add...", NULL);
									gtk_menu_item_set_submenu(GTK_MENU_ITEM(l_pMenuItemInput), GTK_WIDGET(l_pMenuInput));

									// -------------- OUTPUTS --------------

									gtk_menu_add_new_image_menu_item(l_pMenu, l_pMenuItemOutput, GTK_STOCK_PROPERTIES, "modify outputs", NULL);
									::GtkMenu* l_pMenuOutput=GTK_MENU(gtk_menu_new());
									for(i=0; i<l_pBox->getOutputCount(); i++)
									{
										CString l_sName;
										CIdentifier l_oType;
										l_pBox->getOutputName(i, l_sName);
										l_pBox->getOutputType(i, l_oType);
										gtk_menu_add_new_image_menu_item(l_pMenuOutput, l_pMenuOutputMenuItem, GTK_STOCK_PROPERTIES, l_sName, NULL);

										::GtkMenu* l_pMenuOutputMenuAction=GTK_MENU(gtk_menu_new());
										gtk_menu_add_new_image_menu_item(l_pMenuOutputMenuAction, l_pMenuOutputInputMenuItemEdit, GTK_STOCK_EDIT, "edit...", NULL);
										gtk_menu_add_new_image_menu_item(l_pMenuOutputMenuAction, l_pMenuOutputInputMenuItemRemove, GTK_STOCK_REMOVE, "remove...", NULL);
										gtk_menu_item_set_submenu(GTK_MENU_ITEM(l_pMenuOutputMenuItem), GTK_WIDGET(l_pMenuOutputMenuAction));
									}
									gtk_menu_add_separator_menu_item(l_pMenuOutput);
									gtk_menu_add_new_image_menu_item(l_pMenuOutput, l_pMenuOutputMenuItemAdd, GTK_STOCK_ADD, "add...", NULL);
									gtk_menu_item_set_submenu(GTK_MENU_ITEM(l_pMenuItemOutput), GTK_WIDGET(l_pMenuOutput));

									// -------------- SETTINGS --------------

									gtk_menu_add_new_image_menu_item(l_pMenu, l_pMenuItemSetting, GTK_STOCK_PROPERTIES, "modify settings", NULL);
									::GtkMenu* l_pMenuSetting=GTK_MENU(gtk_menu_new());
									for(i=0; i<l_pBox->getSettingCount(); i++)
									{
										CString l_sName;
										CIdentifier l_oType;
										l_pBox->getSettingName(i, l_sName);
										l_pBox->getSettingType(i, l_oType);
										gtk_menu_add_new_image_menu_item(l_pMenuSetting, l_pMenuSettingMenuItem, GTK_STOCK_PROPERTIES, l_sName, NULL);

										::GtkMenu* l_pMenuSettingMenuAction=GTK_MENU(gtk_menu_new());
										gtk_menu_add_new_image_menu_item(l_pMenuSettingMenuAction, l_pMenuSettingInputMenuItemEdit, GTK_STOCK_EDIT, "edit...", NULL);
										gtk_menu_add_new_image_menu_item(l_pMenuSettingMenuAction, l_pMenuSettingInputMenuItemRemove, GTK_STOCK_REMOVE, "remove...", NULL);
										gtk_menu_item_set_submenu(GTK_MENU_ITEM(l_pMenuSettingMenuItem), GTK_WIDGET(l_pMenuSettingMenuAction));
									}
									gtk_menu_add_separator_menu_item(l_pMenuSetting);
									gtk_menu_add_new_image_menu_item(l_pMenuSetting, l_pMenuSettingMenuItemAdd, GTK_STOCK_ADD, "add...", NULL);
									gtk_menu_item_set_submenu(GTK_MENU_ITEM(l_pMenuItemSetting), GTK_WIDGET(l_pMenuSetting));

									// -------------- PROCESSING UNIT --------------

									gtk_menu_add_new_image_menu_item(l_pMenu, l_pMenuItemProcessUnit, GTK_STOCK_EXECUTE, "process unit", NULL);
									::GtkMenu* l_pMenuProcessingUnit=GTK_MENU(gtk_menu_new());
									gtk_menu_add_new_image_menu_item(l_pMenuProcessingUnit, l_pMenuProcessingUnitDefault, GTK_STOCK_HOME, "default", NULL);
									gtk_menu_add_separator_menu_item(l_pMenuProcessingUnit);
									gtk_menu_add_new_image_menu_item(l_pMenuProcessingUnit, l_pMenuProcessingUnitAdd, GTK_STOCK_ADD, "add...", NULL);
									gtk_menu_item_set_submenu(GTK_MENU_ITEM(l_pMenuItemProcessUnit), GTK_WIDGET(l_pMenuProcessingUnit));

									// -------------- ABOUT --------------

									gtk_menu_add_new_image_menu_item(l_pMenu, l_pMenuItemAbout, GTK_STOCK_ABOUT, "about", NULL);

									// -------------- RUN --------------

									#undef gtk_menu_add_separator_menu_item
									#undef gtk_menu_add_new_image_menu_item

									gtk_widget_show_all(GTK_WIDGET(l_pMenu));
									gtk_menu_popup(l_pMenu, NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time());
								}
							}
						}
						break;
					default:
						break;
				}
#endif
				break;

			default:
				break;
		}

		this->redraw();
	}
	void CInterfacedScenario::scenarioDrawingAreaButtonReleasedCB(::GtkWidget* pWidget, ::GdkEventButton* pEvent)
	{
		m_rKernel.getContext()->getLogManager() << LogLevel_Debug << "scenarioDrawingAreaButtonReleasedCB\n";

		if(this->isLocked()) return;

		m_bButtonPressed&=!((pEvent->type==GDK_BUTTON_RELEASE)&&(pEvent->button==1));
		m_f64ReleaseMouseX=pEvent->x;
		m_f64ReleaseMouseY=pEvent->y;

		if(m_ui32CurrentMode!=Mode_None)
		{
			if(m_ui32CurrentMode==Mode_Selection || m_ui32CurrentMode==Mode_SelectionAdd)
			{
				if(m_ui32CurrentMode==Mode_Selection)
				{
					m_vCurrentObject.clear();
				}
				int l_iStartX=(int)min(m_f64PressMouseX, m_f64CurrentMouseX);
				int l_iStartY=(int)min(m_f64PressMouseY, m_f64CurrentMouseY);
				int l_iSizeX=(int)max(m_f64PressMouseX-m_f64CurrentMouseX, m_f64CurrentMouseX-m_f64PressMouseX);
				int l_iSizeY=(int)max(m_f64PressMouseY-m_f64CurrentMouseY, m_f64CurrentMouseY-m_f64PressMouseY);
				pickInterfacedObject(l_iStartX, l_iStartY, l_iSizeX, l_iSizeY);
			}
			if(m_ui32CurrentMode==Mode_Connect)
			{
				uint32 l_ui32InterfacedObjectId=pickInterfacedObject((int)m_f64ReleaseMouseX, (int)m_f64ReleaseMouseY);
				CInterfacedObject l_oCurrentObject=m_vInterfacedObject[l_ui32InterfacedObjectId];
				if(l_oCurrentObject.m_ui32ConnectorType==Connector_Output && m_oCurrentObject.m_ui32ConnectorType==Connector_Input)
				{
					CIdentifier l_oLinkIdentifier;
					m_rScenario.connect(
						l_oCurrentObject.m_oIdentifier,
						l_oCurrentObject.m_ui32ConnectorIndex,
						m_oCurrentObject.m_oIdentifier,
						m_oCurrentObject.m_ui32ConnectorIndex,
						l_oLinkIdentifier);
				}
				if(l_oCurrentObject.m_ui32ConnectorType==Connector_Input && m_oCurrentObject.m_ui32ConnectorType==Connector_Output)
				{
					CIdentifier l_oLinkIdentifier;
					m_rScenario.connect(
						m_oCurrentObject.m_oIdentifier,
						m_oCurrentObject.m_ui32ConnectorIndex,
						l_oCurrentObject.m_oIdentifier,
						l_oCurrentObject.m_ui32ConnectorIndex,
						l_oLinkIdentifier);
				}
			}
			if(m_ui32CurrentMode==Mode_MoveSelection)
			{
				map<CIdentifier, boolean>::const_iterator i;
				for(i=m_vCurrentObject.begin(); i!=m_vCurrentObject.end(); i++)
				{
					if(i->second && m_rScenario.isBox(i->first))
					{
						CBoxProxy l_oBoxProxy(m_rScenario, i->first);
						l_oBoxProxy.setCenter(
							((l_oBoxProxy.getXCenter()+8)&0xfffffff0),
							((l_oBoxProxy.getYCenter()+8)&0xfffffff0));
					}
				}
			}

			this->redraw();
		}

		m_ui32CurrentMode=Mode_None;
	}
	void CInterfacedScenario::scenarioDrawingAreaKeyPressEventCB(::GtkWidget* pWidget, ::GdkEventKey* pEvent)
	{
		m_bShiftPressed  |=(pEvent->keyval==GDK_Shift_L   || pEvent->keyval==GDK_Shift_R);
		m_bControlPressed|=(pEvent->keyval==GDK_Control_L || pEvent->keyval==GDK_Control_R);
		m_bAltPressed    |=(pEvent->keyval==GDK_Alt_L     || pEvent->keyval==GDK_Alt_R);

		m_rKernel.getContext()->getLogManager() << LogLevel_Debug
			<< "scenarioDrawingAreaKeyPressEventCB ("
			<< (m_bShiftPressed?"true":"false") << "|"
			<< (m_bControlPressed?"true":"false") << "|"
			<< (m_bAltPressed?"true":"false") << "|"
			<< ")\n";

		if(this->isLocked()) return;

		if(pEvent->keyval==GDK_Delete || pEvent->keyval==GDK_KP_Delete)
		{
			map<CIdentifier, boolean>::const_iterator i;
			for(i=m_vCurrentObject.begin(); i!=m_vCurrentObject.end(); i++)
			{
				if(i->second)
				{
					if(m_rScenario.isBox(i->first))
					{
						//remove visualisation box from window manager
						m_pDesignerVisualisation->onVisualisationBoxRemoved(i->first);

						//remove box from scenario
						m_rScenario.removeBox(i->first);
					}
					else
					{
						m_rScenario.disconnect(i->first);
					}
				}
			}
			m_vCurrentObject.clear();

			this->redraw();
		}
	}
	void CInterfacedScenario::scenarioDrawingAreaKeyReleaseEventCB(::GtkWidget* pWidget, ::GdkEventKey* pEvent)
	{
		m_bShiftPressed  &=!(pEvent->keyval==GDK_Shift_L   || pEvent->keyval==GDK_Shift_R);
		m_bControlPressed&=!(pEvent->keyval==GDK_Control_L || pEvent->keyval==GDK_Control_R);
		m_bAltPressed    &=!(pEvent->keyval==GDK_Alt_L     || pEvent->keyval==GDK_Alt_R);

		m_rKernel.getContext()->getLogManager() << LogLevel_Debug
			<< "scenarioDrawingAreaKeyReleaseEventCB ("
			<< (m_bShiftPressed?"true":"false") << "|"
			<< (m_bControlPressed?"true":"false") << "|"
			<< (m_bAltPressed?"true":"false") << "|"
			<< ")\n";

		if(this->isLocked()) return;

		// ...
	}

	void CInterfacedScenario::toggleDesignerVisualisation()
	{
		m_bDesignerVisualisationToggled = !m_bDesignerVisualisationToggled;
		
		if(m_bDesignerVisualisationToggled)
		{
			m_pDesignerVisualisation->show();			
		}
		else
		{
			m_pDesignerVisualisation->hide();			
		}
	}

	OpenViBE::boolean CInterfacedScenario::isDesignerVisualisationToggled()
	{
		return m_bDesignerVisualisationToggled;
	}

	void CInterfacedScenario::showCurrentVisualisation()
	{
		if(isLocked())
		{
			if(m_pPlayerVisualisation != NULL)
			{
				m_pPlayerVisualisation->showTopLevelWindows();
			}
		}
		else
		{
			if(m_pDesignerVisualisation != NULL)
			{
				m_pDesignerVisualisation->show();
			}
		}
	}

	void CInterfacedScenario::hideCurrentVisualisation()
	{
		if(isLocked())
		{
			if(m_pPlayerVisualisation != NULL)
			{
				m_pPlayerVisualisation->hideTopLevelWindows();
			}
		}
		else
		{
			if(m_pDesignerVisualisation != NULL)
			{
				m_pDesignerVisualisation->hide();
			}
		}
		
	}

	void CInterfacedScenario::createPlayerVisualisation()
	{
		//hide window manager
		m_pDesignerVisualisation->hide();

		if(m_pPlayerVisualisation == NULL)
			m_pPlayerVisualisation = new CPlayerVisualisation(*m_rKernel.getContext(), m_rScenario, *m_pVisualisationTree);

		//initialize and show windows
		m_pPlayerVisualisation->init();
		m_pPlayerVisualisation->showTopLevelWindows();
	}

	void CInterfacedScenario::releasePlayerVisualisation(void)
	{
		if(m_pPlayerVisualisation != NULL)
		{
			m_pPlayerVisualisation->hideTopLevelWindows();
			//m_pPlayerVisualisation->release();
			delete m_pPlayerVisualisation;
			m_pPlayerVisualisation = NULL;
		}

		//reset designer visualisation
		m_pDesignerVisualisation->reset();

		//show it if it was toggled
		if(m_bDesignerVisualisationToggled == true)
			m_pDesignerVisualisation->show();
	}

	void CInterfacedScenario::generateDisplayPluginName(IBox* pDisplayBox, CString& rDisplayBoxName)
	{
		rDisplayBoxName = pDisplayBox->getName();
		char buf[10];
		int num = 2;

		CIdentifier l_oBoxIdentifier = m_rScenario.getNextBoxIdentifier(OV_UndefinedIdentifier);

		//for all boxes contained in scenario
		while(l_oBoxIdentifier!=OV_UndefinedIdentifier)
		{
			const IBox* l_pBox2=m_rScenario.getBoxDetails(l_oBoxIdentifier);

			if(l_pBox2 != NULL && l_pBox2 != pDisplayBox)
			{
				//a box already has the same name
				if(l_pBox2->getName() == rDisplayBoxName)
				{
					//generate a new name and ensure it is not used yet
					sprintf(buf, " %d", num++);
					rDisplayBoxName = pDisplayBox->getName() + CString(buf);
					l_oBoxIdentifier = OV_UndefinedIdentifier;
				}
			}
			l_oBoxIdentifier = m_rScenario.getNextBoxIdentifier(l_oBoxIdentifier);
		}
	}
