#ifndef __OpenViBEToolkit_TChannelLocalisationDecoder_H__
#define __OpenViBEToolkit_TChannelLocalisationDecoder_H__

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "ovtkTStreamedMatrixDecoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TChannelLocalisationDecoderLocal : public T
	{

	protected:

		OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > m_bOutputDynamic;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pInputMemoryBuffer;
		using T::m_pOutputMatrix;

		OpenViBE::boolean initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_ChannelLocalisationStreamDecoder));
			m_pCodec->initialize();
			m_pInputMemoryBuffer.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_InputParameterId_MemoryBufferToDecode));
			m_pOutputMatrix.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputParameterId_Matrix));
			m_bOutputDynamic.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputParameterId_Dynamic));

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

			m_bOutputDynamic.uninitialize();
			m_pOutputMatrix.uninitialize();
			m_pInputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean >& getOutputDynamic()
		{
			return m_bOutputDynamic;
		}

		virtual OpenViBE::boolean isHeaderReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputTriggerId_ReceivedHeader);
		}

		virtual OpenViBE::boolean isBufferReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputTriggerId_ReceivedBuffer);
		}

		virtual OpenViBE::boolean isEndReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputTriggerId_ReceivedEnd);
		}
	};

	template <class T>
	class TChannelLocalisationDecoder : public TChannelLocalisationDecoderLocal < TStreamedMatrixDecoderLocal < TDecoder < T > > >
	{
	private:
		using TChannelLocalisationDecoderLocal < TStreamedMatrixDecoderLocal < TDecoder < T > > >::m_pBoxAlgorithm;
	public:
		using TChannelLocalisationDecoderLocal < TStreamedMatrixDecoderLocal < TDecoder < T > > >::uninitialize;

		TChannelLocalisationDecoder()
		{
		}
		TChannelLocalisationDecoder(T& rBoxAlgorithm)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm);
		}
		virtual ~TChannelLocalisationDecoder()
		{
			this->uninitialize();
		}
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif //__OpenViBEToolkit_TChannelLocalisationDecoder_H__
