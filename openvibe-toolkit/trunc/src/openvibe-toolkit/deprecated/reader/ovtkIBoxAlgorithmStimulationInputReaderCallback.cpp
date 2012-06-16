#include "ovtkIBoxAlgorithmStimulationInputReaderCallback.h"
#include "ovtkTBoxAlgorithmStimulationInputReaderCallback.inl"

namespace OpenViBEToolkit
{

	typedef TBoxAlgorithmStimulationInputReaderCallback<IBoxAlgorithmStimulationInputReaderCallback> CBoxAlgorithmStimulationInputReaderCallback;

	IBoxAlgorithmStimulationInputReaderCallback* createBoxAlgorithmStimulationInputReaderCallback(IBoxAlgorithmStimulationInputReaderCallback::ICallback& rCallback)
	{
		CBoxAlgorithmStimulationInputReaderCallback* l_pResult=new CBoxAlgorithmStimulationInputReaderCallback();
		l_pResult->setStimulationCallback(&rCallback);
		return l_pResult;
	}

	void releaseBoxAlgorithmStimulationInputReaderCallback(IBoxAlgorithmStimulationInputReaderCallback* pBoxAlgorithmStimulationInputReaderCallback)
	{
		delete pBoxAlgorithmStimulationInputReaderCallback;
	}

}
