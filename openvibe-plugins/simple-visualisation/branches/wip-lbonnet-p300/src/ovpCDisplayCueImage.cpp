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
			m_eCurrentState(EDisplayCueImageState_Idle),
			m_uint32CurrentCueID(0),
			m_pOriginalPicture(NULL),
			m_pScaledPicture(NULL),
			m_bFullScreen(false),
			m_bError(false),
			m_ui64LastOutputChunkDate(-1),
			m_bNewImageRequested(false),
			m_bClearScreenRequested(false),
			m_bRedrawSucess(false)
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
					getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Error couldn't load ressource file : " << m_pImageNames[i] << "!\n";
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

			if(m_bRedrawSucess)
			{
				if(m_bNewImageRequested)
				{
					//this is first redraw() for that image
					m_oStimulationEncoder.getInputStimulationSet()->clear();
					m_oStimulationEncoder.getInputStimulationSet()->appendStimulation(
						m_pStimulationsId[m_uint32CurrentCueID],
						this->getPlayerContext().getCurrentTime(),
						0);
					m_oStimulationEncoder.encodeBuffer(0);
					l_pBoxIO->markOutputAsReadyToSend(0, m_ui64LastOutputChunkDate, this->getPlayerContext().getCurrentTime());
						
					m_ui64LastOutputChunkDate = this->getPlayerContext().getCurrentTime();
					m_bNewImageRequested = false;
				}
				if(m_bClearScreenRequested)
				{
					
					m_oStimulationEncoder.getInputStimulationSet()->clear();
					m_oStimulationEncoder.getInputStimulationSet()->appendStimulation(
						m_ui64ClearScreenStimulation,
						this->getPlayerContext().getCurrentTime(),
						0);
					m_oStimulationEncoder.encodeBuffer(0);
					l_pBoxIO->markOutputAsReadyToSend(0, m_ui64LastOutputChunkDate, this->getPlayerContext().getCurrentTime());
						
					m_ui64LastOutputChunkDate = this->getPlayerContext().getCurrentTime();

					m_bClearScreenRequested = false;
				}

				m_bRedrawSucess = false;
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

			for(uint32 input=0; input<getBoxAlgorithmContext()->getStaticBoxContext()->getInputCount(); input++)
			{
				for(uint32 chunk=0; chunk<l_pBoxIO->getInputChunkCount(input); chunk++)
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
							boolean l_bStateUpdated = false;
							uint64 l_ui64StimulationIdentifier = m_oStimulationDecoder.getOutputStimulationSet()->getStimulationIdentifier(stim);
							if(l_ui64StimulationIdentifier == m_ui64ClearScreenStimulation)
							{
								m_eCurrentState = EDisplayCueImageState_Idle;
								m_bClearScreenRequested = true;
								l_bStateUpdated = true;
							}
							else
							{
								for(uint32 i=0; i<=m_ui32NuberOfCue; i++)
								{
									if(l_ui64StimulationIdentifier == m_pStimulationsId[i])
									{
										m_eCurrentState = EDisplayCueImageState_Cue;
										m_uint32CurrentCueID = i;
										l_bStateUpdated = true;
										m_bNewImageRequested = true;
										break;
									}
								}
							}
							if(l_bStateUpdated)
							{
								if(GTK_WIDGET(m_pDrawingArea)->window)
								{
									gdk_window_invalidate_rect(GTK_WIDGET(m_pDrawingArea)->window,NULL,true);
									// it will trigger the callback redraw()
								}
							}
						}
					}
				}
			}

			return true;
		}

		//Callback called by GTK
		void CDisplayCueImage::redraw()
		{
			switch(m_eCurrentState)
			{
				case EDisplayCueImageState_Cue:
					drawCuePicture(m_uint32CurrentCueID);
					if(m_bNewImageRequested)
					{
						m_bRedrawSucess = true;
					}
					break;

				case EDisplayCueImageState_Idle:
					if(m_bClearScreenRequested)
					{
						m_bRedrawSucess = true; //always succes when drawing nothing at all
					}
					break;
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
