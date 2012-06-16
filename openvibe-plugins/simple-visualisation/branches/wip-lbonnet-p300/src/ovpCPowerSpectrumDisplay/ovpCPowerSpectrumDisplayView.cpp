#include "ovpCPowerSpectrumDisplayView.h"

#include <iostream>

#include <sstream>

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
		void powerSpectrumToggleLeftRulerButtonCallback(::GtkWidget* widget, gpointer data);
		void powerSpectrumToggleBottomRulerButtonCallback(::GtkWidget *widget, gpointer data);
		void powerSpectrumToggleVerticalScaleButtonCallback(::GtkToggleButton *togglebutton, gpointer data);
		void powerSpectrumCustomVerticalScaleChangedCallback(::GtkEditable *editable, gpointer data);
		void powerSpectrumChannelSelectButtonCallback(::GtkButton *button, gpointer data);
		void powerSpectrumChannelSelectDialogApplyButtonCallback(::GtkButton *button, gpointer data);
		void powerSpectrumMinDisplayedFrequencyChangedCallback(::GtkSpinButton *widget, gpointer data);
		void powerSpectrumMaxDisplayedFrequencyChangedCallback(::GtkSpinButton *widget, gpointer data);
		gboolean bottomRulerExposeEventCallback(::GtkWidget *widget, ::GdkEventExpose *event, gpointer data);
		gboolean bottomRulerResizeEventCallback(::GtkWidget *widget, ::GtkAllocation *allocation, gpointer data);

		CPowerSpectrumDisplayView::CPowerSpectrumDisplayView(CPowerSpectrumDatabase& oPowerSpectrumDatabase, float64 f64MinDisplayedFrequency, float64 f64MaxDisplayedFrequency)
			:m_pBuilderInterface(NULL)
			,m_pDisplayTable(NULL)
			,m_pBottomBox(NULL)
			,m_pBottomRuler(NULL)
			,m_pAutoVerticalScaleRadioButton(NULL)
			,m_bAutoVerticalScale(true)
			,m_f64CustomVerticalScaleValue(1.)
			,m_pPowerSpectrumDatabase(&oPowerSpectrumDatabase)
			,m_f64MinDisplayedFrequency(f64MinDisplayedFrequency)
			,m_f64MaxDisplayedFrequency(f64MaxDisplayedFrequency)
		{
			//load the gtk builder interface
			m_pBuilderInterface=gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-PowerSpectrumDisplay.ui", NULL, NULL);
			gtk_builder_add_from_file(m_pBuilderInterface, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-PowerSpectrumDisplay.ui", NULL);

			if(!m_pBuilderInterface)
			{
				g_warning("Couldn't load the interface!");
				return;
			}

			gtk_builder_connect_signals(m_pBuilderInterface, NULL);

			//toolbar
			//-------

			//initialize vertical scale
			m_pAutoVerticalScaleRadioButton = GTK_RADIO_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayAutoVerticalScaleButton"));
			m_bAutoVerticalScale = true;
			::gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_pAutoVerticalScaleRadioButton), m_bAutoVerticalScale);
			m_f64CustomVerticalScaleValue = 1;
			::gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayCustomVerticalScaleEntry")), "1");

			//connect vertical scale callbacks
			g_signal_connect(G_OBJECT(m_pAutoVerticalScaleRadioButton),                                                          "toggled", G_CALLBACK(powerSpectrumToggleVerticalScaleButtonCallback), this);
			g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayCustomVerticalScaleButton")), "toggled", G_CALLBACK(powerSpectrumToggleVerticalScaleButtonCallback), this);
			g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayCustomVerticalScaleEntry")),  "changed", G_CALLBACK(powerSpectrumCustomVerticalScaleChangedCallback), this);

			//select channels button
			g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplaySelectChannelsButton")), "clicked", G_CALLBACK(powerSpectrumChannelSelectButtonCallback), this);

			//bottom box
			m_pBottomBox = GTK_BOX(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayBottomBox"));

			//channels selection
			//------------------

			//apply button click
			g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayChannelSelectApplyButton")), "clicked", G_CALLBACK(powerSpectrumChannelSelectDialogApplyButtonCallback), this);
			//cancel button click
			g_signal_connect_swapped(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayChannelSelectCancelButton")),
				"clicked", G_CALLBACK(::gtk_widget_hide), G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayChannelSelectDialog")));
			//hide dialog on delete event
			g_signal_connect (G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayChannelSelectDialog")),
				"delete_event", G_CALLBACK(::gtk_widget_hide), NULL);
		}

		void CPowerSpectrumDisplayView::getWidgets(::GtkWidget*& pWidget, ::GtkWidget*& pToolbarWidget)
		{
			pWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayScrolledWindow"));
			pToolbarWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "Toolbar"));
		}

		CPowerSpectrumDisplayView::~CPowerSpectrumDisplayView()
		{
			//destroy channel widgets
			for(uint32 i=0 ; i<m_oChannelDisplays.size() ; i++)
			{
				delete m_oChannelDisplays[i];
			}

			//unref the xml file as it's not needed anymore
			g_object_unref(G_OBJECT(m_pBuilderInterface));
			m_pBuilderInterface=NULL;
		}

		void CPowerSpectrumDisplayView::init()
		{
			//retrieve channels count
			OpenViBE::uint32 l_ui32ChannelCount = m_pPowerSpectrumDatabase->getChannelCount();

			//allocate channel labels and channel displays arrays accordingly
			m_oChannelLabels.resize(l_ui32ChannelCount);
			m_oChannelDisplays.resize(l_ui32ChannelCount);

			//retrieve and allocate main table accordingly
			m_pDisplayTable = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayMainTable"));
			//rows : for each channel, [0] channel data, [1] horizontal separator
			//columns : [0] labels, [1] vertical separator, [2] spectrum displays
			::gtk_table_resize(GTK_TABLE(m_pDisplayTable), l_ui32ChannelCount*2-1, 3);

			int32 l_i32LeftRulerWidthRequest = 50;
			int32 l_i32ChannelDisplayWidthRequest = 20;
			int32 l_i32BottomRulerWidthRequest = 0;

			int32 l_i32LeftRulerHeightRequest = 20;
			int32 l_i32ChannelDisplayHeightRequest = 20;
			int32 l_i32HorizontalSeparatorHeightRequest = 5;
			int32 l_i32BottomRulerHeightRequest = 20;

			//set a minimum size for the table (needed to scroll)
			::gtk_widget_set_size_request(
				m_pDisplayTable,
				l_i32LeftRulerWidthRequest + l_i32ChannelDisplayWidthRequest,
				//vertical size request is larger than needed and corresponds to the size requested by the signal display plugin. This
				//ensures both plugins look the same when displayed next to each other
				(l_ui32ChannelCount+1)*l_i32ChannelDisplayHeightRequest + l_ui32ChannelCount*l_i32HorizontalSeparatorHeightRequest);

			//add a vertical separator
			::GtkWidget* l_pSeparator = ::gtk_vseparator_new();
			::gtk_table_attach(GTK_TABLE(m_pDisplayTable), l_pSeparator,
				1, 2, //2nd column
				0, l_ui32ChannelCount*2-1, //whole table height
				GTK_SHRINK, static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), 0, 0);
			::gtk_widget_show(l_pSeparator);

			//create a size group for channel labels and the empty bottom left widget
			//(useful to position the bottom ruler correctly)
			::GtkSizeGroup* l_pSizeGroup = ::gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

			//channels selection widget
			::GtkWidget* l_pChannelSelectList = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayChannelSelectList"));

			stringstream l_oLabelString;

			::GtkListStore* l_pChannelListStore=::gtk_list_store_new(1, G_TYPE_STRING);
			::GtkTreeIter l_oChannelIter;

			//create channel widgets and add them to display table
			for(uint32 i=0 ; i<l_ui32ChannelCount ; i++)
			{
				//add channel label
				//-----------------
				CString l_sChannelLabel;
				m_pPowerSpectrumDatabase->getChannelLabel(i, l_sChannelLabel);
				if(l_sChannelLabel == CString(""))
				{
					//if no name was set, use channel index
					l_oLabelString << "Channel " << i;
				}
				else //prepend name with channel index
				{
					l_oLabelString << i <<" : " << l_sChannelLabel;
				}
				::GtkWidget* l_pLabel = ::gtk_label_new(l_oLabelString.str().c_str());
				m_oChannelLabels[i] = l_pLabel;
				::gtk_table_attach(GTK_TABLE(m_pDisplayTable),l_pLabel,
					0, 1, //1st column
					i*2, (i*2)+1, //ith line
					GTK_FILL, GTK_SHRINK,
					0, 0);
				::gtk_widget_show(l_pLabel);
				::gtk_size_group_add_widget(l_pSizeGroup, l_pLabel);

				//create channel display widget
				//-----------------------------
				m_oChannelDisplays[i] = new CPowerSpectrumChannelDisplay(
					this, i, *m_pPowerSpectrumDatabase,
					l_i32ChannelDisplayWidthRequest, l_i32ChannelDisplayHeightRequest,
					l_i32LeftRulerWidthRequest, l_i32LeftRulerHeightRequest);
				::gtk_table_attach(GTK_TABLE(m_pDisplayTable), m_oChannelDisplays[i]->getTopWidget(),
					2, 3, //3rd column
					(i*2), (i*2)+1, //ith line (top)
					static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), 0, 0);
				::gtk_widget_show_all(m_oChannelDisplays[i]->getTopWidget());

				//add a horizontal separator
				//--------------------------
				l_pSeparator = ::gtk_hseparator_new();
				::gtk_table_attach(GTK_TABLE(m_pDisplayTable), l_pSeparator,
					0, 3, //whole width of the table
					(i*2)+1, (i*2)+2, //ith line (bottom)
					static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 0, 0);
				::gtk_widget_show(l_pSeparator);
				m_vSeparator[i]=l_pSeparator;

				//add checkbox in channel select window
				//-------------------------------------
				::gtk_list_store_append(l_pChannelListStore, &l_oChannelIter);
				::gtk_list_store_set(l_pChannelListStore, &l_oChannelIter, 0, l_sChannelLabel.toASCIIString(), -1);

				//select channel by default
				m_vSelectedChannels[i]=true;

				//clear label
				l_oLabelString.str("");
			}

			::gtk_tree_selection_set_mode(::gtk_tree_view_get_selection(GTK_TREE_VIEW(l_pChannelSelectList)), GTK_SELECTION_MULTIPLE);
			::gtk_tree_view_append_column(GTK_TREE_VIEW(l_pChannelSelectList), ::gtk_tree_view_column_new_with_attributes("Channel", ::gtk_cell_renderer_text_new(), "text", 0, NULL));
			::gtk_tree_view_set_model(GTK_TREE_VIEW(l_pChannelSelectList), GTK_TREE_MODEL(l_pChannelListStore));

			//bottom ruler
			//------------
			m_pBottomRuler = ::gtk_drawing_area_new();
			::gtk_widget_set_size_request(m_pBottomRuler, l_i32BottomRulerWidthRequest, l_i32BottomRulerHeightRequest);
			::gtk_size_group_add_widget(l_pSizeGroup, GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayBottomBoxLabel1")));
			::gtk_box_pack_start(m_pBottomBox, m_pBottomRuler, false, false, 0);
			g_signal_connect_after(G_OBJECT(m_pBottomRuler), "expose_event", G_CALLBACK(bottomRulerExposeEventCallback), this);
			g_signal_connect(G_OBJECT(m_oChannelDisplays[0]->getSpectrumDisplayWidget()), "size-allocate", G_CALLBACK(bottomRulerResizeEventCallback), m_pBottomRuler);
			::gtk_widget_show_all(m_pBottomRuler);

			//displayed frequencies spin buttons
			//----------------------------------

			//retrieve input frequency range
			float64 l_f64MinFrequency=0;
			float64 l_f64MaxFrequency=0;
			m_pPowerSpectrumDatabase->getInputFrequencyRange(l_f64MinFrequency, l_f64MaxFrequency);

			//ensure displayed frequencies lie within that range
			if(m_f64MinDisplayedFrequency < l_f64MinFrequency)
			{
				m_f64MinDisplayedFrequency = l_f64MinFrequency;
			}
			else if(m_f64MinDisplayedFrequency > l_f64MaxFrequency)
			{
				m_f64MinDisplayedFrequency = l_f64MaxFrequency;
			}

			if(m_f64MaxDisplayedFrequency < l_f64MinFrequency)
			{
				m_f64MaxDisplayedFrequency = l_f64MinFrequency;
			}
			else if(m_f64MaxDisplayedFrequency > l_f64MaxFrequency)
			{
				m_f64MaxDisplayedFrequency = l_f64MaxFrequency;
			}

			//send displayed frequency range to database so that min/max computations are restricted to it
			m_pPowerSpectrumDatabase->setMinDisplayedFrequency(m_f64MinDisplayedFrequency);
			m_pPowerSpectrumDatabase->setMaxDisplayedFrequency(m_f64MaxDisplayedFrequency);

			//initialize spin buttons and connect callbacks
			::GtkSpinButton* l_pMinFrequencySpinButton = GTK_SPIN_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayMinFrequencyButton"));
			if(l_pMinFrequencySpinButton != NULL)
			{
				::gtk_spin_button_set_range(l_pMinFrequencySpinButton, 0, l_f64MaxFrequency);
				::gtk_spin_button_set_value(l_pMinFrequencySpinButton, m_f64MinDisplayedFrequency);
				g_signal_connect(G_OBJECT(l_pMinFrequencySpinButton), "value-changed", G_CALLBACK(powerSpectrumMinDisplayedFrequencyChangedCallback), this);
			}

			::GtkSpinButton* l_pMaxFrequencySpinButton = GTK_SPIN_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayMaxFrequencyButton"));
			if(l_pMaxFrequencySpinButton != NULL)
			{
				::gtk_spin_button_set_range(l_pMaxFrequencySpinButton, 0, l_f64MaxFrequency);
				::gtk_spin_button_set_value(l_pMaxFrequencySpinButton, m_f64MaxDisplayedFrequency);
				g_signal_connect(G_OBJECT(l_pMaxFrequencySpinButton), "value-changed", G_CALLBACK(powerSpectrumMaxDisplayedFrequencyChangedCallback), this);
			}

			//set up initial states
			//---------------------

			activateToolbarButtons(true);

			//don't display left rulers by default
			boolean l_bDefaultLeftRulersToggleState = false;
			toggleLeftRulers(l_bDefaultLeftRulersToggleState);
			::gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayToggleLeftRulerButton")), l_bDefaultLeftRulersToggleState ? TRUE : FALSE);
			g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayToggleLeftRulerButton")), "toggled", G_CALLBACK(powerSpectrumToggleLeftRulerButtonCallback), this);

			//display bottom ruler by default
			boolean l_bDefaultBottomRulerToggleState = true;
			toggleBottomRuler(l_bDefaultBottomRulerToggleState);
			::gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayToggleBottomRulerButton")), l_bDefaultBottomRulerToggleState ? TRUE : FALSE);
			g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayToggleBottomRulerButton")), "toggled", G_CALLBACK(powerSpectrumToggleBottomRulerButtonCallback), this);
		}

		void CPowerSpectrumDisplayView::redraw()
		{
			//ensure display table is created and visible
			if(m_pDisplayTable == NULL || !GTK_WIDGET_VISIBLE(m_pDisplayTable))
			{
				return;
			}

			boolean l_bHasOneChannel=false;

			//update channel widgets
			for(size_t i=0 ; i<m_vSelectedChannels.size() ; i++)
			{
				if(m_vSelectedChannels[i])
				{
					m_oChannelDisplays[i]->update();
					l_bHasOneChannel=true;
				}
			}

			//redraw channels and rulers
			if(l_bHasOneChannel)
			{
				if(GTK_WIDGET(m_pDisplayTable)->window)
				{
					gdk_window_invalidate_rect(GTK_WIDGET(m_pDisplayTable)->window, NULL, true);
				}
			}
		}

		void CPowerSpectrumDisplayView::redrawBottomRuler()
		{
			//if the widget is invisible, no need to redraw it
			if(m_pBottomRuler == NULL || !GTK_WIDGET_VISIBLE(m_pBottomRuler))
			{
				return;
			}

			//clear window
			//gdk_window_clear(m_pBottomRuler->window);

			//get window size
			gint l_iBottomRulerWidth;
			gint l_iBottomRulerHeight;
			gdk_drawable_get_size(m_pBottomRuler->window, &l_iBottomRulerWidth, &l_iBottomRulerHeight);

			//draw ruler base (horizontal line)
			gdk_draw_line(m_pBottomRuler->window, m_pBottomRuler->style->fg_gc[GTK_WIDGET_STATE (m_pBottomRuler)], 0, 0, l_iBottomRulerWidth, 0);

			//draw frequency labels
			//---------------------

			//number of frequency bands
			uint32 l_ui32FrequencyBandCount = m_pPowerSpectrumDatabase->getDisplayedFrequencyBandCount();
			if(l_ui32FrequencyBandCount == 0)
			{
				return;
			}
			//width available per frequency band label, in pixels
			float32 l_f32FrequencyBandWidth = static_cast<float32>(l_iBottomRulerWidth) / l_ui32FrequencyBandCount;
			if(l_f32FrequencyBandWidth == 0)
			{
				l_f32FrequencyBandWidth = 1;
			}

			//draw frequency band labels
			uint32 l_iFrequencyStep = 1; //step in pixels between two consecutive frequency labels (computed based on available space)
			gint l_iTextWidth = 0;
			gint l_iTextX = 0;
			gint l_iTextStartX = 0;
			PangoLayout* l_pText;
			stringstream l_oFrequencyLabel;
			float64 l_f64FrequencyBandStart=0;
			float64 l_f64FrequencyBandStop=0;
			uint32 l_ui32MinDisplayedFrequencyBandIndex = m_pPowerSpectrumDatabase->getMinDisplayedFrequencyBandIndex();
			uint32 l_ui32MaxDisplayedFrequencyBandIndex = m_pPowerSpectrumDatabase->getMaxDisplayedFrequencyBandIndex();

			for(uint32 i=l_ui32MinDisplayedFrequencyBandIndex; i<=l_ui32MaxDisplayedFrequencyBandIndex; i+=l_iFrequencyStep)
			{
				//create text corresponding to current frequency
				l_oFrequencyLabel.str("");
				m_pPowerSpectrumDatabase->getFrequencyBandRange(i, l_f64FrequencyBandStart, l_f64FrequencyBandStop);
				l_oFrequencyLabel << l_f64FrequencyBandStart;
				l_pText = ::gtk_widget_create_pango_layout(m_pBottomRuler, l_oFrequencyLabel.str().c_str());

				//compute text position
				l_iTextX = static_cast<gint>(l_iTextStartX + (i-l_ui32MinDisplayedFrequencyBandIndex)*l_f32FrequencyBandWidth);

				//increase step if width allocated per label becomes too small
				pango_layout_get_pixel_size(l_pText, &l_iTextWidth, NULL);
				while(l_iTextWidth >= ((l_iBottomRulerWidth - l_iTextStartX)/(l_ui32FrequencyBandCount/(float)l_iFrequencyStep) - 10))
				{
					l_iFrequencyStep++;
				}

				//break if not enough space to display label
				if(l_iTextX + l_iTextWidth >= l_iBottomRulerWidth)
				{
					break;
				}

				//draw label
				gdk_draw_layout(m_pBottomRuler->window, m_pBottomRuler->style->fg_gc[GTK_WIDGET_STATE (m_pBottomRuler)], l_iTextX, 4, l_pText);

				//draw a small line above it
				gdk_draw_line(m_pBottomRuler->window, m_pBottomRuler->style->fg_gc[GTK_WIDGET_STATE (m_pBottomRuler)], l_iTextX, 0, l_iTextX, 3);
			}
		}

		boolean CPowerSpectrumDisplayView::isAutoVerticalScaleEnabled()
		{
			return m_bAutoVerticalScale;
		}

		float64 CPowerSpectrumDisplayView::getCustomVerticalScaleValue()
		{
			return m_f64CustomVerticalScaleValue;
		}

		void CPowerSpectrumDisplayView::toggleLeftRulers(boolean bActive)
		{
			//forward toggle flag to channel displays
			for(size_t i=0 ; i<m_oChannelDisplays.size() ; i++)
			{
				m_oChannelDisplays[i]->toggleLeftRuler(bActive);
			}
		}

		void CPowerSpectrumDisplayView::toggleBottomRuler(boolean bActive)
		{
			if(bActive)
			{
				::gtk_widget_show_all(GTK_WIDGET(m_pBottomBox));
			}
			else
			{
				::gtk_widget_hide_all(GTK_WIDGET(m_pBottomBox));
			}
		}

		void CPowerSpectrumDisplayView::toggleChannel(uint32 ui32ChannelIndex, boolean bActive)
		{
			if(ui32ChannelIndex < m_oChannelDisplays.size())
			{
				if(bActive == true)
				{
					m_oChannelDisplays[ui32ChannelIndex]->toggle(true);
					::gtk_widget_show(m_oChannelLabels[ui32ChannelIndex]);
					::gtk_widget_show(m_vSeparator[ui32ChannelIndex]);
				}
				else
				{
					m_oChannelDisplays[ui32ChannelIndex]->toggle(false);
					::gtk_widget_hide(m_oChannelLabels[ui32ChannelIndex]);
					::gtk_widget_hide(m_vSeparator[ui32ChannelIndex]);
				}
			}
		}

		void CPowerSpectrumDisplayView::minDisplayedFrequencyChangedCB(::GtkWidget* pWidget)
		{
			m_f64MinDisplayedFrequency = ::gtk_spin_button_get_value(GTK_SPIN_BUTTON(pWidget));
			m_pPowerSpectrumDatabase->setMinDisplayedFrequency(m_f64MinDisplayedFrequency);
			//redraw bottom ruler
			if(m_pBottomRuler->window) gdk_window_invalidate_rect(m_pBottomRuler->window, NULL, true);
		}

		void CPowerSpectrumDisplayView::maxDisplayedFrequencyChangedCB(::GtkWidget* pWidget)
		{
			m_f64MaxDisplayedFrequency = ::gtk_spin_button_get_value(GTK_SPIN_BUTTON(pWidget));
			m_pPowerSpectrumDatabase->setMaxDisplayedFrequency(m_f64MaxDisplayedFrequency);
			//redraw bottom ruler
			if(m_pBottomRuler->window) gdk_window_invalidate_rect(m_pBottomRuler->window, NULL, true);
		}

		void CPowerSpectrumDisplayView::updateMainTableStatus()
		{
			boolean l_bChannels=false;
			for(uint32 i=0; i<m_oChannelLabels.size(); i++)
			{
				l_bChannels|=m_vSelectedChannels[i];
			}

			//if nothing has been selected
			if(!l_bChannels)
			{
				//hide the whole table
				::gtk_widget_hide(GTK_WIDGET(m_pDisplayTable));
			}
			else
			{
				if(!GTK_WIDGET_VISIBLE(GTK_WIDGET(m_pDisplayTable)))
				{
					//if there were no selected channels before, but now there are, show the table again
					::gtk_widget_show(GTK_WIDGET(m_pDisplayTable));
				}
			}
		}

		void CPowerSpectrumDisplayView::activateToolbarButtons(boolean bActive)
		{
			::gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayBestFitButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayBestFitPerChannelButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplaySelectChannelsButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayToggleLeftRulerButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayToggleBottomRulerButton")), bActive);
		}

		void CPowerSpectrumDisplayView::showChannelSelectionDialogCB()
		{
#if 0
			::GtkWidget * l_pChannelSelectDialog = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayChannelSelectDialog"));

			//reset all checkbuttons
			for(size_t i=0 ; i<m_vChannelsCheckButtons.size() ; i++)
				::gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_vChannelsCheckButtons[i]), false);

			//check visible channels
			for(size_t i=0 ; i<m_vSelectedChannels.size() ; i++)
				::gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_vChannelsCheckButtons[(size_t)m_vSelectedChannels[i]]), true);

			::gtk_widget_show_all(l_pChannelSelectDialog);
