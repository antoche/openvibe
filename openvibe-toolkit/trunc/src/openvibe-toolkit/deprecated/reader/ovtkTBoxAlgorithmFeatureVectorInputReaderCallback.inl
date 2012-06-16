#ifndef __OpenViBEToolkit_Reader_TBoxAlgorithmFeatureVectorInputReaderCallback_INL_
#define __OpenViBEToolkit_Reader_TBoxAlgorithmFeatureVectorInputReaderCallback_INL_

#include "ovtkIBoxAlgorithmFeatureVectorInputReaderCallback.h"
#include "ovtkTBoxAlgorithmStreamedMatrixInputReaderCallback.inl"

namespace OpenViBEToolkit {

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
class TBoxAlgorithmFeatureVectorInputReaderCallback : public TBoxAlgorithmStreamedMatrixInputReaderCallback <IBaseInterface>
{
private:

	typedef TBoxAlgorithmFeatureVectorInputReaderCallback <IBaseInterface> CThisClass;
	typedef TBoxAlgorithmStreamedMatrixInputReaderCallback <IBaseInterface> CThisClassParent;

public:

	TBoxAlgorithmFeatureVectorInputReaderCallback(void);
	virtual ~TBoxAlgorithmFeatureVectorInputReaderCallback(void);
	virtual void setFeatureVectorCallback(IBoxAlgorithmFeatureVectorInputReaderCallback::ICallback* pCallback);

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

	IBoxAlgorithmFeatureVectorInputReaderCallback::ICallback* m_pCallback;
	IBoxAlgorithmStreamedMatrixInputReaderCallback::TCallbackProxy1< CThisClass > m_oStreamedMatrixCallbackProxy;

	std::stack<EBML::CIdentifier> m_vNodes;
	EBML::IReaderHelper* m_pReaderHelper;
};

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::TBoxAlgorithmFeatureVectorInputReaderCallback(void)
	:m_pCallback(NULL)
	,m_oStreamedMatrixCallbackProxy(
		*this,
		&TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::setMatrixDimmensionCount,
		&TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::setMatrixDimmensionSize,
		&TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::setMatrixDimmensionLabel,
		&TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::setMatrixBuffer)
	,m_pReaderHelper(NULL)
{
	m_pReaderHelper=EBML::createReaderHelper();
	this->setStreamedMatrixCallback(&m_oStreamedMatrixCallbackProxy);
}

template <class IBaseInterface>
TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::~TBoxAlgorithmFeatureVectorInputReaderCallback(void)
{
	m_pReaderHelper->release();
	m_pReaderHelper=NULL;
}

template <class IBaseInterface>
void TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::setFeatureVectorCallback(IBoxAlgorithmFeatureVectorInputReaderCallback::ICallback* pCallback)
{
	m_pCallback=pCallback;
}

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
EBML::boolean TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	return CThisClassParent::isMasterChild(rIdentifier);
}

template <class IBaseInterface>
void TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	CThisClassParent::openChild(rIdentifier);
}

template <class IBaseInterface>
void TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	CThisClassParent::processChildData(pBuffer, ui64BufferSize);
}

template <class IBaseInterface>
void TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::closeChild(void)
{
	CThisClassParent::closeChild();

	m_vNodes.pop();
}

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
void TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::setMatrixDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount)
{
	// assert(ui32DimmensionCount==1);
}

template <class IBaseInterface>
void TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::setMatrixDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize)
{
	switch(ui32DimmensionIndex)
	{
		case 0: m_pCallback->setFeatureCount(ui32DimmensionSize); break;
	}
}

template <class IBaseInterface>
void TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::setMatrixDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel)
{
	switch(ui32DimmensionIndex)
	{
		case 0: m_pCallback->setFeatureName(ui32DimmensionEntryIndex, sDimmensionLabel);
	}
}

template <class IBaseInterface>
void TBoxAlgorithmFeatureVectorInputReaderCallback<IBaseInterface>::setMatrixBuffer(const OpenViBE::float64* pBuffer)
{
	m_pCallback->setFeatureVector(pBuffer);
}

// ________________________________________________________________________________________________________________
//

};

#endif // __OpenViBEToolkit_Reader_TBoxAlgorithmFeatureVectorInputReaderCallback_INL_
