#include "ovtkIBoxAlgorithmSignalInputReaderCallback.h"
#include "ovtkTBoxAlgorithmSignalInputReaderCallback.inl"

namespace OpenViBEToolkit
{

	typedef TBoxAlgorithmSignalInputReaderCallback<IBoxAlgorithmSignalInputReaderCallback> CBoxAlgorithmSignalInputReaderCallback;

	IBoxAlgorithmSignalInputReaderCallback* createBoxAlgorithmSignalInputReaderCallback(IBoxAlgorithmSignalInputReaderCallback::ICallback& rCallback)
	{
		CBoxAlgorithmSignalInputReaderCallback* l_pResult=new CBoxAlgorithmSignalInputReaderCallback();
		l_pResult->setSignalCallback(&rCallback);
		return l_pResult;
	}

	void releaseBoxAlgorithmSignalInputReaderCallback(IBoxAlgorithmSignalInputReaderCallback* pBoxAlgorithmSignalInputReaderCallback)
	{
		delete pBoxAlgorithmSignalInputReaderCallback;
	}

};
