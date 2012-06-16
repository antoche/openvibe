#ifndef __OpenViBEPlugins_BoxAlgorithm_SignalConcatenation_H__
#define __OpenViBEPlugins_BoxAlgorithm_SignalConcatenation_H__

//You may have to change this path to match your folder organisation
#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <stdio.h>
#include <vector>
#include <list>
// The unique identifiers for the box and its descriptor.
// Identifier are randomly chosen by the skeleton-generator.
#define OVP_ClassId_BoxAlgorithm_SignalConcatenation OpenViBE::CIdentifier(0x372F3A9D, 0x49E20CD2)
#define OVP_ClassId_BoxAlgorithm_SignalConcatenationDesc OpenViBE::CIdentifier(0x372F3A9D, 0x49E20CD2)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		/**
		 * \class CBoxAlgorithmSignalConcatenation
		 * \author Laurent Bonnet (INRIA)
		 * \date Tue Jun 28 09:52:48 2011
		 * \brief The class CBoxAlgorithmSignalConcatenation describes the box Signal Concatenation.
		 *
		 */
		class CBoxAlgorithmSignalConcatenation : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:
			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::uint64 getClockFrequency(void) { return 8LL<<32; }

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			
			
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SignalConcatenation);

		protected:
			virtual OpenViBE::boolean concate(void);
			OpenViBE::boolean m_bConcatenationFinished;
			OpenViBE::boolean m_bResynchroDone;
			OpenViBE::uint64 m_ui64TimeOut;

			OpenViBE::boolean m_bHeaderSent;
			OpenViBE::uint32 m_ui32HeaderReceivedCount;
			OpenViBE::boolean m_bStimHeaderSent;
			OpenViBE::boolean m_bEndSent;

			OpenViBE::uint32 m_ui32SampleCountPerBuffer;

			std::vector < OpenViBE::uint64 > m_vEndOfFileStimulations;
			std::vector < OpenViBE::boolean > m_vEndOfFileReached;

			struct Chunk
			{
				OpenViBE::IMemoryBuffer * m_pMemoryBuffer;
				OpenViBE::uint64          m_ui64StartTime;
				OpenViBE::uint64          m_ui64EndTime;
			};

			// File end times
			std::vector < OpenViBE::uint64 > m_vFileEndTimes;

			// The signal buffers, one per file
			std::vector < std::vector < Chunk > > m_vSignalChunkBuffers;
			
			
			// The stimulations are stored in one stimulation set per file. The chunk are reconstructed.
			std::vector < OpenViBE::IStimulationSet *> m_vStimulationSets;
			
			//The decoders, (1 signal/1 stim) per file
			std::vector < OpenViBEToolkit::TStimulationDecoder < CBoxAlgorithmSignalConcatenation > *>m_vStimulationDecoders;
			std::vector < OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmSignalConcatenation > *> m_vSignalDecoders;
			
			// the encoders : signal, stim and trigger encoder.
			OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmSignalConcatenation > m_oSignalEncoder;
			OpenViBEToolkit::TStimulationEncoder < CBoxAlgorithmSignalConcatenation > m_oStimulationEncoder;
			OpenViBEToolkit::TStimulationEncoder < CBoxAlgorithmSignalConcatenation > m_oTriggerEncoder;
	
			OpenViBE::uint64 m_ui64TriggerDate;
			OpenViBE::uint64 m_ui64LastChunkStartTime;
			OpenViBE::uint64 m_ui64LastChunkEndTime;

			struct ConcatenationState
			{
				OpenViBE::uint32 ui32CurrentFileIndex;
				OpenViBE::uint32 ui32CurrentChunkIndex;
				OpenViBE::uint32 ui32CurrentStimulationIndex;
			};
			ConcatenationState m_sState;
		};

		
		// The box listener can be used to call specific callbacks whenever the box structure changes : input added, name changed, etc.
		// Please uncomment below the callbacks you want to use.
		class CBoxAlgorithmSignalConcatenationListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				OpenViBE::uint32 i;

				for(i=0; i<rBox.getInputCount()>>1; i++)
				{
					sprintf(l_sName, "Input signal %u", i+1);
					rBox.setInputName(i*2, l_sName);
					rBox.setInputType(i*2, OV_TypeId_Signal);

					sprintf(l_sName, "Input stimulations %u", i+1);
					rBox.setInputName(i*2+1, l_sName);
					rBox.setInputType(i*2+1, OV_TypeId_Stimulations);

					sprintf(l_sName, "End-of-file stimulation for input %u", i+1);
					rBox.setSettingName(i+1, l_sName);
				}

				return true;
			}

			virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				if(ui32Index&1)//odd index
				{
					rBox.removeInput(ui32Index-1);
				}
				else //even index
				{
					rBox.removeInput(ui32Index);
				}
				
				rBox.removeSetting(ui32Index>>1);

				return this->check(rBox);
			}

			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.addInput("", OV_TypeId_Stimulations);
				rBox.addSetting("",OV_TypeId_Stimulation,"OVTK_StimulationId_ExperimentStop");
				return this->check(rBox);
			};
			
			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};
		

		/**
		 * \class CBoxAlgorithmSignalConcatenationDesc
		 * \author Laurent Bonnet (INRIA)
		 * \date Tue Jun 28 09:52:48 2011
		 * \brief Descriptor of the box Signal Concatenation.
		 *
		 */
		class CBoxAlgorithmSignalConcatenationDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Signal Concatenation"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Laurent Bonnet"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Concatenates multiple signal streams"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("The signal stream concatenation box reads multiple streams in parallel, and produces a single stream that is the concatenation of all inputs."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("2.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-add"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_SignalConcatenation; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBoxAlgorithmSignalConcatenation; }
			
			
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmSignalConcatenationListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }
			
			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input signal 1",OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInput("Input stimulations 1",OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput("Input signal 2",OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInput("Input stimulations 2",OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				
				rBoxAlgorithmPrototype.addOutput("Signal",OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Stimulations",OV_TypeId_Stimulations);
				
				rBoxAlgorithmPrototype.addOutput("Status",OV_TypeId_Stimulations);
				
				rBoxAlgorithmPrototype.addSetting("Time out before assuming end-of-file (in sec)",OV_TypeId_Integer,"5");
				rBoxAlgorithmPrototype.addSetting("End-of-file stimulation for input 1",OV_TypeId_Stimulation,"OVTK_StimulationId_ExperimentStop");
				rBoxAlgorithmPrototype.addSetting("End-of-file stimulation for input 2",OV_TypeId_Stimulation,"OVTK_StimulationId_ExperimentStop");
				
				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);
				
				return true;
			}
			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalConcatenationDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_SignalConcatenation_H__
