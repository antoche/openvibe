#ifndef __OpenViBEPlugins_OutputChannel_H__
#define __OpenViBEPlugins_OutputChannel_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class COutputChannel
		{
		private:
			typedef enum
			{	SIGNAL_CHANNEL,
				STIMULATION_CHANNEL,
			} channel_t;
		public:

			OpenViBE::boolean initialize(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm>* pTBoxAlgorithm);
			OpenViBE::boolean uninitialize();
			
			void sendStimulation(OpenViBE::IStimulationSet* stimset, OpenViBE::uint64 startTimestamp, OpenViBE::uint64 endTimestamp);
			void sendSignal(OpenViBE::CMatrix* pMatrix, OpenViBE::uint64 startTimestamp, OpenViBE::uint64 endTimestamp);

			void sendHeader(OpenViBE::uint64 samplingRate, OpenViBE::CMatrix* pMatrix);
			void processSynchroSignal(OpenViBE::uint64 stimulationPosition, OpenViBE::uint64 signalPosition);

		protected:

			OpenViBE::CMatrix*                                                      m_oMatrixBuffer;
			
			OpenViBE::uint64                                                        m_ui64TimeStimulationPosition;
			OpenViBE::uint64                                                        m_ui64TimeSignalPosition;
			
			OpenViBE::uint64                                                        m_ui64SamplingRate;

			// parent memory
			OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm>*     m_pTBoxAlgorithm;
			
			// signal section
			OpenViBE::Kernel::IAlgorithmProxy*                                      m_pStreamEncoderSignal;
			
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* >	    op_pMemoryBufferSignal;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* >				ip_pMatrixSignal;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >				ip_ui64SamplingRateSignal;			
			
			// stimulation section
			OpenViBE::Kernel::IAlgorithmProxy*                                      m_pStreamEncoderStimulation;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* >		op_pMemoryBufferStimulation;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* >		ip_pStimulationSetStimulation;
		};
	};
};

#endif // __OpenViBEPlugins_OutputChannel_H__
