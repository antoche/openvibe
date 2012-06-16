#include "ovtkIBoxAlgorithmSpectrumInputReaderCallback.h"
#include "ovtkTBoxAlgorithmSpectrumInputReaderCallback.inl"

namespace OpenViBEToolkit
{

	typedef TBoxAlgorithmSpectrumInputReaderCallback<IBoxAlgorithmSpectrumInputReaderCallback> CBoxAlgorithmSpectrumInputReaderCallback;

	IBoxAlgorithmSpectrumInputReaderCallback* createBoxAlgorithmSpectrumInputReaderCallback(IBoxAlgorithmSpectrumInputReaderCallback::ICallback& rCallback)
	{
		CBoxAlgorithmSpectrumInputReaderCallback* l_pResult=new CBoxAlgorithmSpectrumInputReaderCallback();
		l_pResult->setSpectrumCallback(&rCallback);
		return l_pResult;
	}

	void releaseBoxAlgorithmSpectrumInputReaderCallback(IBoxAlgorithmSpectrumInputReaderCallback* pBoxAlgorithmSpectrumInputReaderCallback)
	{
		delete pBoxAlgorithmSpectrumInputReaderCallback;
	}

};
