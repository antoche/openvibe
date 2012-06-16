#include "ovpCTopographicMap2DView.h"

#include <iostream>

#include <sstream>

#include <cmath>
#include <cstring>

#ifdef OVP_OS_Windows
#ifndef NDEBUG
#include <cassert>
#endif
#endif

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace OpenViBEToolkit;

using namespace std;

//#define INTERPOLATE_AT_CHANNEL_LOCATION

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		static const int s_nbColors = 13;
		static GdkColor s_palette[13];
		static uint8 s_palette8[13*3];

		static gboolean redrawCallback(::GtkWidget* pWidget, ::GdkEventExpose* pEvent, gpointer data);
		static gboolean resizeCallback(::GtkWidget* pWidget, ::GtkAllocation* pAllocation, gpointer data);
		static void toggleElectrodesCallback(::GtkWidget* pWidget, gpointer data);
		static void setProjectionCallback(::GtkWidget* pWidget, gpointer data);
		static void setViewCallback(::GtkWidget* pWidget, gpointer data);
		static void setInterpolationCallback(::GtkWidget* pWidget, gpointer data);
		static void setDelayCallback(::GtkRange *range, gpointer data);

		CTopographicMap2DView::CTopographicMap2DView(CTopographicMapDatabase& rTopographicMapDatabase,
			uint64 ui64DefaultInterpolation, float64 f64Delay)
			:m_rTopographicMapDatabase(rTopographicMapDatabase)
			,m_f64MaxDelay(2.0) //maximum delay : 2s
			,m_pBuilderInterface(NULL)
			,m_pDrawingArea(NULL)
			,m_pClipmask(NULL)
			,m_ui32ClipmaskWidth(0)
			,m_ui32ClipmaskHeight(0)
			,m_pClipmaskGC(NULL)
			,m_pVisibleRegion(NULL)
			,m_ui32CurrentProjection(TopographicMap2DProjection_Radial)
			,m_pAxialProjectionButton(NULL)
			,m_pRadialProjectionButton(NULL)
			,m_ui32CurrentView(TopographicMap2DView_Top)
			,m_pTopViewButton(NULL)
			,m_pLeftViewButton(NULL)
			,m_pRightViewButton(NULL)
			,m_pBackViewButton(NULL)
			,m_ui64CurrentInterpolation(ui64DefaultInterpolation)
			,m_pMapPotentials(NULL)
			,m_pMapCurrents(NULL)
			,m_pElectrodesToggleButton(NULL)
			,m_bElectrodesToggledOn(true)
			,m_bNeedResize(true)
			,m_ui32GridSize(0)
			,m_ui32CellSize(0)
			,m_ui32MinPaletteBarHeight(10)
			,m_ui32MaxPaletteBarHeight(30)
			,m_ui32HeadWindowWidth(0)
			,m_ui32HeadWindowHeight(0)
			,m_ui32PaletteWindowWidth(0)
			,m_ui32PaletteWindowHeight(0)
			,m_ui32SkullX(0)
			,m_ui32SkullY(0)
			,m_ui32SkullDiameter(0)
			,m_ui32NoseY(0)
			,m_pSkullRGBBuffer(NULL)
			,m_ui32RowStride(0)
		{
			m_oSampleCoordinatesMatrix.setDimensionCount(2);

			//load the gtk builder interface
			m_pBuilderInterface=gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-TopographicMap2D.ui", NULL, NULL);
			gtk_builder_add_from_file(m_pBuilderInterface, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-TopographicMap2D.ui", NULL);

			if(!m_pBuilderInterface)
			{
				g_warning("Couldn't load the interface!");
				return;
			}

			gtk_builder_connect_signals(m_pBuilderInterface, NULL);

			m_oBackgroundColor.pixel = 0;
			m_oBackgroundColor.red = 0xFFFF;
			m_oBackgroundColor.green = 0;//0xFFFF;
			m_oBackgroundColor.blue = 0;//0xFFFF;

			//toolbar
			//-------

			//get pointers to projection mode buttons
			m_pAxialProjectionButton = GTK_RADIO_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "AxialProjection"));
			m_pRadialProjectionButton = GTK_RADIO_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "RadialProjection"));

			g_signal_connect(G_OBJECT(m_pAxialProjectionButton), "toggled", G_CALLBACK (setProjectionCallback), this);
			g_signal_connect(G_OBJECT(m_pRadialProjectionButton), "toggled", G_CALLBACK (setProjectionCallback), this);

			//get pointers to view buttons
			m_pTopViewButton = GTK_RADIO_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "TopView"));
			m_pLeftViewButton = GTK_RADIO_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "LeftView"));
			m_pRightViewButton = GTK_RADIO_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "RightView"));
			m_pBackViewButton = GTK_RADIO_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "BackView"));

			g_signal_connect(G_OBJECT(m_pTopViewButton), "toggled", G_CALLBACK (setViewCallback), this);
			g_signal_connect(G_OBJECT(m_pLeftViewButton), "toggled", G_CALLBACK (setViewCallback), this);
			g_signal_connect(G_OBJECT(m_pRightViewButton), "toggled", G_CALLBACK (setViewCallback), this);
			g_signal_connect(G_OBJECT(m_pBackViewButton), "toggled", G_CALLBACK (setViewCallback), this);

			//get pointers to interpolation type buttons
			m_pMapPotentials = GTK_RADIO_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "MapPotentials"));
			m_pMapCurrents = GTK_RADIO_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "MapCurrents"));

			g_signal_connect(G_OBJECT(m_pMapPotentials), "toggled", G_CALLBACK (setInterpolationCallback), this);
			g_signal_connect(G_OBJECT(m_pMapCurrents), "toggled", G_CALLBACK (setInterpolationCallback), this);

			//get pointer to electrodes toggle button
			m_pElectrodesToggleButton = GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "ToggleElectrodes"));

			g_signal_connect(G_OBJECT(m_pElectrodesToggleButton), "toggled", G_CALLBACK(toggleElectrodesCallback), this);

			//tell database about maximum delay
			m_rTopographicMapDatabase.adjustNumberOfDisplayedBuffers(m_f64MaxDelay);
			//ensure default delay lies in [0, m_f64MaxDelay]
			if(f64Delay > m_f64MaxDelay)
			{
				f64Delay = m_f64MaxDelay;
			}
			//set default delay
			setDelayCB(f64Delay);
			//configure delay slider
			GtkWidget* l_pDelayScale = gtk_hscale_new_with_range(0.0, m_f64MaxDelay, 0.1);
			gtk_range_set_value(GTK_RANGE(l_pDelayScale), f64Delay);
			gtk_scale_set_value_pos(GTK_SCALE(l_pDelayScale), GTK_POS_TOP);
			gtk_range_set_update_policy(GTK_RANGE(l_pDelayScale), GTK_UPDATE_CONTINUOUS);
			gtk_widget_set_size_request(l_pDelayScale, 100, -1);
			gtk_widget_show_all(l_pDelayScale);
			g_signal_connect(G_OBJECT(l_pDelayScale), "value_changed", G_CALLBACK(setDelayCallback), this);

			//replace existing scale (which somehow can't be used) with the newly created one
			GtkWidget* l_pOldScale = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "DelayScale"));
			GtkWidget* l_pScaleParent = gtk_widget_get_parent(l_pOldScale);
			if(l_pScaleParent != NULL && GTK_IS_CONTAINER(l_pScaleParent))
			{
				gtk_container_remove(GTK_CONTAINER(l_pScaleParent), l_pOldScale);
				if(GTK_IS_BOX(l_pScaleParent))
				{
					gtk_box_pack_start(GTK_BOX(l_pScaleParent), l_pDelayScale, TRUE, TRUE, 0);
					gtk_box_reorder_child(GTK_BOX(l_pScaleParent), l_pDelayScale, 0);
				}
			}

			//color palettes
			s_palette[0].red = 255*65535/255; s_palette[0].green = 0*65535/255; s_palette[0].blue = 0*65535/255;
			s_palette[1].red = 234*65535/255; s_palette[1].green = 1*65535/255; s_palette[1].blue = 0*65535/255;
			s_palette[2].red = 205*65535/255; s_palette[2].green = 0*65535/255; s_palette[2].blue = 101*65535/255;
			s_palette[3].red = 153*65535/255; s_palette[3].green = 0*65535/255; s_palette[3].blue = 178*65535/255;
			s_palette[4].red = 115*65535/255; s_palette[4].green = 1*65535/255; s_palette[4].blue = 177*65535/255;
			s_palette[5].red = 77*65535/255; s_palette[5].green = 0*65535/255; s_palette[5].blue = 178*65535/255;
			s_palette[6].red = 0*65535/255; s_palette[6].green = 0*65535/255; s_palette[6].blue = 152*65535/255;
			s_palette[7].red = 0*65535/255; s_palette[7].green = 97*65535/255; s_palette[7].blue = 121*65535/255;
			s_palette[8].red = 0*65535/255; s_palette[8].green = 164*65535/255; s_palette[8].blue = 100*65535/255;
			s_palette[9].red = 0*65535/255; s_palette[9].green = 225*65535/255; s_palette[9].blue = 25*65535/255;
			s_palette[10].red = 150*65535/255; s_palette[10].green = 255*65535/255; s_palette[10].blue = 0*65535/255;
			s_palette[11].red = 200*65535/255; s_palette[11].green = 255*65535/255; s_palette[11].blue = 0*65535/255;
			s_palette[12].red = 255*65535/255; s_palette[12].green = 255*65535/255; s_palette[12].blue = 0*65535/255;

			s_palette8[0] = 255; s_palette8[1] = 0; s_palette8[2] = 0;
			s_palette8[3] = 234; s_palette8[4] = 1; s_palette8[5] = 0;
			s_palette8[6] = 205; s_palette8[7] = 0; s_palette8[8] = 101;
			s_palette8[9] = 153; s_palette8[10] = 0; s_palette8[11] = 178;
			s_palette8[12] = 115; s_palette8[13] = 1; s_palette8[14] = 177;
			s_palette8[15] = 77; s_palette8[16] = 0; s_palette8[17] = 178;
			s_palette8[18] = 0; s_palette8[19] = 0; s_palette8[20] = 152;
			s_palette8[21] = 0; s_palette8[22] = 97; s_palette8[23] = 121;
			s_palette8[24] = 0; s_palette8[25] = 164; s_palette8[26] = 100;
			s_palette8[27] = 0; s_palette8[28] = 225; s_palette8[29] = 25;
			s_palette8[30] = 150; s_palette8[31] = 255; s_palette8[32] = 0;
			s_palette8[33] = 200; s_palette8[34] = 255; s_palette8[35] = 0;
			s_palette8[36] = 255; s_palette8[37] = 255; s_palette8[38] = 0;
		}

		CTopographicMap2DView::~CTopographicMap2DView()
		{
			//destroy clip mask
			if(m_pClipmask)
			{
				g_object_unref(m_pClipmask);
				m_pClipmask = NULL;
			}
			if(m_pClipmaskGC)
			{
				g_object_unref(m_pClipmaskGC);
				m_pClipmaskGC=NULL;
			}
			//destroy visible region
			if(m_pVisibleRegion)
			{
				gdk_region_destroy(m_pVisibleRegion);
				m_pVisibleRegion = NULL;
			}
			//destroy pixmap
			if(m_pSkullRGBBuffer)
			{
				delete m_pSkullRGBBuffer;
				m_pSkullRGBBuffer = NULL;
			}

			//unref the xml file as it's not needed anymore
			g_object_unref(G_OBJECT(m_pBuilderInterface));
			m_pBuilderInterface=NULL;
		}

		void CTopographicMap2DView::init()
		{
			m_pDrawingArea = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "TopographicMap2DDrawingArea"));

			gtk_widget_set_double_buffered(m_pDrawingArea, TRUE);

			g_signal_connect(G_OBJECT(m_pDrawingArea), "expose_event", G_CALLBACK(redrawCallback), this);
			g_signal_connect(G_OBJECT(m_pDrawingArea), "size-allocate", G_CALLBACK(resizeCallback), this);

			gtk_widget_show(m_pDrawingArea);

			//set radial projection by default
			m_ui32CurrentProjection = TopographicMap2DProjection_Radial;
			enableProjectionButtonSignals(false);
			gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(m_pRadialProjectionButton), TRUE);
			enableProjectionButtonSignals(true);

			//set top view by default
			m_ui32CurrentView = TopographicMap2DView_Top;
			enableViewButtonSignals(false);
			gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(m_pTopViewButton), TRUE);
			enableViewButtonSignals(true);

			//reflect default interpolation type
			m_rTopographicMapDatabase.setInterpolationType(m_ui64CurrentInterpolation);
			enableInterpolationButtonSignals(false);
			gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(m_pMapPotentials), m_ui64CurrentInterpolation == OVP_TypeId_SphericalLinearInterpolationType_Spline);
			gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(m_pMapCurrents), m_ui64CurrentInterpolation == OVP_TypeId_SphericalLinearInterpolationType_Laplacian);
			enableInterpolationButtonSignals(true);

			//hide electrodes by default
			m_bElectrodesToggledOn = false;
			enableElectrodeButtonSignals(false);
			gtk_toggle_tool_button_set_active(m_pElectrodesToggleButton, m_bElectrodesToggledOn);
			enableElectrodeButtonSignals(true);

			//recompute sample points coordinates
			m_bNeedResize = true;
		}

		void CTopographicMap2DView::redraw()
		{
			if(m_pDrawingArea != NULL && GTK_WIDGET_VISIBLE(m_pDrawingArea))
			{
				if(m_bNeedResize == true)
				{
					resizeData();
				}

				//draw face
				drawFace(0, 0, m_ui32HeadWindowWidth, m_ui32HeadWindowHeight);

				//draw head
				drawHead();

				//draw palette
				drawPalette(0, m_ui32HeadWindowHeight, m_ui32PaletteWindowWidth, m_ui32PaletteWindowHeight);

				//don't clear screen at every redraw, it introduces major flickering
				//gdk_window_invalidate_rect(m_pDrawingArea->window, NULL, true);
			}
		}

		void CTopographicMap2DView::getWidgets(GtkWidget*& pWidget, GtkWidget*& pToolbarWidget)
		{
			pWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "TopographicMap2DDrawingArea"));
			pToolbarWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "Toolbar"));
		}

		CTopographicMap2DView::ETopographicMap2DView CTopographicMap2DView::getCurrentView()
		{
			return m_ui32CurrentView;
		}

		CMatrix* CTopographicMap2DView::getSampleCoordinatesMatrix()
		{
			if(m_bNeedResize == true)
			{
				resizeData();
			}

			return &m_oSampleCoordinatesMatrix;
		}

		boolean CTopographicMap2DView::setSampleValuesMatrix(IMatrix* pSampleValuesMatrix)
		{
			//ensure matrix has the right size
			if(pSampleValuesMatrix == NULL || pSampleValuesMatrix->getBufferElementCount() < m_oSampleValues.size())
			{
				return false;
			}

			//retrieve min/max potentials
			float64 l_f64MinPotential, l_f64MaxPotential;
			m_rTopographicMapDatabase.getLastBufferInterpolatedMinMaxValue(l_f64MinPotential, l_f64MaxPotential);

			int32 l_colorStartIndex = 0;
			int32 l_colorEndIndex = s_nbColors-1;

			float64 l_f64InvPotentialStep = 0;

			if(l_f64MinPotential < l_f64MaxPotential)
			{
				l_f64InvPotentialStep = (l_colorEndIndex-l_colorStartIndex+1) / (l_f64MaxPotential - l_f64MinPotential);
			}

			//determine color index of each sample
			for(uint32 i=0; i<m_oSampleValues.size(); i++)
			{
				float64 l_f64Value = *(pSampleValuesMatrix->getBuffer() + i);
				int32 l_iColorIndex;

				if(l_f64Value < l_f64MinPotential)
				{
					l_iColorIndex = 0;
				}
				else if(l_f64Value > l_f64MaxPotential)
				{
					l_iColorIndex = s_nbColors-1;
				}
				else //linear itp
				{
					l_iColorIndex = l_colorStartIndex + (int32)((l_f64Value - l_f64MinPotential) * l_f64InvPotentialStep);
					if(l_iColorIndex > s_nbColors-1)
					{
						l_iColorIndex = s_nbColors-1;
					}
				}
				m_oSampleValues[i] = l_iColorIndex;
			}

			refreshPotentials();

			//force redraw

			return true;
		}

		void CTopographicMap2DView::resizeCB(uint32 ui32Width, uint32 ui32Height)
		{
			m_bNeedResize = true;
		}

		void CTopographicMap2DView::toggleElectrodesCB()
		{
			m_bElectrodesToggledOn = !m_bElectrodesToggledOn;

			if(m_bElectrodesToggledOn == false)
			{
				//clear screen so that electrode labels are hidden
				if(m_pDrawingArea->window) gdk_window_invalidate_rect(m_pDrawingArea->window, NULL, true);
			}
		}

		void CTopographicMap2DView::setProjectionCB(GtkWidget* pWidget)
		{
			if(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(pWidget)) == FALSE)
			{
				return;
			}

			if(pWidget == GTK_WIDGET(m_pAxialProjectionButton))
			{
				m_ui32CurrentProjection = TopographicMap2DProjection_Axial;
			}
			else if(pWidget == GTK_WIDGET(m_pRadialProjectionButton))
			{
				m_ui32CurrentProjection = TopographicMap2DProjection_Radial;
			}

			//recompute sample points coordinates
			m_bNeedResize = true;

			//clear screen
			if(m_pDrawingArea->window) gdk_window_invalidate_rect(m_pDrawingArea->window, NULL, true);
		}

		void CTopographicMap2DView::setViewCB(GtkWidget* pWidget)
		{
			if(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(pWidget)) == FALSE)
			{
				return;
			}

			if(pWidget == GTK_WIDGET(m_pTopViewButton))
			{
				m_ui32CurrentView = TopographicMap2DView_Top;
			}
			else if(pWidget == GTK_WIDGET(m_pLeftViewButton))
			{
				m_ui32CurrentView = TopographicMap2DView_Left;
			}
			else if(pWidget == GTK_WIDGET(m_pRightViewButton))
			{
				m_ui32CurrentView = TopographicMap2DView_Right;
			}
			else if(pWidget == GTK_WIDGET(m_pBackViewButton))
			{
				m_ui32CurrentView = TopographicMap2DView_Back;
			}

			//recompute sample points coordinates, update clipmask
			m_bNeedResize = true;

			//clear screen
			if(m_pDrawingArea->window) gdk_window_invalidate_rect(m_pDrawingArea->window, NULL, true);
		}

		void CTopographicMap2DView::setInterpolationCB(GtkWidget* pWidget)
		{
			if(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(pWidget)) == FALSE)
			{
				return;
			}

			if(pWidget == GTK_WIDGET(m_pMapPotentials))
			{
				m_ui64CurrentInterpolation = OVP_TypeId_SphericalLinearInterpolationType_Spline;
				m_rTopographicMapDatabase.setInterpolationType(OVP_TypeId_SphericalLinearInterpolationType_Spline);
			}
			else if(pWidget == GTK_WIDGET(m_pMapCurrents))
			{
				m_ui64CurrentInterpolation = OVP_TypeId_SphericalLinearInterpolationType_Laplacian;
				m_rTopographicMapDatabase.setInterpolationType(OVP_TypeId_SphericalLinearInterpolationType_Laplacian);
			}

			//recompute sample points coordinates
			m_bNeedResize = true;
		}

		void CTopographicMap2DView::setDelayCB(float64 f64Value)
		{
			m_rTopographicMapDatabase.setDelay(f64Value);
		}

		void CTopographicMap2DView::drawPalette(uint32 ui32X, uint32 ui32Y, uint32 ui32Width, uint32 ui32Height)
		{
			if(ui32Width == 0 || ui32Height == 0)
			{
				return;
			}

			boolean l_bDrawText = true;

			//retrieve text size
			PangoLayout* l_pText = gtk_widget_create_pango_layout(GTK_WIDGET(m_pDrawingArea), "0");
			gint l_iTextHeight;
			pango_layout_get_pixel_size(l_pText, NULL, &l_iTextHeight);

			//don't draw text if not enough room
			if(l_iTextHeight >= (gint)(ui32Height - m_ui32MinPaletteBarHeight))
			{
				l_bDrawText = false;
			}
			//determine palette bar dims
			gint l_iPaletteBarWidth = (gint)(0.9 * ui32Width);
			gint l_iPaletteBarHeight = (gint)(ui32Height - l_iTextHeight);
			if(l_iPaletteBarHeight < (gint)(m_ui32MinPaletteBarHeight))
			{
				l_iPaletteBarHeight = (gint)m_ui32MinPaletteBarHeight;
			}
			else if(l_iPaletteBarHeight > (gint)(m_ui32MaxPaletteBarHeight))
			{
				l_iPaletteBarHeight = (gint)(m_ui32MaxPaletteBarHeight);
			}
			gint l_iPaletteBarStartX = (gint)(ui32X + (ui32Width - l_iPaletteBarWidth)/2);
			gint l_iPaletteBarStartY = (gint)(ui32Y);

			gint l_iTextWidth;
			gint l_iLabelX;
			gint l_iLabelY = l_iPaletteBarStartY + l_iPaletteBarHeight + (ui32Height - l_iPaletteBarHeight - l_iTextHeight)/2;

			//draw 0 label
			pango_layout_get_pixel_size(l_pText, &l_iTextWidth, NULL);
			l_iLabelX = ui32X + (ui32Width - l_iTextWidth)/2;

			gdk_draw_layout(m_pDrawingArea->window, m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
				l_iLabelX,
				l_iLabelY,
				l_pText);

			//draw + label
			pango_layout_set_text(l_pText, "+", 1);
			pango_layout_get_pixel_size(l_pText, &l_iTextWidth, NULL);
			l_iLabelX = l_iPaletteBarStartX - l_iTextWidth/2;
			if(l_iLabelX < 0)
			{
				l_iLabelX = 0;
			}
			gdk_draw_layout(m_pDrawingArea->window, m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
				l_iLabelX,
				l_iLabelY,
				l_pText);

			//draw - label
			pango_layout_set_text(l_pText, "-", 1);
			pango_layout_get_pixel_size(l_pText, &l_iTextWidth, NULL);
			l_iLabelX = l_iPaletteBarStartX + l_iPaletteBarWidth - l_iTextWidth/2;
			if(l_iLabelX + l_iTextWidth >= (gint)ui32Width)
			{
				l_iLabelX = ui32Width - l_iTextWidth;
			}
			gdk_draw_layout(m_pDrawingArea->window, m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
				l_iLabelX,
				l_iLabelY,
				l_pText);

			//draw palette bar (typically reversed : high potentials to the left; low potentials to the right)
			gint l_iCurrentX = l_iPaletteBarStartX;

			for(int i=s_nbColors-1; i>=0; i--)
			{
				gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
					&s_palette[i]);

				gdk_draw_rectangle(m_pDrawingArea->window,
					m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
					TRUE,
					l_iCurrentX,
					l_iPaletteBarStartY,
					l_iPaletteBarWidth/s_nbColors,
					l_iPaletteBarHeight);

				l_iCurrentX += l_iPaletteBarWidth/13;
			}

			//restore default black color
			GdkColor l_oBlack;
			l_oBlack.red = l_oBlack.green = l_oBlack.blue = 0;
			gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], &l_oBlack);

			//delete pango layout
			g_object_unref(l_pText);
		}

		void CTopographicMap2DView::drawFace(uint32 ui32X, uint32 ui32Y, uint32 ui32Width, uint32 ui32Height)
		{
			gdk_gc_set_line_attributes(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)], 2, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_BEVEL);

			//head center
			uint32 l_ui32SkullCenterX = m_ui32SkullX + m_ui32SkullDiameter/2;
			uint32 l_ui32SkullCenterY = m_ui32SkullY + m_ui32SkullDiameter/2;

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif
#define DEG2RAD(x) (x)*M_PI/180

			if(m_ui32CurrentView == TopographicMap2DView_Top)
			{
				const float32 l_f32NoseHalfAngle = 6;

				//nose lower left anchor
				uint32 l_ui32NoseLowerLeftX = (uint32)(l_ui32SkullCenterX + m_ui32SkullDiameter/2*cos(DEG2RAD(90+l_f32NoseHalfAngle)));
				uint32 l_ui32NoseLowerLeftY = (uint32)(l_ui32SkullCenterY - m_ui32SkullDiameter/2*sin(DEG2RAD(90+l_f32NoseHalfAngle)));

				//nose lower right anchor
				uint32 l_ui32NoseLowerRightX = (uint32)(l_ui32SkullCenterX + m_ui32SkullDiameter/2*cos(DEG2RAD(90-l_f32NoseHalfAngle)));
				uint32 l_ui32NoseLowerRightY = (uint32)(l_ui32SkullCenterY - m_ui32SkullDiameter/2*sin(DEG2RAD(90-l_f32NoseHalfAngle)));

				gdk_draw_line(m_pDrawingArea->window,
					m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
					(gint)l_ui32NoseLowerLeftX,
					(gint)l_ui32NoseLowerLeftY,
					(gint)l_ui32SkullCenterX,
					(gint)m_ui32NoseY);

				gdk_draw_line(m_pDrawingArea->window,
					m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
					(gint)l_ui32NoseLowerRightX,
					(gint)l_ui32NoseLowerRightY,
					(gint)l_ui32SkullCenterX,
					(gint)m_ui32NoseY);
			}
			else if(m_ui32CurrentView == TopographicMap2DView_Back)
			{
				gdk_draw_line(m_pDrawingArea->window,
					m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
					(gint)m_ui32SkullOutlineLeftPointX,
					(gint)m_ui32SkullOutlineLeftPointY,
					(gint)m_ui32LeftNeckX,
					(gint)m_ui32LeftNeckY);

				gdk_draw_line(m_pDrawingArea->window,
					m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
					(gint)m_ui32SkullOutlineRightPointX,
					(gint)m_ui32SkullOutlineRightPointY,
					(gint)m_ui32RightNeckX,
					(gint)m_ui32RightNeckY);
			}
			else if(m_ui32CurrentView == TopographicMap2DView_Left ||
				m_ui32CurrentView == TopographicMap2DView_Right)
			{
				gdk_draw_line(m_pDrawingArea->window,
					m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
					(gint)m_ui32NoseTopX,
					(gint)m_ui32NoseTopY,
					(gint)m_ui32NoseBumpX,
					(gint)m_ui32NoseBumpY);

				gdk_draw_line(m_pDrawingArea->window,
					m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
					(gint)m_ui32NoseBumpX,
					(gint)m_ui32NoseBumpY,
					(gint)m_ui32NoseTipX,
					(gint)m_ui32NoseTipY);

				gdk_draw_line(m_pDrawingArea->window,
					m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
					(gint)m_ui32NoseTipX,
					(gint)m_ui32NoseTipY,
					(gint)m_ui32NoseBaseX,
					(gint)m_ui32NoseBaseY);

				gdk_draw_line(m_pDrawingArea->window,
					m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
					(gint)m_ui32NoseBaseX,
					(gint)m_ui32NoseBaseY,
					(gint)m_ui32NoseBottomX,
					(gint)m_ui32NoseBottomY);
			}
		}

		void CTopographicMap2DView::drawHead()
		{
			//draw head outline
			gdk_gc_set_line_attributes(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)], 2, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_BEVEL);
			gdk_draw_arc(m_pDrawingArea->window,
				m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
				FALSE,
				(gint)m_ui32SkullX,
				(gint)m_ui32SkullY,
				(gint)m_ui32SkullDiameter,
				(gint)m_ui32SkullDiameter,
				(gint)(64 * m_f32SkullOutlineStartAngle),
				(gint)(64 * (m_f32SkullOutlineEndAngle-m_f32SkullOutlineStartAngle)));

			gdk_gc_set_line_attributes(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)], 1, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_BEVEL);

			gdk_gc_set_clip_origin(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], (gint)m_ui32SkullX, (gint)m_ui32SkullY);

			gdk_gc_set_clip_mask(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], m_pClipmask);

			drawPotentials();

			drawElectrodes();

			gdk_gc_set_clip_mask(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], NULL);
		}

		void CTopographicMap2DView::resizeData()
		{
			//window size
			uint32 l_iWindowWidth = m_pDrawingArea->allocation.width;
			uint32 l_iWindowHeight = m_pDrawingArea->allocation.height;

			//retrieve text size
			PangoLayout* l_pText = gtk_widget_create_pango_layout(GTK_WIDGET(m_pDrawingArea), "0");
			int l_iTextHeight;
			pango_layout_get_pixel_size(l_pText, NULL, &l_iTextHeight);
			g_object_unref(l_pText);
			l_pText = NULL;

			//palette sub-window dims
			m_ui32PaletteWindowWidth = l_iWindowWidth;
			m_ui32PaletteWindowHeight = (uint32)(0.1 * l_iWindowHeight);
			if(m_ui32PaletteWindowHeight > (uint32)(m_ui32MaxPaletteBarHeight + l_iTextHeight + 4))
			{
				m_ui32PaletteWindowHeight = m_ui32MaxPaletteBarHeight + l_iTextHeight + 4;
			}
			else if(m_ui32PaletteWindowHeight < (uint32)(m_ui32MinPaletteBarHeight + l_iTextHeight))
			{
				m_ui32PaletteWindowHeight = (uint32)(m_ui32MinPaletteBarHeight + l_iTextHeight);
			}

			//return if not enough room available
			if(m_ui32PaletteWindowHeight > l_iWindowHeight)
			{
				return;
			}

			//head sub-window dims
			m_ui32HeadWindowWidth = l_iWindowWidth;
			m_ui32HeadWindowHeight = l_iWindowHeight - m_ui32PaletteWindowHeight;

			uint32 l_ui32HeadMaxSize;
			if(m_ui32HeadWindowWidth < m_ui32HeadWindowHeight)
			{
				l_ui32HeadMaxSize = (uint32)(0.9 * m_ui32HeadWindowWidth);
			}
			else
			{
				l_ui32HeadMaxSize = (uint32)(0.9 * m_ui32HeadWindowHeight);
			}

			if(m_ui32CurrentView == TopographicMap2DView_Top)
			{
				//height used up by nose
				uint32 l_ui32NoseProtrudingHeight = (uint32)(0.1 * l_ui32HeadMaxSize);
				//Y coordinate where nose starts
				m_ui32NoseY = (uint32)((m_ui32HeadWindowHeight - l_ui32HeadMaxSize)/2);
				//skull diameter
				m_ui32SkullDiameter = l_ui32HeadMaxSize - l_ui32NoseProtrudingHeight;
				//skull upper left corner
				m_ui32SkullX = (m_ui32HeadWindowWidth - m_ui32SkullDiameter)/2;
				m_ui32SkullY = m_ui32NoseY + l_ui32NoseProtrudingHeight;

				//skull outline and filled area start/end angles
				m_f32SkullOutlineStartAngle = 0;
				m_f32SkullOutlineEndAngle = 360;
				m_f32SkullFillStartAngle = 0;
				m_f32SkullFillEndAngle = 360;

				//clip mask
				m_ui32ClipmaskWidth = m_ui32SkullDiameter;
				m_ui32ClipmaskHeight = m_ui32SkullDiameter;
			}
			else if(m_ui32CurrentView == TopographicMap2DView_Back)
			{
				//FIXME take into account width used up by ears

				//height used up by neck
				uint32 l_ui32NeckProtrudingHeight = (uint32)(0.072 * l_ui32HeadMaxSize);

				//skull diameter
				m_ui32SkullDiameter = l_ui32HeadMaxSize - l_ui32NeckProtrudingHeight;
				//skull upper left corner
				m_ui32SkullX = (m_ui32HeadWindowWidth - m_ui32SkullDiameter)/2;
				m_ui32SkullY = (m_ui32HeadWindowHeight - l_ui32HeadMaxSize)/2;

				//skull outline and filled area start/end angles
				m_f32SkullOutlineStartAngle = -38;
				m_f32SkullOutlineEndAngle = 180 - m_f32SkullOutlineStartAngle;
				m_f32SkullFillStartAngle = -30;
				m_f32SkullFillEndAngle = 180 - m_f32SkullFillStartAngle;

				uint32 l_ui32SkullCenterX = m_ui32SkullX + m_ui32SkullDiameter / 2;
				uint32 l_ui32SkullCenterY = m_ui32SkullY + m_ui32SkullDiameter / 2;

				m_ui32SkullOutlineLeftPointX = l_ui32SkullCenterX + (uint32)(m_ui32SkullDiameter/2 * cosf(DEG2RAD(m_f32SkullOutlineEndAngle)));
				m_ui32SkullOutlineLeftPointY = l_ui32SkullCenterY - (uint32)(m_ui32SkullDiameter/2 * sinf(DEG2RAD(m_f32SkullOutlineEndAngle)));
				m_ui32SkullOutlineRightPointX = l_ui32SkullCenterX + (uint32)(m_ui32SkullDiameter/2 * cosf(DEG2RAD(m_f32SkullOutlineStartAngle)));
				m_ui32SkullOutlineRightPointY = l_ui32SkullCenterY - (uint32)(m_ui32SkullDiameter/2 * sinf(DEG2RAD(m_f32SkullOutlineStartAngle)));

				m_ui32SkullFillLeftPointX = l_ui32SkullCenterX + (uint32)(m_ui32SkullDiameter/2 * cosf(DEG2RAD(m_f32SkullFillEndAngle)));
				m_ui32SkullFillLeftPointY = l_ui32SkullCenterY - (uint32)(m_ui32SkullDiameter/2 * sinf(DEG2RAD(m_f32SkullFillEndAngle)));
				m_ui32SkullFillRightPointX = l_ui32SkullCenterX + (uint32)(m_ui32SkullDiameter/2 * cosf(DEG2RAD(m_f32SkullFillStartAngle)));
				m_ui32SkullFillRightPointY = l_ui32SkullCenterY - (uint32)(m_ui32SkullDiameter/2 * sinf(DEG2RAD(m_f32SkullFillStartAngle)));
				m_ui32SkullFillBottomPointX = m_ui32SkullX + m_ui32SkullDiameter / 2;
				m_ui32SkullFillBottomPointY = m_ui32SkullFillRightPointY;

				//neck extremities
				m_ui32LeftNeckX = m_ui32SkullOutlineLeftPointX + (uint32)(0.025f * m_ui32SkullDiameter);
				m_ui32LeftNeckY = m_ui32SkullOutlineLeftPointY + l_ui32NeckProtrudingHeight;
				m_ui32RightNeckX = m_ui32SkullOutlineRightPointX - (uint32)(0.025f * m_ui32SkullDiameter);
				m_ui32RightNeckY = m_ui32LeftNeckY;

				//clip mask
				m_ui32ClipmaskWidth = m_ui32SkullDiameter;
				m_ui32ClipmaskHeight = m_ui32SkullFillBottomPointY - m_ui32SkullY + 1;
			}
			else if(m_ui32CurrentView == TopographicMap2DView_Left || m_ui32CurrentView == TopographicMap2DView_Right)
			{
				//width used up by nose
				uint32 l_ui32NoseProtrudingWidth = (uint32)(0.06 * m_ui32SkullDiameter);//(uint32)(0.047 * m_ui32SkullDiameter);

				//skull diameter
				m_ui32SkullDiameter = l_ui32HeadMaxSize - l_ui32NoseProtrudingWidth;

				//topmost skull coordinate
				m_ui32SkullY = (m_ui32HeadWindowHeight - m_ui32SkullDiameter)/2;

				if(m_ui32CurrentView == TopographicMap2DView_Left)
				{
					//X coordinate of nose tip
					m_ui32NoseTipX = (m_ui32HeadWindowWidth - l_ui32HeadMaxSize)/2;
					//leftmost skull coordinate
					m_ui32SkullX = m_ui32NoseTipX + l_ui32NoseProtrudingWidth;
					//skull outline and filled area start/end angles
					m_f32SkullOutlineStartAngle = -41;
					m_f32SkullOutlineEndAngle = 193;//194;
					m_f32SkullFillStartAngle = -22;
					m_f32SkullFillEndAngle = 188;

					uint32 l_ui32SkullCenterX = m_ui32SkullX + m_ui32SkullDiameter / 2;
					uint32 l_ui32SkullCenterY = m_ui32SkullY + m_ui32SkullDiameter / 2;

					//nose top = head outline left boundary
					m_ui32NoseTopX = l_ui32SkullCenterX + (uint32)(m_ui32SkullDiameter/2*cosf(DEG2RAD(m_f32SkullOutlineEndAngle)));
					m_ui32NoseTopY = l_ui32SkullCenterY - (uint32)(m_ui32SkullDiameter/2*sinf(DEG2RAD(m_f32SkullOutlineEndAngle)));
					//nose bump
					m_ui32NoseBumpX = m_ui32NoseTipX;
					m_ui32NoseBumpY = m_ui32NoseTopY + (uint32)(0.15f * m_ui32SkullDiameter);//(uint32)(0.179f * m_ui32SkullDiameter);
					//nose tip
					//m_ui32NoseTipX = m_ui32NoseBumpX;
					m_ui32NoseTipY = m_ui32NoseBumpY + (uint32)(0.03f * m_ui32SkullDiameter);//(uint32)(0.021f * m_ui32SkullDiameter);
					//nose base
					m_ui32NoseBaseX = m_ui32NoseTipX + (uint32)(0.1f * m_ui32SkullDiameter);
					m_ui32NoseBaseY = m_ui32NoseTipY;
					//nose bottom
					m_ui32NoseBottomX = m_ui32NoseBaseX;
					m_ui32NoseBottomY = m_ui32NoseBaseY + (uint32)(0.02f * m_ui32SkullDiameter);//(uint32)(0.016f * m_ui32SkullDiameter);
				}
				else
				{
					//X coordinate of nose tip
					m_ui32NoseTipX = (m_ui32HeadWindowWidth + l_ui32HeadMaxSize)/2;
					//leftmost skull coordinate
					m_ui32SkullX = (m_ui32HeadWindowWidth - l_ui32HeadMaxSize)/2;
					//skull outline and filled area start/end angles
					m_f32SkullOutlineStartAngle = -13; //-14;
					m_f32SkullOutlineEndAngle = 221;
					m_f32SkullFillStartAngle = -8;
					m_f32SkullFillEndAngle = 202;

					uint32 l_ui32SkullCenterX = m_ui32SkullX + m_ui32SkullDiameter / 2;
					uint32 l_ui32SkullCenterY = m_ui32SkullY + m_ui32SkullDiameter / 2;

					//nose top = head outline right boundary
					m_ui32NoseTopX = l_ui32SkullCenterX + (uint32)(m_ui32SkullDiameter/2*cosf(DEG2RAD(m_f32SkullOutlineStartAngle)));
					m_ui32NoseTopY = l_ui32SkullCenterY - (uint32)(m_ui32SkullDiameter/2*sinf(DEG2RAD(m_f32SkullOutlineStartAngle)));
					//nose bump
					m_ui32NoseBumpX = m_ui32NoseTipX;
					m_ui32NoseBumpY = m_ui32NoseTopY + (uint32)(0.15f * m_ui32SkullDiameter);//(uint32)(0.179f * m_ui32SkullDiameter);
					//nose tip
					//m_ui32NoseTipX = m_ui32NoseBumpX;
					m_ui32NoseTipY = m_ui32NoseBumpY + (uint32)(0.03f * m_ui32SkullDiameter);//(uint32)(0.021f * m_ui32SkullDiameter);
					//nose base
					m_ui32NoseBaseX = m_ui32NoseTipX - (uint32)(0.1f * m_ui32SkullDiameter);
					m_ui32NoseBaseY = m_ui32NoseTipY;
					//nose bottom
					m_ui32NoseBottomX = m_ui32NoseBaseX;
					m_ui32NoseBottomY = m_ui32NoseBaseY + (uint32)(0.02f * m_ui32SkullDiameter);//(uint32)(0.016f * m_ui32SkullDiameter);
				}

				uint32 l_ui32SkullCenterX = m_ui32SkullX + m_ui32SkullDiameter / 2;
				uint32 l_ui32SkullCenterY = m_ui32SkullY + m_ui32SkullDiameter / 2;
				m_ui32SkullFillLeftPointX = l_ui32SkullCenterX + (uint32)(m_ui32SkullDiameter/2 * cosf(DEG2RAD(m_f32SkullFillEndAngle)));
				m_ui32SkullFillLeftPointY = l_ui32SkullCenterY - (uint32)(m_ui32SkullDiameter/2 * sinf(DEG2RAD(m_f32SkullFillEndAngle)));
				m_ui32SkullFillRightPointX = l_ui32SkullCenterX + (uint32)(m_ui32SkullDiameter/2 * cosf(DEG2RAD(m_f32SkullFillStartAngle)));
				m_ui32SkullFillRightPointY = l_ui32SkullCenterY - (uint32)(m_ui32SkullDiameter/2 * sinf(DEG2RAD(m_f32SkullFillStartAngle)));

				m_ui32SkullFillBottomPointX = m_ui32SkullX + m_ui32SkullDiameter / 2;
				m_ui32SkullFillBottomPointY = m_ui32SkullY + (uint32)(0.684f * m_ui32SkullDiameter);

				//clip mask
				m_ui32ClipmaskWidth = m_ui32SkullDiameter;
				m_ui32ClipmaskHeight = m_ui32SkullFillBottomPointY - m_ui32SkullY + 1;
			}

			//free existing clipmask, if any
			if(m_pClipmaskGC != NULL)
			{
				g_object_unref(m_pClipmaskGC);
			}
			if(m_pClipmask != NULL)
			{
				g_object_unref(m_pClipmask);
			}

			//allocate clipmask
			m_pClipmask = gdk_pixmap_new(m_pDrawingArea->window, m_ui32ClipmaskWidth, m_ui32ClipmaskHeight, 1);
			m_pClipmaskGC = gdk_gc_new(GDK_DRAWABLE(m_pClipmask));
			gdk_gc_set_colormap(m_pClipmaskGC, gdk_gc_get_colormap(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)]));

			//redraw it
			redrawClipmask();

			//allocate main pixmap
			//TODO!

			//allocate RGB pixmap
			if(m_pSkullRGBBuffer != NULL)
			{
				delete [] m_pSkullRGBBuffer;
			}
			//align lines on 32bit boundaries
			m_ui32RowStride = ((m_ui32SkullDiameter*3)%4 == 0) ? (m_ui32SkullDiameter*3) : ((((m_ui32SkullDiameter*3)>>2)+1)<<2);
			m_pSkullRGBBuffer = new guchar[m_ui32RowStride*m_ui32SkullDiameter];

			//determine size of colored cells