#else
			::GtkWidget * l_pChannelSelectDialog = GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayChannelSelectDialog"));
			::GtkTreeView* l_pChannelSelectTreeView = GTK_TREE_VIEW(::gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayChannelSelectList"));
			::GtkTreeSelection* l_pChannelSelectTreeSelection = ::gtk_tree_view_get_selection(l_pChannelSelectTreeView);
			::GtkTreeModel* l_pChannelSelectTreeModel = ::gtk_tree_view_get_model(l_pChannelSelectTreeView);
			::GtkTreeIter l_oIter;
			uint32 l_ui32Index=0;

			if(::gtk_tree_model_get_iter_first(l_pChannelSelectTreeModel, &l_oIter))
			{
				do
				{
					if(m_vSelectedChannels[l_ui32Index])
					{
						::gtk_tree_selection_select_iter(l_pChannelSelectTreeSelection, &l_oIter);
					}
					else
					{
						::gtk_tree_selection_unselect_iter(l_pChannelSelectTreeSelection, &l_oIter);
					}
					l_ui32Index++;
				}
				while(::gtk_tree_model_iter_next(l_pChannelSelectTreeModel, &l_oIter));
			}

			//finally, show the information dialog
			::gtk_widget_show_all(l_pChannelSelectDialog);
#endif
		}

		void CPowerSpectrumDisplayView::applyChannelSelectionCB()
		{
#if 0
			//clears the list of selected channels
			m_vSelectedChannels.clear();

			for(unsigned int i=0 ; i<m_vChannelsCheckButtons.size() ; i++)
			{
				if(::gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_vChannelsCheckButtons[i])))
				{
					//if a button is checked, add the corresponding channel to the list of selected ones
					m_vSelectedChannels.push_back(i);
					//show the channel's display
					toggleChannel(i, true);
				}
				else
				{
					//this channel is not selected, hides it
					toggleChannel(i, false);
				}
			}

			updateMainTableStatus();

			//hides the channel selection dialog
			::gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayChannelSelectDialog")));
