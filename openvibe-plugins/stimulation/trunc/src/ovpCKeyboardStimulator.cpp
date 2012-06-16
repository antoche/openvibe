#include "ovpCKeyboardStimulator.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace OpenViBE;
using namespace Plugins;
using namespace Kernel;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Stimulation;

using namespace OpenViBEToolkit;

using namespace std;

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
/*
		//! Callback for the close window button
		static void keyboard_stimulator_gtk_widget_do_nothing(::GtkWidget* pWidget)
		{
		}
*/
		// Called when a key is pressed on the keyboard
		gboolean KeyboardStimulator_KeyPressCallback(GtkWidget *widget, GdkEventKey *event, gpointer data)
		{
			reinterpret_cast<CKeyboardStimulator*>(data)->processKey(event->keyval, true);
			return true;
		}

		// Called when a key is released on the keyboard
		gboolean KeyboardStimulator_KeyReleaseCallback(GtkWidget *widget, GdkEventKey *event, gpointer data)
		{
			reinterpret_cast<CKeyboardStimulator*>(data)->processKey(event->keyval, false);
			return true;
		}

		/**
		 * Called when a key has been pressed.
		 * \param uiKey The gdk value to the pressed key.
		 * */
		void CKeyboardStimulator::processKey(guint uiKey, bool bState)
		{
			//if there is one entry, adds the stimulation to the list of stims to be sent
			if(m_oKeyToStimulation.count(uiKey) != 0 && bState != m_oKeyToStimulation[uiKey].m_bStatus)
			{
				if(bState)
				{
					// getLogManager() << LogLevel_Trace << "Pressed key code " << (uint32)uiKey << "\n";
					m_oStimulationToSend.push_back(m_oKeyToStimulation[uiKey].m_ui64StimulationPress);
				}
				else
				{
					// getLogManager() << LogLevel_Trace << "Released key code " << (uint32)uiKey << "\n";
					m_oStimulationToSend.push_back(m_oKeyToStimulation[uiKey].m_ui64StimulationRelease);
				}
				m_oKeyToStimulation[uiKey].m_bStatus=bState;
			}
			else
			{
				// this->getLogManager() << LogLevel_Warning << "Unhandled key code " << (uint32)uiKey << "\n";
				m_bUnknownKeyPressed = true;
				m_ui32UnknownKeyCode = (uint32)uiKey;
			}
		}

		/**
		 * Parse the configuration file and creates the Key/Stimulation associations.
		 * \param pFilename The name of the configuration file.
		 * \return True if the file was correctly parsed.
		 * */
		boolean CKeyboardStimulator::parseConfigurationFile(const char * pFilename)
		{
			ifstream l_oFile;
			l_oFile.open(pFilename);

			if(!l_oFile)
			{
				return false;
			}

			string l_oKeyName;
			string l_oStimulationPress;
			string l_oStimulationRelease;

			//reads all the couples key name/stim
			while(!l_oFile.eof() && !l_oFile.fail())
			{
				l_oFile>>l_oKeyName>>l_oStimulationPress>>l_oStimulationRelease;

				SKey l_oKey;
				l_oKey.m_ui64StimulationPress=0;
				l_oKey.m_ui64StimulationRelease=0;
				l_oKey.m_bStatus=false;

				// MAY CAUSE ENDIANNESS PROBLEMS !
				sscanf(l_oStimulationPress.c_str(),   "0x%08Lx", &l_oKey.m_ui64StimulationPress);
				sscanf(l_oStimulationRelease.c_str(), "0x%08Lx", &l_oKey.m_ui64StimulationRelease);

				m_oKeyToStimulation[gdk_keyval_from_name(l_oKeyName.c_str())] = l_oKey;
			}

			l_oFile.close();

			return true;
		}

		void CKeyboardStimulator::writeStimulationOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
		{
			appendOutputChunkData<0>(pBuffer, ui64BufferSize);
		}

		CKeyboardStimulator::CKeyboardStimulator(void) :
			m_pWriter(NULL),
			m_pOutputWriterCallbackProxy(NULL),
			m_pStimulationOutputWriterHelper(NULL),
			m_pWidget(NULL),
			m_ui64PreviousActivationTime(0),
			m_bError(false),
			m_bUnknownKeyPressed(false),
			m_ui32UnknownKeyCode(0)
		{
		}

		boolean CKeyboardStimulator::initialize()
		{
			const IBox* l_pBoxContext=getBoxAlgorithmContext()->getStaticBoxContext();
			CString l_sFileName;

			// Parses box settings to find input file's name
			l_pBoxContext->getSettingValue(0, l_sFileName);

			if(!parseConfigurationFile((const char*)l_sFileName))
			{
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning <<"Problem while parsing configuration file!\n";
				m_bError = true;

				return false;
			}

			//EBML writer, ...
			m_pOutputWriterCallbackProxy = new EBML::TWriterCallbackProxy1<OpenViBEPlugins::Stimulation::CKeyboardStimulator>(*this, &CKeyboardStimulator::writeStimulationOutput);

			m_pWriter=EBML::createWriter(*m_pOutputWriterCallbackProxy);

			m_pStimulationOutputWriterHelper=createBoxAlgorithmStimulationOutputWriter();

			string l_sRed("#602020");
			string l_sGreen("#206020");
			string l_sBlue("#202060");

			stringstream ss;
			ss << "\nUse your keyboard to send stimulations\nAvailable keys are :\n\n";
			for(map<guint, SKey>::const_iterator i=m_oKeyToStimulation.begin(); i!=m_oKeyToStimulation.end(); i++)
			{
				ss << "<span size=\"smaller\">\t";
				ss << "<span style=\"italic\" foreground=\"" << l_sGreen << "\">" << gdk_keyval_name(i->first) << "</span>";
				ss << "\t";
				ss << "Pressed : <span style=\"italic\" foreground=\"" << l_sBlue << "\">" << this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, i->second.m_ui64StimulationPress) << "</span>";
				ss << "\t";
				ss << "Released : <span style=\"italic\" foreground=\"" << l_sBlue << "\">" << this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, i->second.m_ui64StimulationRelease) << "</span>";
				ss << "\t</span>\n";
			}

			::GtkBuilder* l_pBuilder=gtk_builder_new(); // glade_xml_new("../share/openvibe-plugins/stimulation/keyboard-stimulator.ui", NULL, NULL);
			gtk_builder_add_from_file(l_pBuilder, "../share/openvibe-plugins/stimulation/keyboard-stimulator.ui", NULL);
			gtk_builder_connect_signals(l_pBuilder, NULL);

			m_pWidget=GTK_WIDGET(gtk_builder_get_object(l_pBuilder, "keyboard_stimulator-eventbox"));

			gtk_label_set_markup(GTK_LABEL(gtk_builder_get_object(l_pBuilder, "keyboard_stimulator-label")), ss.str().c_str());

			g_signal_connect(m_pWidget, "key-press-event",   G_CALLBACK(KeyboardStimulator_KeyPressCallback),   this);
			g_signal_connect(m_pWidget, "key-release-event", G_CALLBACK(KeyboardStimulator_KeyReleaseCallback), this);
			g_object_unref(l_pBuilder);

			this->getVisualisationContext().setWidget(m_pWidget);

			//write stimulation stream header
			m_pStimulationOutputWriterHelper->writeHeader(*m_pWriter);
			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, 0, 0);

			return true;
		}

		boolean CKeyboardStimulator::uninitialize()
		{
			delete m_pOutputWriterCallbackProxy;
			m_pOutputWriterCallbackProxy= NULL;

			if(m_pWriter)
			{
				m_pWriter->release();
				m_pWriter = NULL;
			}

			if(m_pStimulationOutputWriterHelper)
			{
				releaseBoxAlgorithmStimulationOutputWriter(m_pStimulationOutputWriterHelper);
				m_pStimulationOutputWriterHelper=NULL;
			}

			g_object_unref(m_pWidget);

			return true;
		}

		boolean CKeyboardStimulator::processClock(CMessageClock &rMessageClock)
		{
			if(m_bError)
			{
				return false;
			}

			if (m_bUnknownKeyPressed)
			{
				this->getLogManager() << LogLevel_Warning << "Unhandled key code " << m_ui32UnknownKeyCode << "\n";
				m_bUnknownKeyPressed = false;
			}

			uint64 l_ui64CurrentTime=rMessageClock.getTime();

			if(l_ui64CurrentTime!=m_ui64PreviousActivationTime)
			{
				IBoxIO * l_pBoxIO = getBoxAlgorithmContext()->getDynamicBoxContext();
				m_pStimulationOutputWriterHelper->setStimulationCount(m_oStimulationToSend.size());
				for(size_t i=0 ; i<m_oStimulationToSend.size() ; i++)
				{
					m_pStimulationOutputWriterHelper->setStimulation(i, m_oStimulationToSend[i], l_ui64CurrentTime);
				}
				m_oStimulationToSend.clear();

				m_pStimulationOutputWriterHelper->writeBuffer(*m_pWriter);
				l_pBoxIO->markOutputAsReadyToSend(0, m_ui64PreviousActivationTime, l_ui64CurrentTime);
				getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
			}

			m_ui64PreviousActivationTime = l_ui64CurrentTime;
			return true;
		}

		boolean CKeyboardStimulator::process()
		{
			return true;
		}
	};
};
