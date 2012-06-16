#ifndef __OpenViBEPlugins_BoxAlgorithm_SoundPlayer_H__
#define __OpenViBEPlugins_BoxAlgorithm_SoundPlayer_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <map>
#include <vector>

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmSoundPlayer : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			// virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			// virtual OpenViBE::boolean processEvent(OpenViBE::CMessageEvent& rMessageEvent);
			// virtual OpenViBE::boolean processSignal(OpenViBE::CMessageSignal& rMessageSignal);
			// virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SoundPlayer);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			std::map < OpenViBE::uint64, std::vector < OpenViBE::CString > > m_vSoundInfo;
		};

		class CBoxAlgorithmSoundPlayerListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				for(OpenViBE::uint32 i=0; i<rBox.getSettingCount()/2; i++)
				{
					rBox.setSettingName(i*2, "Stimulation");
					rBox.setSettingType(i*2, OV_TypeId_Stimulation);

					rBox.setSettingName(i*2+1, "Sound to play");
					rBox.setSettingType(i*2+1, OV_TypeId_Filename);
				}

				return true;
			}

			virtual OpenViBE::boolean onSettingRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				if(ui32Index&1)
				{
					rBox.removeSetting(ui32Index-1);
				}
				else
				{
					rBox.removeSetting(ui32Index+1);
				}
				return this->check(rBox);
			};

			virtual OpenViBE::boolean onSettingAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.addSetting("", OV_UndefinedIdentifier, "");
				rBox.setSettingDefaultValue(ui32Index, "OVTK_StimulationId_Beep");
				rBox.setSettingValue(ui32Index, "OVTK_StimulationId_Beep");
				rBox.setSettingDefaultValue(ui32Index+1, "../share/openvibe-plugins/stimulation/ov_beep.wav");
				rBox.setSettingValue(ui32Index+1, "../share/openvibe-plugins/stimulation/ov_beep.wav");

				return this->check(rBox);
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmSoundPlayerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Sound Player (Deprecated)"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard / Matthieu Goyat"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA / Gipsa-Lab"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Deprecated."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stimulation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_SoundPlayer; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Stimulation::CBoxAlgorithmSoundPlayer; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmSoundPlayerListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-media-play"); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Stimulation stream", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Stimulation", OV_TypeId_Stimulation,"OVTK_StimulationId_Beep");
				rBoxAlgorithmPrototype.addSetting("Sound to play", OV_TypeId_Filename, "../share/openvibe-plugins/stimulation/ov_beep.wav");
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddSetting);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsUnstable);

				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsDeprecated);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SoundPlayerDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_SoundPlayer_H__
