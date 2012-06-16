#include "ovpCDisplayCueImage.h"

#include <cmath>
#include <iostream>
#include <stdlib.h>

#if defined OVP_OS_Linux
  #include <unistd.h>
#endif

using namespace OpenViBE;
using namespace Plugins;
using namespace Kernel;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace OpenViBEToolkit;

using namespace std;

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		gboolean DisplayCueImage_SizeAllocateCallback(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
		{
			reinterpret_cast<CDisplayCueImage*>(data)->resize((uint32)allocation->width, (uint32)allocation->height);
			return FALSE;
		}

		gboolean DisplayCueImage_RedrawCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
		{
			reinterpret_cast<CDisplayCueImage*>(data)->redraw();
			return TRUE;
		}

		CDisplayCueImage::CDisplayCueImage(void) :
			m_pBuilderInterface(NULL),
			m_pMainWindow(NULL),
			m_pDrawingArea(NULL),
			//m_pStimulationReaderCallBack(NULL),
			m_bImageRequested(false),
			m_int32RequestedImageID(-1),
			m_bImageDrawn(false),
			m_int32DrawnImageID(-1),
			m_pOriginalPicture(NULL),
			m_pScaledPicture(NULL),
			m_bFullScreen(false),
			m_ui64LastOutputChunkDate(-1),
			m_bError(false)
		{
			//m_pReader[0] = NULL;

			m_oBackgroundColor.pixel = 0;
			m_oBackgroundColor.red = 0;
			m_oBackgroundColor.green = 0;
			m_oBackgroundColor.blue = 0;

			m_oForegroundColor.pixel = 0;
			m_oForegroundColor.red = 0xFFFF;
			m_oForegroundColor.green = 0xFFFF;
			m_oForegroundColor.blue = 0xFFFF;
		}

		boolean CDisplayCueImage::initialize()
		{
			m_bError=false;

			//>>>> Reading Settings:

			//Number of Cues:
			CString l_sSettingValue;
			m_ui32NuberOfCue = getStaticBoxContext().getSettingCount()/2 -1;

			//Do we display the images in full screen?
			getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(0, l_sSettingValue);
			m_bFullScreen=(l_sSettingValue==CString("true")?true:false);

			//Clear screen stimulation:
			getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(1, l_sSettingValue);
			m_ui64ClearScreenStimulation=getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, l_sSettingValue);

			//Stimulation ID and images file names for each cue
			m_pImageNames = new CString[m_ui32NuberOfCue];
			m_pStimulationsId = new uint64[m_ui32NuberOfCue];
			for(uint32 i=0; i<m_ui32NuberOfCue; i++)
			{
				getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(2*i+2, m_pImageNames[i]);
				getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(2*i+3, l_sSettingValue);
				m_pStimulationsId[i]=getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, l_sSettingValue);
			}

			//>>>> Initialisation
			m_oStimulationDecoder.initialize(*this);
			m_oStimulationEncoder.initialize(*this);

			//load the gtk builder interface
			m_pBuilderInterface=gtk_builder_new();
			gtk_builder_add_from_file(m_pBuilderInterface, "../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-DisplayCueImage.ui", NULL);

			if(!m_pBuilderInterface)
			{
				m_bError = true;
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_ImportantWarning << "Couldn't load the interface !";
				return false;
			}

			gtk_builder_connect_signals(m_pBuilderInterface, NULL);

			m_pDrawingArea = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "DisplayCueImageDrawingArea"));
			g_signal_connect(G_OBJECT(m_pDrawingArea), "expose_event", G_CALLBACK(DisplayCueImage_RedrawCallback), this);
			g_signal_connect(G_OBJECT(m_pDrawingArea), "size-allocate", G_CALLBACK(DisplayCueImage_SizeAllocateCallback), this);

			//set widget bg color
			gtk_widget_modify_bg(m_pDrawingArea, GTK_STATE_NORMAL, &m_oBackgroundColor);
			gtk_widget_modify_bg(m_pDrawingArea, GTK_STATE_PRELIGHT, &m_oBackgroundColor);
			gtk_widget_modify_bg(m_pDrawingArea, GTK_STATE_ACTIVE, &m_oBackgroundColor);

			gtk_widget_modify_fg(m_pDrawingArea, GTK_STATE_NORMAL, &m_oForegroundColor);
			gtk_widget_modify_fg(m_pDrawingArea, GTK_STATE_PRELIGHT, &m_oForegroundColor);
			gtk_widget_modify_fg(m_pDrawingArea, GTK_STATE_ACTIVE, &m_oForegroundColor);

			//Load the pictures:
			m_pOriginalPicture = new GdkPixbuf*[m_ui32NuberOfCue];
			m_pScaledPicture = new GdkPixbuf*[m_ui32NuberOfCue];

			for(uint32 i=0; i<m_ui32NuberOfCue; i++)
			{
				m_pOriginalPicture[i] = gdk_pixbuf_new_from_file_at_size(m_pImageNames[i], -1, -1, NULL);
				m_pScaledPicture[i]=0;
				if(!m_pOriginalPicture[i])
				{
					getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_ImportantWarning << "Error couldn't load ressource file : " << m_pImageNames[i] << "!\n";
					m_bError = true;
					return false;
				}
			}

			getBoxAlgorithmContext()->getVisualisationContext()->setWidget(m_pDrawingArea);

			return true;
		}

		boolean CDisplayCueImage::uninitialize()
		{
			if(m_bError)
			{
				return true;
			}

			m_oStimulationDecoder.uninitialize();
			m_oStimulationEncoder.uninitialize();

			//destroy drawing area
			if(m_pDrawingArea)
			{
				gtk_widget_destroy(m_pDrawingArea);
				m_pDrawingArea = NULL;
			}

			// unref the xml file as it's not needed anymore
			g_object_unref(G_OBJECT(m_pBuilderInterface));
			m_pBuilderInterface=NULL;

			delete[] m_pStimulationsId;
			delete[] m_pImageNames;

			for(uint32 i=0; i<m_ui32NuberOfCue; i++)
			{
				if(m_pOriginalPicture[i]){ g_object_unref(G_OBJECT(m_pOriginalPicture[i])); }
				if(m_pScaledPicture[i]){ g_object_unref(G_OBJECT(m_pScaledPicture[i])); }
			}
			delete[] m_pOriginalPicture;
			delete[] m_pScaledPicture;

			return true;
		}

		boolean CDisplayCueImage::processClock(CMessageClock& rMessageClock)
		{
			IBoxIO* l_pBoxIO=getBoxAlgorithmContext()->getDynamicBoxContext();
			m_oStimulationEncoder.getInputStimulationSet()->clear();

			if(m_bImageDrawn)
			{
				// this is first redraw() for that image or clear screen
				// we send a stimulation to signal it.


				if (m_int32DrawnImageID>=0)
				{
					// it was a image
					m_oStimulationEncoder.getInputStimulationSet()->appendStimulation(
								m_pStimulationsId[m_int32DrawnImageID],
								this->getPlayerContext().getCurrentTime(),
								0);
				}
				else
				{
					// it was a clear_screen
					m_oStimulationEncoder.getInputStimulationSet()->appendStimulation(
								m_ui64ClearScreenStimulation,
								this->getPlayerContext().getCurrentTime(),
								0);
				}

				m_bImageDrawn = false;

				if (m_int32DrawnImageID != m_int32RequestedImageID)
				{
					// We must be late...
					getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "One image may have been skipped => we must be late...\n";
				}
			}

			m_oStimulationEncoder.encodeBuffer(0);
			l_pBoxIO->markOutputAsReadyToSend(0, m_ui64LastOutputChunkDate, this->getPlayerContext().getCurrentTime());
			m_ui64LastOutputChunkDate = this->getPlayerContext().getCurrentTime();

			// We check if some images must be display
			for(uint32 stim = 0; stim < m_oPendingStimulationSet.getStimulationCount() ; )
			{
				uint64 l_ui64StimDate = m_oPendingStimulationSet.getStimulationDate(stim);
				uint64 l_ui64Time = this->getPlayerContext().getCurrentTime();
				if (l_ui64StimDate < l_ui64Time)
				{
					float l_fDelay = ((l_ui64Time - l_ui64StimDate)>> 16) / 65.5360; //delay in ms
					if (l_fDelay>50)
						getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Image was late: "<< l_fDelay <<" ms \n";


					uint64 l_ui64StimID =   m_oPendingStimulationSet.getStimulationIdentifier(stim);
					if(l_ui64StimID== m_ui64ClearScreenStimulation)
					{
						if (m_bImageRequested)
							getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_ImportantWarning << "One image was skipped => Not enough time between two images!!\n";
						m_bImageRequested = true;
						m_int32RequestedImageID = -1;
					}
					else
					{
						for(uint32 i=0; i<=m_ui32NuberOfCue; i++)
						{
							if(l_ui64StimID == m_pStimulationsId[i])
							{
								if (m_bImageRequested)
									getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_ImportantWarning << "One image was skipped => Not enough time between two images!!\n";
								m_bImageRequested = true;
								m_int32RequestedImageID = i;
								break;
							}
						}
					}

					m_oPendingStimulationSet.removeStimulation(stim);

					if(GTK_WIDGET(m_pDrawingArea)->window)
					{
						gdk_window_invalidate_rect(GTK_WIDGET(m_pDrawingArea)->window,NULL,true);
						// it will trigger the callback redraw()
					}

				}
				else
				{
					stim++;
				}
			}

			return true;
		}

		boolean CDisplayCueImage::processInput(uint32 ui32InputIndex)
		{
			if(m_bError)
			{
				return false;
			}

			getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
			return true;
		}

		boolean CDisplayCueImage::process()
		{
			IBoxIO* l_pBoxIO=getBoxAlgorithmContext()->getDynamicBoxContext();

			// We decode and save the received stimulations.
			for(uint32 input=0; input < getBoxAlgorithmContext()->getStaticBoxContext()->getInputCount(); input++)
			{
				for(uint32 chunk=0; chunk < l_pBoxIO->getInputChunkCount(input); chunk++)
				{
					m_oStimulationDecoder.decode(0,chunk,true);
					if(m_oStimulationDecoder.isHeaderReceived())
					{
						m_ui64LastOutputChunkDate = this->getPlayerContext().getCurrentTime();
						m_oStimulationEncoder.encodeHeader(0);
						l_pBoxIO->markOutputAsReadyToSend(0, 0, m_ui64LastOutputChunkDate);
					}
					if(m_oStimulationDecoder.isBufferReceived())
					{
						for(uint32 stim = 0; stim < m_oStimulationDecoder.getOutputStimulationSet()->getStimulationCount(); stim++)
						{
							uint64 l_ui64StimID =  m_oStimulationDecoder.getOutputStimulationSet()->getStimulationIdentifier(stim);


							boolean l_bAddStim = false;
							if(l_ui64StimID == m_ui64ClearScreenStimulation)
								l_bAddStim = true;
							else
								for(uint32 i=0; i<=m_ui32NuberOfCue; i++)
									if(l_ui64StimID == m_pStimulationsId[i])
									{
										l_bAddStim = true;
										break;
									}

							if (l_bAddStim)
							{
								uint64 l_ui64StimDate =     m_oStimulationDecoder.getOutputStimulationSet()->getStimulationDate(stim);
								uint64 l_ui64StimDuration = m_oStimulationDecoder.getOutputStimulationSet()->getStimulationDuration(stim);

								uint64 l_ui64Time = this->getPlayerContext().getCurrentTime();
								if (l_ui64StimDate < l_ui64Time)
								{
									float l_fDelay = ((l_ui64Time - l_ui64StimDate)>> 16) / 65.5360; //delay in ms
									if (l_fDelay>50)
										getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Stimulation was received late: "<< l_fDelay <<" ms \n";
								}

								if (l_ui64StimDate < l_pBoxIO->getInputChunkStartTime(input, chunk))
								{
									this->getLogManager() << LogLevel_ImportantWarning << "Input Stimulation Date before beginning of the buffer\n";
								}

								m_oPendingStimulationSet.appendStimulation(
											l_ui64StimID,
											l_ui64StimDate,
											l_ui64StimDuration);
							}


						}
					}
					l_pBoxIO->markInputAsDeprecated(input, chunk);
				}
			}

			return true;
		}

		//Callback called by GTK
		void CDisplayCueImage::redraw()
		{
			if (m_int32RequestedImageID >= 0)
			{

				drawCuePicture(m_int32RequestedImageID);
			}
			if(m_bImageRequested)
			{
				m_bImageRequested = false;
				m_bImageDrawn = true;
				m_int32DrawnImageID = m_int32RequestedImageID;
			}
		}

		void CDisplayCueImage::drawCuePicture(OpenViBE::uint32 uint32CueID)
		{
			gint l_iWindowWidth = m_pDrawingArea->allocation.width;
			gint l_iWindowHeight = m_pDrawingArea->allocation.height;

			if(m_bFullScreen)
			{
				gdk_draw_pixbuf(m_pDrawingArea->window, NULL, m_pScaledPicture[uint32CueID], 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
			}
			else
			{
				gint l_iX = (l_iWindowWidth/2) - gdk_pixbuf_get_width(m_pScaledPicture[uint32CueID])/2;
				gint l_iY = (l_iWindowHeight/2) - gdk_pixbuf_get_height(m_pScaledPicture[uint32CueID])/2;;
				gdk_draw_pixbuf(m_pDrawingArea->window, NULL, m_pScaledPicture[uint32CueID], 0, 0, l_iX, l_iY, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
			}
		}

		void CDisplayCueImage::resize(uint32 ui32Width, uint32 ui32Height)
		{
			for(uint32 i=0; i<m_ui32NuberOfCue; i++)
			{
				if(m_pScaledPicture[i]){ g_object_unref(G_OBJECT(m_pScaledPicture[i])); }
			}

			if(m_bFullScreen)
			{
				for(uint32 i=0; i<m_ui32NuberOfCue; i++)
				{
					m_pScaledPicture[i] = gdk_pixbuf_scale_simple(m_pOriginalPicture[i], ui32Width, ui32Height, GDK_INTERP_BILINEAR);
				}
			}
			else
			{
				float l_fX = (ui32Width<64?64:ui32Width);
				float l_fY = (ui32Height<64?64:ui32Height);
				for(uint32 i=0; i<m_ui32NuberOfCue; i++)
				{
					float l_fx = gdk_pixbuf_get_width(m_pOriginalPicture[i]);
					float l_fy = gdk_pixbuf_get_height(m_pOriginalPicture[i]);
					if((l_fX/l_fx) < (l_fY/l_fy))
					{
						l_fy = l_fX*l_fy/(3*l_fx);
						l_fx = l_fX/3;
					}
					else
					{
						l_fx = l_fY*l_fx/(3*l_fy);
						l_fy = l_fY/3;
					}
					m_pScaledPicture[i] = gdk_pixbuf_scale_simple(m_pOriginalPicture[i], l_fx, l_fy, GDK_INTERP_BILINEAR);
				}
			}
		}
	};
};
