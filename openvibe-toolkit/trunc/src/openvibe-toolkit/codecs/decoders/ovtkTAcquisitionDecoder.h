#ifndef __OpenViBEToolkit_TAcquisitionDecoder_H__
#define __OpenViBEToolkit_TAcquisitionDecoder_H__

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTStreamedMatrixDecoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TAcquisitionDecoderLocal : public T
	{
	protected:

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64BufferDuration;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pExperimentInformationStream;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pSignalStream;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pStimulationStream;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pChannelLocalisationStream;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pInputMemoryBuffer;

		OpenViBE::boolean initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_AcquisitionStreamDecoder));
			m_pCodec->initialize();

			m_pInputMemoryBuffer.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_InputParameterId_MemoryBufferToDecode));

			op_ui64BufferDuration.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_BufferDuration));
			op_pExperimentInformationStream.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ExperimentInformationStream));
			op_pSignalStream.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_SignalStream));
			op_pStimulationStream.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_StimulationStream));
			op_pChannelLocalisationStream.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelLocalisationStream));

			return true;
		}

	public:

		using T::initialize;

		OpenViBE::boolean uninitialize(void)
		{
			if(m_pBoxAlgorithm == NULL || m_pCodec == NULL)
			{
				return false;
			}

			op_pChannelLocalisationStream.uninitialize();
			op_pStimulationStream.uninitialize();
			op_pSignalStream.uninitialize();
			op_pExperimentInformationStream.uninitialize();
			op_ui64BufferDuration.uninitialize();

			m_pInputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getBufferDuration() { return op_ui64BufferDuration; }
		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* >& getExperimentInformationStream() { return op_pExperimentInformationStream; }
		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* >& getSignalStream() { return op_pSignalStream; }
		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* >& getStimulationStream() { return op_pStimulationStream; }
		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* >& getChannelLocalisationStream() { return op_pChannelLocalisationStream; }

		virtual OpenViBE::boolean isHeaderReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedHeader);
		}

		virtual OpenViBE::boolean isBufferReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedBuffer);
		}

		virtual OpenViBE::boolean isEndReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedEnd);
		}
	};

	template <class T>
	class TAcquisitionDecoder : public TAcquisitionDecoderLocal < TStreamedMatrixDecoderLocal < TDecoder < T > > >
	{
	private:

		using TAcquisitionDecoderLocal < TStreamedMatrixDecoderLocal < TDecoder < T > > >::m_pBoxAlgorithm;

	public:

		using TAcquisitionDecoderLocal < TStreamedMatrixDecoderLocal < TDecoder < T > > >::uninitialize;

		TAcquisitionDecoder()
		{
		}

		TAcquisitionDecoder(T& rBoxAlgorithm)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm);
		}

		virtual ~TAcquisitionDecoder()
		{
			this->uninitialize();
		}
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif //__OpenViBEToolkit_TAcquisitionDecoder_H__
