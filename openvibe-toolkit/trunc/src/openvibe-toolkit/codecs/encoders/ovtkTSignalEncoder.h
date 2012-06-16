#ifndef __OpenViBEToolkit_TSignalEncoder_H__
#define __OpenViBEToolkit_TSignalEncoder_H__

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTStreamedMatrixEncoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TSignalEncoderLocal : public T
	{

	protected:
		//The signal stream is a streamed matrix plus a sampling rate
		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_pInputSamplingRate;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pOutputMemoryBuffer;
		using T::m_pInputMatrix;

		OpenViBE::boolean initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
			m_pCodec->initialize();
			m_pInputMatrix.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
			m_pInputSamplingRate.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));
			m_pOutputMemoryBuffer.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

			return true;
		}

	public:
		//agin... we propagate initialize from upperclass.
		using T::initialize;

		OpenViBE::boolean uninitialize(void)
		{
			if(m_pBoxAlgorithm == NULL || m_pCodec == NULL)
			{
				return false;
			}

			m_pInputMatrix.uninitialize();
			m_pInputSamplingRate.uninitialize();
			m_pOutputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getInputSamplingRate()
		{
			return m_pInputSamplingRate;
		}

	protected:

		/*
		The methods specific to the Signal encoder (overriding the TStreamedMatrixEncoderLocal implementations):
		*/
		OpenViBE::boolean encodeHeaderImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
		}

		OpenViBE::boolean encodeBufferImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
		}

		OpenViBE::boolean encodeEndImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeEnd);
		}
	};

	/*
	The Signal encoder can be instanciated easily through this class.
	You just need one template class : the box (T).
	*/
	template <class T>
	class TSignalEncoder : public TSignalEncoderLocal < TStreamedMatrixEncoderLocal < TEncoder < T > > >
	{
	private:
		using TSignalEncoderLocal < TStreamedMatrixEncoderLocal < TEncoder < T > > >::m_pBoxAlgorithm;
	public:
		using TSignalEncoderLocal < TStreamedMatrixEncoderLocal < TEncoder < T > > >::uninitialize;

		TSignalEncoder()
		{
		}
		TSignalEncoder(T& rBoxAlgorithm)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm);
		}
		virtual ~TSignalEncoder()
		{
			this->uninitialize();
		}
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif //__OpenViBEToolkit_TSignalEncoder_H__
