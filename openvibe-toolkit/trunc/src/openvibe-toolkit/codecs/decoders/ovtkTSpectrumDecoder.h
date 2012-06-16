#ifndef __OpenViBEToolkit_TSpectrumDecoder_H__
#define __OpenViBEToolkit_TSpectrumDecoder_H__

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTStreamedMatrixDecoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TSpectrumDecoderLocal : public T
	{

	protected:

		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_pOutputBands;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pInputMemoryBuffer;
		using T::m_pOutputMatrix;

		OpenViBE::boolean initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
			m_pCodec->initialize();
			m_pOutputMatrix.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_Matrix));
			m_pOutputBands.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));
			m_pInputMemoryBuffer.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_InputParameterId_MemoryBufferToDecode));

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

			m_pOutputMatrix.uninitialize();
			m_pOutputBands.uninitialize();
			m_pInputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* >& getOutputMinMaxFrequencyBands()
		{
			return m_pOutputBands;
		}

		virtual OpenViBE::boolean isHeaderReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedHeader);
		}

		virtual OpenViBE::boolean isBufferReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedBuffer);
		}

		virtual OpenViBE::boolean isEndReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedEnd);
		}
	};

	template <class T>
	class TSpectrumDecoder : public TSpectrumDecoderLocal < TStreamedMatrixDecoderLocal < TDecoder < T > > >
	{
	private:
		using TSpectrumDecoderLocal < TStreamedMatrixDecoderLocal < TDecoder < T > > >::m_pBoxAlgorithm;
	public:
		using TSpectrumDecoderLocal < TStreamedMatrixDecoderLocal < TDecoder < T > > >::uninitialize;

		TSpectrumDecoder()
		{
		}
		TSpectrumDecoder(T& rBoxAlgorithm)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm);
		}
		virtual ~TSpectrumDecoder()
		{
			this->uninitialize();
		}
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif //__OpenViBEToolkit_TSpectrumDecoder_H__
