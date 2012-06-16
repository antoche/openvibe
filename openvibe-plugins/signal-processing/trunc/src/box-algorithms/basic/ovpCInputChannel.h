#ifndef __OpenViBEPlugins_InputChannel_H__
#define __OpenViBEPlugins_InputChannel_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#define SET_BIT(bit)	(1 << bit)
namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CInputChannel
		{
		private:
			typedef enum
			{	NOT_STARTED                    = 0,
				SIGNAL_HEADER_DETECTED         = SET_BIT(0),
				STIMULATION_SYNCHRO_DETECTED   = SET_BIT(1),
				SIGNAL_SYNCHRO_DETECTED        = SET_BIT(2),
				IN_WORK                        = SET_BIT(3),
			} status_t;

			typedef enum
			{	SIGNAL_CHANNEL,
				STIMULATION_CHANNEL,
			} channel_t;
						
		public:

			~CInputChannel();
			OpenViBE::boolean initialize(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm>* pTBoxAlgorithm);
			OpenViBE::boolean uninitialize();

			OpenViBE::boolean			hasHeader() { return (m_ui32Status & SIGNAL_HEADER_DETECTED) != 0; }
			OpenViBE::boolean			hasSynchro() { return hasSynchroStimulation() && hasSynchroSignal(); }
			OpenViBE::boolean			isWorking() { return (m_ui32Status & IN_WORK) != 0; }
			OpenViBE::boolean           waitForSignalHeader();
			void                        waitForSynchro();
			void                        startWorking() { m_ui32Status |= IN_WORK; }
			OpenViBE::uint64            getStimulationPosition() { return m_ui64TimeStimulationPosition; }
			OpenViBE::uint64            getSignalPosition() { return m_ui64TimeSignalPosition; }
			OpenViBE::uint32            getNbOfStimulationBuffers();
			OpenViBE::uint32            getNbOfSignalBuffers();
			OpenViBE::IStimulationSet*  getStimulation(OpenViBE::uint64& startTimestamp, OpenViBE::uint64& endTimestamp, const OpenViBE::uint32 stimulationIndex);
			OpenViBE::CMatrix*          getSignal(OpenViBE::uint64& startTimestamp, OpenViBE::uint64& endTimestamp, const OpenViBE::uint32 signalIndex);
			OpenViBE::CMatrix*          getMatrixPtr();
			OpenViBE::uint64            getSamplingRate() {return op_ui64SamplingRateSignal;}
		
		private:
			OpenViBE::boolean			hasSynchroStimulation() { return (m_ui32Status & STIMULATION_SYNCHRO_DETECTED) != 0; }
			OpenViBE::boolean			hasSynchroSignal() { return (m_ui32Status & SIGNAL_SYNCHRO_DETECTED) != 0; }
			void                        waitForSynchroStimulation();
			void                        waitForSynchroSignal();
			void                        processSynchroSignal();
			OpenViBE::CMatrix*          getMatrix();
			void                        copyData(const OpenViBE::boolean copyFirstBlock, OpenViBE::uint64 matrixIndex);

		protected:
			OpenViBE::uint16														m_ui32Status;
			OpenViBE::CMatrix*                                                      m_oMatrixBuffer[2];
			OpenViBE::uint64                                                        m_ui64PtrMatrixIndex;
			OpenViBE::uint64                                                        m_ui64SynchroStimulation;

			OpenViBE::uint64                                                        m_ui64TimeStimulationPosition;
			OpenViBE::uint64                                                        m_ui64TimeStampStartStimulation;
			OpenViBE::uint64                                                        m_ui64TimeStampEndStimulation;
			OpenViBE::boolean                                                       m_bFirstStimulation;

			
			OpenViBE::uint64                                                        m_ui64TimeSignalPosition;
			OpenViBE::uint64                                                        m_ui64TimeStampStartSignal;
			OpenViBE::uint64                                                        m_ui64TimeStampEndSignal;
		
			OpenViBE::uint64                                                        m_ui64FirstBlock;
			OpenViBE::uint64                                                        m_ui64SecondBlock;
			OpenViBE::uint64                                                        m_ui64NbSamples;
			OpenViBE::uint64                                                        m_ui64NbChannels;
			OpenViBE::boolean                                                       m_bFirstChunk;

			OpenViBE::IStimulationSet*                                              m_oIStimulationSet;

			// parent memory
			OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm>*     m_pTBoxAlgorithm;
			
			// signal section
			OpenViBE::Kernel::IAlgorithmProxy*                                      m_pStreamDecoderSignal;
			
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* >	ip_pMemoryBufferSignal;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* >				op_pMatrixSignal;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >				op_ui64SamplingRateSignal;
			
			
			// stimulation section
			OpenViBE::Kernel::IAlgorithmProxy*                                      m_pStreamDecoderStimulation;

			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* >	ip_pMemoryBufferStimulation;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* >		op_pStimulationSetStimulation;
		};
	};
};

#endif // __OpenViBEPlugins_InputChannel_H__
