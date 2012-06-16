#ifndef __OpenViBEPlugins_SimpleVisualisation_CDisplayCueImage_H__
#define __OpenViBEPlugins_SimpleVisualisation_CDisplayCueImage_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <map>
#include <deque>

#define OVP_ClassId_DisplayCueImage                                            OpenViBE::CIdentifier(0x005789A4, 0x3AB78A36)
#define OVP_ClassId_DisplayCueImageDesc                                        OpenViBE::CIdentifier(0x086185A4, 0x796A854C)

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{

		class CDisplayCueImage :
				public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
				//public OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback::ICallback
		{
		public:

			CDisplayCueImage(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize();
			virtual OpenViBE::boolean uninitialize();
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::uint64 getClockFrequency(void){ return (128LL<<32); }
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process();
			virtual void redraw(void);
			virtual void resize(OpenViBE::uint32 ui32Width, OpenViBE::uint32 ui32Height);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_DisplayCueImage)

			protected:

				//virtual void setStimulationCount(const OpenViBE::uint32 ui32StimulationCount);
				//virtual void setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate);

				//virtual void processState(void);
				virtual void drawCuePicture(OpenViBE::uint32 uint32CueID);

			//The Builder handler used to create the interface
			::GtkBuilder* m_pBuilderInterface;
			::GtkWidget*  m_pMainWindow;
			::GtkWidget*  m_pDrawingArea;

			OpenViBEToolkit::TStimulationDecoder<CDisplayCueImage> m_oStimulationDecoder;
			OpenViBEToolkit::TStimulationEncoder<CDisplayCueImage> m_oStimulationEncoder;

			/*penViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pStimulationSet;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pMemoryBuffer;*/

			// For the display of the images:
			OpenViBE::boolean m_bImageRequested;        //when true: a new image must be drawn
			OpenViBE::int32   m_int32RequestedImageID;  //ID of the requested image. -1 => clear the screen

			OpenViBE::boolean m_bImageDrawn;            //when true: the new image has been drawn
			OpenViBE::int32   m_int32DrawnImageID;      //ID of the drawn image. -1 => clear the screen


			::GdkPixbuf** m_pOriginalPicture;
			::GdkPixbuf** m_pScaledPicture;

			::GdkColor m_oBackgroundColor;
			::GdkColor m_oForegroundColor;

			//Settings
			OpenViBE::uint32   m_ui32NuberOfCue;
			OpenViBE::CString* m_pImageNames;
			OpenViBE::uint64*  m_pStimulationsId;
			OpenViBE::uint64   m_ui64ClearScreenStimulation;
			OpenViBE::boolean  m_bFullScreen;

			//Start and end time of the last buffer
			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::uint64 m_ui64EndTime;
			OpenViBE::uint64 m_ui64LastOutputChunkDate;

			//We save the received stimulations
			OpenViBE::CStimulationSet m_oPendingStimulationSet;

			OpenViBE::boolean m_bError;
		};

		class CDisplayCueImageListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			virtual OpenViBE::boolean onSettingAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				char l_sName[1024] = "../share/openvibe-plugins/simple-visualisation/p300-magic-card/bomberman.png";

				rBox.setSettingDefaultValue(ui32Index, l_sName);
				rBox.setSettingValue(ui32Index, l_sName);

				sprintf(l_sName, "OVTK_StimulationId_Label_%02X", ui32Index/2);
				rBox.addSetting("", OV_TypeId_Stimulation, l_sName);
				rBox.setSettingDefaultValue(ui32Index+1, l_sName);
				rBox.setSettingValue(ui32Index+1, l_sName);

				this->checkSettingNames(rBox);
				return true;
			}

			virtual OpenViBE::boolean onSettingRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.removeSetting((ui32Index/2)*2);
				this->checkSettingNames(rBox);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);

		private:

			OpenViBE::boolean checkSettingNames(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				for(OpenViBE::uint32 i=2; i<rBox.getSettingCount()-1; i+=2)
				{
					sprintf(l_sName, "Cue Image %i", i/2);
					rBox.setSettingName(i, l_sName);
					rBox.setSettingType(i, OV_TypeId_Filename);
					sprintf(l_sName, "Stimulation %i", i/2);
					rBox.setSettingName(i+1, l_sName);
					rBox.setSettingType(i+1, OV_TypeId_Stimulation);
				}
				return true;
			}
		};

		/**
		 * Plugin's description
		 */
		class CDisplayCueImageDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Display cue image"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Joan Fruitet"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA Sophia"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Display cue images when receiving stimulations"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Display cue images when receiving specified stimulations and a fixation cross for OVTK_GDF_Cross_On_Screen"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Visualisation/Presentation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.1"); }
			virtual void release(void)                                   { }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_DisplayCueImage; }

			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-fullscreen"); }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SimpleVisualisation::CDisplayCueImage(); }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CDisplayCueImageListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }
			virtual OpenViBE::boolean hasFunctionality(OpenViBE::Kernel::EPluginFunctionality ePF) const
			{
				return ePF == OpenViBE::Kernel::PluginFunctionality_Visualization;
			}

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Stimulations", OV_TypeId_Stimulations);
				rPrototype.addOutput ("Stimulations", OV_TypeId_Stimulations);
				rPrototype.addSetting("Display images in full screen", OV_TypeId_Boolean, "false");
				rPrototype.addSetting("Clear screen Stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_VisualStimulationStop");
				rPrototype.addSetting("Cue Image 1", OV_TypeId_Filename, "../share/openvibe-plugins/simple-visualisation/p300-magic-card/mario.png");
				rPrototype.addSetting("Stimulation 1", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01");
				rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddSetting);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_DisplayCueImageDesc)
		};
	};
};

#endif
