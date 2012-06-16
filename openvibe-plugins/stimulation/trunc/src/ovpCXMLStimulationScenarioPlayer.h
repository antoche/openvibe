#ifndef __OpenViBEPlugins_Stimulation_CXMLStimulationScenarioPlayer_H__
#define __OpenViBEPlugins_Stimulation_CXMLStimulationScenarioPlayer_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <automaton/IXMLAutomatonReader.h>

#include <ebml/IReader.h>
#include <ebml/IWriter.h>
#include <ebml/TWriterCallbackProxy.h>

#include <vector>
#include <string>

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CXMLStimulationScenarioPlayer : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, virtual public OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback::ICallback
		{
		public:

			CXMLStimulationScenarioPlayer(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize();
			virtual OpenViBE::boolean uninitialize();

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);

			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock &rMessageClock);

			virtual OpenViBE::uint64 getClockFrequency(){ return (128LL<<32); }

			virtual OpenViBE::boolean process();

			virtual OpenViBE::boolean readAutomaton(const OpenViBE::CString& oFilename);

			virtual void setStimulationCount(const OpenViBE::uint32 ui32StimulationCount);
			virtual void setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate);

			virtual void writeStimulationOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_XMLStimulationScenarioPlayer)

		public:
			EBML::IReader* m_pReader;
			OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback* m_pStimulationReaderCallBack;

			std::vector<std::pair<OpenViBE::uint64, OpenViBE::uint64> > m_oStimulationReceived;

			EBML::IWriter* m_pWriter;
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::Stimulation::CXMLStimulationScenarioPlayer> * m_pOutputWriterCallbackProxy;
			OpenViBEToolkit::IBoxAlgorithmStimulationOutputWriter * m_pStimulationOutputWriterHelper;

			Automaton::IXMLAutomatonReader * m_pXMLAutomatonReader;
			Automaton::IAutomatonController * m_pAutomatonController;
			Automaton::IAutomatonContext * m_pAutomatonContext;
			Automaton::boolean m_bEndOfAutomaton;

			OpenViBE::uint64 m_ui64PreviousActivationTime;
		};

		/**
		* Plugin's description
		*/
		class CXMLStimulationScenarioPlayerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("XML stimulation scenario player"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Bruno Renier"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Plays a stimulation scenarion from an XML file"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Plays a stimulation scenarion from an XML file"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stimulation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }
			virtual void release(void)                                   { }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_XMLStimulationScenarioPlayer; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Stimulation::CXMLStimulationScenarioPlayer(); }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Incoming Stimulations", OV_TypeId_Stimulations);
				rPrototype.addOutput ("Outgoing Stimulations", OV_TypeId_Stimulations);
				rPrototype.addSetting("Filename", OV_TypeId_Filename, "");
				rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsDeprecated);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_XMLStimulationScenarioPlayerDesc)
		};
	};
};

#endif

