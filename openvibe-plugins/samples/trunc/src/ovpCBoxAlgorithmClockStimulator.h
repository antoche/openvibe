#ifndef __OpenViBEPlugins_BoxAlgorithm_ClockStimulator_H__
#define __OpenViBEPlugins_BoxAlgorithm_ClockStimulator_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_ClockStimulator     OpenViBE::CIdentifier(0x4F756D3F, 0x29FF0B96)
#define OVP_ClassId_BoxAlgorithm_ClockStimulatorDesc OpenViBE::CIdentifier(0x4FD067E9, 0x740D2AF0)


namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CBoxAlgorithmClockStimulator : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ClockStimulator);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationEncoder;
			OpenViBE::uint64 m_ui64StimulationId;
			OpenViBE::uint64 m_ui64InterstimulationInterval;
			OpenViBE::uint64 m_ui64LastStimulationDate;
			OpenViBE::uint64 m_ui64LastEndTime;
		};

		class CBoxAlgorithmClockStimulatorDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Clock stimulator"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Periodic stimulation generator"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Triggers stimulation at fixed frequency"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_ClockStimulator; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CBoxAlgorithmClockStimulator; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addOutput ("Generated stimulations",             OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Interstimulation interval (in sec)", OV_TypeId_Float,       "1.0");
				rBoxAlgorithmPrototype.addSetting("Stimulation",                        OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ClockStimulatorDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_ClockStimulator_H__
