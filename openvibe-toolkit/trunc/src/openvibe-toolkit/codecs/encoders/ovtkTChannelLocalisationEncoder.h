#ifndef __OpenViBEToolkit_TChannelLocalisationEncoder_H__
#define __OpenViBEToolkit_TChannelLocalisationEncoder_H__

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "ovtkTStreamedMatrixEncoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TChannelLocalisationEncoderLocal : public T
	{

	protected:

		OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > m_bInputDynamic;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pOutputMemoryBuffer;
		using T::m_pInputMatrix;

		OpenViBE::boolean initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_ChannelLocalisationStreamEncoder));
			m_pCodec->initialize();
			m_pInputMatrix.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Matrix));
			m_bInputDynamic.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Dynamic));
			m_pOutputMemoryBuffer.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

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

			m_pInputMatrix.uninitialize();
			m_bInputDynamic.uninitialize();
			m_pOutputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean >& getInputDynamic()
		{
			return m_bInputDynamic;
		}

	protected:
		OpenViBE::boolean encodeHeaderImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeHeader);
		}

		OpenViBE::boolean encodeBufferImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeBuffer);
		}

		OpenViBE::boolean encodeEndImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeEnd);
		}
	};

	template <class T>
	class TChannelLocalisationEncoder : public TChannelLocalisationEncoderLocal < TStreamedMatrixEncoderLocal < TEncoder < T > > >
	{
	private:
		using TChannelLocalisationEncoderLocal < TStreamedMatrixEncoderLocal < TEncoder < T > > >::m_pBoxAlgorithm;
	public:
		using TChannelLocalisationEncoderLocal < TStreamedMatrixEncoderLocal < TEncoder < T > > >::uninitialize;

		TChannelLocalisationEncoder()
		{
		}
		TChannelLocalisationEncoder(T& rBoxAlgorithm)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm);
		}
		virtual ~TChannelLocalisationEncoder()
		{
			this->uninitialize();
		}
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif //__OpenViBEToolkit_TChannelLocalisationEncoder_H__
