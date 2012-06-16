#ifndef __OpenViBEToolkit_Reader_TBoxAlgorithmSpectrumInputReaderCallback_INL__
#define __OpenViBEToolkit_Reader_TBoxAlgorithmSpectrumInputReaderCallback_INL__

#include "ovtkIBoxAlgorithmSpectrumInputReaderCallback.h"
#include "ovtkTBoxAlgorithmStreamedMatrixInputReaderCallback.inl"

namespace OpenViBEToolkit
{

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
class TBoxAlgorithmSpectrumInputReaderCallback : public TBoxAlgorithmStreamedMatrixInputReaderCallback <IBaseInterface>
{
private:

	typedef TBoxAlgorithmSpectrumInputReaderCallback <IBaseInterface> CThisClass;
	typedef TBoxAlgorithmStreamedMatrixInputReaderCallback <IBaseInterface> CThisClassParent;

public:

	TBoxAlgorithmSpectrumInputReaderCallback(void);
	virtual ~TBoxAlgorithmSpectrumInputReaderCallback(void);
	virtual void setSpectrumCallback(IBoxAlgorithmSpectrumInputReaderCallback::ICallback* pCallback);

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

	IBoxAlgorithmSpectrumInputReaderCallback::ICallback* m_pCallback;
	IBoxAlgorithmStreamedMatrixInputReaderCallback::TCallbackProxy1< CThisClass > m_oStreamedMatrixCallbackProxy;

	OpenViBE::uint32 m_ui32FrequencyBandIndex;
	std::stack<EBML::CIdentifier> m_vNodes;
	EBML::IReaderHelper* m_pReaderHelper;
};

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::TBoxAlgorithmSpectrumInputReaderCallback(void)
	:m_pCallback(NULL)
	,m_oStreamedMatrixCallbackProxy(
		*this,
		&TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::setMatrixDimmensionCount,
		&TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::setMatrixDimmensionSize,
		&TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::setMatrixDimmensionLabel,
		&TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::setMatrixBuffer)
	,m_pReaderHelper(NULL)
{
	m_pReaderHelper=EBML::createReaderHelper();
	this->setStreamedMatrixCallback(&m_oStreamedMatrixCallbackProxy);
}

template <class IBaseInterface>
TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::~TBoxAlgorithmSpectrumInputReaderCallback(void)
{
	m_pReaderHelper->release();
	m_pReaderHelper=NULL;
}

template <class IBaseInterface>
void TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::setSpectrumCallback(IBoxAlgorithmSpectrumInputReaderCallback::ICallback* pCallback)
{
	m_pCallback=pCallback;
}

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
EBML::boolean TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	     if(rIdentifier==OVTK_NodeId_Header_Spectrum)                              { return true; }
	else if(rIdentifier==OVTK_NodeId_Header_Spectrum_FrequencyBand)                { return true; }
	else if(rIdentifier==OVTK_NodeId_Header_Spectrum_FrequencyBand_Start)          { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop)           { return false; }
	return CThisClassParent::isMasterChild(rIdentifier);
}

template <class IBaseInterface>
void TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_Spectrum)
	 ||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand)
	 ||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Start)
	 ||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop))
	{
		if(l_rTop==OVTK_NodeId_Header_Spectrum)
		{
			m_ui32FrequencyBandIndex=0;
		}
	}
	else
	{
		CThisClassParent::openChild(rIdentifier);
	}
}

template <class IBaseInterface>
void TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_Spectrum)
	 ||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand)
	 ||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Start)
	 ||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop))
	{
		if(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Start) m_pCallback->setFrequencyBandStart(m_ui32FrequencyBandIndex, m_pReaderHelper->getFloatFromChildData(pBuffer, ui64BufferSize));
		if(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop)  m_pCallback->setFrequencyBandStop (m_ui32FrequencyBandIndex, m_pReaderHelper->getFloatFromChildData(pBuffer, ui64BufferSize));
	}
	else
	{
		CThisClassParent::processChildData(pBuffer, ui64BufferSize);
	}
}

template <class IBaseInterface>
void TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::closeChild(void)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_Spectrum)
	 ||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand)
	 ||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Start)
	 ||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop))
	{
		if(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand)
		{
			m_ui32FrequencyBandIndex++;
		}
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
void TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::setMatrixDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount)
{
	// assert(ui32DimmensionCount==2);
}

template <class IBaseInterface>
void TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::setMatrixDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize)
{
	switch(ui32DimmensionIndex)
	{
		case 0: m_pCallback->setChannelCount(ui32DimmensionSize); break;
		case 1: m_pCallback->setFrequencyBandCount(ui32DimmensionSize); break;
	}
}

template <class IBaseInterface>
void TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::setMatrixDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel)
{
	switch(ui32DimmensionIndex)
	{
		case 0: m_pCallback->setChannelName(ui32DimmensionEntryIndex, sDimmensionLabel); break;
		case 1: m_pCallback->setFrequencyBandName(ui32DimmensionEntryIndex, sDimmensionLabel); break;
	}
}

template <class IBaseInterface>
void TBoxAlgorithmSpectrumInputReaderCallback<IBaseInterface>::setMatrixBuffer(const OpenViBE::float64* pBuffer)
{
	m_pCallback->setBuffer(pBuffer);
}

// ________________________________________________________________________________________________________________
//

};

#endif // __OpenViBEToolkit_Reader_TBoxAlgorithmSpectrumInputReaderCallback_INL__
