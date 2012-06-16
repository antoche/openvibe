#ifndef __OpenViBEPlugins_BoxAlgorithm_StimulationMultiplexer_H__
#define __OpenViBEPlugins_BoxAlgorithm_StimulationMultiplexer_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <vector>
#include <map>
#include <cstdio>

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmStimulationMultiplexer : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
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

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StimulationMultiplexer);

		protected:

			typedef struct
			{
				OpenViBE::uint64 m_ui64Identifier;
				OpenViBE::uint64 m_ui64Date;
				OpenViBE::uint64 m_ui64Duration;
			} SStimulation;

			std::vector < OpenViBE::Kernel::IAlgorithmProxy* > m_vStreamDecoder;
			std::vector < OpenViBE::uint64 > m_vStreamDecoderEndTime;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;

			OpenViBE::uint64 m_ui64LastStartTime;
			OpenViBE::uint64 m_ui64LastEndTime;
			OpenViBE::boolean m_bHasSentHeader;

			std::multimap < OpenViBE::uint64, CBoxAlgorithmStimulationMultiplexer::SStimulation > m_vStimulation;
		};

		class CBoxAlgorithmStimulationMultiplexerListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				OpenViBE::uint32 i;

				for(i=0; i<rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Input stimulations %u", i+1);
					rBox.setInputName(i, l_sName);
					rBox.setInputType(i, OV_TypeId_Stimulations);
				}

				return true;
			}

			virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return this->check(rBox); }
			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return this->check(rBox); };

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmStimulationMultiplexerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Stimulation multiplexer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Merges several stimulation streams into one stimulation stream."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("The stimulations are ordered according to their start date. Thus each time all the input have chunks covering a period of time, a new output chunk is sent. This box may eventually produce output chunk reflecting a different duration depending on the inputs."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Streaming"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-sort-ascending"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_StimulationMultiplexer; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Stimulation::CBoxAlgorithmStimulationMultiplexer; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmStimulationMultiplexerListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput ("Input stimulations 1",     OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput ("Input stimulations 2",     OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput("Multiplexed stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddInput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationMultiplexerDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_StimulationMultiplexer_H__
