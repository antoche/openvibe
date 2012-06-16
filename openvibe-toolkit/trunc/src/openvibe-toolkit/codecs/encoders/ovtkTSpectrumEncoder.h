#ifndef __OpenViBEToolkit_TSpectrumEncoder_H__
#define __OpenViBEToolkit_TSpectrumEncoder_H__

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"
#include "ovtkTStreamedMatrixEncoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TSpectrumEncoderLocal : public T
	{

	protected:

		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_pInputBands;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pOutputMemoryBuffer;
		using T::m_pInputMatrix;

		OpenViBE::boolean initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
			m_pCodec->initialize();
			m_pInputMatrix.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_Matrix));
			m_pInputBands.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands));
			m_pOutputMemoryBuffer.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

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
			m_pInputBands.uninitialize();
			m_pOutputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* >& getInputMinMaxFrequencyBands()
		{
			return m_pInputBands;
		}

	protected:
		OpenViBE::boolean encodeHeaderImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeHeader);
		}

		OpenViBE::boolean encodeBufferImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeBuffer);
		}

		OpenViBE::boolean encodeEndImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeEnd);
		}
	};

	template <class T>
	class TSpectrumEncoder : public TSpectrumEncoderLocal < TStreamedMatrixEncoderLocal < TEncoder < T > > >
	{
	private:
		using TSpectrumEncoderLocal < TStreamedMatrixEncoderLocal < TEncoder < T > > >::m_pBoxAlgorithm;
	public:
		using TSpectrumEncoderLocal < TStreamedMatrixEncoderLocal < TEncoder < T > > >::uninitialize;

		TSpectrumEncoder()
		{
		}
		TSpectrumEncoder(T& rBoxAlgorithm)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm);
		}
		virtual ~TSpectrumEncoder()
		{
			this->uninitialize();
		}
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif //__OpenViBEToolkit_TSpectrumEncoder_H__
