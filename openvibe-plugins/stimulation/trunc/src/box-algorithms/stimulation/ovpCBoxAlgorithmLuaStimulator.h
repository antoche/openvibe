#ifndef __OpenViBEPlugins_BoxAlgorithm_LuaStimulator_H__
#define __OpenViBEPlugins_BoxAlgorithm_LuaStimulator_H__

#if defined TARGET_HAS_ThirdPartyLua

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/version.hpp>

#include <map>
#include <vector>
#include <cstdio>
#include <cstring>

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_LuaStimulator     OpenViBE::CIdentifier(0x0B5A2787, 0x02750621)
#define OVP_ClassId_BoxAlgorithm_LuaStimulatorDesc OpenViBE::CIdentifier(0x67AF36F3, 0x2B424F46)

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmLuaStimulator : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, public boost::noncopyable
		{
		public:

			CBoxAlgorithmLuaStimulator(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			// virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_LuaStimulator);

			virtual OpenViBE::boolean _waitForStimulation(OpenViBE::uint32 ui32InputIndex, OpenViBE::uint32 ui32StimulationIndex);

			virtual OpenViBE::boolean getInputCountCB(OpenViBE::uint32& rui64Count);
			virtual OpenViBE::boolean getOutputCountCB(OpenViBE::uint32& rui64Count);
			virtual OpenViBE::boolean getSettingCountCB(OpenViBE::uint32& rui64Count);
			virtual OpenViBE::boolean getSettingCB(OpenViBE::uint32 ui32SettingIndex, OpenViBE::CString& rsSetting);
			virtual OpenViBE::boolean getConfigCB(const OpenViBE::CString& rsString, OpenViBE::CString& rsConfig);

			virtual OpenViBE::boolean getCurrentTimeCB(OpenViBE::uint64& rui64Time);
			virtual OpenViBE::boolean sleepCB(void);
			virtual OpenViBE::boolean getStimulationCountCB(OpenViBE::uint32 ui32InputIndex, OpenViBE::uint32& rui32Count);
			virtual OpenViBE::boolean getStimulationCB(OpenViBE::uint32 ui32InputIndex, OpenViBE::uint32 ui32StimulationIndex, OpenViBE::uint64& rui64Identifier, OpenViBE::uint64& rui64Time, OpenViBE::uint64& rui64Duration);
			virtual OpenViBE::boolean removeStimulationCB(OpenViBE::uint32 ui32InputIndex, OpenViBE::uint32 ui32StimulationIndex);
			virtual OpenViBE::boolean sendStimulationCB(OpenViBE::uint32 ui32OutputIndex, OpenViBE::uint64 ui64Identifier, OpenViBE::uint64 ui64Time, OpenViBE::uint64 ui64Duration);
			virtual OpenViBE::boolean log(const OpenViBE::Kernel::ELogLevel eLogLevel, const OpenViBE::CString& sText);

		public:

			void doThread(void);

			enum
			{
				State_Unstarted,
				State_Processing,
				State_Finished,
			};

			OpenViBE::uint32 m_ui32State;

		protected:

			lua_State* m_pLuaState;

			OpenViBE::uint64 m_ui64LastTime;
			std::vector < std::multimap < OpenViBE::uint64, std::pair < OpenViBE::uint64, OpenViBE::uint64 > > > m_vOutputStimulation;
			std::vector < std::multimap < OpenViBE::uint64, std::pair < OpenViBE::uint64, OpenViBE::uint64 > > > m_vInputStimulation;

			boost::thread* m_pLuaThread;
			boost::mutex m_oMutex;
			boost::mutex::scoped_lock m_oInnerLock;
			boost::mutex::scoped_lock m_oOuterLock;
			boost::condition m_oCondition;

		protected:

			std::vector < OpenViBE::Kernel::IAlgorithmProxy* > m_vStreamDecoder;
			std::vector < OpenViBE::Kernel::IAlgorithmProxy* > m_vStreamEncoder;

		private:

			CBoxAlgorithmLuaStimulator(const CBoxAlgorithmLuaStimulator&);
		};

		class CBoxAlgorithmLuaStimulatorListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.setInputType(ui32Index, OV_TypeId_Stimulations);
				return true;
			}

			virtual OpenViBE::boolean onOutputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.setOutputType(ui32Index, OV_TypeId_Stimulations);
				return true;
			}

			virtual OpenViBE::boolean onSettingAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				char l_sSettingName[1024];
				::sprintf(l_sSettingName, "Setting %i", ui32Index);
				rBox.setSettingType(ui32Index, OV_TypeId_String);
				rBox.setSettingName(ui32Index, l_sSettingName);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmLuaStimulatorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Lua Stimulator"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Generates some stimulations according to an Lua script"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Scripting"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_LuaStimulator; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Stimulation::CBoxAlgorithmLuaStimulator; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmLuaStimulatorListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addOutput ("Stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Lua Script", OV_TypeId_Script, "");
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddOutput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddSetting);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifySetting);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_LuaStimulatorDesc);
		};
	};
};

#endif // TARGET_HAS_ThirdPartyLua

#endif // __OpenViBEPlugins_BoxAlgorithm_LuaStimulator_H__
