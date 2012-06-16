#ifndef __OpenViBEToolkit_Reader_TBoxAlgorithmStreamedMatrixInputReaderCallback_INL__
#define __OpenViBEToolkit_Reader_TBoxAlgorithmStreamedMatrixInputReaderCallback_INL__

#include "ovtkIBoxAlgorithmStreamedMatrixInputReaderCallback.h"
#include "ovtkTBoxAlgorithmEBMLInputReaderCallback.inl"

namespace OpenViBEToolkit {

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
class TBoxAlgorithmStreamedMatrixInputReaderCallback : public TBoxAlgorithmEBMLInputReaderCallback <IBaseInterface>
{
private:

	typedef TBoxAlgorithmStreamedMatrixInputReaderCallback <IBaseInterface> CThisClass;
	typedef TBoxAlgorithmEBMLInputReaderCallback <IBaseInterface> CThisClassParent;

public:

	enum
	{
		Status_ParsingNothing,
		Status_ParsingHeader,
		Status_ParsingBuffer,
		Status_ParsingDimension,
	};

	TBoxAlgorithmStreamedMatrixInputReaderCallback(void);
	virtual ~TBoxAlgorithmStreamedMatrixInputReaderCallback(void);
	virtual void setStreamedMatrixCallback(IBoxAlgorithmStreamedMatrixInputReaderCallback::ICallback* pCallback);

	virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier);
	virtual void openChild(const EBML::CIdentifier& rIdentifier);
	virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
	virtual void closeChild(void);

	_IsDerivedFromClass_Final_(CThisClassParent, OVTK_ClassId_);

protected:

	virtual void sendHeaderToOwner(void);
	virtual void sendBufferToOwner(void);

protected:

	IBoxAlgorithmStreamedMatrixInputReaderCallback::ICallback* m_pCallback;
	OpenViBE::uint32 m_ui32Status;

	std::stack<EBML::CIdentifier> m_vNodes;
	EBML::IReaderHelper* m_pReaderHelper;

	const OpenViBE::float64* m_pMatrixBuffer;
	OpenViBE::uint32 m_ui32DimensionCount;
	std::vector<OpenViBE::uint32> m_vDimensionSize;
	std::vector<std::vector< std::string > > m_vDimensionLabel;
	std::vector<std::string> m_vCurrentDimensionLabel;
};

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
TBoxAlgorithmStreamedMatrixInputReaderCallback<IBaseInterface>::TBoxAlgorithmStreamedMatrixInputReaderCallback(void)
	:m_pCallback(NULL)
	,m_ui32Status(Status_ParsingNothing)
	,m_pReaderHelper(NULL)
	,m_pMatrixBuffer(NULL)
	,m_ui32DimensionCount(0)
{
	m_pReaderHelper=EBML::createReaderHelper();
}

template <class IBaseInterface>
TBoxAlgorithmStreamedMatrixInputReaderCallback<IBaseInterface>::~TBoxAlgorithmStreamedMatrixInputReaderCallback(void)
{
	m_pReaderHelper->release();
	m_pReaderHelper=NULL;
}

template <class IBaseInterface>
void TBoxAlgorithmStreamedMatrixInputReaderCallback<IBaseInterface>::setStreamedMatrixCallback(IBoxAlgorithmStreamedMatrixInputReaderCallback::ICallback* pCallback)
{
	m_pCallback=pCallback;
}

// ________________________________________________________________________________________________________________
//

template <class IBaseInterface>
EBML::boolean TBoxAlgorithmStreamedMatrixInputReaderCallback<IBaseInterface>::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	     if(rIdentifier==OVTK_NodeId_Header_StreamedMatrix)                  return true;
	else if(rIdentifier==OVTK_NodeId_Header_StreamedMatrix_Dimension)       return true;
	else if(rIdentifier==OVTK_NodeId_Header_StreamedMatrix_DimensionCount)  return false;
	else if(rIdentifier==OVTK_NodeId_Header_StreamedMatrix_Dimension_Size)  return false;
	else if(rIdentifier==OVTK_NodeId_Header_StreamedMatrix_Dimension_Label) return false;
	else if(rIdentifier==OVTK_NodeId_Buffer_StreamedMatrix)                  return true;
	else if(rIdentifier==OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer)        return false;
	return CThisClassParent::isMasterChild(rIdentifier);
}

