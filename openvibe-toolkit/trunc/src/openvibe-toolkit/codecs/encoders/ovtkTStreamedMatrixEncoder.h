#ifndef __OpenViBEToolkit_TStreamedMatrixEncoder_H__
#define __OpenViBEToolkit_TStreamedMatrixEncoder_H__

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTEncoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TStreamedMatrixEncoderLocal : public T
	{

	protected:
		//specific attribute : a matrix handler
		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_pInputMatrix;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pOutputMemoryBuffer;

		OpenViBE::boolean initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
			m_pCodec->initialize();
			m_pInputMatrix.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
			m_pOutputMemoryBuffer.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

			return true;
		}

	public:
		// we propagate the visiblity of TCodec::initialize
		using T::initialize;

		OpenViBE::boolean uninitialize(void)
		{
			if(m_pBoxAlgorithm == NULL || m_pCodec == NULL)
			{
				return false;
			}

			m_pInputMatrix.uninitialize();
			m_pOutputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* >& getInputMatrix()
		{
			return m_pInputMatrix;
		}

	protected:

		/*
		The methods specific to the Streamed Matrix encoder :
		*/
		OpenViBE::boolean encodeHeaderImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
		}

		OpenViBE::boolean encodeBufferImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
		}

		OpenViBE::boolean encodeEndImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
		}
	};

	/*
	This class provides an access to the Local class.
	It makes a lot easier the declaration of a Streamed Matrix encoder, as you don't have to specify any more template than the Box class (T).
	*/
	template <class T>
	class TStreamedMatrixEncoder : public TStreamedMatrixEncoderLocal < TEncoder < T > >
	{
	private:
		using TStreamedMatrixEncoderLocal < TEncoder < T > >::m_pBoxAlgorithm;
	public:
		using TStreamedMatrixEncoderLocal < TEncoder < T > >::uninitialize;

		TStreamedMatrixEncoder()
		{
		}
		TStreamedMatrixEncoder(T& rBoxAlgorithm)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm);
		}
		virtual ~TStreamedMatrixEncoder()
		{
			this->uninitialize();
		}
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif //__OpenViBEToolkit_TStreamedMatrixEncoder_H__