#else
			::GtkTreeView* l_pChannelSelectTreeView = GTK_TREE_VIEW(::gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayChannelSelectList"));
			::GtkTreeSelection* l_pChannelSelectTreeSelection = ::gtk_tree_view_get_selection(l_pChannelSelectTreeView);
			::GtkTreeModel* l_pChannelSelectTreeModel = ::gtk_tree_view_get_model(l_pChannelSelectTreeView);
			::GtkTreeIter l_oIter;
			uint32 l_ui32Index=0;

			if(::gtk_tree_model_get_iter_first(l_pChannelSelectTreeModel, &l_oIter))
			{
				do
				{
					m_vSelectedChannels[l_ui32Index]=(::gtk_tree_selection_iter_is_selected(l_pChannelSelectTreeSelection, &l_oIter)?true:false);
					toggleChannel(l_ui32Index, (::gtk_tree_selection_iter_is_selected(l_pChannelSelectTreeSelection, &l_oIter)?true:false));
					l_ui32Index++;
				}
				while(::gtk_tree_model_iter_next(l_pChannelSelectTreeModel, &l_oIter));
			}

			updateMainTableStatus();

			//hides the channel selection dialog
			::gtk_widget_hide(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "PowerSpectrumDisplayChannelSelectDialog")));
#endif
		}

		boolean CPowerSpectrumDisplayView::onVerticalScaleModeToggledCB(::GtkToggleButton* pToggleButton)
		{
			m_bAutoVerticalScale = (pToggleButton == GTK_TOGGLE_BUTTON(m_pAutoVerticalScaleRadioButton));
			return true;
		}

		boolean CPowerSpectrumDisplayView::onCustomVerticalScaleChangedCB(::GtkEditable *pEditable)
		{
			float32 l_f32Value;
			int32 l_i32NbValuesRead = sscanf(::gtk_entry_get_text(GTK_ENTRY(pEditable)), "%f", &l_f32Value);

			if(l_i32NbValuesRead == 1)
			{
				m_f64CustomVerticalScaleValue = l_f32Value;
			}

			return l_i32NbValuesRead == 1;
		}

		//CALLBACKS

		void powerSpectrumToggleLeftRulerButtonCallback(::GtkWidget* widget, gpointer data)
		{
			CPowerSpectrumDisplayView* l_pPowerSpectrumDisplayView = reinterpret_cast<CPowerSpectrumDisplayView*>(data);
			l_pPowerSpectrumDisplayView->toggleLeftRulers(::gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget))?true:false);
		}

		void powerSpectrumToggleBottomRulerButtonCallback(::GtkWidget *widget, gpointer data)
		{
			CPowerSpectrumDisplayView* l_pPowerSpectrumDisplayView = reinterpret_cast<CPowerSpectrumDisplayView*>(data);
			l_pPowerSpectrumDisplayView->toggleBottomRuler(::gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget))?true:false);
		}

		void powerSpectrumToggleVerticalScaleButtonCallback(::GtkToggleButton *togglebutton, gpointer data)
		{
			CPowerSpectrumDisplayView* l_pView = reinterpret_cast<CPowerSpectrumDisplayView*>(data);
			if(::gtk_toggle_button_get_active(togglebutton) != 0)
			{
				l_pView->onVerticalScaleModeToggledCB(togglebutton);
			}
		}

		void powerSpectrumCustomVerticalScaleChangedCallback(::GtkEditable *editable, gpointer data)
		{
			CPowerSpectrumDisplayView* l_pView = reinterpret_cast<CPowerSpectrumDisplayView*>(data);
			l_pView->onCustomVerticalScaleChangedCB(editable);
		}

		//called when the channel select button is pressed (opens the channel selection dialog)
		void powerSpectrumChannelSelectButtonCallback(::GtkButton *button, gpointer data)
		{
			CPowerSpectrumDisplayView* l_pPowerSpectrumDisplayView = reinterpret_cast<CPowerSpectrumDisplayView*>(data);
			l_pPowerSpectrumDisplayView->showChannelSelectionDialogCB();
		}

		//Called when the user presses the apply button of the channel selection dialog
		void powerSpectrumChannelSelectDialogApplyButtonCallback(::GtkButton *button, gpointer data)
		{
			CPowerSpectrumDisplayView* l_pPowerSpectrumDisplayView = reinterpret_cast<CPowerSpectrumDisplayView*>(data);
			l_pPowerSpectrumDisplayView->applyChannelSelectionCB();
		}

		void powerSpectrumMinDisplayedFrequencyChangedCallback(::GtkSpinButton *widget, gpointer data)
		{
			CPowerSpectrumDisplayView* l_pPowerSpectrumDisplayView = reinterpret_cast<CPowerSpectrumDisplayView*>(data);
			l_pPowerSpectrumDisplayView->minDisplayedFrequencyChangedCB(GTK_WIDGET(widget));
		}

		void powerSpectrumMaxDisplayedFrequencyChangedCallback(::GtkSpinButton *widget, gpointer data)
		{
			CPowerSpectrumDisplayView* l_pPowerSpectrumDisplayView = reinterpret_cast<CPowerSpectrumDisplayView*>(data);
			l_pPowerSpectrumDisplayView->maxDisplayedFrequencyChangedCB(GTK_WIDGET(widget));
		}

		//! Callback to redraw the bottom ruler
		gboolean bottomRulerExposeEventCallback(::GtkWidget *widget, ::GdkEventExpose *event, gpointer data)
		{
			CPowerSpectrumDisplayView* l_pPowerSpectrumDisplayView = reinterpret_cast<CPowerSpectrumDisplayView*>(data);
			l_pPowerSpectrumDisplayView->redrawBottomRuler();
			//don't propagate this signal to children if any
			return TRUE;
		}

		gboolean bottomRulerResizeEventCallback(::GtkWidget *widget, ::GtkAllocation *allocation, gpointer data)
		{
			//resizes the bottom ruler
			::gtk_widget_set_size_request(reinterpret_cast < ::GtkWidget* >(data), allocation->width, 20/*-1*/);

			return FALSE;
		}

	};
};
