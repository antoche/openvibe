#include "ovpCSignalDisplayView.h"

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
		void scrollModeButtonCallback(::GtkWidget *widget, gpointer data);
		void scanModeButtonCallback(::GtkWidget *widget, gpointer data);
		void toggleLeftRulerButtonCallback(::GtkWidget *widget, gpointer data);
		void toggleBottomRulerButtonCallback(::GtkWidget *widget, gpointer data);
		void toggleAutoVerticalScaleButtonCallback(::GtkToggleButton *togglebutton, gpointer data);
		void customVerticalScaleChangedCallback(::GtkSpinButton* pSpinButton, gpointer data);
		gboolean spinButtonValueChangedCallback(::GtkSpinButton *widget,  gpointer data);
		void channelSelectButtonCallback(::GtkButton *button, gpointer data);
		void channelSelectDialogApplyButtonCallback(::GtkButton *button, gpointer data);
		void stimulationColorsButtonCallback(::GtkButton *button, gpointer data);
		void informationButtonCallback(::GtkButton *button, gpointer data);
		void multiViewButtonCallback(::GtkButton *button, gpointer data);
		void multiViewDialogApplyButtonCallback(::GtkButton *button, gpointer data);

		CSignalDisplayView::CSignalDisplayView(CBufferDatabase& oBufferDatabase, float64 f64TimeScale, CIdentifier oDisplayMode, boolean bAutoVerticalScale, float64 f64VerticalScale)
			:m_pBuilderInterface(NULL)
			,m_pMainWindow(NULL)
			,m_pSignalDisplayTable(NULL)
			,m_bShowLeftRulers(false)
			,m_bShowBottomRuler(true)
			,m_ui64LeftmostDisplayedTime(0)
			,m_f64LargestDisplayedValueRange(0)
			,m_f64ValueRangeMargin(0)
			,m_f64MarginFactor(0.4f) //add 40% space above and below extremums
			,m_bVerticalScaleChanged(false)
			,m_bAutoVerticalScale(true)
			,m_f64CustomVerticalScaleValue(1.)
			,m_pBufferDatabase(&oBufferDatabase)
			,m_bMultiViewInitialized(false)
			,m_pBottomBox(NULL)
			,m_pBottomRuler(NULL)
		{
			m_bAutoVerticalScale=bAutoVerticalScale;
			m_bVerticalScaleChanged=!bAutoVerticalScale;
			if(!bAutoVerticalScale)
			{
				m_f64CustomVerticalScaleValue=f64VerticalScale;
			}
			construct(oBufferDatabase,f64TimeScale,oDisplayMode);
		}

		CSignalDisplayView::CSignalDisplayView(CBufferDatabase& oBufferDatabase, float64 f64TimeScale, CIdentifier oDisplayMode)
			:m_pBuilderInterface(NULL)
			,m_pMainWindow(NULL)
			,m_pSignalDisplayTable(NULL)
			,m_bShowLeftRulers(false)
			,m_bShowBottomRuler(true)
			,m_ui64LeftmostDisplayedTime(0)
			,m_f64LargestDisplayedValueRange(0)
			,m_f64ValueRangeMargin(0)
			,m_f64MarginFactor(0.4f) //add 40% space above and below extremums
			,m_bVerticalScaleChanged(false)
			,m_bAutoVerticalScale(true)
			,m_f64CustomVerticalScaleValue(1.)
			,m_pBufferDatabase(&oBufferDatabase)
			,m_bMultiViewInitialized(false)
			,m_pBottomBox(NULL)
			,m_pBottomRuler(NULL)
		{
			construct(oBufferDatabase,f64TimeScale,oDisplayMode);
		}

		void CSignalDisplayView::construct(CBufferDatabase& oBufferDatabase, float64 f64TimeScale, CIdentifier oDisplayMode)
		{
			//load the gtk builder interface
			m_pBuilderInterface=::gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-SignalDisplay.ui", NULL, NULL);
			gtk_builder_add_from_file(m_pBuilderInterface, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-SignalDisplay.ui", NULL);

			if(!m_pBuilderInterface)
			{
				g_warning("Couldn't load the interface!");
				return;
			}

			::gtk_builder_connect_signals(m_pBuilderInterface, NULL);

			//initialize display mode
			m_pBufferDatabase->setDisplayMode(oDisplayMode);
			::gtk_toggle_tool_button_set_active(
				GTK_TOGGLE_TOOL_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayScrollModeButton")),
				oDisplayMode == OVP_TypeId_SignalDisplayMode_Scroll);

			//connect display mode callbacks
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayScrollModeButton")), "toggled", G_CALLBACK (scrollModeButtonCallback), this);
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayScanModeButton")),   "toggled", G_CALLBACK (scanModeButtonCallback),   this);

			//creates the cursors
			m_pCursor[0] = gdk_cursor_new(GDK_LEFT_PTR);
			m_pCursor[1] = gdk_cursor_new(GDK_SIZING);

			//button callbacks
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayChannelSelectButton")),     "clicked", G_CALLBACK(channelSelectButtonCallback),     this);
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayStimulationColorsButton")), "clicked", G_CALLBACK(stimulationColorsButtonCallback), this);
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayMultiViewButton")),         "clicked", G_CALLBACK(multiViewButtonCallback),         this);
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayInformationButton")),       "clicked", G_CALLBACK(informationButtonCallback),       this);

			//initialize vertical scale
			::gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayVerticalScaleToggleButton")), m_bAutoVerticalScale);
			::gtk_spin_button_set_value(GTK_SPIN_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayCustomVerticalScaleSpinButton")), m_f64CustomVerticalScaleValue);
			::gtk_spin_button_set_increments(GTK_SPIN_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayCustomVerticalScaleSpinButton")),0.001,1.0);
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayCustomVerticalScaleSpinButton")), !m_bAutoVerticalScale);

			//connect vertical scale callbacks
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayVerticalScaleToggleButton")),     "toggled",       G_CALLBACK(toggleAutoVerticalScaleButtonCallback), this);
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayCustomVerticalScaleSpinButton")), "value-changed", G_CALLBACK(customVerticalScaleChangedCallback), this);

			//time scale
			//----------
			::GtkSpinButton* l_pSpinButton = GTK_SPIN_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayTimeScale"));
			::gtk_spin_button_set_value(l_pSpinButton, f64TimeScale);
			g_signal_connect(G_OBJECT(l_pSpinButton), "value-changed", G_CALLBACK(spinButtonValueChangedCallback),  this);
			//notify database of current time scale
			m_pBufferDatabase->adjustNumberOfDisplayedBuffers(::gtk_spin_button_get_value(l_pSpinButton));

			//channel select dialog's signals
			//-------------------------------
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayChannelSelectApplyButton")), "clicked", G_CALLBACK(channelSelectDialogApplyButtonCallback), this);

			//connect the cancel button to the dialog's hide command
			g_signal_connect_swapped(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayChannelSelectCancelButton")),
					"clicked",
					G_CALLBACK(::gtk_widget_hide),
					G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayChannelSelectDialog")));

			//hides the dialog if the user tries to close it
			g_signal_connect (G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayChannelSelectDialog")),
					"delete_event",
					G_CALLBACK(::gtk_widget_hide), NULL);

			//stimulation colors dialog's signals
			//-----------------------------------
			//connect the close button to the dialog's hide command
			g_signal_connect_swapped(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayStimulationColorsCloseButton")),
					"clicked",
					G_CALLBACK(::gtk_widget_hide),
					G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayStimulationColorsDialog")));

			//hides the dialog if the user tries to close it
			g_signal_connect (G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayStimulationColorsDialog")),
					"delete_event",
					G_CALLBACK(::gtk_widget_hide), NULL);

			//multiview signals
			//-----------------
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayMultiViewApplyButton")), "clicked", G_CALLBACK(multiViewDialogApplyButtonCallback), this);

			//connect the cancel button to the dialog's hide command
			g_signal_connect_swapped(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayMultiViewCancelButton")),
				"clicked",
				G_CALLBACK(::gtk_widget_hide),
				G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayMultiViewDialog")));

			//hides the dialog if the user tries to close it
			g_signal_connect (G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayMultiViewDialog")),
				"delete_event",
				G_CALLBACK(::gtk_widget_hide), NULL);

			//bottom box
			//----------
			m_pBottomBox = GTK_BOX(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayBottomBox"));
		}

		CSignalDisplayView::~CSignalDisplayView()
		{
			//destroy the window and its children
			::gtk_widget_destroy(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayMainWindow")));

			//destroy the rest
			for(int i=0 ; i<2 ; i++)
			{
				gdk_cursor_unref(m_pCursor[i]);
			}

			//unref the xml file as it's not needed anymore
			g_object_unref(G_OBJECT(m_pBuilderInterface));
			m_pBuilderInterface=NULL;
		}

		void CSignalDisplayView::getWidgets(::GtkWidget*& pWidget, ::GtkWidget*& pToolbarWidget)
		{
			pWidget=GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayScrolledWindow"));
			pToolbarWidget=GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "Toolbar"));
		}

		void CSignalDisplayView::changeMultiView()
		{
			if(!m_bMultiViewInitialized)
			{
				return;
			}

			CSignalChannelDisplay* l_pChannelDisplay = getChannelDisplay(m_oChannelDisplay.size()-1);

			//if there are no channels to display in the multiview
			boolean l_bMultiView=false;
			for(uint32 i=0; i<m_oChannelLabel.size(); i++)
			{
				l_bMultiView|=m_vMultiViewSelectedChannels[i];
			}

			if(!l_bMultiView)
			{
				//hides the multiview display (last one in the list)
				l_pChannelDisplay->resetChannelList();
				toggleChannel(m_oChannelDisplay.size() - 1, false);
			}
			//there are channels to display in the multiview
			else
			{
				if(!GTK_WIDGET_VISIBLE(GTK_WIDGET(m_pSignalDisplayTable)))
				{
					//if there were no selected channels before, but now there are, show the table again
					::gtk_widget_show(GTK_WIDGET(m_pSignalDisplayTable));
				}

				if(isChannelDisplayVisible(m_oChannelDisplay.size()-1) == false)
				{
					toggleChannel(m_oChannelDisplay.size() - 1, true);
				}

				//updates channels to display list
				l_pChannelDisplay->resetChannelList();

				for(size_t i=0 ; i<m_vMultiViewSelectedChannels.size() ; i++)
				{
					if(m_vMultiViewSelectedChannels[i])
					{
						l_pChannelDisplay->addChannel(i);
					}
				}

				//request a redraw
				if(l_pChannelDisplay->getSignalDisplayWidget()->window) gdk_window_invalidate_rect(l_pChannelDisplay->getSignalDisplayWidget()->window, NULL, false);
			}
		}

		void CSignalDisplayView::init()
		{
			//retrieve channel count
			OpenViBE::uint32 l_ui32ChannelCount = (uint32)m_pBufferDatabase->getChannelCount();

			//allocate channel labels and channel displays arrays accordingly
			m_oChannelDisplay.resize(l_ui32ChannelCount+1);
			m_oChannelLabel.resize(l_ui32ChannelCount+1);

			//retrieve and allocate main table accordingly
			m_pSignalDisplayTable = GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayMainTable"));
			//rows : for each channel, [0] channel data, [1] horizontal separator
			//columns : [0] label, [1] vertical separator, [2] left ruler, [3] signal display
			::gtk_table_resize(GTK_TABLE(m_pSignalDisplayTable), l_ui32ChannelCount*2+1, 4);

			int32 l_i32LeftRulerWidthRequest = 50;
			int32 l_i32ChannelDisplayWidthRequest = 20;
			int32 l_i32BottomRulerWidthRequest = 0;

			int32 l_i32LeftRulerHeightRequest = 20;
			int32 l_i32ChannelDisplayHeightRequest = 20;
			int32 l_i32HorizontalSeparatorHeightRequest = 5;
			int32 l_i32BottomRulerHeightRequest = 20;

			//sets a minimum size for the table (needed to scroll)
			::gtk_widget_set_size_request(
				m_pSignalDisplayTable,
				l_i32LeftRulerWidthRequest + l_i32ChannelDisplayHeightRequest,
				(l_ui32ChannelCount+1)*l_i32ChannelDisplayHeightRequest + l_ui32ChannelCount*l_i32HorizontalSeparatorHeightRequest);

			//add a vertical separator
			::GtkWidget* l_pSeparator = ::gtk_vseparator_new();
			::gtk_table_attach(GTK_TABLE(m_pSignalDisplayTable), l_pSeparator,
				1, 2, //second column
				0, l_ui32ChannelCount*2+1, //run over the whole table height
				GTK_SHRINK, static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), 0, 0);
			::gtk_widget_show(l_pSeparator);

			//create a size group for channel labels and the empty bottom left widget
			//(useful to position the bottom ruler correctly)
			::GtkSizeGroup* l_pSizeGroup = ::gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

			//channels selection widget
			::GtkWidget * l_pChannelSelectList = GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayChannelSelectList"));

			//multiple channels selection widget
			::GtkWidget * l_pMultiViewSelectList = GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayMultiViewSelectList"));

			//vector of channel names
			vector<string>& l_oChannelName = m_pBufferDatabase->m_pDimmesionLabels[0];

			stringstream l_oLabelString;

			::GtkListStore* l_pChannelListStore=::gtk_list_store_new(1, G_TYPE_STRING);
			::GtkTreeIter l_oChannelIter;

			//create channel widgets and add them to display table
			for(uint32 i=0 ; i<l_ui32ChannelCount ; i++)
			{
				//add channel label
				//-----------------
				if(l_oChannelName[i] == "")
				{
					//if no name has been set, use channel index
					l_oLabelString << "Channel " << i;
				}
				else //prepend name with channel index
				{
					l_oLabelString << i << " : " << l_oChannelName[i];
				}
				::GtkWidget* l_pLabel = ::gtk_label_new(l_oLabelString.str().c_str());
				m_oChannelLabel[i] = l_pLabel;
				::gtk_table_attach(GTK_TABLE(m_pSignalDisplayTable),l_pLabel,
					0, 1, //first column
					i*2, (i*2)+1,
					GTK_FILL, GTK_SHRINK,
					0, 0);
				::gtk_widget_show(l_pLabel);
				::gtk_size_group_add_widget(l_pSizeGroup, l_pLabel);

				//create channel display widget
				//-----------------------------
				m_oChannelDisplay[i] = new CSignalChannelDisplay(
					this,
					l_i32ChannelDisplayWidthRequest, l_i32ChannelDisplayHeightRequest,
					l_i32LeftRulerWidthRequest, l_i32LeftRulerHeightRequest);
				m_oChannelDisplay[i]->addChannel(i);

				::gtk_table_attach(GTK_TABLE(m_pSignalDisplayTable),
					m_oChannelDisplay[i]->getRulerWidget(),
					2, 3, //third column
					i*2, (i*2)+1,
					GTK_FILL, GTK_FILL,	0, 0);
				::gtk_widget_show(m_oChannelDisplay[i]->getRulerWidget());
				::gtk_table_attach(GTK_TABLE(m_pSignalDisplayTable),
					m_oChannelDisplay[i]->getSignalDisplayWidget(),
					3, 4, //fourth column
					i*2, (i*2)+1,
					static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), 0, 0);
				::gtk_widget_show(m_oChannelDisplay[i]->getSignalDisplayWidget());

				//add horizontal separator
				//------------------------
				l_pSeparator = ::gtk_hseparator_new();
				::gtk_table_attach(GTK_TABLE(m_pSignalDisplayTable), l_pSeparator,
					0, 4, //whole width of the table
					(i*2)+1, (i*2)+2, //ith line (bottom)
					static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 0, 0);
				::gtk_widget_show(l_pSeparator);
				m_vSeparator[i]=l_pSeparator;

				//add checkbox in channel select window
				//-------------------------------------
				::gtk_list_store_append(l_pChannelListStore, &l_oChannelIter);
				::gtk_list_store_set(l_pChannelListStore, &l_oChannelIter, 0, l_oChannelName[i].c_str(), -1);

				l_oLabelString.str("");

				//a channel is selected by default
				m_vSelectedChannels[i]=true;
				m_vMultiViewSelectedChannels[i]=false;

				//clear label
				l_oLabelString.str("");
			}

			::gtk_tree_selection_set_mode(::gtk_tree_view_get_selection(GTK_TREE_VIEW(l_pChannelSelectList)), GTK_SELECTION_MULTIPLE);
			::gtk_tree_view_append_column(GTK_TREE_VIEW(l_pChannelSelectList), ::gtk_tree_view_column_new_with_attributes("Channel", ::gtk_cell_renderer_text_new(), "text", 0, NULL));
			::gtk_tree_view_set_model(GTK_TREE_VIEW(l_pChannelSelectList), GTK_TREE_MODEL(l_pChannelListStore));
			::gtk_tree_selection_set_mode(::gtk_tree_view_get_selection(GTK_TREE_VIEW(l_pMultiViewSelectList)), GTK_SELECTION_MULTIPLE);
			::gtk_tree_view_append_column(GTK_TREE_VIEW(l_pMultiViewSelectList), ::gtk_tree_view_column_new_with_attributes("Channel", ::gtk_cell_renderer_text_new(), "text", 0, NULL));
			::gtk_tree_view_set_model(GTK_TREE_VIEW(l_pMultiViewSelectList), GTK_TREE_MODEL(l_pChannelListStore));

			//multiview channel
			//-----------------
			//create and attach label
			::GtkWidget * l_pLabel =  ::gtk_label_new("Multi-View");
			m_oChannelLabel[l_ui32ChannelCount] = l_pLabel;
			::gtk_table_attach(GTK_TABLE(m_pSignalDisplayTable),l_pLabel,
				0, 1,
				l_ui32ChannelCount*2, (l_ui32ChannelCount*2)+1,
				GTK_FILL, GTK_SHRINK,
				0, 0);

			//create and attach display widget
			CSignalChannelDisplay* l_pChannelDisplay = new CSignalChannelDisplay(
				this,
				l_i32ChannelDisplayWidthRequest, l_i32ChannelDisplayHeightRequest,
				l_i32LeftRulerWidthRequest, l_i32LeftRulerHeightRequest);
			m_oChannelDisplay[l_ui32ChannelCount] = l_pChannelDisplay;
			l_pChannelDisplay->addChannel(0);
			::gtk_table_attach(GTK_TABLE(m_pSignalDisplayTable),
				l_pChannelDisplay->getRulerWidget(),
				2, 3, //third column
				(l_ui32ChannelCount*2), (l_ui32ChannelCount*2)+1,
				GTK_FILL, GTK_FILL,
				0, 0);
			::gtk_table_attach(GTK_TABLE(m_pSignalDisplayTable),
				l_pChannelDisplay->getSignalDisplayWidget(),
				3, 4, //fourth column
				(l_ui32ChannelCount*2), (l_ui32ChannelCount*2)+1,
				static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL),
				0, 0);

			m_bMultiViewInitialized = true;

			//create bottom ruler
			//-------------------
			m_pBottomRuler = new CBottomTimeRuler(*m_pBufferDatabase, l_i32BottomRulerWidthRequest, l_i32BottomRulerHeightRequest);
			::gtk_size_group_add_widget(l_pSizeGroup, GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayEmptyLabel1")));
			::gtk_box_pack_start(m_pBottomBox, m_pBottomRuler->getWidget(), false, false, 0);
			// tell ruler has to resize when channel displays are resized
			if(m_oChannelDisplay.size() != 0)
			{
				m_pBottomRuler->linkWidthToWidget(m_oChannelDisplay[0]->getSignalDisplayWidget());
			}
			::gtk_widget_show_all(m_pBottomRuler->getWidget());

			//allocate memory to store sample points
			//--------------------------------------
			//reserve the maximum space needed for computing the points to display
			//(when cropping the lines, there can be up to two times the number of original points)
			m_pPoints.reserve((size_t)(m_pBufferDatabase->m_pDimmensionSizes[1]*m_pBufferDatabase->m_ui64NumberOfBufferToDisplay * 2));
			//resize the vector of raw points
			m_pRawPoints.resize((size_t)(m_pBufferDatabase->m_pDimmensionSizes[1]*m_pBufferDatabase->m_ui64NumberOfBufferToDisplay));

			//Don't display left ruler (default)
			m_bShowLeftRulers = false;
			toggleLeftRulers(m_bShowLeftRulers);
			::gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayToggleLeftRulerButton")), m_bShowLeftRulers);
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayToggleLeftRulerButton")),     "toggled",       G_CALLBACK(toggleLeftRulerButtonCallback),   this);

			//Display bottom ruler
			m_bShowBottomRuler = true;
			toggleBottomRuler(m_bShowBottomRuler);
			::gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayToggleBottomRulerButton")), m_bShowBottomRuler);
			g_signal_connect(G_OBJECT(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayToggleBottomRulerButton")),   "toggled",       G_CALLBACK(toggleBottomRulerButtonCallback), this);

			activateToolbarButtons(true);
		}

		void CSignalDisplayView::redraw()
		{
			//nothing to redraw if the table isn't visible or no data was received
			if(m_pSignalDisplayTable == NULL || !GTK_WIDGET_VISIBLE(m_pSignalDisplayTable) || m_pBufferDatabase->isFirstBufferReceived() == false)
			{
				return;
			}

			float64 l_f64LargestDisplayedValueRange = 0;

			//update channels display parameters based on current signal data
			for(uint32 i=0 ; i<m_oChannelDisplay.size(); i++)
			{
				if(m_oChannelDisplay[i] == NULL)
				{
					return;
				}

				//FIXME : should hidden channels be taken into account when computing largest value range?
				if(GTK_WIDGET_VISIBLE(m_oChannelDisplay[i]->getSignalDisplayWidget()))
				{
					float64 l_f64ValueRange;
					m_oChannelDisplay[i]->checkTranslation(l_f64ValueRange);
					if(l_f64ValueRange > l_f64LargestDisplayedValueRange)
					{
						l_f64LargestDisplayedValueRange = l_f64ValueRange;
					}
				}
			}

			if(m_bAutoVerticalScale == true) //global best fit
			{
				//if we just switched back to auto scale, or if range increased or decreased beyond margin boundaries
				if(m_bVerticalScaleChanged == true ||
					l_f64LargestDisplayedValueRange > (m_f64LargestDisplayedValueRange + m_f64ValueRangeMargin) ||
					l_f64LargestDisplayedValueRange < (m_f64LargestDisplayedValueRange - m_f64ValueRangeMargin))
				{
					m_f64LargestDisplayedValueRange = l_f64LargestDisplayedValueRange;
					m_f64ValueRangeMargin = m_f64MarginFactor * l_f64LargestDisplayedValueRange;

					for(uint32 i=0; i<m_oChannelDisplay.size(); i++)
					{
						//set new parameters
						m_oChannelDisplay[i]->setGlobalBestFitParameters(m_f64LargestDisplayedValueRange, m_f64ValueRangeMargin);
					}
				}
			}
			else //fixed scale
			{
				//tell all channels about new fixed range if it just changed
				if(m_bVerticalScaleChanged == true)
				{
					for(uint32 i=0; i<m_oChannelDisplay.size(); i++)
					{
						//set new parameters
						float64 l_f64Margin = 0;
						m_oChannelDisplay[i]->setGlobalBestFitParameters(m_f64CustomVerticalScaleValue, l_f64Margin);
					}
				}
			}

			//if in scan mode, check whether time scale needs to be updated
			if(m_pBufferDatabase->getDisplayMode() == OVP_TypeId_SignalDisplayMode_Scan && m_ui64LeftmostDisplayedTime < m_pBufferDatabase->m_oStartTime[0])
			{
				//printf("Time basis needs to be updated\n");
				if(m_pBufferDatabase->m_oSampleBuffers.size() < m_pBufferDatabase->m_ui64NumberOfBufferToDisplay)
				{
					m_ui64LeftmostDisplayedTime = m_pBufferDatabase->m_oStartTime[0];
				}
				else //catch up with current time interval
				{
					if(m_pBufferDatabase->m_ui64TotalStep == 0)
					{
						//error
					}
					else
					{
						m_ui64LeftmostDisplayedTime += m_pBufferDatabase->m_ui64TotalStep;

						//while there is time to catch up
						while(m_ui64LeftmostDisplayedTime < (m_pBufferDatabase->m_oStartTime[0] - m_pBufferDatabase->m_ui64BufferStep))
						{
							m_ui64LeftmostDisplayedTime += m_pBufferDatabase->m_ui64TotalStep;
						}

						//round leftmost displayed time to start of closest data buffer
						for(uint32 i=0; i<m_pBufferDatabase->m_oStartTime.size(); i++)
						{
							if(m_pBufferDatabase->m_oEndTime[i] > m_ui64LeftmostDisplayedTime)
							{
								m_ui64LeftmostDisplayedTime = m_pBufferDatabase->m_oStartTime[i];
							}
						}

						//if drawing is not up to date, force a full redraw
						if(m_oChannelDisplay[0]->m_ui64LatestDisplayedTime != m_ui64LeftmostDisplayedTime)
						{
							for(size_t i=0; i<m_oChannelDisplay.size(); i++)
							{
								m_oChannelDisplay[i]->redrawAllAtNextRefresh();
							}
						}
					}
				}
			}

			//redraw channels
			for(size_t i=0 ; i<m_oChannelDisplay.size(); i++)
			{
				if(GTK_WIDGET_VISIBLE(m_oChannelDisplay[i]->getSignalDisplayWidget()))
				{
					/*
					//if in scroll mode, or if time basis changed, redraw all
					if(m_pBufferDatabase->getDisplayMode() == OVP_TypeId_SignalDisplayMode_Scroll || l_pChannelDisplay->mustRedrawAll() == true)
					{
						printf("full redraw\n");*/
						GdkRectangle l_oUpdateRect;
						m_oChannelDisplay[i]->getUpdateRectangle(l_oUpdateRect);
						if(m_oChannelDisplay[i]->getSignalDisplayWidget()->window) gdk_window_invalidate_rect(m_oChannelDisplay[i]->getSignalDisplayWidget()->window, &l_oUpdateRect, false);
					/*}
					else
					{
						GdkRectangle l_oUpdateRect;
						m_oChannelDisplay[i]->getUpdateRectangle(l_oUpdateRect);
						//printf("partial redraw : x=%d, w=%d\n", l_oUpdateRect.x, l_oUpdateRect.width);
						gdk_window_clear_area_e(m_oChannelDisplay[i]->getSignalDisplayWidget()->window, l_oUpdateRect.x, l_oUpdateRect.y, l_oUpdateRect.width, l_oUpdateRect.height);
					}*/
				}
			}

			//redraw ruler
			m_pBottomRuler->setLeftmostDisplayedTime(m_ui64LeftmostDisplayedTime);
			if(GTK_WIDGET(m_pBottomRuler->getWidget())->window) gdk_window_invalidate_rect(GTK_WIDGET(m_pBottomRuler->getWidget())->window, NULL, true);

			m_bVerticalScaleChanged = false;
		}

		void CSignalDisplayView::toggleLeftRulers(boolean bActive)
		{
			m_bShowLeftRulers = bActive;

			for(size_t i=0 ; i<m_oChannelDisplay.size() ; i++)
			{
				if(isChannelDisplayVisible(i) == true)
				{
					if(bActive)
					{
						::gtk_widget_show(m_oChannelDisplay[i]->getRulerWidget());
					}
					else
					{
						::gtk_widget_hide(m_oChannelDisplay[i]->getRulerWidget());
					}
				}
			}
		}

		void CSignalDisplayView::toggleBottomRuler(boolean bActive)
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

		void CSignalDisplayView::toggleChannel(uint32 ui32ChannelIndex, boolean bActive)
		{
			CSignalChannelDisplay* l_pChannelDisplay = getChannelDisplay(ui32ChannelIndex);

			if(bActive)
			{
				::gtk_widget_show(m_oChannelLabel[ui32ChannelIndex]);
				if(m_bShowLeftRulers == true)
				{
					::gtk_widget_show(l_pChannelDisplay->getRulerWidget());
				}
				::gtk_widget_show(l_pChannelDisplay->getSignalDisplayWidget());
				::gtk_widget_show(m_vSeparator[ui32ChannelIndex]);
			}
			else
			{
				::gtk_widget_hide(m_oChannelLabel[ui32ChannelIndex]);
				::gtk_widget_hide(l_pChannelDisplay->getRulerWidget());
				::gtk_widget_hide(l_pChannelDisplay->getSignalDisplayWidget());
				::gtk_widget_hide(m_vSeparator[ui32ChannelIndex]);
			}
		}

		void CSignalDisplayView::updateMainTableStatus()
		{
			uint32 l_ui32Index=0;

			boolean l_bMultiView=false;
			for(uint32 i=0; i<m_oChannelLabel.size(); i++)
			{
				l_bMultiView|=m_vMultiViewSelectedChannels[i];
				if(l_bMultiView)
				{
					l_ui32Index=m_oChannelDisplay.size()-1;
				}
			}

			boolean l_bChannels=false;
			for(uint32 i=0; i<m_oChannelLabel.size(); i++)
			{
				l_bChannels|=m_vSelectedChannels[i];
				if(m_vSelectedChannels[i])
				{
					l_ui32Index=i;
				}
			}

			//if nothing has been selected
			if(!l_bChannels && !l_bMultiView)
			{
				//hide the whole table
				::gtk_widget_hide(GTK_WIDGET(m_pSignalDisplayTable));
			}
			else
			{
				if(!GTK_WIDGET_VISIBLE(GTK_WIDGET(m_pSignalDisplayTable)))
				{
					//if there were no selected channels before, but now there are, show the table again
					::gtk_widget_show(GTK_WIDGET(m_pSignalDisplayTable));
				}
				m_pBottomRuler->linkWidthToWidget(m_oChannelDisplay[l_ui32Index]->getSignalDisplayWidget());
			}
		}

		void CSignalDisplayView::activateToolbarButtons(boolean bActive)
		{
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayScrollModeButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayScanModeButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayToggleLeftRulerButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayToggleBottomRulerButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayChannelSelectButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayStimulationColorsButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayMultiViewButton")), bActive);
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayInformationButton")), bActive);
		}

		boolean CSignalDisplayView::onDisplayModeToggledCB(CIdentifier oDisplayMode)
		{
			m_pBufferDatabase->setDisplayMode(oDisplayMode);

			//force full redraw of all channels when display mode changes
			for(size_t i=0 ; i<m_oChannelDisplay.size(); i++)
			{
				m_oChannelDisplay[i]->redrawAllAtNextRefresh();
			}

			//redraw channels
			redraw();

			return true;
		}

		boolean CSignalDisplayView::onVerticalScaleModeToggledCB(::GtkToggleButton* pToggleButton)
		{
			m_bVerticalScaleChanged = true;
			m_bAutoVerticalScale = ::gtk_toggle_button_get_active(pToggleButton) != 0;
#if 0
			if(m_bAutoVerticalScale)
			{
				::gtk_widget_hide(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayCustomVerticalScaleSpinButton")));
			}
			else
			{
				::gtk_spin_button_set_value(GTK_SPIN_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayCustomVerticalScaleSpinButton")), m_f64LargestDisplayedValueRange);
				::gtk_widget_show(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayCustomVerticalScaleSpinButton")));
			}
#else
			::gtk_widget_set_sensitive(GTK_WIDGET(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayCustomVerticalScaleSpinButton")), !m_bAutoVerticalScale);
			::gtk_spin_button_set_value(GTK_SPIN_BUTTON(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayCustomVerticalScaleSpinButton")), m_f64LargestDisplayedValueRange);
#endif
			return true;
		}

		boolean CSignalDisplayView::onCustomVerticalScaleChangedCB(::GtkSpinButton *pSpinButton)
		{
			m_bVerticalScaleChanged = true;
			m_f64CustomVerticalScaleValue = ::gtk_spin_button_get_value(pSpinButton);
			return true;
		}

		CSignalChannelDisplay* CSignalDisplayView::getChannelDisplay(uint32 ui32ChannelIndex)
		{
			if(ui32ChannelIndex < m_oChannelDisplay.size())
			{
				return m_oChannelDisplay[ui32ChannelIndex];
			}
			else
			{
				return NULL;
			}
		}

		boolean CSignalDisplayView::isChannelDisplayVisible(uint32 ui32ChannelIndex)
		{
			return GTK_WIDGET_VISIBLE(getChannelDisplay(ui32ChannelIndex)->getSignalDisplayWidget());
		}

		void CSignalDisplayView::onStimulationReceivedCB(uint64 ui64StimulationCode, const CString& rStimulationName)
		{
			if(m_mStimulations.find(ui64StimulationCode) == m_mStimulations.end())
			{
				//only the lower 32 bits of the stimulation code are currently used to compute the color
				uint32 ui32Code = (uint32)ui64StimulationCode;
				GdkColor l_oColor;
				l_oColor.red = 0;
				l_oColor.green = 0;
				l_oColor.blue = 0;

				//go through the lower 32 bits to compute RGB components. Bit positions are
				//inverted so that close code values result in very different colors.
				for(uint32 i=0; i<11; i++)
				{
					l_oColor.red   |= ( (ui32Code>>(3*i))   & 0x1 ) << (10-i);
					l_oColor.green |= ( (ui32Code>>(3*i+1)) & 0x1 ) << (10-i);
					if(i<10) //only 10 bits for blue component
					{
						l_oColor.blue  |= ( (ui32Code>>(3*i+2)) & 0x1 ) << (9-i);
					}
				}

				//convert to 16 bits per channel
				l_oColor.red   = (l_oColor.red * 65535)   / 0x7FF; //red coded on 11 bits
				l_oColor.green = (l_oColor.green * 65535) / 0x7FF; //green coded on 11 bits
				l_oColor.blue  = (l_oColor.blue * 65535)  / 0x3FF; //blue coded on 10 bits

				//store stimulation color in map
				m_mStimulations[ui64StimulationCode].first = rStimulationName;
				m_mStimulations[ui64StimulationCode].second = l_oColor;

				//update stimulations dialog
				updateStimulationColorsDialog(rStimulationName, l_oColor);
			}

			//force full redraw of all channels when display mode changes
			for(size_t i=0 ; i<m_oChannelDisplay.size(); i++)
			{
				m_oChannelDisplay[i]->redrawAllAtNextRefresh();
			}

			//redraw channels
			redraw();
		}

		void CSignalDisplayView::getStimulationColor(uint64 ui64StimulationCode, GdkColor& rColor)
		{
			if(m_mStimulations.find(ui64StimulationCode) != m_mStimulations.end())
			{
				rColor = m_mStimulations[ui64StimulationCode].second;
			}
		}

		void CSignalDisplayView::getMultiViewColor(uint32 ui32ChannelIndex, ::GdkColor& rColor)
		{
			if(m_mSignals.find(ui32ChannelIndex) != m_mSignals.end())
			{
				rColor = m_mSignals[ui32ChannelIndex].second;
			}
			else
			{
				uint32 ui32Code = ui32ChannelIndex;
				rColor.red = 0;
				rColor.green = 0;
				rColor.blue = 0;

				//go through the lower 32 bits to compute RGB components. Bit positions are
				//inverted so that close code values result in very different colors.
				for(uint32 i=0; i<11; i++)
				{
					rColor.red   |= ( (ui32Code>>(3*i))   & 0x1 ) << (10-i);
					rColor.green |= ( (ui32Code>>(3*i+1)) & 0x1 ) << (10-i);
					if(i<10) //only 10 bits for blue component
					{
						rColor.blue  |= ( (ui32Code>>(3*i+2)) & 0x1 ) << (9-i);
					}
				}

				//convert to 16 bits per channel
				rColor.red   = (rColor.red * 65535)   / 0x7FF; //red coded on 11 bits
				rColor.green = (rColor.green * 65535) / 0x7FF; //green coded on 11 bits
				rColor.blue  = (rColor.blue * 65535)  / 0x3FF; //blue coded on 10 bits

				//store signal color in map
				m_mSignals[ui32ChannelIndex].first = "";
				m_mSignals[ui32ChannelIndex].second = rColor;
			}
		}

		void CSignalDisplayView::updateStimulationColorsDialog(const CString& rStimulationLabel, const GdkColor& rStimulationColor)
		{
			//retrieve table
			::GtkTable* l_pStimulationColorsTable = GTK_TABLE(::gtk_builder_get_object(m_pBuilderInterface, "SignalDisplayStimulationColorsTable"));

			//resize table and store new colors
			::gtk_table_resize(l_pStimulationColorsTable, l_pStimulationColorsTable->nrows + 1, 2);

			//set a minimum size request (needed to scroll)
			int32 l_i32LabelWidthRequest = -1;
			int32 l_i32ColorWidthRequest = 50;
			int32 l_i32RowHeightRequest = 20;

			::gtk_widget_set_size_request(
				GTK_WIDGET(l_pStimulationColorsTable),
				-1,
				(l_pStimulationColorsTable->nrows + 1) * l_i32RowHeightRequest);

			::GtkLabel* l_pStimLabel = GTK_LABEL(::gtk_label_new("Stimulations"));
			::gtk_widget_set_size_request(GTK_WIDGET(l_pStimLabel), -1, 20);
			::gtk_table_attach(l_pStimulationColorsTable, GTK_WIDGET(l_pStimLabel), 0, 1, 0, 1,
				static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), GTK_FILL,	0, 0);

			::GtkLabel* l_pColorLabel = GTK_LABEL(::gtk_label_new("Colors"));
			::gtk_widget_set_size_request(GTK_WIDGET(l_pColorLabel), -1, 20);
			::gtk_table_attach(l_pStimulationColorsTable, GTK_WIDGET(l_pColorLabel), 1, 2, 0, 1,
				static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), GTK_FILL,	0, 0);

			::GtkLabel* l_pLabel = GTK_LABEL(::gtk_label_new(rStimulationLabel.toASCIIString()));
			::gtk_widget_set_size_request(GTK_WIDGET(l_pLabel), l_i32LabelWidthRequest, l_i32RowHeightRequest);
			::gtk_table_attach(l_pStimulationColorsTable, GTK_WIDGET(l_pLabel),
				0, 1, //first column
				l_pStimulationColorsTable->nrows-1, l_pStimulationColorsTable->nrows-1+1, //last row
				static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL),	0, 0);
#if 1
			GdkPixbuf* l_pPixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, l_i32ColorWidthRequest, l_i32RowHeightRequest);
			//fill with RGBA value
			guint32 l_ui32Color = (((guint32)(rStimulationColor.red * 255 / 65535)) << 24) +
				(((guint32)(rStimulationColor.green * 255 / 65535)) << 16) +
				(((guint32)(rStimulationColor.blue * 255 / 65535)) << 8);
			gdk_pixbuf_fill(l_pPixbuf, l_ui32Color);
			::GtkWidget* l_pImage = ::gtk_image_new_from_pixbuf(l_pPixbuf);
			::gtk_table_attach(l_pStimulationColorsTable, GTK_WIDGET(l_pImage),
				1, 2, //2nd column
				l_pStimulationColorsTable->nrows-1, l_pStimulationColorsTable->nrows-1+1, //last row
				static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL),	0, 0);
#else
			::GtkColorButton* l_pButton = GTK_COLOR_BUTTON(::gtk_color_button_new_with_color(&rStimulationColor));
			::gtk_widget_set_size_request(GTK_WIDGET(l_pButton), l_i32ColorWidthRequest, l_i32RowHeightRequest);
			//g_signal_connect(G_OBJECT(l_pButton), "clicked", G_CALLBACK(dummyButtonCallback), NULL);
			::gtk_table_attach(l_pStimulationColorsTable, GTK_WIDGET(l_pButton),
				1, 2, //2nd column
				l_pStimulationColorsTable->nrows-1, l_pStimulationColorsTable->nrows-1+1, //last row
				static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), static_cast < ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL),	0, 0);
#endif
		}

		//
		//CALLBACKS
		//

		void scrollModeButtonCallback(::GtkWidget *widget, gpointer data)
		{
			reinterpret_cast < CSignalDisplayView* >(data)->onDisplayModeToggledCB(
				::gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget)) != 0 ?
					OVP_TypeId_SignalDisplayMode_Scroll : OVP_TypeId_SignalDisplayMode_Scan);
		}

		void scanModeButtonCallback(::GtkWidget *widget, gpointer data)
		{
			reinterpret_cast < CSignalDisplayView* >(data)->onDisplayModeToggledCB(
				::gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget)) != 0 ?
					OVP_TypeId_SignalDisplayMode_Scan : OVP_TypeId_SignalDisplayMode_Scroll);
		}

		void toggleLeftRulerButtonCallback(::GtkWidget *widget, gpointer data)
		{
			CSignalDisplayView* l_pView = reinterpret_cast < CSignalDisplayView* >(data);
			l_pView->toggleLeftRulers(::gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget))?true:false);
		}

		void toggleBottomRulerButtonCallback(::GtkWidget *widget, gpointer data)
		{
			CSignalDisplayView* l_pView = reinterpret_cast < CSignalDisplayView* >(data);
			l_pView->toggleBottomRuler(::gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget))?true:false);
		}

		void toggleAutoVerticalScaleButtonCallback(::GtkToggleButton *togglebutton, gpointer data)
		{
			CSignalDisplayView* l_pView = reinterpret_cast < CSignalDisplayView* >(data);
			l_pView->onVerticalScaleModeToggledCB(togglebutton);
		}

		void customVerticalScaleChangedCallback(::GtkSpinButton* pSpinButton, gpointer data)
		{
			CSignalDisplayView* l_pView = reinterpret_cast < CSignalDisplayView* >(data);
			l_pView->onCustomVerticalScaleChangedCB(pSpinButton);
		}

		gboolean spinButtonValueChangedCallback(::GtkSpinButton *widget,  gpointer data)
		{
			CSignalDisplayView* l_pView = reinterpret_cast < CSignalDisplayView* >(data);

			//Compute and save the new number of buffers to display
			boolean l_bNumberOfDisplayedBufferChanged = l_pView->m_pBufferDatabase->adjustNumberOfDisplayedBuffers(::gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget)));

			if(l_bNumberOfDisplayedBufferChanged)
			{
				//reserve the maximum space needed for computing the points to display
				//(when cropping the lines, there can be up to two times the number of original points)
				l_pView->m_pPoints.reserve((size_t)(l_pView->m_pBufferDatabase->m_pDimmensionSizes[1] * l_pView->m_pBufferDatabase->m_ui64NumberOfBufferToDisplay * 2));

				//resize the vector of raw points (before cropping)
				l_pView->m_pRawPoints.resize((size_t)(l_pView->m_pBufferDatabase->m_pDimmensionSizes[1] * l_pView->m_pBufferDatabase->m_ui64NumberOfBufferToDisplay));

				//force full redraw of all channels when time scale changes
				for(size_t i=0 ; i<l_pView->m_oChannelDisplay.size(); i++)
				{
					l_pView->getChannelDisplay(i)->updateScale();
				}

				//redraw channels
				l_pView->redraw();
			}

			return FALSE;
		}

		//called when the channel select button is pressed (opens the channel selection dialog)
		void channelSelectButtonCallback(::GtkButton *button, gpointer data)
		{
			CSignalDisplayView* l_pView = reinterpret_cast < CSignalDisplayView* >(data);

			::GtkWidget * l_pChannelSelectDialog = GTK_WIDGET(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayChannelSelectDialog"));
			::GtkTreeView* l_pChannelSelectTreeView = GTK_TREE_VIEW(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayChannelSelectList"));
			::GtkTreeSelection* l_pChannelSelectTreeSelection = ::gtk_tree_view_get_selection(l_pChannelSelectTreeView);
			::GtkTreeModel* l_pChannelSelectTreeModel = ::gtk_tree_view_get_model(l_pChannelSelectTreeView);
			::GtkTreeIter l_oIter;
			uint32 l_ui32Index=0;

			if(::gtk_tree_model_get_iter_first(l_pChannelSelectTreeModel, &l_oIter))
			{
				do
				{
					if(l_pView->m_vSelectedChannels[l_ui32Index])
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

		//Called when the user presses the apply button of the channel selection dialog
		void channelSelectDialogApplyButtonCallback(::GtkButton *button, gpointer data)
		{
			CSignalDisplayView* l_pView = reinterpret_cast < CSignalDisplayView* >(data);

			::GtkTreeView* l_pChannelSelectTreeView = GTK_TREE_VIEW(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayChannelSelectList"));
			::GtkTreeSelection* l_pChannelSelectTreeSelection = ::gtk_tree_view_get_selection(l_pChannelSelectTreeView);
			::GtkTreeModel* l_pChannelSelectTreeModel = ::gtk_tree_view_get_model(l_pChannelSelectTreeView);
			::GtkTreeIter l_oIter;
			uint32 l_ui32Index=0;

			if(::gtk_tree_model_get_iter_first(l_pChannelSelectTreeModel, &l_oIter))
			{
				do
				{
					l_pView->m_vSelectedChannels[l_ui32Index]=(::gtk_tree_selection_iter_is_selected(l_pChannelSelectTreeSelection, &l_oIter)?true:false);
					l_pView->toggleChannel(l_ui32Index, ::gtk_tree_selection_iter_is_selected(l_pChannelSelectTreeSelection, &l_oIter)?true:false);
					l_ui32Index++;
				}
				while(::gtk_tree_model_iter_next(l_pChannelSelectTreeModel, &l_oIter));
			}

			l_pView->updateMainTableStatus();

			//hides the channel selection dialog
			::gtk_widget_hide(GTK_WIDGET(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayChannelSelectDialog")));
		}

		void stimulationColorsButtonCallback(::GtkButton *button, gpointer data)
		{
			CSignalDisplayView* l_pView = reinterpret_cast < CSignalDisplayView* >(data);
			::GtkWidget* l_pStimulationColorsDialog = GTK_WIDGET(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayStimulationColorsDialog"));
			::gtk_widget_show_all(l_pStimulationColorsDialog);
		}

		//Called when the user presses the Information button (opens the information dialog)
		void informationButtonCallback(::GtkButton *button, gpointer data)
		{
			CSignalDisplayView* l_pView = reinterpret_cast < CSignalDisplayView* >(data);

			//gets the different values from the database and updates the corresponding label's text field
			stringstream l_oValueString;
			l_oValueString<<l_pView->m_pBufferDatabase->m_pDimmensionSizes[0];
			::gtk_label_set_text(GTK_LABEL(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayNumberOfChannels")),
				 l_oValueString.str().c_str() );

			l_oValueString.str("");
			l_oValueString<<l_pView->m_pBufferDatabase->m_ui32SamplingFrequency;
			::gtk_label_set_text(GTK_LABEL(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplaySamplingFrequency")),
				l_oValueString.str().c_str() );

			l_oValueString.str("");
			l_oValueString<<l_pView->m_pBufferDatabase->m_pDimmensionSizes[1];
			::gtk_label_set_text(GTK_LABEL(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplaySamplesPerBuffer")),
				l_oValueString.str().c_str() );

			l_oValueString.str("");
			l_oValueString<<l_pView->m_pBufferDatabase->m_f64MinimumValue;
			::gtk_label_set_text(GTK_LABEL(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayMinimumValue")),
				l_oValueString.str().c_str() );

			l_oValueString.str("");
			l_oValueString<<l_pView->m_pBufferDatabase->m_f64MaximumValue;
			::gtk_label_set_text(GTK_LABEL(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayMaximumValue")),
				   l_oValueString.str().c_str() );

			::GtkWidget * l_pInformationDialog = GTK_WIDGET(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayInformationDialog"));

			//connect the close button to the dialog's hide command
			g_signal_connect_swapped(G_OBJECT(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayInformationClose")),
					"clicked",
					G_CALLBACK(::gtk_widget_hide),
					G_OBJECT(l_pInformationDialog));
			
			g_signal_connect(G_OBJECT(l_pInformationDialog),
				"delete_event",
				G_CALLBACK(::gtk_widget_hide),
				NULL);

			//finally, show the information dialog
			::gtk_widget_show_all(l_pInformationDialog);
		}

		//called when the channel select button is pressed (opens the channel selection dialog)
		void multiViewButtonCallback(::GtkButton *button, gpointer data)
		{
			CSignalDisplayView* l_pView = reinterpret_cast < CSignalDisplayView* >(data);

			::GtkWidget * l_pMultiViewDialog = GTK_WIDGET(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayMultiViewDialog"));
			::GtkTreeView* l_pMultiViewTreeView = GTK_TREE_VIEW(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayMultiViewSelectList"));
			::GtkTreeSelection* l_pMultiViewTreeSelection = ::gtk_tree_view_get_selection(l_pMultiViewTreeView);
			::GtkTreeModel* l_pMultiViewTreeModel = ::gtk_tree_view_get_model(l_pMultiViewTreeView);
			::GtkTreeIter l_oIter;
			uint32 l_ui32Index=0;

			if(::gtk_tree_model_get_iter_first(l_pMultiViewTreeModel, &l_oIter))
			{
				do
				{
					if(l_pView->m_vMultiViewSelectedChannels[l_ui32Index])
					{
						::gtk_tree_selection_select_iter(l_pMultiViewTreeSelection, &l_oIter);
					}
					else
					{
						::gtk_tree_selection_unselect_iter(l_pMultiViewTreeSelection, &l_oIter);
					}
					l_ui32Index++;
				}
				while(::gtk_tree_model_iter_next(l_pMultiViewTreeModel, &l_oIter));
			}

			//finally, show the information dialog
			::gtk_widget_show_all(l_pMultiViewDialog);
		}

		//Called when the user presses the apply button of the channel selection dialog
		void multiViewDialogApplyButtonCallback(::GtkButton *button, gpointer data)
		{
			CSignalDisplayView* l_pView = reinterpret_cast < CSignalDisplayView* >(data);

			::GtkTreeView* l_pMultiViewTreeView = GTK_TREE_VIEW(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayMultiViewSelectList"));
			::GtkTreeSelection* l_pMultiViewTreeSelection = ::gtk_tree_view_get_selection(l_pMultiViewTreeView);
			::GtkTreeModel* l_pMultiViewTreeModel = ::gtk_tree_view_get_model(l_pMultiViewTreeView);
			::GtkTreeIter l_oIter;
			uint32 l_ui32Index=0;

			if(::gtk_tree_model_get_iter_first(l_pMultiViewTreeModel, &l_oIter))
			{
				do
				{
					l_pView->m_vMultiViewSelectedChannels[l_ui32Index]=(::gtk_tree_selection_iter_is_selected(l_pMultiViewTreeSelection, &l_oIter)?true:false);
					l_ui32Index++;
				}
				while(::gtk_tree_model_iter_next(l_pMultiViewTreeModel, &l_oIter));
			}

			l_pView->changeMultiView();
			l_pView->updateMainTableStatus();

			//hides the channel selection dialog
			::gtk_widget_hide(GTK_WIDGET(::gtk_builder_get_object(l_pView->m_pBuilderInterface, "SignalDisplayMultiViewDialog")));
		}
	}
}
