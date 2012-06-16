#ifndef __OpenViBEToolkit_TDecoder_H__
#define __OpenViBEToolkit_TDecoder_H__

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "../ovtkTCodec.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TDecoderLocal : public T
	{
	protected:

		OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > m_pInputMemoryBuffer;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;

		virtual void setInputChunk(const OpenViBE::IMemoryBuffer * pInputChunkMemoryBuffer)
		{
			m_pInputMemoryBuffer = pInputChunkMemoryBuffer;
		}

		virtual OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* >& getInputMemoryBuffer()
		{
			return m_pInputMemoryBuffer;
		}

		virtual OpenViBE::boolean isOutputTriggerActive(OpenViBE::CIdentifier oTrigger)
		{
			return m_pCodec->isOutputTriggerActive(oTrigger);
		}

		virtual OpenViBE::boolean process(OpenViBE::CIdentifier oTrigger)
		{
			return m_pCodec->process(oTrigger);
		}
		virtual OpenViBE::boolean process(void)
		{
			return m_pCodec->process();
		}

	public:
		// We make visible the initialize methods of the superclass (should be TCodec), in the same scope (public)
		using T::initialize;

		/*
		This public function handles every aspects of the decoding process:
		- fill the input memory buffer with a chunk
		- decode it (specific for each decoder)
		- mark input as deprecated
		*/
		virtual OpenViBE::boolean decode(OpenViBE::uint32 ui32InputIndex, OpenViBE::uint32 ui32ChunkIndex, OpenViBE::boolean bMarkInputAsDeprecated = true)
		{
			this->setInputChunk(m_pBoxAlgorithm->getDynamicBoxContext().getInputChunk(ui32InputIndex, ui32ChunkIndex));
			if(! m_pCodec->process()) return false;
			if( bMarkInputAsDeprecated) m_pBoxAlgorithm->getDynamicBoxContext().markInputAsDeprecated(ui32InputIndex, ui32ChunkIndex);
			return true;
		}

		// The functions that need to be specified by the decoders (specific Trigger ID)
		virtual OpenViBE::boolean isHeaderReceived(void) = 0;
		virtual OpenViBE::boolean isBufferReceived(void) = 0;
		virtual OpenViBE::boolean isEndReceived(void) = 0;

	};

	/*
	This class provides an access to the superclass TDecoder.
	Use case : iterating over a vector of TDecoder, calling decode() each time.
	You don't need to know which type of decoder is in the vector.
	*/
	template <class T>
	class TDecoder : public TDecoderLocal < TCodec < T > >
	{
	protected:
		// constructor is protected, ensuring we can't instanciate a TDecoder
		TDecoder()
		{
		}
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif //__OpenViBEToolkit_TDecoder_H__
