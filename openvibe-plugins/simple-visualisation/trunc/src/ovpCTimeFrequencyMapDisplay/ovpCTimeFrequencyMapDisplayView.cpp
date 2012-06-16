#include "ovpCTimeFrequencyMapDisplayView.h"

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
		//Gtk widget callbacks
		void timeFrequencyMapToggleLeftRulerButtonCallback(::GtkWidget *widget, gpointer data);
		void timeFrequencyMapToggleBottomRulerButtonCallback(::GtkWidget *widget, gpointer data);
		gboolean timeFrequencyMapTimeScaleChangedCallback(::GtkSpinButton *widget,  gpointer data);
		void timeFrequencyMapChannelSelectButtonCallback(::GtkButton *button, gpointer data);
		void timeFrequencyMapChannelSelectDialogApplyButtonCallback(::GtkButton *button, gpointer data);
		gboolean timeFrequencyMapMinMaxAttenuationSpinButtonValueChangedCallback(::GtkSpinButton *widget,  gpointer data);
		void timeFrequencyMapMinDisplayedFrequencyChangedCallback(GtkSpinButton *widget, gpointer data);
		void timeFrequencyMapMaxDisplayedFrequencyChangedCallback(GtkSpinButton *widget, gpointer data);

		//
		//CTimeFrequencyMapDisplayView implementation
		//

		CTimeFrequencyMapDisplayView::CTimeFrequencyMapDisplayView(CSpectrumDatabase& rSpectrumDatabase,
			float64 f64MinDisplayedFrequency, float64 f64MaxDisplayedFrequency, float64 f64TimeScale) :
			m_rSpectrumDatabase(rSpectrumDatabase),
			m_pBuilderInterface(NULL),
			m_pDisplayTable(NULL),
			m_bShowLeftRulers(false),
			m_bShowBottomRuler(true),
			m_f64Attenuation(0),
			m_f64MinDisplayedFrequency(f64MinDisplayedFrequency),
			m_f64MaxDisplayedFrequency(f64MaxDisplayedFrequency),
			m_pBottomBox(NULL),
			m_pBottomRuler(NULL)
		{
			//load the gtk builder interface
			m_pBuilderInterface=::gtk_builder_new(); // ::glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-TimeFrequencyMapDisplay.ui", NULL, NULL);
			gtk_builder_add_from_file(m_pBuilderInterface, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-TimeFrequencyMapDisplay.ui", NULL);

			if(!m_pBuilderInterface)
			{
				g_warning("Couldn't load the interface!");
				return;
			}

			::gtk_builder_connect_signals(m_pBuilderInterface, NULL);

			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayChannelSelectButton")), "clicked",       G_CALLBACK(timeFrequencyMapChannelSelectButtonCallback),     this);
			GtkSpinButton* l_pSpinButton = GTK_SPIN_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayTimeScaleButton"));
			::gtk_spin_button_set_value(l_pSpinButton, f64TimeScale);
			g_signal_connect(G_OBJECT(l_pSpinButton), "value-changed", G_CALLBACK(timeFrequencyMapTimeScaleChangedCallback),  this);
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayMinMaxAttenuationButton")), "value-changed", G_CALLBACK(timeFrequencyMapMinMaxAttenuationSpinButtonValueChangedCallback),  this);

			//channel select dialog's signals
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayChannelSelectApplyButton")), "clicked", G_CALLBACK(timeFrequencyMapChannelSelectDialogApplyButtonCallback), this);

			//connect the cancel button to the dialog's hide command
			g_signal_connect_swapped(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayChannelSelectCancelButton")),
				"clicked",
				G_CALLBACK(::gtk_widget_hide),
				G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayChannelSelectDialog")));

			//hides the dialog if the user tries to close it
			g_signal_connect (G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayChannelSelectDialog")),
				"delete_event", G_CALLBACK(::gtk_widget_hide), NULL);

			//does nothing on the main window if the user tries to close it
			g_signal_connect (G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayMainWindow")),
				"delete_event", G_CALLBACK(::gtk_widget_hide), NULL);

			// gets attenuation
			::gtk_spin_button_set_value(GTK_SPIN_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayMinMaxAttenuationButton")), 0.9);
			m_f64Attenuation=::gtk_spin_button_get_value(GTK_SPIN_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayMinMaxAttenuationButton")));

			//bottom box
			m_pBottomBox = GTK_BOX(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayBottomBox"));
		}

		CTimeFrequencyMapDisplayView::~CTimeFrequencyMapDisplayView()
		{
			//destroy channel widgets
			for(uint32 i=0 ; i<m_vChannelDisplays.size() ; i++)
			{
				delete m_vChannelDisplays[i];
			}

			//unref the xml file as it's not needed anymore
			g_object_unref(G_OBJECT(m_pBuilderInterface));
			m_pBuilderInterface=NULL;
		}

		boolean CTimeFrequencyMapDisplayView::init()
		{
			//tell database about time scale to display
			m_rSpectrumDatabase.setTimeScale(
				::gtk_spin_button_get_value(
					GTK_SPIN_BUTTON(
						::gtk_builder_get_object(
							m_pBuilderInterface, "TimeFrequencyMapDisplayTimeScaleButton"))));

			//tell database about frequency range to display
			//TODO
			//m_rSpectrumDatabase.setDisplayedFrequencyRange(m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);

			//retrieve channel count
			uint32 l_ui32ChannelCount = m_rSpectrumDatabase.getChannelCount();

			//create array of channel displays
			m_vChannelDisplays.resize(l_ui32ChannelCount);
			m_vChannelLabels.resize(l_ui32ChannelCount);

			//get pointer to channels table
			m_pDisplayTable = GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayMainTable"));

			//resize this table according to the number of channels
			::gtk_table_resize(GTK_TABLE(m_pDisplayTable), (l_ui32ChannelCount)*2, 3);

			int32 l_i32LeftRulerWidthRequest = 50;
			int32 l_i32ChannelDisplayWidthRequest = 20;
			int32 l_i32BottomRulerWidthRequest = 0;

			int32 l_i32LeftRulerHeightRequest = 20;
			int32 l_i32ChannelDisplayHeightRequest = 20;
			int32 l_i32HorizontalSeparatorHeightRequest = 5;
			int32 l_i32BottomRulerHeightRequest = 20;

			//sets a minimum size for the table (needed to scroll)
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
				0, (l_ui32ChannelCount)*2, //whole table height
				GTK_SHRINK, static_cast< ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), 0, 0);
			::gtk_widget_show(l_pSeparator);

			//size group for the channel labels and the empty widget in the bottom box
			//(useful to position the bottom time ruler correctly)
			GtkSizeGroup* l_pSizeGroup = ::gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

			//channels selection widget
			GtkWidget* l_pChannelSelectList = GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayChannelSelectList"));

			stringstream l_oLabelString;

			::GtkListStore* l_pChannelListStore=::gtk_list_store_new(1, G_TYPE_STRING);
			::GtkTreeIter l_oChannelIter;

			//create channel widgets and add them to display table
			for(uint32 i=0 ; i<l_ui32ChannelCount ; i++)
			{
				//add channel label
				//-----------------
				CString l_sChannelLabel;
				m_rSpectrumDatabase.getChannelLabel(i, l_sChannelLabel);
				if(l_sChannelLabel == CString(""))
				{
					//if no name has been set, use channel index
					l_oLabelString << "Channel " << i;
				}
				else //prepend name with channel index
				{
					l_oLabelString << i << " : " << l_sChannelLabel;
				}
				GtkWidget* l_pLabel =  ::gtk_label_new(l_oLabelString.str().c_str());
				m_vChannelLabels[i] = l_pLabel;
				::gtk_table_attach(GTK_TABLE(m_pDisplayTable),l_pLabel,
					0, 1, //first column
					i*2, (i*2)+1, //ith line
					GTK_FILL, GTK_SHRINK,
					0, 0);
				::gtk_widget_show(l_pLabel);
				::gtk_size_group_add_widget(l_pSizeGroup, l_pLabel);

				//create channel display widget
				//-----------------------------
				m_vChannelDisplays[i] = new CTimeFrequencyMapChannelDisplay(
					m_rSpectrumDatabase, i, m_f64Attenuation,
					m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency,
					l_i32ChannelDisplayWidthRequest, l_i32ChannelDisplayHeightRequest,
					l_i32LeftRulerWidthRequest, l_i32LeftRulerHeightRequest);
				::gtk_table_attach(GTK_TABLE(m_pDisplayTable), m_vChannelDisplays[i]->getTopWidget(),
					2, 3, //3rd column
					(i*2), (i*2)+1, //ith line
					static_cast< ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), static_cast< ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), 0, 0);
				::gtk_widget_show_all(m_vChannelDisplays[i]->getTopWidget());

				//add a horizontal separator
				//--------------------------
				l_pSeparator = ::gtk_hseparator_new();
				::gtk_table_attach(GTK_TABLE(m_pDisplayTable), l_pSeparator,
					0, 3, //whole width of the table
					(i*2)+1, (i*2)+2,	//ith line (bottom)
					static_cast< ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 0, 0);
				::gtk_widget_show(l_pSeparator);
				m_vSeparator[i]=l_pSeparator;

				//add checkbox in channel select window
				//-------------------------------------
				::gtk_list_store_append(l_pChannelListStore, &l_oChannelIter);
				::gtk_list_store_set(l_pChannelListStore, &l_oChannelIter, 0, l_sChannelLabel.toASCIIString(), -1);

				//a channel is selected by default
				m_vSelectedChannels[i]=true;

				//clear label
				l_oLabelString.str("");
			}

			::gtk_tree_selection_set_mode(::gtk_tree_view_get_selection(GTK_TREE_VIEW(l_pChannelSelectList)), GTK_SELECTION_MULTIPLE);
			::gtk_tree_view_append_column(GTK_TREE_VIEW(l_pChannelSelectList), ::gtk_tree_view_column_new_with_attributes("Channel", ::gtk_cell_renderer_text_new(), "text", 0, NULL));
			::gtk_tree_view_set_model(GTK_TREE_VIEW(l_pChannelSelectList), GTK_TREE_MODEL(l_pChannelListStore));

			//bottom ruler
			//------------
			m_pBottomRuler = new CTimeRuler(m_rSpectrumDatabase, l_i32BottomRulerWidthRequest, l_i32BottomRulerHeightRequest);
			::gtk_size_group_add_widget(l_pSizeGroup, GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayBottomBoxLabel1")));
			::gtk_box_pack_start(m_pBottomBox, m_pBottomRuler->getWidget(), false, false, 0);
			if(m_vChannelDisplays.size() != 0)
			{
				m_pBottomRuler->linkWidthToWidget(m_vChannelDisplays[0]->getMapWidget());
			}
			::gtk_widget_show_all(m_pBottomRuler->getWidget());

			//displayed frequencies spin buttons
			//----------------------------------

			//retrieve input frequency range
			float64 l_f64MinFrequency = m_rSpectrumDatabase.getFrequencyBandStart(0);
			float64 l_f64MaxFrequency = m_rSpectrumDatabase.getFrequencyBandStop(m_rSpectrumDatabase.getFrequencyBandCount()-1);

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
			//FIXME
			/*m_pBufferDatabase->setMinDisplayedFrequency(m_f64MinDisplayedFrequency);
			m_pBufferDatabase->setMaxDisplayedFrequency(m_f64MaxDisplayedFrequency);*/

			//initialize spin buttons and connect callbacks
			GtkSpinButton* l_pMinFrequencySpinButton = GTK_SPIN_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayMinFrequencyButton"));
			if(l_pMinFrequencySpinButton != NULL)
			{
				::gtk_spin_button_set_range(l_pMinFrequencySpinButton, 0, l_f64MaxFrequency);
				::gtk_spin_button_set_value(l_pMinFrequencySpinButton, m_f64MinDisplayedFrequency);
				g_signal_connect(G_OBJECT(l_pMinFrequencySpinButton), "value-changed", G_CALLBACK(timeFrequencyMapMinDisplayedFrequencyChangedCallback), this);
			}

			GtkSpinButton* l_pMaxFrequencySpinButton = GTK_SPIN_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayMaxFrequencyButton"));
			if(l_pMaxFrequencySpinButton != NULL)
			{
				::gtk_spin_button_set_range(l_pMaxFrequencySpinButton, 0, l_f64MaxFrequency);
				::gtk_spin_button_set_value(l_pMaxFrequencySpinButton, m_f64MaxDisplayedFrequency);
				g_signal_connect(G_OBJECT(l_pMaxFrequencySpinButton), "value-changed", G_CALLBACK(timeFrequencyMapMaxDisplayedFrequencyChangedCallback), this);
			}

			for(uint32 i=0; i<m_vChannelDisplays.size(); i++)
			{
				m_vChannelDisplays[i]->setDisplayedFrequencyRange(m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);
			}

			//set up initial states
			//---------------------

			//display left rulers
			toggleLeftRulers(false);
			::gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayToggleLeftRulerButton")), m_bShowLeftRulers);
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayToggleLeftRulerButton")),	"toggled", G_CALLBACK(timeFrequencyMapToggleLeftRulerButtonCallback), this);

			//display bottom ruler
			toggleBottomRuler(true);
			::gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayToggleBottomRulerButton")), m_bShowBottomRuler);
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayToggleBottomRulerButton")),	"toggled", G_CALLBACK(timeFrequencyMapToggleBottomRulerButtonCallback), this);

			activateToolbarButtons(true);

			return true;
		}

		boolean CTimeFrequencyMapDisplayView::redraw()
		{
			for(size_t i=0 ; i<m_vChannelDisplays.size() ; i++)
			{
				m_vChannelDisplays[i]->update();
			}

			//nothing to redraw if the table isn't visible
			if(m_pDisplayTable && GTK_WIDGET_VISIBLE(m_pDisplayTable))
			{
				//if(m_pBufferDatabase->getChannelCount() > 0)
				{
					//The ruler needs to be redrawn
					if(GTK_WIDGET(m_pBottomRuler->getWidget())->window)
						gdk_window_invalidate_rect(GTK_WIDGET(m_pBottomRuler->getWidget())->window,
							NULL,
							true);

					//The channels table needs to be redrawn
					if(GTK_WIDGET(m_pDisplayTable)->window)
						gdk_window_invalidate_rect(GTK_WIDGET(m_pDisplayTable)->window,
							NULL,
							true);
				}
			}

			return true;
		}

		void CTimeFrequencyMapDisplayView::getWidgets(::GtkWidget*& pWidget, ::GtkWidget*& pToolbarWidget)
		{
			pWidget=GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayScrolledWindow"));
			pToolbarWidget=GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "Toolbar"));
		}

		void CTimeFrequencyMapDisplayView::toggleLeftRulers(boolean bActive)
		{
			m_bShowLeftRulers = bActive;

			for(size_t i=0 ; i<m_vChannelDisplays.size() ; i++)
			{
				if(isChannelDisplayVisible(i) == true)
				{
					m_vChannelDisplays[i]->toggleLeftRuler(bActive);
				}
			}
		}

		void CTimeFrequencyMapDisplayView::toggleBottomRuler(boolean bActive)
		{
			m_bShowBottomRuler = bActive;

			if(bActive)
			{
				::gtk_widget_show_all(GTK_WIDGET(m_pBottomBox));
			}
			else
			{
				::gtk_widget_hide_all(GTK_WIDGET(m_pBottomBox));
			}
		}

		void CTimeFrequencyMapDisplayView::toggleChannel(uint32 ui32ChannelIndex, boolean bActive)
		{
			// CTimeFrequencyMapChannelDisplay* l_pChannelDisplay = getChannelDisplay(ui32ChannelIndex);

			if(bActive)
			{
				::gtk_widget_show(m_vChannelLabels[ui32ChannelIndex]);
				if(m_bShowLeftRulers == true)
				{
					// ::gtk_widget_show(l_pChannelDisplay->getRulerWidget());
				}
				::gtk_widget_show(m_vChannelDisplays[ui32ChannelIndex]->getTopWidget());
				::gtk_widget_show(m_vSeparator[ui32ChannelIndex]);
			}
			else
			{
				::gtk_widget_hide(m_vChannelLabels[ui32ChannelIndex]);
				::gtk_widget_hide(m_vChannelDisplays[ui32ChannelIndex]->getTopWidget());
				::gtk_widget_hide(m_vSeparator[ui32ChannelIndex]);
			}
		}

		void CTimeFrequencyMapDisplayView::updateMainTableStatus()
		{
			uint32 l_ui32Index=0;

			boolean l_bChannels=false;
			for(uint32 i=0; i<m_vChannelLabels.size(); i++)
			{
				l_bChannels|=m_vSelectedChannels[i];
				if(m_vSelectedChannels[i])
				{
					l_ui32Index=i;
				}
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
				m_pBottomRuler->linkWidthToWidget(m_vChannelDisplays[l_ui32Index]->getMapWidget());
			}
		}

		void CTimeFrequencyMapDisplayView::activateToolbarButtons(boolean bActive)
		{
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayToggleLeftRulerButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayToggleBottomRulerButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayTimeScaleItem")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayChannelSelectButton")), bActive);
		}

		void CTimeFrequencyMapDisplayView::setMinMaxAttenuation(float64 f64Attenuation)
		{
			m_f64Attenuation=f64Attenuation;

			for(uint32 i=0 ; i<m_vChannelDisplays.size() ; i++)
			{
				m_vChannelDisplays[i]->setMinMaxAttenuation(m_f64Attenuation);
			}
		}

		void CTimeFrequencyMapDisplayView::minDisplayedFrequencyChangedCB(GtkWidget* pWidget)
		{
			m_f64MinDisplayedFrequency = ::gtk_spin_button_get_value(GTK_SPIN_BUTTON(pWidget));

			//TODO (if min/max computation should be restricted to this range)
			//m_rSpectrumDatabase.setDisplayedFrequencyRange(m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);

			for(uint32 i=0 ; i<m_vChannelDisplays.size() ; i++)
			{
				m_vChannelDisplays[i]->setDisplayedFrequencyRange(m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);
			}

			//redraw left ruler
			//TODO
			//gdk_window_invalidate_rect(m_pLeftRuler->window, NULL, true);
		}

		void CTimeFrequencyMapDisplayView::maxDisplayedFrequencyChangedCB(GtkWidget* pWidget)
		{
			m_f64MaxDisplayedFrequency = ::gtk_spin_button_get_value(GTK_SPIN_BUTTON(pWidget));

			//TODO (if min/max computation should be restricted to this range)
			//m_rSpectrumDatabase.setDisplayedFrequencyRange(m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);

			for(uint32 i=0 ; i<m_vChannelDisplays.size() ; i++)
			{
				m_vChannelDisplays[i]->setDisplayedFrequencyRange(m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);
			}

			//redraw left ruler
			//TODO
			//gdk_window_invalidate_rect(m_pLeftRuler->window, NULL, true);
		}

		void CTimeFrequencyMapDisplayView::timeScaleChangedCB(GtkSpinButton* pWidget)
		{
			boolean l_bNumberOfDisplayedBufferChanged = m_rSpectrumDatabase.setTimeScale(::gtk_spin_button_get_value(pWidget));

			if(l_bNumberOfDisplayedBufferChanged)
			{
				redraw();
			}
		}

		void CTimeFrequencyMapDisplayView::channelSelectionDialogOpenedCB(GtkButton* pButton)
		{
			::GtkWidget * l_pChannelSelectDialog = GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayChannelSelectDialog"));
			::GtkTreeView* l_pChannelSelectTreeView = GTK_TREE_VIEW(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayChannelSelectList"));
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
		}

		void CTimeFrequencyMapDisplayView::channelSelectionChangedCB(GtkButton* pButton)
		{
			::GtkTreeView* l_pChannelSelectTreeView = GTK_TREE_VIEW(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayChannelSelectList"));
			::GtkTreeSelection* l_pChannelSelectTreeSelection = ::gtk_tree_view_get_selection(l_pChannelSelectTreeView);
			::GtkTreeModel* l_pChannelSelectTreeModel = ::gtk_tree_view_get_model(l_pChannelSelectTreeView);
			::GtkTreeIter l_oIter;
			uint32 l_ui32Index=0;

			if(::gtk_tree_model_get_iter_first(l_pChannelSelectTreeModel, &l_oIter))
			{
				do
				{
					m_vSelectedChannels[l_ui32Index]=(::gtk_tree_selection_iter_is_selected(l_pChannelSelectTreeSelection, &l_oIter)?true:false);
					this->toggleChannel(l_ui32Index, (::gtk_tree_selection_iter_is_selected(l_pChannelSelectTreeSelection, &l_oIter)?true:false));
					l_ui32Index++;
				}
				while(::gtk_tree_model_iter_next(l_pChannelSelectTreeModel, &l_oIter));
			}

			this->updateMainTableStatus();

			//hides the channel selection dialog
			::gtk_widget_hide(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "TimeFrequencyMapDisplayChannelSelectDialog")));
		}

		boolean CTimeFrequencyMapDisplayView::isChannelDisplayVisible(uint32 ui32ChannelIndex)
		{
			return GTK_WIDGET_VISIBLE(m_vChannelDisplays[ui32ChannelIndex]->getMapWidget());
		}

		//
		//CALLBACKS
		//

		void timeFrequencyMapToggleLeftRulerButtonCallback(::GtkWidget *widget, gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pView = reinterpret_cast < CTimeFrequencyMapDisplayView* >(data);
			l_pView->toggleLeftRulers(::gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget))?true:false);
		}

		void timeFrequencyMapToggleBottomRulerButtonCallback(::GtkWidget *widget, gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pView = reinterpret_cast < CTimeFrequencyMapDisplayView* >(data);
			l_pView->toggleBottomRuler(::gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget))?true:false);
		}

		gboolean timeFrequencyMapTimeScaleChangedCallback(::GtkSpinButton *widget,  gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pView = reinterpret_cast < CTimeFrequencyMapDisplayView* >(data);
			l_pView->timeScaleChangedCB(widget);
			return FALSE;
		}

		//called when the channel select button is pressed (opens the channel selection dialog)
		void timeFrequencyMapChannelSelectButtonCallback(::GtkButton *button, gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pView = reinterpret_cast < CTimeFrequencyMapDisplayView* >(data);
			l_pView->channelSelectionDialogOpenedCB(button);
		}

		//Called when the user press the apply button of the channel selection dialog
		void timeFrequencyMapChannelSelectDialogApplyButtonCallback(::GtkButton *button, gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pView = reinterpret_cast < CTimeFrequencyMapDisplayView* >(data);
			l_pView->channelSelectionChangedCB(button);
		}

		gboolean timeFrequencyMapMinMaxAttenuationSpinButtonValueChangedCallback(::GtkSpinButton *widget,  gpointer data)
		{
			reinterpret_cast < CTimeFrequencyMapDisplayView* >(data)->setMinMaxAttenuation(static_cast < float64 >(::gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget))));
			return FALSE;
		}

		void timeFrequencyMapMinDisplayedFrequencyChangedCallback(GtkSpinButton *widget, gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pTimeFrequencyMapDisplayView = reinterpret_cast < CTimeFrequencyMapDisplayView* >(data);
			l_pTimeFrequencyMapDisplayView->minDisplayedFrequencyChangedCB(GTK_WIDGET(widget));
		}

		void timeFrequencyMapMaxDisplayedFrequencyChangedCallback(GtkSpinButton *widget, gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pTimeFrequencyMapDisplayView = reinterpret_cast < CTimeFrequencyMapDisplayView* >(data);
			l_pTimeFrequencyMapDisplayView->maxDisplayedFrequencyChangedCB(GTK_WIDGET(widget));
		}
	}
}