#if 1
			uint32 l_ui32CellMinSize = 6;
			uint32 l_ui32CellMaxSize = 6;
			float64 l_f64CellOverSkullSizeRatio = 0.02;
			m_ui32CellSize = (uint32)(m_ui32SkullDiameter * l_f64CellOverSkullSizeRatio);

			if(m_ui32CellSize < l_ui32CellMinSize)
			{
				m_ui32CellSize = l_ui32CellMinSize;
			}
			else if(m_ui32CellSize > l_ui32CellMaxSize)
			{
				m_ui32CellSize = l_ui32CellMaxSize;
			}
#else
			m_ui32CellSize = m_ui32SkullDiameter/2;
#endif
			if(m_ui32CellSize == 0)
			{
				return;
			}

			//number of samples in a row or column
			m_ui32GridSize = (uint32)ceil(m_ui32SkullDiameter / (double)m_ui32CellSize);

			//determine number of samples lying within skull
			uint32 l_ui32NbSamples = computeSamplesNormalizedCoordinates(false);

			//resize sample grids accordingly
			m_oSample2DCoordinates.resize(l_ui32NbSamples);
			m_oSampleCoordinatesMatrix.setDimensionSize(0, l_ui32NbSamples);
			m_oSampleCoordinatesMatrix.setDimensionSize(1, 3);
			m_oSampleValues.resize(l_ui32NbSamples);

			//compute samples normalized coordinates
			computeSamplesNormalizedCoordinates(true);

			//resizing completed
			m_bNeedResize = false;
		}

		void CTopographicMap2DView::redrawClipmask()
		{
			//clear clipmask by drawing a black rectangle
			GdkColor l_oBlack;
			l_oBlack.red = l_oBlack.green = l_oBlack.blue = 0;
			gdk_gc_set_rgb_fg_color(m_pClipmaskGC, &l_oBlack);
			gdk_draw_rectangle(m_pClipmask, m_pClipmaskGC, TRUE, 0, 0, m_ui32ClipmaskWidth, m_ui32ClipmaskHeight);

			//draw visible circular region with a white filled arc
			GdkColor l_oWhite;
			l_oWhite.red = l_oWhite.green = l_oWhite.blue = 65535;
			gdk_gc_set_rgb_fg_color(m_pClipmaskGC, &l_oWhite);
			gdk_draw_arc(m_pClipmask,
				m_pClipmaskGC,
				TRUE,
				0,
				0,
				(gint)m_ui32SkullDiameter,
				(gint)m_ui32SkullDiameter,
				(gint)(64 * m_f32SkullFillStartAngle),
				(gint)(64 * (m_f32SkullFillEndAngle-m_f32SkullFillStartAngle)));

			//views other than top have an extra non-clipped area
			if(m_ui32CurrentView == TopographicMap2DView_Left || m_ui32CurrentView == TopographicMap2DView_Right ||
				m_ui32CurrentView == TopographicMap2DView_Back)
			{
				//draw polygon : { skullCenter, skullFillStartPoint, skullFillBottomPoint, skullFillEndPoint, skullCenter }
				GdkPoint l_pPolygon[4];
				l_pPolygon[0].x = m_ui32SkullX + m_ui32SkullDiameter/2 - m_ui32SkullX;
				l_pPolygon[0].y = m_ui32SkullY + m_ui32SkullDiameter/2 - m_ui32SkullY - 2;
				l_pPolygon[1].x = m_ui32SkullFillRightPointX - m_ui32SkullX;
				l_pPolygon[1].y = m_ui32SkullFillRightPointY - m_ui32SkullY - 2;
				l_pPolygon[2].x = m_ui32SkullFillBottomPointX - m_ui32SkullX;
				l_pPolygon[2].y = m_ui32SkullFillBottomPointY - m_ui32SkullY - 2;
				l_pPolygon[3].x = m_ui32SkullFillLeftPointX - m_ui32SkullX;
				l_pPolygon[3].y = m_ui32SkullFillLeftPointY - m_ui32SkullY - 2;
				gdk_draw_polygon(
					m_pClipmask,
					m_pClipmaskGC,
					TRUE,
					l_pPolygon,
					4);
			}

			//restore default black color
			gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], &l_oBlack);

			//update visible region
			if(m_pVisibleRegion != NULL)
			{
				gdk_region_destroy(m_pVisibleRegion);
			}
			m_pVisibleRegion = gdk_drawable_get_visible_region(GDK_DRAWABLE(m_pClipmask));
		}

		void CTopographicMap2DView::refreshPotentials()
		{
			uint32 w, h;

#ifdef INTERPOLATE_AT_CHANNEL_LOCATION
			for(uint32 i=(uint32)m_rTopographicMapDatabase.getChannelCount(); i<m_oSampleValues.size(); i++)
#else
			for(uint32 i=0; i<m_oSampleValues.size(); i++)
#endif
			{
				//cells of last row and last column may be smaller than other ones
				if(m_oSample2DCoordinates[i].first + m_ui32CellSize >= m_ui32SkullDiameter)
				{
					w = m_ui32SkullDiameter - m_oSample2DCoordinates[i].first;
				}
				else
				{
					w = m_ui32CellSize;
				}

				if(m_oSample2DCoordinates[i].second + m_ui32CellSize >= m_ui32SkullDiameter)
				{
					h = m_ui32SkullDiameter - m_oSample2DCoordinates[i].second;
				}
				else
				{
					h = m_ui32CellSize;
				}

				uint32 l_ui32Index=m_oSampleValues[i];
				if(l_ui32Index>12)
				{
#if defined OVP_OS_Windows
#ifndef NDEBUG
					_asm int 3;
#endif
#endif
					l_ui32Index=12;
				}

				drawBoxToBuffer(
					m_oSample2DCoordinates[i].first,
					m_oSample2DCoordinates[i].second,
					w,
					h,
					s_palette8[3*l_ui32Index],
					s_palette8[3*l_ui32Index+1],
					s_palette8[3*l_ui32Index+2]);
			}
		}

		void CTopographicMap2DView::drawPotentials()
		{
			gdk_draw_rgb_image(
				m_pDrawingArea->window,
				m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
				m_ui32SkullX, m_ui32SkullY,
				m_ui32SkullDiameter, m_ui32SkullDiameter,
				GDK_RGB_DITHER_NONE,
				m_pSkullRGBBuffer,
				m_ui32RowStride);
		}

		void CTopographicMap2DView::drawElectrodes()
		{
			if(m_bElectrodesToggledOn == false)
			{
				return;
			}

			//determine size of electrode rings
			float64 l_f64ElectrodeRingOverSkullSizeRatio = 0.05;
			gint l_i32ElectrodeRingSize = (gint)(m_ui32SkullDiameter * l_f64ElectrodeRingOverSkullSizeRatio);

#if 0
			if(l_i32ElectrodeRingSize < (gint)l_ui32ElectrodeRingMinSize)
			{
				l_i32ElectrodeRingSize = (gint)l_ui32ElectrodeRingMinSize;
			}
			else if(l_i32ElectrodeRingSize > (gint)l_ui32ElectrodeRingMaxSize)
			{
				l_i32ElectrodeRingSize = (gint)l_ui32ElectrodeRingMaxSize;
			}
#else
			l_i32ElectrodeRingSize = 5;
#endif

			if(l_i32ElectrodeRingSize == 0)
				return;

			GdkColor l_oWhite;
			l_oWhite.red = 65535;
			l_oWhite.green = 65535;
			l_oWhite.blue = 65535;

			GdkColor l_oBlack;
			l_oBlack.red = 0;
			l_oBlack.green = 0;
			l_oBlack.blue = 0;

			//set electrode ring thickness
			gint l_i32ElectrodeRingThickness = 1;
			gdk_gc_set_line_attributes(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)], l_i32ElectrodeRingThickness, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_BEVEL);

			//electrode label
			CString l_oElectrodeLabel;
			PangoLayout* l_pElectrodeLabelLayout = gtk_widget_create_pango_layout(GTK_WIDGET(m_pDrawingArea), " ");
			gint l_iTextHeight;
			gint l_iTextWidth;
			pango_layout_get_pixel_size(l_pElectrodeLabelLayout, NULL, &l_iTextHeight);

			//draw rings
			uint32 l_ui32ChannelCount = (uint32)m_rTopographicMapDatabase.getChannelCount();
			gint l_i32ChannelX;
			gint l_i32ChannelY;

			for(uint32 i=0; i<l_ui32ChannelCount; i++)
			{
				if(getChannel2DPosition(i, l_i32ChannelX, l_i32ChannelY) == false)
				{
					continue;
				}

#ifdef INTERPOLATE_AT_CHANNEL_LOCATION
				//disk colored according to value interpolated at this channel location
				gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], &s_palette[m_oSampleValues[i]]);
