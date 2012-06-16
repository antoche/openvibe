#ifndef __OpenViBEToolkit_Reader_TBoxAlgorithmStimulationInputReaderCallback_INL__
#define __OpenViBEToolkit_Reader_TBoxAlgorithmStimulationInputReaderCallback_INL__

#include "ovtkIBoxAlgorithmStimulationInputReaderCallback.h"
#include "ovtkTBoxAlgorithmEBMLInputReaderCallback.inl"

namespace OpenViBEToolkit {

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
class TBoxAlgorithmStimulationInputReaderCallback : public TBoxAlgorithmEBMLInputReaderCallback <IBaseInterface>
{
private:

	typedef TBoxAlgorithmStimulationInputReaderCallback <IBaseInterface> CThisClass;
	typedef TBoxAlgorithmEBMLInputReaderCallback <IBaseInterface> CThisClassParent;

public:

	TBoxAlgorithmStimulationInputReaderCallback(void);
	virtual ~TBoxAlgorithmStimulationInputReaderCallback(void);
	virtual void setStimulationCallback(IBoxAlgorithmStimulationInputReaderCallback::ICallback* pCallback);

	virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier);
	virtual void openChild(const EBML::CIdentifier& rIdentifier);
	virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
	virtual void closeChild(void);

	_IsDerivedFromClass_Final_(CThisClassParent, OVTK_ClassId_);

protected:

	IBoxAlgorithmStimulationInputReaderCallback::ICallback* m_pCallback;

	std::stack<EBML::CIdentifier> m_vNodes;
	EBML::IReaderHelper* m_pReaderHelper;

	OpenViBE::uint32 m_ui32StimulationIndex;
	OpenViBE::uint64 m_ui64StimulationIdentifier;
	OpenViBE::uint64 m_ui64StimulationDate;
};

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
TBoxAlgorithmStimulationInputReaderCallback<IBaseInterface>::TBoxAlgorithmStimulationInputReaderCallback(void)
	:m_pCallback(NULL)
	,m_pReaderHelper(NULL)
{
	m_pReaderHelper=EBML::createReaderHelper();
}

template <class IBaseInterface>
TBoxAlgorithmStimulationInputReaderCallback<IBaseInterface>::~TBoxAlgorithmStimulationInputReaderCallback(void)
{
	m_pReaderHelper->release();
	m_pReaderHelper=NULL;
}

template <class IBaseInterface>
void TBoxAlgorithmStimulationInputReaderCallback<IBaseInterface>::setStimulationCallback(IBoxAlgorithmStimulationInputReaderCallback::ICallback* pCallback)
{
	m_pCallback=pCallback;
}

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
EBML::boolean TBoxAlgorithmStimulationInputReaderCallback<IBaseInterface>::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	     if(rIdentifier==OVTK_NodeId_Buffer_Stimulation)                        { return true; }
	else if(rIdentifier==OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)   { return false; }
	else if(rIdentifier==OVTK_NodeId_Buffer_Stimulation_Stimulation)            { return true; }
	else if(rIdentifier==OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier) { return false; }
	else if(rIdentifier==OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)       { return false; }
	return CThisClassParent::isMasterChild(rIdentifier);
}

template <class IBaseInterface>
void TBoxAlgorithmStimulationInputReaderCallback<IBaseInterface>::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Buffer_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Date))
	{
	}
	else
	{
		CThisClassParent::openChild(rIdentifier);
	}

}

template <class IBaseInterface>
void TBoxAlgorithmStimulationInputReaderCallback<IBaseInterface>::processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Buffer_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Date))
	{
		if(l_rTop==OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)   { m_pCallback->setStimulationCount(static_cast<OpenViBE::uint32>(m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize))); m_ui32StimulationIndex=0; }
		if(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier) { m_ui64StimulationIdentifier=m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize); }
		if(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)       { m_ui64StimulationDate=m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize); }
	}
	else
	{
		CThisClassParent::processChildData(pBuffer, ui64BufferSize);
	}
}

template <class IBaseInterface>
void TBoxAlgorithmStimulationInputReaderCallback<IBaseInterface>::closeChild(void)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Buffer_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Date))
	{
		if(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation) { m_pCallback->setStimulation(m_ui32StimulationIndex, m_ui64StimulationIdentifier, m_ui64StimulationDate); m_ui32StimulationIndex++; }
	}
	else
	{
		CThisClassParent::closeChild();
	}

	m_vNodes.pop();
}

// ________________________________________________________________________________________________________________
//

};

#endif // __OpenViBEToolkit_Reader_TBoxAlgorithmStimulationInputReaderCallback_INL__
