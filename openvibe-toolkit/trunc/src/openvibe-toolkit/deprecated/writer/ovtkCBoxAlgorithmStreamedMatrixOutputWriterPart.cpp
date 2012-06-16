#include "ovtkCBoxAlgorithmStreamedMatrixOutputWriterPart.inl"

#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace OpenViBEToolkit;
using namespace OpenViBE;

// ________________________________________________________________________________________________________________
//

CBoxAlgorithmStreamedMatrixOutputWriterPart::CBoxAlgorithmStreamedMatrixOutputWriterPart(void)
	:m_pMatrixBuffer(NULL)
	,m_ui64MatrixBufferSize(0)
	,m_ui32Status(Status_SendingNothing)
	,m_ui32DimensionCount(0)
{
}

// ________________________________________________________________________________________________________________
//

boolean CBoxAlgorithmStreamedMatrixOutputWriterPart::setDimmensionCount(const uint32 ui32DimensionCount)
{
	if(m_ui32Status!=Status_SendingNothing)
	{
		return false;
	}

	m_ui32DimensionCount=ui32DimensionCount;
	m_vDimensionSize.clear();
	m_vDimensionLabel.clear();
	m_ui64MatrixBufferSize=0;
	return true;
}

boolean CBoxAlgorithmStreamedMatrixOutputWriterPart::setDimmensionSize(const uint32 ui32DimensionIndex, const uint32 ui32DimensionSize)
{
	if(ui32DimensionIndex>=m_ui32DimensionCount)
	{
		return false;
	}

	m_vDimensionSize[ui32DimensionIndex]=ui32DimensionSize;
	return true;
}

boolean CBoxAlgorithmStreamedMatrixOutputWriterPart::setDimmensionLabel(const uint32 ui32DimensionIndex, const uint32 ui32DimensionEntryIndex, const char* sDimensionLabel)
{
	if(ui32DimensionIndex>=m_ui32DimensionCount)
	{
		return false;
	}

	if(ui32DimensionEntryIndex>=m_vDimensionSize[ui32DimensionIndex])
	{
		return false;
	}

	m_vDimensionLabel[ui32DimensionIndex][ui32DimensionEntryIndex]=sDimensionLabel;
	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CBoxAlgorithmStreamedMatrixOutputWriterPart::setBuffer(const float64* pBuffer)
{
	m_pMatrixBuffer=pBuffer;
	return m_pMatrixBuffer!=NULL;
}

// ________________________________________________________________________________________________________________
//

boolean CBoxAlgorithmStreamedMatrixOutputWriterPart::writeHeaderStart(EBML::IWriter& rWriter)
{
	if(m_ui32Status!=Status_SendingNothing)
	{
		return false;
	}

	m_ui64MatrixBufferSize=(m_ui32DimensionCount==0?0:sizeof(float64));

	m_ui32Status=Status_SendingHeader;

	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.openChild(OVTK_NodeId_Header_StreamedMatrix);
	  m_oWriterHelper.openChild(OVTK_NodeId_Header_StreamedMatrix_DimensionCount);
	   m_oWriterHelper.setUIntegerAsChildData(m_ui32DimensionCount);
	  m_oWriterHelper.closeChild();
	  for(uint32 i=0; i<m_ui32DimensionCount; i++)
	  {
	   m_ui64MatrixBufferSize*=m_vDimensionSize[i];
	   m_oWriterHelper.openChild(OVTK_NodeId_Header_StreamedMatrix_Dimension);
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_StreamedMatrix_Dimension_Size);
	     m_oWriterHelper.setUIntegerAsChildData(m_vDimensionSize[i]);
	    m_oWriterHelper.closeChild();
	    for(uint32 j=0; j<m_vDimensionSize[i]; j++)
	    {
	     m_oWriterHelper.openChild(OVTK_NodeId_Header_StreamedMatrix_Dimension_Label);
	      m_oWriterHelper.setASCIIStringAsChildData(m_vDimensionLabel[i][j].c_str());
	     m_oWriterHelper.closeChild();
	    }
	   m_oWriterHelper.closeChild();
	  }

	m_oWriterHelper.disconnect();
	return true;
}

boolean CBoxAlgorithmStreamedMatrixOutputWriterPart::writeHeaderStop(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();

	m_ui32Status=Status_SendingNothing;
	return true;
}

boolean CBoxAlgorithmStreamedMatrixOutputWriterPart::writeBufferStart(EBML::IWriter& rWriter)
{
	if(m_ui32Status!=Status_SendingNothing)
	{
		return false;
	}

#if 0
	for(uint32 i=0; i<m_vDimensionSize[0]; i++)
		for(uint32 j=0; j<m_vDimensionSize[1]; j++)
		{
			if(isnan(m_pMatrixBuffer[i*m_vDimensionSize[1]+j]))
			{
				std::cout << "Streamed matrix output writer got NAN values (replaced by 0)" << std::endl;
				const_cast<float64*>(m_pMatrixBuffer)[i*m_vDimensionSize[1]+j]=0;
			}
		}
#endif

	m_ui32Status=Status_SendingBuffer;

	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.openChild(OVTK_NodeId_Buffer_StreamedMatrix);
	  m_oWriterHelper.openChild(OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer);
	   m_oWriterHelper.setBinaryAsChildData(m_pMatrixBuffer, m_ui64MatrixBufferSize);
	  m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();
	return true;
}

boolean CBoxAlgorithmStreamedMatrixOutputWriterPart::writeBufferStop(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();

	m_ui32Status=Status_SendingNothing;
	return true;
}