template <class IBaseInterface>
void TBoxAlgorithmStreamedMatrixInputReaderCallback<IBaseInterface>::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop=m_vNodes.top();


	if((l_rTop==OVTK_NodeId_Header_StreamedMatrix)
	 ||(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension)
	 ||(l_rTop==OVTK_NodeId_Header_StreamedMatrix_DimensionCount)
	 ||(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension_Size)
	 ||(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension_Label)
	 ||(l_rTop==OVTK_NodeId_Buffer_StreamedMatrix)
	 ||(l_rTop==OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer))
	{
		if(l_rTop==OVTK_NodeId_Header_StreamedMatrix && m_ui32Status==Status_ParsingNothing)
		{
			m_ui32Status=Status_ParsingHeader;
			m_ui32DimensionCount=0;
			m_vDimensionSize.clear();
			m_vDimensionLabel.clear();
		}
		else if(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension && m_ui32Status==Status_ParsingHeader)
		{
			m_ui32Status=Status_ParsingDimension;
			m_vCurrentDimensionLabel.clear();
		}
		else if(l_rTop==OVTK_NodeId_Buffer_StreamedMatrix && m_ui32Status==Status_ParsingNothing)
		{
			m_ui32Status=Status_ParsingBuffer;
		}
	}
	else
	{
		CThisClassParent::openChild(rIdentifier);
	}
}

template <class IBaseInterface>
void TBoxAlgorithmStreamedMatrixInputReaderCallback<IBaseInterface>::processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_StreamedMatrix)
	 ||(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension)
	 ||(l_rTop==OVTK_NodeId_Header_StreamedMatrix_DimensionCount)
	 ||(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension_Size)
	 ||(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension_Label)
	 ||(l_rTop==OVTK_NodeId_Buffer_StreamedMatrix)
	 ||(l_rTop==OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer))
	{
		switch(m_ui32Status)
		{
			case Status_ParsingHeader:
				if(l_rTop==OVTK_NodeId_Header_StreamedMatrix_DimensionCount)  { m_ui32DimensionCount=static_cast<OpenViBE::uint32>(m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize)); }
				break;

			case Status_ParsingDimension:
				if(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension_Size)  { m_vDimensionSize.push_back(static_cast<OpenViBE::uint32>(m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize))); }
				if(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension_Label) { m_vCurrentDimensionLabel.push_back(m_pReaderHelper->getASCIIStringFromChildData(pBuffer, ui64BufferSize)); }
				break;

			case Status_ParsingBuffer:
				if(l_rTop==OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer)        { m_pMatrixBuffer=static_cast<const OpenViBE::float64*>(pBuffer); sendBufferToOwner(); }
				break;
		}
	}
	else
	{
		CThisClassParent::processChildData(pBuffer, ui64BufferSize);
	}
}

template <class IBaseInterface>
void TBoxAlgorithmStreamedMatrixInputReaderCallback<IBaseInterface>::closeChild(void)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_StreamedMatrix)
	 ||(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension)
	 ||(l_rTop==OVTK_NodeId_Header_StreamedMatrix_DimensionCount)
	 ||(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension_Size)
	 ||(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension_Label)
	 ||(l_rTop==OVTK_NodeId_Buffer_StreamedMatrix)
	 ||(l_rTop==OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer))
	{
		if(l_rTop==OVTK_NodeId_Buffer_StreamedMatrix && m_ui32Status==Status_ParsingBuffer)
		{
			m_ui32Status=Status_ParsingNothing;
		}
		else if(l_rTop==OVTK_NodeId_Header_StreamedMatrix_Dimension && m_ui32Status==Status_ParsingDimension)
		{
			m_ui32Status=Status_ParsingHeader;
			m_vDimensionLabel.push_back(m_vCurrentDimensionLabel);
		}
		else if(l_rTop==OVTK_NodeId_Header_StreamedMatrix && m_ui32Status==Status_ParsingHeader)
		{
			m_ui32Status=Status_ParsingNothing;
			sendHeaderToOwner();
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
void TBoxAlgorithmStreamedMatrixInputReaderCallback<IBaseInterface>::sendHeaderToOwner(void)
{
	// should send header to owner
	m_pCallback->setMatrixDimmensionCount(m_ui32DimensionCount);
	for(OpenViBE::uint32 i=0; i<m_ui32DimensionCount; i++)
	{
		if(i<m_vDimensionSize.size())
		{
			m_pCallback->setMatrixDimmensionSize(i, m_vDimensionSize[i]);
			for(OpenViBE::uint32 j=0; j<m_vDimensionSize[i]; j++)
			{
				if(i<m_vDimensionLabel.size())
				{
					if(j<m_vDimensionLabel[i].size())
					{
						m_pCallback->setMatrixDimmensionLabel(i, j, m_vDimensionLabel[i][j].c_str());
					}
					else
					{
						m_pCallback->setMatrixDimmensionLabel(i, j, "");
					}
				}
				else
				{
					// this reflects a problem !
					m_pCallback->setMatrixDimmensionLabel(i, j, "");
				}
			}
		}
		else
		{
			// this reflects a problem !
			m_pCallback->setMatrixDimmensionSize(i, 0);
		}
	}
}

template <class IBaseInterface>
void TBoxAlgorithmStreamedMatrixInputReaderCallback<IBaseInterface>::sendBufferToOwner(void)
{
	m_pCallback->setMatrixBuffer(m_pMatrixBuffer);
}

// ________________________________________________________________________________________________________________
//

};

#endif // __OpenViBEToolkit_Reader_TBoxAlgorithmStreamedMatrixInputReaderCallback_INL__
