#include "ovtkIBoxAlgorithmFeatureVectorInputReaderCallback.h"
#include "ovtkTBoxAlgorithmFeatureVectorInputReaderCallback.inl"

namespace OpenViBEToolkit
{

	typedef TBoxAlgorithmFeatureVectorInputReaderCallback<IBoxAlgorithmFeatureVectorInputReaderCallback> CBoxAlgorithmFeatureVectorInputReaderCallback;

	IBoxAlgorithmFeatureVectorInputReaderCallback* createBoxAlgorithmFeatureVectorInputReaderCallback(IBoxAlgorithmFeatureVectorInputReaderCallback::ICallback& rCallback)
	{
		CBoxAlgorithmFeatureVectorInputReaderCallback* l_pResult=new CBoxAlgorithmFeatureVectorInputReaderCallback();
		l_pResult->setFeatureVectorCallback(&rCallback);
		return l_pResult;
	}

	void releaseBoxAlgorithmFeatureVectorInputReaderCallback(IBoxAlgorithmFeatureVectorInputReaderCallback* pBoxAlgorithmFeatureVectorInputReaderCallback)
	{
		delete pBoxAlgorithmFeatureVectorInputReaderCallback;
	}

};