#else
				//fill ring with white
				gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], &l_oWhite);
#endif
				gdk_draw_arc(m_pDrawingArea->window,
				m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
				TRUE,
				l_i32ChannelX - l_i32ElectrodeRingSize/2,
				l_i32ChannelY - l_i32ElectrodeRingSize/2,
				l_i32ElectrodeRingSize,
				l_i32ElectrodeRingSize,
				0,
				64*360);

				//ring centered on channel location
				gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], &l_oBlack);

				gdk_draw_arc(m_pDrawingArea->window,
				m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
				FALSE,
				l_i32ChannelX - l_i32ElectrodeRingSize/2,
				l_i32ChannelY - l_i32ElectrodeRingSize/2,
				l_i32ElectrodeRingSize,
				l_i32ElectrodeRingSize,
				0,
				64*360);

				//channel label
				gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], &l_oBlack/*&l_oWhite*/);

				m_rTopographicMapDatabase.getChannelLabel(i, l_oElectrodeLabel);
				pango_layout_set_text(l_pElectrodeLabelLayout, l_oElectrodeLabel, strlen(l_oElectrodeLabel));
				pango_layout_get_pixel_size(l_pElectrodeLabelLayout, &l_iTextWidth, NULL);
				gdk_draw_layout(m_pDrawingArea->window, m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)],
					l_i32ChannelX-l_iTextWidth/2,
					l_i32ChannelY-l_i32ElectrodeRingSize/2-l_iTextHeight-5,
					l_pElectrodeLabelLayout);
			}

			//restore default line thickness
			gdk_gc_set_line_attributes(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE (m_pDrawingArea)], 1, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_BEVEL);

			//restore default black color
			gdk_gc_set_rgb_fg_color(m_pDrawingArea->style->fg_gc[GTK_WIDGET_STATE(m_pDrawingArea)], &l_oBlack);

			//delete pango layout
			g_object_unref(l_pElectrodeLabelLayout);
		}

		boolean CTopographicMap2DView::getChannel2DPosition(uint32 ui32ChannelIndex, gint& l_i32ChannelX, gint& l_i32ChannelY)
		{
			uint32 l_ui32SkullCenterX = m_ui32SkullX + m_ui32SkullDiameter/2;
			uint32 l_ui32SkullCenterY = m_ui32SkullY + m_ui32SkullDiameter/2;
			//get normalized coordinates
			float64* l_pElectrodePosition;
			m_rTopographicMapDatabase.getChannelPosition(ui32ChannelIndex, l_pElectrodePosition);

			if(m_ui32CurrentView == TopographicMap2DView_Top)
			{
				if(m_ui32CurrentProjection == TopographicMap2DProjection_Axial)
				{
					l_i32ChannelX = (gint)(l_ui32SkullCenterX + l_pElectrodePosition[0] * m_ui32SkullDiameter/2);
					l_i32ChannelY = (gint)(l_ui32SkullCenterY - l_pElectrodePosition[1] * m_ui32SkullDiameter/2);
				}
				else //radial
				{
					//compute back frame 2D coordinates
					float64 l_f64Theta = getThetaFromCartesianCoordinates(l_pElectrodePosition);
					float64 l_f64Phi = getPhiFromCartesianCoordinates(l_pElectrodePosition);
					compute2DCoordinates(l_f64Theta, l_f64Phi, l_ui32SkullCenterX, l_ui32SkullCenterY, l_i32ChannelX, l_i32ChannelY);
				}
			}
			else if(m_ui32CurrentView == TopographicMap2DView_Back)
			{
				//if(l_pElectrodePosition[1] > 0) //electrode not visible
				if(l_pElectrodePosition[1] > sin(1.f/90*M_PI/2))
				{
					return false;
				}

				if(m_ui32CurrentProjection == TopographicMap2DProjection_Axial)
				{
					l_i32ChannelX = (gint)(l_ui32SkullCenterX + l_pElectrodePosition[0] * m_ui32SkullDiameter/2);
					l_i32ChannelY = (gint)(l_ui32SkullCenterY - l_pElectrodePosition[2] * m_ui32SkullDiameter/2);
				}
				else //radial
				{
					//transform coordinates from top frame to back frame
					float64 l_pBackElectrodePosition[3];
					l_pBackElectrodePosition[0] = l_pElectrodePosition[0];
					l_pBackElectrodePosition[1] = l_pElectrodePosition[2];
					l_pBackElectrodePosition[2] = -l_pElectrodePosition[1];
					//compute back frame 2D coordinates
					float64 l_f64Theta = getThetaFromCartesianCoordinates(l_pBackElectrodePosition);
					float64 l_f64Phi = getPhiFromCartesianCoordinates(l_pBackElectrodePosition);
					compute2DCoordinates(l_f64Theta, l_f64Phi, l_ui32SkullCenterX, l_ui32SkullCenterY, l_i32ChannelX, l_i32ChannelY);
				}
			}
			else if(m_ui32CurrentView == TopographicMap2DView_Left)
			{
				//if(l_pElectrodePosition[0] > 0) //electrode not visible
				if(l_pElectrodePosition[0] > cos(89.f/90*M_PI/2))
				{
					return false;
				}

				if(m_ui32CurrentProjection == TopographicMap2DProjection_Axial)
				{
					l_i32ChannelX = (gint)(l_ui32SkullCenterX - l_pElectrodePosition[1] * m_ui32SkullDiameter/2);
					l_i32ChannelY = (gint)(l_ui32SkullCenterY - l_pElectrodePosition[2] * m_ui32SkullDiameter/2);
				}
				else //radial
				{
					//transform coordinates from top frame to left frame
					float64 l_pBackElectrodePosition[3];
					l_pBackElectrodePosition[0] = -l_pElectrodePosition[1];
					l_pBackElectrodePosition[1] = l_pElectrodePosition[2];
					l_pBackElectrodePosition[2] = -l_pElectrodePosition[0];
					//compute back frame 2D coordinates
					float64 l_f64Theta = getThetaFromCartesianCoordinates(l_pBackElectrodePosition);
					float64 l_f64Phi = getPhiFromCartesianCoordinates(l_pBackElectrodePosition);
					compute2DCoordinates(l_f64Theta, l_f64Phi, l_ui32SkullCenterX, l_ui32SkullCenterY, l_i32ChannelX, l_i32ChannelY);
				}
			}
			else if(m_ui32CurrentView == TopographicMap2DView_Right)
			{
				//if(l_pElectrodePosition[0] < 0) //electrode not visible
				if(l_pElectrodePosition[0] < -cos(89.f/90*M_PI/2))
				{
					return false;
				}

				if(m_ui32CurrentProjection == TopographicMap2DProjection_Axial)
				{
					l_i32ChannelX = (gint)(l_ui32SkullCenterX + l_pElectrodePosition[1] * m_ui32SkullDiameter/2);
					l_i32ChannelY = (gint)(l_ui32SkullCenterY - l_pElectrodePosition[2] * m_ui32SkullDiameter/2);
				}
				else //radial
				{
					//transform coordinates from top frame to left frame
					float64 l_pBackElectrodePosition[3];
					l_pBackElectrodePosition[0] = l_pElectrodePosition[1];
					l_pBackElectrodePosition[1] = l_pElectrodePosition[2];
					l_pBackElectrodePosition[2] = l_pElectrodePosition[0];
					//compute back frame 2D coordinates
					float64 l_f64Theta = getThetaFromCartesianCoordinates(l_pBackElectrodePosition);
					float64 l_f64Phi = getPhiFromCartesianCoordinates(l_pBackElectrodePosition);
					compute2DCoordinates(l_f64Theta, l_f64Phi, l_ui32SkullCenterX, l_ui32SkullCenterY, l_i32ChannelX, l_i32ChannelY);
				}
			}

			//make sure electrode is in the non clipped area of the display
			//TODO : perform this test once per view only!
			return gdk_region_point_in(m_pVisibleRegion, l_i32ChannelX-m_ui32SkullX, l_i32ChannelY-m_ui32SkullY) != 0;
		}

		void CTopographicMap2DView::drawBoxToBuffer(uint32 ui32X, uint32 ui32Y, uint32 ui32Width, uint32 ui32Height, uint8 ui8Red, uint8 ui8Green, uint8 ui8Blue)
		{
#ifdef OVP_OS_Windows
#ifndef NDEBUG
			//m_pSkullRGBBuffer == m_ui32RowStride*m_ui32SkullDiameter
			assert(ui32X < m_ui32SkullDiameter);
			assert(ui32Y < m_ui32SkullDiameter);
			assert( (m_ui32RowStride*ui32Y) + (ui32X*3) + 2 < m_ui32RowStride * m_ui32SkullDiameter );
#endif
#endif
			guchar* l_pLineBase = m_pSkullRGBBuffer + (m_ui32RowStride*ui32Y) + (ui32X*3);

			for(uint32 j=0 ; j<ui32Height ; j++)
			{
				for(uint32 i=0 ; i<(ui32Width*3) ; i+=3)
				{
					*(l_pLineBase + i) =  ui8Red;
					*(l_pLineBase + i + 1) =  ui8Green;
					*(l_pLineBase + i + 2) =  ui8Blue;
				}

				l_pLineBase+=(m_ui32RowStride);
			}
		}

		uint32 CTopographicMap2DView::computeSamplesNormalizedCoordinates(boolean bComputeCoordinates)
		{
			uint32 l_ui32CurSample = 0;

#ifdef INTERPOLATE_AT_CHANNEL_LOCATION
			uint32 l_ui32ChannelCount = (uint32)m_rTopographicMapDatabase.getChannelCount();
			float64* l_pElectrodePosition=NULL;

			//sampling at electrode locations
			for(l_ui32CurSample=0; l_ui32CurSample<l_ui32ChannelCount; l_ui32CurSample++)
			{
				m_rTopographicMapDatabase.getChannelPosition(l_ui32CurSample, l_pElectrodePosition);

				//dummy 2D coords - actual coords are computed when drawing electrode rings
				m_oSample2DCoordinates[l_ui32CurSample].first = 0;
				m_oSample2DCoordinates[l_ui32CurSample].second= 0;

				*(m_oSampleCoordinatesMatrix.getBuffer() + 3*l_ui32CurSample) = *l_pElectrodePosition;
				*(m_oSampleCoordinatesMatrix.getBuffer() + 3*l_ui32CurSample+1) = *(l_pElectrodePosition+1);
				*(m_oSampleCoordinatesMatrix.getBuffer() + 3*l_ui32CurSample+2) = *(l_pElectrodePosition+2);
			}
#endif

			//sampling over skull area
			float32 l_f32CurX, l_f32CurY;
			uint32 i, j;
			float32 l_f32SkullCenterX = m_ui32SkullX + m_ui32SkullDiameter/2.f;
			float32 l_f32SkullCenterY = m_ui32SkullY + m_ui32SkullDiameter/2.F;
			float32 l_f32ClosestX, l_f32ClosestY;
			float32 l_f32X, l_f32Y;
			float64* l_pBuffer = m_oSampleCoordinatesMatrix.getBuffer();

			//for each row
			for(i=0, l_f32CurY=(float32)m_ui32SkullY; i<m_ui32GridSize; i++, l_f32CurY+=m_ui32CellSize)
			{
				//for each column
				for(j=0, l_f32CurX=(float32)m_ui32SkullX; j<m_ui32GridSize; j++, l_f32CurX+=m_ui32CellSize)
				{
					//find corner closest to skull center
					l_f32ClosestX = fabs(l_f32CurX - l_f32SkullCenterX) < fabs(l_f32CurX + m_ui32CellSize - l_f32SkullCenterX) ? l_f32CurX : (l_f32CurX + m_ui32CellSize);
					l_f32ClosestY = fabs(l_f32CurY - l_f32SkullCenterY) < fabs(l_f32CurY + m_ui32CellSize - l_f32SkullCenterY) ? l_f32CurY : (l_f32CurY + m_ui32CellSize);

					//make sure electrode is in the non clipped area of the display
					//TODO : perform this test once per view only!
					//ensure closest corner lies within "skull sphere"
					if((l_f32ClosestX - l_f32SkullCenterX)*(l_f32ClosestX - l_f32SkullCenterX) +
						(l_f32ClosestY - l_f32SkullCenterY)*(l_f32ClosestY - l_f32SkullCenterY) <= (m_ui32SkullDiameter*m_ui32SkullDiameter/4.f))
					{
						//ensure this point is in the non clipped skull area
						//FIXME : the previous test remains necessary to get rid of all points lying outside "skull sphere"... Bug in gdk_region_point_in()?
						if(gdk_region_point_in(m_pVisibleRegion, (int)(l_f32ClosestX-m_ui32SkullX), (int)(l_f32ClosestY-m_ui32SkullY)))
						{
							if(bComputeCoordinates == true)
							{
								m_oSample2DCoordinates[l_ui32CurSample].first = j*m_ui32CellSize;
								m_oSample2DCoordinates[l_ui32CurSample].second= i*m_ui32CellSize;

								//compute normalized coordinates to be fed to spherical spline algorithm
								//----------------------------------------------------------------------
								uint32 l_ui32BaseIndex = 3* l_ui32CurSample;

								//normalized X, Y coords in (X, Y) projection plane
								l_f32X = (l_f32ClosestX - l_f32SkullCenterX) / (m_ui32SkullDiameter/2.f);
								l_f32Y = -(l_f32ClosestY - l_f32SkullCenterY) / (m_ui32SkullDiameter/2.f); //y axis down in 2D but up in 3D convention

								if(m_ui32CurrentProjection == TopographicMap2DProjection_Axial)
								{
									if(m_ui32CurrentView == TopographicMap2DView_Top)
									{
										*(l_pBuffer + l_ui32BaseIndex) = l_f32X;
										*(l_pBuffer + l_ui32BaseIndex+1) = l_f32Y;
										//z = sqrt(1-x*x-y*y)
										float32 l_f32SquareXYSum = l_f32X * l_f32X + l_f32Y * l_f32Y;
										*(l_pBuffer + l_ui32BaseIndex+2) = (l_f32SquareXYSum >= 1) ? 0 : sqrt(1 - l_f32SquareXYSum);
									}
									else if(m_ui32CurrentView == TopographicMap2DView_Back)
									{
										*(l_pBuffer + l_ui32BaseIndex) = l_f32X;
										*(l_pBuffer + l_ui32BaseIndex+2) = l_f32Y;
										//y = sqrt(1-x*x-z*z)
										float32 l_f32SquareXYSum = l_f32X * l_f32X + l_f32Y * l_f32Y;
										*(l_pBuffer + l_ui32BaseIndex+1) = (l_f32SquareXYSum >= 1) ? 0 : sqrt(1 - l_f32SquareXYSum);
									}
									else if(m_ui32CurrentView == TopographicMap2DView_Left)
									{
										*(l_pBuffer + l_ui32BaseIndex+1) = -l_f32X;
										*(l_pBuffer + l_ui32BaseIndex+2) = l_f32Y;
										//x = sqrt(1-y*y-z*z)
										float32 l_f32SquareXYSum = l_f32X * l_f32X + l_f32Y * l_f32Y;
										*(l_pBuffer + l_ui32BaseIndex) = (l_f32SquareXYSum >= 1) ? 0 : sqrt(1 - l_f32SquareXYSum);
									}
									else if(m_ui32CurrentView == TopographicMap2DView_Right)
									{
										*(l_pBuffer + l_ui32BaseIndex+1) = l_f32X;
										*(l_pBuffer + l_ui32BaseIndex+2) = l_f32Y;
										//x = sqrt(1-y*y-z*z)
										float32 l_f32SquareXYSum = l_f32X * l_f32X + l_f32Y * l_f32Y;
										*(l_pBuffer + l_ui32BaseIndex) = (l_f32SquareXYSum >= 1) ? 0 : sqrt(1 - l_f32SquareXYSum);
									}
								}
								else //radial
								{
									//theta = (X,Y) arc length
									float32 l_f32Theta = float32(M_PI/2 * sqrtf(l_f32X*l_f32X + l_f32Y*l_f32Y));
									float32 l_f32ScalingFactor = (l_f32Theta <= 1e-3) ? 0 : (sinf(l_f32Theta) / l_f32Theta);
									float32 l_f32SampleLocalCoordinates[3];
									//x = sin(theta) / theta * X
									l_f32SampleLocalCoordinates[0] = float32(l_f32ScalingFactor * l_f32X * (M_PI/2));
									//y = sin(theta) / theta * Y
									l_f32SampleLocalCoordinates[1] = float32(l_f32ScalingFactor * l_f32Y * (M_PI/2));
									//z = cos(theta)
									l_f32SampleLocalCoordinates[2] = cosf(l_f32Theta);

									if(m_ui32CurrentView == TopographicMap2DView_Top)
									{
										*(l_pBuffer + l_ui32BaseIndex) = l_f32SampleLocalCoordinates[0];
										*(l_pBuffer + l_ui32BaseIndex+1) = l_f32SampleLocalCoordinates[1];
										*(l_pBuffer + l_ui32BaseIndex+2) = l_f32SampleLocalCoordinates[2];
									}
									else if(m_ui32CurrentView == TopographicMap2DView_Back)
									{
										*(l_pBuffer + l_ui32BaseIndex) = l_f32SampleLocalCoordinates[0];
										*(l_pBuffer + l_ui32BaseIndex+1) = -l_f32SampleLocalCoordinates[2];
										*(l_pBuffer + l_ui32BaseIndex+2) = l_f32SampleLocalCoordinates[1];
									}
									else if(m_ui32CurrentView == TopographicMap2DView_Left)
									{
										*(l_pBuffer + l_ui32BaseIndex) = -l_f32SampleLocalCoordinates[2];
										*(l_pBuffer + l_ui32BaseIndex+1) = -l_f32SampleLocalCoordinates[0];
										*(l_pBuffer + l_ui32BaseIndex+2) = l_f32SampleLocalCoordinates[1];
									}
									else if(m_ui32CurrentView == TopographicMap2DView_Right)
									{
										*(l_pBuffer + l_ui32BaseIndex) = l_f32SampleLocalCoordinates[2];
										*(l_pBuffer + l_ui32BaseIndex+1) = l_f32SampleLocalCoordinates[0];
										*(l_pBuffer + l_ui32BaseIndex+2) = l_f32SampleLocalCoordinates[1];
									}
								}
							}

							l_ui32CurSample++;
						} //point in non clipped area
					} //point in "skull sphere"
				}
			}

			return l_ui32CurSample;
		}

		void CTopographicMap2DView::enableElectrodeButtonSignals(boolean enable)
		{
			if(enable == true)
			{
				g_signal_connect(G_OBJECT(m_pElectrodesToggleButton), "toggled", G_CALLBACK (toggleElectrodesCallback), this);
			}
			else
			{
				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pElectrodesToggleButton), reinterpret_cast<void*>(G_CALLBACK (toggleElectrodesCallback)), this);
			}
		}

		void CTopographicMap2DView::enableProjectionButtonSignals(boolean enable)
		{
			if(enable == true)
			{
				g_signal_connect(G_OBJECT(m_pAxialProjectionButton), "toggled", G_CALLBACK (setProjectionCallback), this);
				g_signal_connect(G_OBJECT(m_pRadialProjectionButton), "toggled", G_CALLBACK (setProjectionCallback), this);
			}
			else
			{
				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pAxialProjectionButton), (void*)(G_CALLBACK (setProjectionCallback)), this);
				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pRadialProjectionButton), (void*)(G_CALLBACK (setProjectionCallback)), this);
			}
		}

		void CTopographicMap2DView::enableViewButtonSignals(boolean enable)
		{
			if(enable == true)
			{
				g_signal_connect(G_OBJECT(m_pTopViewButton), "toggled", G_CALLBACK (setViewCallback), this);
				g_signal_connect(G_OBJECT(m_pLeftViewButton), "toggled", G_CALLBACK (setViewCallback), this);
				g_signal_connect(G_OBJECT(m_pRightViewButton), "toggled", G_CALLBACK (setViewCallback), this);
				g_signal_connect(G_OBJECT(m_pBackViewButton), "toggled", G_CALLBACK (setViewCallback), this);
			}
			else
			{
				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pTopViewButton), (void*)(G_CALLBACK (setViewCallback)), this);
				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pLeftViewButton), (void*)(G_CALLBACK (setViewCallback)), this);
				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pRightViewButton), (void*)(G_CALLBACK (setViewCallback)), this);
				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pBackViewButton), (void*)(G_CALLBACK (setViewCallback)), this);
			}
		}

		void CTopographicMap2DView::enableInterpolationButtonSignals(boolean enable)
		{
			if(enable == true)
			{
				g_signal_connect(G_OBJECT(m_pMapPotentials), "toggled", G_CALLBACK (setInterpolationCallback), this);
				g_signal_connect(G_OBJECT(m_pMapCurrents), "toggled", G_CALLBACK (setInterpolationCallback), this);
			}
			else
			{
				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pMapPotentials), (void*)(G_CALLBACK (setInterpolationCallback)), this);
				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pMapCurrents), (void*)(G_CALLBACK (setInterpolationCallback)), this);
			}
		}

		float64 CTopographicMap2DView::getThetaFromCartesianCoordinates(const float64* pCartesianCoords) const
		{
			return acos(pCartesianCoords[2]);
		}

		float64 CTopographicMap2DView::getPhiFromCartesianCoordinates(const float64* pCartesianCoords) const
		{
			float64 l_f64Phi;
			if(pCartesianCoords[0] > 0.001)
			{
				l_f64Phi = atan(pCartesianCoords[1] / pCartesianCoords[0]);

				if(l_f64Phi < 0)
				{
					l_f64Phi += 2 * M_PI;
				}
			}
			else if(pCartesianCoords[0] < -0.001)
			{
				l_f64Phi = atan(pCartesianCoords[1] / pCartesianCoords[0]) + M_PI;
			}
			else
			{
				l_f64Phi = pCartesianCoords[1] > 0 ? (M_PI / 2) : (3 * M_PI/2);
			}

			return l_f64Phi;
		}

		boolean CTopographicMap2DView::compute2DCoordinates(float64 f64Theta, float64 f64Phi,
			uint32 ui32SkullCenterX, uint32 ui32SkullCenterY, gint& rX, gint& rY) const
		{
			//linear plotting along radius
			float64 l_f64Length = f64Theta/(M_PI/2) * m_ui32SkullDiameter/2;
			//determine coordinates on unit circle
			float64 l_f64X = cos(f64Phi);
			float64 l_f64Y = sin(f64Phi);
			//scale vector so that it is l_f64Length long
			rX = (gint)(ui32SkullCenterX + l_f64Length * l_f64X);
			rY = (gint)(ui32SkullCenterY - l_f64Length * l_f64Y);
			return true;
		}

		//CALLBACKS

		gboolean redrawCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
		{
			reinterpret_cast<CTopographicMap2DView*>(data)->redraw();
			return TRUE;
		}

		gboolean resizeCallback(GtkWidget* pWidget, GtkAllocation* pAllocation, gpointer data)
		{
			reinterpret_cast<CTopographicMap2DView*>(data)->resizeCB(pAllocation->width, pAllocation->height);
			return FALSE;
		}

		void toggleElectrodesCallback(GtkWidget* pWidget, gpointer data)
		{
			CTopographicMap2DView* l_pTopographicMap2DView = reinterpret_cast<CTopographicMap2DView*>(data);
			l_pTopographicMap2DView->toggleElectrodesCB();
		}

		void setProjectionCallback(GtkWidget* pWidget, gpointer data)
		{
			CTopographicMap2DView* l_pTopographicMap2DView = reinterpret_cast<CTopographicMap2DView*>(data);
			l_pTopographicMap2DView->setProjectionCB(pWidget);
		}

		void setViewCallback(GtkWidget* pWidget, gpointer data)
		{
			CTopographicMap2DView* l_pTopographicMap2DView = reinterpret_cast<CTopographicMap2DView*>(data);
			l_pTopographicMap2DView->setViewCB(pWidget);
		}

		void setInterpolationCallback(GtkWidget* pWidget, gpointer data)
		{
			CTopographicMap2DView* l_pTopographicMap2DView = reinterpret_cast<CTopographicMap2DView*>(data);
			l_pTopographicMap2DView->setInterpolationCB(pWidget);
		}

		void setDelayCallback(GtkRange* pRange, gpointer data)
		{
			CTopographicMap2DView* l_pTopographicMap2DView = reinterpret_cast<CTopographicMap2DView*>(data);
			l_pTopographicMap2DView->setDelayCB(gtk_range_get_value(pRange));
		}
	};
};
