#ifndef __OpenViBEPlugins_BoxAlgorithm_RunCommand_H__
#define __OpenViBEPlugins_BoxAlgorithm_RunCommand_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <cstdio>
#include <vector>
#include <map>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_RunCommand     OpenViBE::CIdentifier(0x48843891, 0x7BFC57F4)
#define OVP_ClassId_BoxAlgorithm_RunCommandDesc OpenViBE::CIdentifier(0x29D449AE, 0x2CA94942)

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmRunCommand : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
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

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_RunCommand);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationDecoder;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > op_pStimulationSet;
			std::map < OpenViBE::uint64, std::vector < OpenViBE::CString > > m_vCommand;
		};

		class CBoxAlgorithmRunCommandListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				for(OpenViBE::uint32 i=0; i<rBox.getSettingCount(); i+=2)
				{
					sprintf(l_sName, "Stimulation %i", i/2+1);
					rBox.setSettingName(i, l_sName);
					rBox.setSettingType(i, OV_TypeId_Stimulation);
					sprintf(l_sName, "Command %i", i/2+1);
					rBox.setSettingName(i+1, l_sName);
					rBox.setSettingType(i+1, OV_TypeId_String);
				}
				return true;
			}

			virtual OpenViBE::boolean onSettingAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				char l_sName[1024];
				sprintf(l_sName, "OVTK_StimulationId_Label_%02X", ui32Index/2+1);
				rBox.setSettingDefaultValue(ui32Index, l_sName);
				rBox.setSettingValue(ui32Index, l_sName);
				rBox.addSetting("", OV_TypeId_String, "");
				this->check(rBox);
				return true;
			}

			virtual OpenViBE::boolean onSettingRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.removeSetting((ui32Index/2)*2);
				this->check(rBox);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmRunCommandDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Run Command"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Runs some command using system call depending on provided stimulations"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stimulation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_RunCommand; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Stimulation::CBoxAlgorithmRunCommand; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmRunCommandListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Stimulations",  OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Stimulation 1", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01");
				rBoxAlgorithmPrototype.addSetting("Command 1",     OV_TypeId_String,      "");
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddSetting);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_RunCommandDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_RunCommand_H__
