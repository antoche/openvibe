#include "ovpCTopographicMap3DView.h"

#include <iostream>

#include <sstream>

#include <cmath>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace OpenViBEToolkit;

using namespace std;

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		static void setInterpolationCallback(GtkWidget* pWidget,	gpointer data);
		static void toggleElectrodesCallback(GtkWidget* widget, gpointer data);
		//static void toggleSamplingPointsCallback(GtkWidget* widget, gpointer data);
		static void setDelayCallback(::GtkRange *range, gpointer data);

		CTopographicMap3DView::CTopographicMap3DView(CTopographicMap3DDisplay& rTopographicMap3DDisplay,
			CTopographicMapDatabase& rTopographicMapDatabase, uint64 ui64DefaultInterpolation, float64 f64Delay) :
			m_rTopographicMap3DDisplay(rTopographicMap3DDisplay),
			m_rTopographicMapDatabase(rTopographicMapDatabase),
			m_f64MaxDelay(2.0), //maximum delay : 2s
			m_pBuilderInterface(NULL),
			m_ui64CurrentInterpolation(ui64DefaultInterpolation),
			m_pMapPotentials(NULL),
			m_pMapCurrents(NULL),
			m_pElectrodesToggleButton(NULL),
			m_bElectrodesToggledOn(false)//,
			//m_pSamplingPointsToggleButton(NULL),
			//m_bSamplingPointsToggledOn(false)
		{
			//load the gtk builder interface
			m_pBuilderInterface=gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-TopographicMap3D.ui", NULL, NULL);
			gtk_builder_add_from_file(m_pBuilderInterface, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-TopographicMap3D.ui", NULL);

			if(!m_pBuilderInterface)
			{
				g_warning("Couldn't load the interface!");
				return;
			}

			gtk_builder_connect_signals(m_pBuilderInterface, NULL);

			//toolbar
			//-------

			//get pointers to interpolation type buttons
			m_pMapPotentials = GTK_RADIO_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "MapPotentials"));
			m_pMapCurrents = GTK_RADIO_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "MapCurrents"));

			g_signal_connect(G_OBJECT(m_pMapPotentials), "toggled", G_CALLBACK (setInterpolationCallback), this);
			g_signal_connect(G_OBJECT(m_pMapCurrents), "toggled", G_CALLBACK (setInterpolationCallback), this);

			//get pointer to electrodes toggle button
			m_pElectrodesToggleButton = GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "ToggleElectrodes"));
			//disable electrodes by default
			m_bElectrodesToggledOn = false;
			gtk_toggle_tool_button_set_active(m_pElectrodesToggleButton, m_bElectrodesToggledOn);
			//connect callback
			g_signal_connect(G_OBJECT(m_pElectrodesToggleButton), "toggled", G_CALLBACK(toggleElectrodesCallback), this);

			//get pointer to sampling points toggle button
			//m_pSamplingPointsToggleButton = GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "ToggleSamplingPoints"));
			//disable sampling points by default
			//m_bSamplingPointsToggledOn = false;
			//gtk_toggle_tool_button_set_active(m_pSamplingPointsToggleButton, m_bSamplingPointsToggledOn);
			//connect callback
			//g_signal_connect(G_OBJECT(m_pSamplingPointsToggleButton), "toggled", G_CALLBACK(toggleSamplingPointsCallback), this);

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
		}

		CTopographicMap3DView::~CTopographicMap3DView()
		{
			//unref the xml file as it's not needed anymore
			g_object_unref(G_OBJECT(m_pBuilderInterface));
			m_pBuilderInterface=NULL;
		}

		void CTopographicMap3DView::init()
		{
			//reflect default interpolation type
			m_rTopographicMapDatabase.setInterpolationType(m_ui64CurrentInterpolation);
			enableInterpolationButtonSignals(false);
			gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(m_pMapPotentials), m_ui64CurrentInterpolation == OVP_TypeId_SphericalLinearInterpolationType_Spline);
			gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(m_pMapCurrents), m_ui64CurrentInterpolation == OVP_TypeId_SphericalLinearInterpolationType_Laplacian);
			enableInterpolationButtonSignals(true);
		}

		void CTopographicMap3DView::getToolbar(GtkWidget*& pToolbarWidget)
		{
			pToolbarWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "Toolbar"));
		}

		void CTopographicMap3DView::setInterpolationCB(GtkWidget* pWidget)
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
		}

		void CTopographicMap3DView::toggleElectrodesCB()
		{
			m_bElectrodesToggledOn = !m_bElectrodesToggledOn;
			gtk_toggle_tool_button_set_active(m_pElectrodesToggleButton, m_bElectrodesToggledOn);
			m_rTopographicMap3DDisplay.toggleElectrodes(m_bElectrodesToggledOn);
		}

		/*void CTopographicMap3DView::toggleSamplingPointsCB()
		{
			m_bSamplingPointsToggledOn = !m_bSamplingPointsToggledOn;
			gtk_toggle_tool_button_set_active(m_pSamplingPointsToggleButton, m_bSamplingPointsToggledOn);
			m_rTopographicMap3DDisplay.toggleSamplingPoints(m_bSamplingPointsToggledOn);
		}*/

		void CTopographicMap3DView::setDelayCB(float64 f64Value)
		{
			m_rTopographicMapDatabase.setDelay(f64Value);
		}

		void CTopographicMap3DView::enableInterpolationButtonSignals(boolean enable)
		{
			if(enable == true)
			{
				g_signal_connect(G_OBJECT(m_pMapPotentials), "toggled", G_CALLBACK (setInterpolationCallback), this);
				g_signal_connect(G_OBJECT(m_pMapCurrents), "toggled", G_CALLBACK (setInterpolationCallback), this);
			}
			else
			{
				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pMapPotentials), reinterpret_cast<void*>(G_CALLBACK (setInterpolationCallback)), this);
				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pMapCurrents), reinterpret_cast<void*>(G_CALLBACK (setInterpolationCallback)), this);
			}
		}

		//CALLBACKS

		void setInterpolationCallback(GtkWidget* pWidget, gpointer data)
		{
			CTopographicMap3DView* l_pTopographicMap3DView = reinterpret_cast<CTopographicMap3DView*>(data);
			l_pTopographicMap3DView->setInterpolationCB(pWidget);
		}

		void toggleElectrodesCallback(GtkWidget* pWidget, gpointer data)
		{
			CTopographicMap3DView* l_pTopographicMap3DView = reinterpret_cast<CTopographicMap3DView*>(data);
			l_pTopographicMap3DView->toggleElectrodesCB();
		}

		/*void toggleSamplingPointsCallback(GtkWidget* pWidget, gpointer data)
		{
			CTopographicMap3DView* l_pTopographicMap3DView = reinterpret_cast<CTopographicMap3DView*>(data);
			l_pTopographicMap3DView->toggleSamplingPointsCB();
		}*/

		void setDelayCallback(GtkRange* pRange, gpointer data)
		{
			CTopographicMap3DView* l_pTopographicMap3DView = reinterpret_cast<CTopographicMap3DView*>(data);
			l_pTopographicMap3DView->setDelayCB(gtk_range_get_value(pRange));
		}
	};
};
