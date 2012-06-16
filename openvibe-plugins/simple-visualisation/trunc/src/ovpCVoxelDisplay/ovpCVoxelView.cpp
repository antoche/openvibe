#include "ovpCVoxelView.h"

#include <iostream>

#include <sstream>

#include <cmath>
#include <cstring>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace OpenViBEToolkit;

using namespace std;

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		static void setVoxelObjectCallback(GtkButton *button, gpointer data);
		static void toggleColorModificationCallback(::GtkWidget* pWidget, gpointer data);
		static void toggleTransparencyModificationCallback(::GtkWidget* pWidget, gpointer data);
		static void toggleSizeModificationCallback(::GtkWidget* pWidget, gpointer data);
		static gboolean setMinScaleFactorCallback(::GtkWidget* pWidget, gpointer data);
		static gboolean setMaxScaleFactorCallback(::GtkWidget* pWidget, gpointer data);
		static void setMinDisplayThresholdCallback(::GtkRange *range, gpointer data);
		static void setMaxDisplayThresholdCallback(::GtkRange *range, gpointer data);
		static void setMinDisplayThresholdBoundaryTypeCallback(::GtkButton *button, gpointer data);
		static void setMaxDisplayThresholdBoundaryTypeCallback(::GtkButton *button, gpointer data);
		static void setSkullOpacityCallback(::GtkRange *range, gpointer data);
		static void toggleAnimateCameraCallback(GtkButton *button, gpointer data);
		static void repositionCameraCallback(GtkButton *button, gpointer data);

		CVoxelView::CVoxelView(CVoxelDisplay& rVoxelDisplay) :
			m_rVoxelDisplay(rVoxelDisplay),
			m_pBuilderInterface(NULL),
			m_pCubeButton(NULL),
			m_pSphereButton(NULL),
			m_pMinScaleFactorSpinButton(NULL),
			m_pMaxScaleFactorSpinButton(NULL),
			m_f64MinScaleFactor(1),
			m_f64MaxScaleFactor(2),
			m_pThresholdRangeAndOrLabel(NULL),
			m_pMinDisplayThresholdBoundaryButton(NULL),
			m_pMaxDisplayThresholdBoundaryButton(NULL),
			m_pMinDisplayThresholdScale(NULL),
			m_pMaxDisplayThresholdScale(NULL),
			m_f64MinDisplayThreshold(0.25),
			m_f64MaxDisplayThreshold(0.75)
		{
			//load the gtk builder interface
			m_pBuilderInterface=gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-VoxelDisplay.ui", NULL, NULL);
			gtk_builder_add_from_file(m_pBuilderInterface, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-VoxelDisplay.ui", NULL);

			if(!m_pBuilderInterface)
			{
				g_warning("Couldn't load the interface!");
				return;
			}

			gtk_builder_connect_signals(m_pBuilderInterface, NULL);

			//toolbar
			//-------

			//voxel object buttons
			m_pCubeButton = GTK_RADIO_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "CubeButton"));
			m_pSphereButton = GTK_RADIO_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "SphereButton"));

			g_signal_connect(G_OBJECT(m_pCubeButton), "toggled", G_CALLBACK(setVoxelObjectCallback), this);
			g_signal_connect(G_OBJECT(m_pSphereButton), "toggled", G_CALLBACK(setVoxelObjectCallback), this);

			g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "ModifyColorToolButton")), "toggled", G_CALLBACK(toggleColorModificationCallback), this);
			g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "ModifyTransparencyToolButton")), "toggled", G_CALLBACK(toggleTransparencyModificationCallback), this);
			g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "ModifySizeToolButton")), "toggled", G_CALLBACK(toggleSizeModificationCallback), this);

			//min voxel scale factor
			m_pMinScaleFactorSpinButton = GTK_SPIN_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "MinScaleFactorSpinButton"));
			m_rVoxelDisplay.setMinScaleFactor(m_f64MinScaleFactor);
			gtk_spin_button_configure(
				m_pMinScaleFactorSpinButton,
				GTK_ADJUSTMENT(gtk_adjustment_new(
					m_f64MinScaleFactor, //initial value
					0, //lower
					5, //upper
					0.1, //step increment
					0.5, //page increment
					0)), //page size - deprecated
					0.1, //climb rate
					1); //digits
			g_signal_connect(G_OBJECT(m_pMinScaleFactorSpinButton), "value-changed", G_CALLBACK(setMinScaleFactorCallback), this);

			//max voxel scale factor
			m_pMaxScaleFactorSpinButton = GTK_SPIN_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "MaxScaleFactorSpinButton"));
			m_rVoxelDisplay.setMaxScaleFactor(m_f64MaxScaleFactor);
			gtk_spin_button_configure(
				m_pMaxScaleFactorSpinButton,
				GTK_ADJUSTMENT(gtk_adjustment_new(
					m_f64MaxScaleFactor, //initial value
					0, //lower
					5, //upper
					0.1, //step increment
					0.5, //page increment
					0)), //page size - deprecated
					0.1, //climb rate
					1); //digits
			g_signal_connect(G_OBJECT(m_pMaxScaleFactorSpinButton), "value-changed", G_CALLBACK(setMaxScaleFactorCallback), this);

			boolean l_bInclusiveDisplayThresholdBoundary = true;
			m_rVoxelDisplay.setDisplayThresholdBoundaryType(l_bInclusiveDisplayThresholdBoundary);

			//AND/OR label
			m_pThresholdRangeAndOrLabel = GTK_LABEL(gtk_builder_get_object(m_pBuilderInterface, "ThresholdRangeAndOrLabel"));
			gtk_label_set_label(m_pThresholdRangeAndOrLabel, l_bInclusiveDisplayThresholdBoundary? "AND" : "OR");

			//min display threshold boundary type
			m_pMinDisplayThresholdBoundaryButton = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "MinDisplayThresholdBoundaryButton"));
			gtk_button_set_label(m_pMinDisplayThresholdBoundaryButton, l_bInclusiveDisplayThresholdBoundary ? ">" : "<");
			g_signal_connect(G_OBJECT(m_pMinDisplayThresholdBoundaryButton), "clicked", G_CALLBACK(setMinDisplayThresholdBoundaryTypeCallback), this);

			//max display threshold boundary type
			m_pMaxDisplayThresholdBoundaryButton = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "MaxDisplayThresholdBoundaryButton"));
			gtk_button_set_label(m_pMaxDisplayThresholdBoundaryButton, l_bInclusiveDisplayThresholdBoundary ? "<" : ">");
			g_signal_connect(G_OBJECT(m_pMaxDisplayThresholdBoundaryButton), "clicked", G_CALLBACK(setMaxDisplayThresholdBoundaryTypeCallback), this);

			//min display threshold slider
			m_pMinDisplayThresholdScale = GTK_SCALE(gtk_hscale_new_with_range(0.0, 1.0, 0.01));
			m_rVoxelDisplay.setMinDisplayThreshold(m_f64MinDisplayThreshold);
			gtk_range_set_value(GTK_RANGE(m_pMinDisplayThresholdScale), m_f64MinDisplayThreshold);
			gtk_scale_set_value_pos(m_pMinDisplayThresholdScale, GTK_POS_TOP);
			gtk_range_set_update_policy(GTK_RANGE(m_pMinDisplayThresholdScale), GTK_UPDATE_CONTINUOUS);
			gtk_widget_set_size_request(GTK_WIDGET(m_pMinDisplayThresholdScale), 100, -1);
			gtk_widget_show_all(GTK_WIDGET(m_pMinDisplayThresholdScale));
			g_signal_connect(G_OBJECT(m_pMinDisplayThresholdScale), "value_changed", G_CALLBACK(setMinDisplayThresholdCallback), this);

			//replace existing scale (which somehow can't be used) with the newly created one
			GtkWidget* l_pOldMinScale = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "MinDisplayThresholdScale"));
			GtkWidget* l_pMinScaleParent = gtk_widget_get_parent(l_pOldMinScale);
			if(l_pMinScaleParent != NULL && GTK_IS_CONTAINER(l_pMinScaleParent))
			{
				gtk_container_remove(GTK_CONTAINER(l_pMinScaleParent), l_pOldMinScale);
				if(GTK_IS_BOX(l_pMinScaleParent))
				{
					gtk_box_pack_start(GTK_BOX(l_pMinScaleParent), GTK_WIDGET(m_pMinDisplayThresholdScale), TRUE, TRUE, 0);
					gtk_box_reorder_child(GTK_BOX(l_pMinScaleParent), GTK_WIDGET(m_pMinDisplayThresholdScale), 1);
				}
			}

			//max display threshold slider
			m_pMaxDisplayThresholdScale = GTK_SCALE(gtk_hscale_new_with_range(0.0, 1.0, 0.01));
			m_rVoxelDisplay.setMaxDisplayThreshold(m_f64MaxDisplayThreshold);
			gtk_range_set_value(GTK_RANGE(m_pMaxDisplayThresholdScale), m_f64MaxDisplayThreshold);
			gtk_scale_set_value_pos(m_pMaxDisplayThresholdScale, GTK_POS_TOP);
			gtk_range_set_update_policy(GTK_RANGE(m_pMaxDisplayThresholdScale), GTK_UPDATE_CONTINUOUS);
			gtk_widget_set_size_request(GTK_WIDGET(m_pMaxDisplayThresholdScale), 100, -1);
			gtk_widget_show_all(GTK_WIDGET(m_pMaxDisplayThresholdScale));
			g_signal_connect(G_OBJECT(m_pMaxDisplayThresholdScale), "value_changed", G_CALLBACK(setMaxDisplayThresholdCallback), this);

			//replace existing scale (which somehow can't be used) with the newly created one
			GtkWidget* l_pOldMaxScale = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "MaxDisplayThresholdScale"));
			GtkWidget* l_pMaxScaleParent = gtk_widget_get_parent(l_pOldMaxScale);
			if(l_pMaxScaleParent != NULL && GTK_IS_CONTAINER(l_pMaxScaleParent))
			{
				gtk_container_remove(GTK_CONTAINER(l_pMaxScaleParent), l_pOldMaxScale);
				if(GTK_IS_BOX(l_pMaxScaleParent))
				{
					gtk_box_pack_start(GTK_BOX(l_pMaxScaleParent), GTK_WIDGET(m_pMaxDisplayThresholdScale), TRUE, TRUE, 0);
					gtk_box_reorder_child(GTK_BOX(l_pMaxScaleParent), GTK_WIDGET(m_pMaxDisplayThresholdScale), 1);
				}
			}

			//skull opacity slider
			GtkWidget* l_pSkullOpacityScale = gtk_hscale_new_with_range(0.0, 1.0, 0.05);
			float64 l_f64SkullOpacity = 0.07;
			gtk_range_set_value(GTK_RANGE(l_pSkullOpacityScale), l_f64SkullOpacity);
			m_rVoxelDisplay.setSkullOpacity(l_f64SkullOpacity);
			gtk_scale_set_value_pos(GTK_SCALE(l_pSkullOpacityScale), GTK_POS_TOP);
			gtk_range_set_update_policy(GTK_RANGE(l_pSkullOpacityScale), GTK_UPDATE_CONTINUOUS);
			gtk_widget_set_size_request(l_pSkullOpacityScale, 100, -1);
			gtk_widget_show_all(l_pSkullOpacityScale);
			g_signal_connect(G_OBJECT(l_pSkullOpacityScale), "value_changed", G_CALLBACK(setSkullOpacityCallback), this);

			//replace existing scale (which somehow can't be used) with the newly created one
			GtkWidget* l_pOldScale = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "SkullOpacityScale"));
			GtkWidget* l_pScaleParent = gtk_widget_get_parent(l_pOldScale);
			if(l_pScaleParent != NULL && GTK_IS_CONTAINER(l_pScaleParent))
			{
				gtk_container_remove(GTK_CONTAINER(l_pScaleParent), l_pOldScale);
				if(GTK_IS_BOX(l_pScaleParent))
				{
					gtk_box_pack_start(GTK_BOX(l_pScaleParent), l_pSkullOpacityScale, TRUE, TRUE, 0);
					gtk_box_reorder_child(GTK_BOX(l_pScaleParent), l_pSkullOpacityScale, 0);
				}
			}

			//camera animation button
			GtkToggleToolButton* l_pAnimateCameraButton = GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "AnimateCameraButton"));
			g_signal_connect(G_OBJECT(l_pAnimateCameraButton), "toggled", G_CALLBACK(toggleAnimateCameraCallback), this);

			//reposition camera
			g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "RepositionCamera")),	"clicked", G_CALLBACK(repositionCameraCallback), this);

			this->toggleColorModificationCB(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "ModifyColorToolButton")))?true:false);
			this->toggleTransparencyModificationCB(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "ModifyTransparencyToolButton")))?true:false);
			this->toggleSizeModificationCB(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "ModifySizeToolButton")))?true:false);
			this->enableAutoCameraMovementCB(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "AnimateCameraButton")))?true:false);
		}

		CVoxelView::~CVoxelView()
		{
			//unref the xml file as it's not needed anymore
			g_object_unref(G_OBJECT(m_pBuilderInterface));
			m_pBuilderInterface=NULL;
		}

		boolean CVoxelView::init()
		{
			//setup database parameters here
			return true;
		}

		boolean CVoxelView::redraw()
		{
			//drawing handled by CVoxelDisplay
			return true;
		}

		void CVoxelView::getToolbar(GtkWidget*& pToolbarWidget)
		{
			pToolbarWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "Toolbar"));
		}

		void CVoxelView::setVoxelObjectCB(GtkWidget* pWidget)
		{
			if(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(pWidget)) == FALSE)
			{
				return;
			}

			if(pWidget == GTK_WIDGET(m_pCubeButton))
			{
				m_rVoxelDisplay.setVoxelObject(Standard3DObject_Cube);
			}
			else if(pWidget == GTK_WIDGET(m_pSphereButton))
			{
				m_rVoxelDisplay.setVoxelObject(Standard3DObject_Sphere);
			}
		}

		void CVoxelView::toggleColorModificationCB(boolean bModifyColor)
		{
			m_rVoxelDisplay.toggleColorModification(bModifyColor);
		}

		void CVoxelView::toggleTransparencyModificationCB(boolean bModifyTransparency)
		{
			m_rVoxelDisplay.toggleTransparencyModification(bModifyTransparency);
		}

		void CVoxelView::toggleSizeModificationCB(boolean bModifySize)
		{
			m_rVoxelDisplay.toggleSizeModification(bModifySize);
		}

		void CVoxelView::setMinVoxelScaleFactorCB(GtkSpinButton* pButton)
		{
			m_f64MinScaleFactor = gtk_spin_button_get_value(pButton);
			m_rVoxelDisplay.setMinScaleFactor(m_f64MinScaleFactor);

			//ensure minimum value is smaller than maximum value
			if(m_f64MinScaleFactor > gtk_spin_button_get_value(m_pMaxScaleFactorSpinButton))
			{
				m_f64MaxScaleFactor = m_f64MinScaleFactor;
				m_rVoxelDisplay.setMinScaleFactor(m_f64MinScaleFactor);

				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pMaxScaleFactorSpinButton), (void*)(G_CALLBACK (setMaxDisplayThresholdCallback)), this);
				gtk_spin_button_set_value(m_pMaxScaleFactorSpinButton, m_f64MaxScaleFactor);
				g_signal_connect(G_OBJECT(m_pMaxDisplayThresholdScale), "value_changed", G_CALLBACK(setMaxDisplayThresholdCallback), this);
			}
		}

		void CVoxelView::setMaxVoxelScaleFactorCB(GtkSpinButton* pButton)
		{
			m_f64MaxScaleFactor = gtk_spin_button_get_value(pButton);
			m_rVoxelDisplay.setMaxScaleFactor(m_f64MaxScaleFactor);

			//ensure maximum value is bigger than minimum value
			if(m_f64MaxScaleFactor < gtk_spin_button_get_value(m_pMinScaleFactorSpinButton))
			{
				m_f64MinScaleFactor = m_f64MaxScaleFactor;
				m_rVoxelDisplay.setMinScaleFactor(m_f64MinScaleFactor);

				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pMinScaleFactorSpinButton), (void*)(G_CALLBACK (setMinDisplayThresholdCallback)), this);
				gtk_spin_button_set_value(m_pMinScaleFactorSpinButton, m_f64MinScaleFactor);
				g_signal_connect(G_OBJECT(m_pMinDisplayThresholdScale), "value_changed", G_CALLBACK(setMinDisplayThresholdCallback), this);
			}
		}

		void CVoxelView::setMinDisplayThresholdCB(float64 f64MinDisplayThreshold)
		{
			m_f64MinDisplayThreshold = f64MinDisplayThreshold;
			m_rVoxelDisplay.setMinDisplayThreshold(m_f64MinDisplayThreshold);

			//ensure minimum value is smaller than maximum value
			if(m_f64MinDisplayThreshold > m_f64MaxDisplayThreshold)
			{
				m_f64MaxDisplayThreshold = m_f64MinDisplayThreshold;
				m_rVoxelDisplay.setMaxDisplayThreshold(m_f64MaxDisplayThreshold);

				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pMaxDisplayThresholdScale), (void*)(G_CALLBACK (setMaxDisplayThresholdCallback)), this);
				gtk_range_set_value(GTK_RANGE(m_pMaxDisplayThresholdScale), m_f64MaxDisplayThreshold);
				g_signal_connect(G_OBJECT(m_pMaxDisplayThresholdScale), "value_changed", G_CALLBACK(setMaxDisplayThresholdCallback), this);
			}
		}

		void CVoxelView::setMaxDisplayThresholdCB(float64 f64MaxDisplayThreshold)
		{
			m_f64MaxDisplayThreshold = f64MaxDisplayThreshold;
			m_rVoxelDisplay.setMaxDisplayThreshold(m_f64MaxDisplayThreshold);

			//ensure maximum value is bigger than minimum value
			if(m_f64MaxDisplayThreshold < m_f64MinDisplayThreshold)
			{
				m_f64MinDisplayThreshold = m_f64MaxDisplayThreshold;
				m_rVoxelDisplay.setMinDisplayThreshold(m_f64MinDisplayThreshold);

				g_signal_handlers_disconnect_by_func(G_OBJECT(m_pMinDisplayThresholdScale), (void*)(G_CALLBACK (setMinDisplayThresholdCallback)), this);
				gtk_range_set_value(GTK_RANGE(m_pMinDisplayThresholdScale), m_f64MinDisplayThreshold);
				g_signal_connect(G_OBJECT(m_pMinDisplayThresholdScale), "value_changed", G_CALLBACK(setMinDisplayThresholdCallback), this);
			}
		}

		void CVoxelView::setDisplayThresholdBoundaryTypeCB(boolean bInclusiveBoundary)
		{
			gtk_button_set_label(m_pMinDisplayThresholdBoundaryButton, bInclusiveBoundary ? ">" : "<");
			gtk_button_set_label(m_pMaxDisplayThresholdBoundaryButton, bInclusiveBoundary ? "<" : ">");
			gtk_label_set_label(m_pThresholdRangeAndOrLabel, bInclusiveBoundary? "AND" : "OR");
			m_rVoxelDisplay.setDisplayThresholdBoundaryType(bInclusiveBoundary);
		}

		void CVoxelView::setSkullOpacityCB(float64 f64Opacity)
		{
			m_rVoxelDisplay.setSkullOpacity(f64Opacity);
		}

		void CVoxelView::enableAutoCameraMovementCB(boolean bEnable)
		{
			m_rVoxelDisplay.enableAutoCameraMovement(bEnable);
		}

		void CVoxelView::repositionCameraCB()
		{
			m_rVoxelDisplay.repositionCamera();
		}

		//CALLBACKS

		void setVoxelObjectCallback(GtkButton *pButton, gpointer data)
		{
			CVoxelView* l_pVoxelView = reinterpret_cast<CVoxelView*>(data);
			l_pVoxelView->setVoxelObjectCB(GTK_WIDGET(pButton));
		}

		void toggleColorModificationCallback(GtkWidget* pWidget, gpointer data)
		{
			CVoxelView* l_pVoxelView = reinterpret_cast<CVoxelView*>(data);
			l_pVoxelView->toggleColorModificationCB(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(pWidget)) != 0);
		}

		void toggleTransparencyModificationCallback(GtkWidget* pWidget, gpointer data)
		{
			CVoxelView* l_pVoxelView = reinterpret_cast<CVoxelView*>(data);
			l_pVoxelView->toggleTransparencyModificationCB(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(pWidget)) != 0);
		}

		void toggleSizeModificationCallback(GtkWidget* pWidget, gpointer data)
		{
			CVoxelView* l_pVoxelView = reinterpret_cast<CVoxelView*>(data);
			l_pVoxelView->toggleSizeModificationCB(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(pWidget)) != 0);
		}

		gboolean setMinScaleFactorCallback(GtkWidget* pWidget, gpointer data)
		{
			CVoxelView* l_pView = reinterpret_cast<CVoxelView*>(data);
			l_pView->setMinVoxelScaleFactorCB(GTK_SPIN_BUTTON(pWidget));
			return FALSE;
		}

		gboolean setMaxScaleFactorCallback(GtkWidget* pWidget, gpointer data)
		{
			CVoxelView* l_pView = reinterpret_cast<CVoxelView*>(data);
			l_pView->setMaxVoxelScaleFactorCB(GTK_SPIN_BUTTON(pWidget));
			return FALSE;
		}

		void setMinDisplayThresholdCallback(::GtkRange *pRange, gpointer data)
		{
			CVoxelView* l_pVoxelView = reinterpret_cast<CVoxelView*>(data);
			l_pVoxelView->setMinDisplayThresholdCB(gtk_range_get_value(pRange));
		}

		void setMaxDisplayThresholdCallback(::GtkRange *pRange, gpointer data)
		{
			CVoxelView* l_pVoxelView = reinterpret_cast<CVoxelView*>(data);
			l_pVoxelView->setMaxDisplayThresholdCB(gtk_range_get_value(pRange));
		}

		void setMinDisplayThresholdBoundaryTypeCallback(::GtkButton* pButton, gpointer data)
		{
			boolean l_bIsCurrentBoundaryTypeInclusive = strcmp(gtk_button_get_label(pButton), ">") == 0;
			CVoxelView* l_pVoxelView = reinterpret_cast<CVoxelView*>(data);
			l_pVoxelView->setDisplayThresholdBoundaryTypeCB(!l_bIsCurrentBoundaryTypeInclusive);
		}

		void setMaxDisplayThresholdBoundaryTypeCallback(::GtkButton* pButton, gpointer data)
		{
			boolean l_bIsCurrentBoundaryTypeInclusive = strcmp(gtk_button_get_label(pButton), "<") == 0;
			CVoxelView* l_pVoxelView = reinterpret_cast<CVoxelView*>(data);
			l_pVoxelView->setDisplayThresholdBoundaryTypeCB(!l_bIsCurrentBoundaryTypeInclusive);
		}

		void setSkullOpacityCallback(::GtkRange *pRange, gpointer data)
		{
			CVoxelView* l_pVoxelView = reinterpret_cast<CVoxelView*>(data);
			l_pVoxelView->setSkullOpacityCB(gtk_range_get_value(pRange));
		}

		void toggleAnimateCameraCallback(GtkButton *button, gpointer data)
		{
			CVoxelView* l_pVoxelView = reinterpret_cast<CVoxelView*>(data);
			l_pVoxelView->enableAutoCameraMovementCB(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(button)) != 0);
		}

		void repositionCameraCallback(GtkButton *pButton, gpointer data)
		{
			CVoxelView* l_pVoxelView = reinterpret_cast<CVoxelView*>(data);
			l_pVoxelView->repositionCameraCB();
		}
	};
};
