#ifndef __OpenViBEToolkit_TStreamedMatrixDecoder_H__
#define __OpenViBEToolkit_TStreamedMatrixDecoder_H__

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTDecoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TStreamedMatrixDecoderLocal : public T
	{

	protected:

		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_pOutputMatrix;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pInputMemoryBuffer;

		virtual OpenViBE::boolean initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
			m_pCodec->initialize();
			m_pInputMemoryBuffer.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
			m_pOutputMatrix.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));

			return true;
		}

	public:
		using T::initialize;

		virtual OpenViBE::boolean uninitialize(void)
		{
			if(m_pBoxAlgorithm == NULL || m_pCodec == NULL)
			{
				return false;
			}

			m_pOutputMatrix.uninitialize();
			m_pInputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		virtual OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* >& getOutputMatrix()
		{
			return m_pOutputMatrix;
		}

		virtual OpenViBE::boolean isHeaderReceived(void)
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader);
		}

		virtual OpenViBE::boolean isBufferReceived(void)
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer);
		}

		virtual OpenViBE::boolean isEndReceived(void)
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd);
		}

	};

	template <class T>
	class TStreamedMatrixDecoder : public TStreamedMatrixDecoderLocal < TDecoder < T > >
	{
	private:
		using TStreamedMatrixDecoderLocal < TDecoder < T > >::m_pBoxAlgorithm;
	public:
		using TStreamedMatrixDecoderLocal < TDecoder < T > >::uninitialize;

		TStreamedMatrixDecoder()
		{
		}
		TStreamedMatrixDecoder(T& rBoxAlgorithm)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm);
		}
		virtual ~TStreamedMatrixDecoder()
		{
			this->uninitialize();
		}
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif //__OpenViBEToolkit_TStreamedMatrixDecoder_H__
