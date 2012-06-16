#ifndef __OpenViBEToolkit_Reader_TBoxAlgorithmSignalInputReaderCallback_INL__
#define __OpenViBEToolkit_Reader_TBoxAlgorithmSignalInputReaderCallback_INL__

#include "ovtkIBoxAlgorithmSignalInputReaderCallback.h"
#include "ovtkTBoxAlgorithmStreamedMatrixInputReaderCallback.inl"

namespace OpenViBEToolkit
{

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
class TBoxAlgorithmSignalInputReaderCallback : public TBoxAlgorithmStreamedMatrixInputReaderCallback <IBaseInterface>
{
private:

	typedef TBoxAlgorithmSignalInputReaderCallback <IBaseInterface> CThisClass;
	typedef TBoxAlgorithmStreamedMatrixInputReaderCallback <IBaseInterface> CThisClassParent;

public:

	TBoxAlgorithmSignalInputReaderCallback(void);
	virtual ~TBoxAlgorithmSignalInputReaderCallback(void);
	virtual void setSignalCallback(IBoxAlgorithmSignalInputReaderCallback::ICallback* pCallback);

	virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier);
	virtual void openChild(const EBML::CIdentifier& rIdentifier);
	virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
	virtual void closeChild(void);

	virtual void setMatrixDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount);
	virtual void setMatrixDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize);
	virtual void setMatrixDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel);
	virtual void setMatrixBuffer(const OpenViBE::float64* pBuffer);

	_IsDerivedFromClass_Final_(CThisClassParent, OVTK_ClassId_);

protected:

	IBoxAlgorithmSignalInputReaderCallback::ICallback* m_pCallback;
	IBoxAlgorithmStreamedMatrixInputReaderCallback::TCallbackProxy1< CThisClass > m_oStreamedMatrixCallbackProxy;

	std::stack<EBML::CIdentifier> m_vNodes;
	EBML::IReaderHelper* m_pReaderHelper;
};

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::TBoxAlgorithmSignalInputReaderCallback(void)
	:m_pCallback(NULL)
	,m_oStreamedMatrixCallbackProxy(
		*this,
		&TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::setMatrixDimmensionCount,
		&TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::setMatrixDimmensionSize,
		&TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::setMatrixDimmensionLabel,
		&TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::setMatrixBuffer)
	,m_pReaderHelper(NULL)
{
	m_pReaderHelper=EBML::createReaderHelper();
	this->setStreamedMatrixCallback(&m_oStreamedMatrixCallbackProxy);
}

template <class IBaseInterface>
TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::~TBoxAlgorithmSignalInputReaderCallback(void)
{
	m_pReaderHelper->release();
	m_pReaderHelper=NULL;
}

template <class IBaseInterface>
void TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::setSignalCallback(IBoxAlgorithmSignalInputReaderCallback::ICallback* pCallback)
{
	m_pCallback=pCallback;
}

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
EBML::boolean TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	     if(rIdentifier==OVTK_NodeId_Header_Signal)                              { return true; }
	else if(rIdentifier==OVTK_NodeId_Header_Signal_SamplingRate)                 { return false; }
	return CThisClassParent::isMasterChild(rIdentifier);
}

template <class IBaseInterface>
void TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_Signal)
	 ||(l_rTop==OVTK_NodeId_Header_Signal_SamplingRate))
	{
	}
	else
	{
		CThisClassParent::openChild(rIdentifier);
	}
}

template <class IBaseInterface>
void TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_Signal)
	 ||(l_rTop==OVTK_NodeId_Header_Signal_SamplingRate))
	{
		if(l_rTop==OVTK_NodeId_Header_Signal_SamplingRate) m_pCallback->setSamplingRate(static_cast<OpenViBE::uint32>(m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize)));
	}
	else
	{
		CThisClassParent::processChildData(pBuffer, ui64BufferSize);
	}
}

template <class IBaseInterface>
void TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::closeChild(void)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_Signal)
	 ||(l_rTop==OVTK_NodeId_Header_Signal_SamplingRate))
	{
	}
	else
	{
		CThisClassParent::closeChild();
	}

	m_vNodes.pop();
}

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
void TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::setMatrixDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount)
{
	// assert(ui32DimmensionCount==2);
}

template <class IBaseInterface>
void TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::setMatrixDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize)
{
	switch(ui32DimmensionIndex)
	{
		case 0: m_pCallback->setChannelCount(ui32DimmensionSize); break;
		case 1: m_pCallback->setSampleCountPerBuffer(ui32DimmensionSize); break;
	}
}

template <class IBaseInterface>
void TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::setMatrixDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel)
{
	switch(ui32DimmensionIndex)
	{
		case 0: m_pCallback->setChannelName(ui32DimmensionEntryIndex, sDimmensionLabel);
	}
}

template <class IBaseInterface>
void TBoxAlgorithmSignalInputReaderCallback<IBaseInterface>::setMatrixBuffer(const OpenViBE::float64* pBuffer)
{
	m_pCallback->setSampleBuffer(pBuffer);
}

// ________________________________________________________________________________________________________________
//

};

#endif // __OpenViBEToolkit_Reader_TBoxAlgorithmSignalInputReaderCallback_INL__
