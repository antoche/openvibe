#ifndef __OpenViBEToolkit_TCodec_H__
#define __OpenViBEToolkit_TCodec_H__

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../ovtk_base.h"

/*
Few words about the Codec Toolkit:
----------------------------------
The Codec Toolkit gives a simple and easy way to use the Encoders and Decoders, which are the most frequently used algorithms when developing a plugin.
The Toolkit makes wide use of Templates, in a way that may seem complex for a reviewer. Complexity is not in the number of line of code, but in the architecture.
The comments will try to help, and you may find relevant information on the dedicated tutorial and online doc:
- http://openvibe.inria.fr/documentation/unstable/Doc_Tutorial_Developer_SignalProcessing_CodecToolkit.html
- http://openvibe.inria.fr/documentation/unstable/Doc_Tutorial_Developer_SignalProcessing_CodecToolkit_Ref.html

Contact :
---------
Laurent Bonnet, INRIA Rennes.
www.openvibe.inria.fr/forum
*/


/*
The Template inheritance:
-------------------------

The Codec Toolkit uses template inheritance. This means that every class in templated by its superclass, starting with the most basic class : TCodec.
This architecture allows the Toolkit developer to enhance it anywhere in the inheritance diagram.
Here is the inheritance architecture hidden behind the templates :

Level 0 : TCodec
	> Purely Abstract
	> Contains the box pointer and the codec algorithm
	> Implements the public initialize(T&) function

Level 1 : TEncoderLocal TDecoderLocal
	> Partial implementation
	> Contains the input or output memory buffer, and high level public functions (to decode and encode)
	> This level is accessible but not instanciable, through the TEncoder and TDecoder subclasses. 

Level 2 : T_XXX_DecoderLocal T_XXX_EncoderLocal
	> Implementation Level, for each stream type (except Streamed Matrix subtypes)
	> These local classes contains 
	> - The specific parameter handlers (e.g. a matrix for the Streamed Matrix codecs)
	> - the specific implementations of each functions (init, uninit, isXXXreceived, encodeXXX)
	> They are instanciable through their non-local subclasses (e.g. TStreamedMatrixDecoder)

Level 3 : T_StreamedMatrixSubType_DecoderLocal T_StreamedMatrixSubType_EncoderLocal
	> Implementation level of the Streamed Matrix subtypes (Feature Vector, Spectrum, Signal)
	> They are instanciable through their non-local subclasses (e.g. TFeatureVectorDecoder)

Please look at TDecoder, TEncoder, TStreamedMatrixEncoder and TSignalEncoder classes for more details.
The Codec Toolkit reference page may also be useful.

GL&HF
*/

namespace OpenViBEToolkit
{
	template <class T>
	class TCodec
	{
	protected:

		// We will need the dynamic box context when trying to decode and encode, thus we keep a pointer on the underlying box.
		T* m_pBoxAlgorithm;

		// Every codec has an algorithm
		OpenViBE::Kernel::IAlgorithmProxy* m_pCodec;

	public:
		TCodec()
			:m_pBoxAlgorithm(NULL)
		{
		}

		//The initialization need a reference to the underlying box
		//it will certainly be called in the box in such manner : m_oCodec.initialize(*this);
		OpenViBE::boolean initialize(T& rBoxAlgorithm)
		{
			if(m_pBoxAlgorithm == NULL)
			{
				m_pBoxAlgorithm = &rBoxAlgorithm;
			}
			else
			{
				return false;
			}
			// we call the initialization process specific to each codec
			return initializeImpl();
		}
		
		// As we need to properly uninit parameter handlers before anything else, we can't design a common uninit behavior
		virtual OpenViBE::boolean uninitialize(void) = 0;

	protected:

		// Note that this method is NOT public.
		virtual OpenViBE::boolean initializeImpl() = 0;


		// for easier access to algorithm functyionnality, we redefine some functions:

		virtual OpenViBE::boolean isOutputTriggerActive(OpenViBE::CIdentifier oTrigger)
		{
			return m_pCodec->isOutputTriggerActive(oTrigger);
		}

		virtual OpenViBE::boolean process(OpenViBE::CIdentifier &oTrigger)
		{
			return m_pCodec->process(oTrigger);
		}

		virtual OpenViBE::boolean process(void)
		{
			return m_pCodec->process();
		}

		

	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif //__OpenViBEToolkit_TCodec_H__
