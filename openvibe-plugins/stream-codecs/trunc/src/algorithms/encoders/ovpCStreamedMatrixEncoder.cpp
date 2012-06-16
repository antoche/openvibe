#include "ovpCStreamedMatrixEncoder.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

boolean CStreamedMatrixEncoder::initialize(void)
{
	CEBMLBaseEncoder::initialize();

	ip_pMatrix.initialize(getInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));

	m_ui64MatrixBufferSize=0;

	return true;
}

boolean CStreamedMatrixEncoder::uninitialize(void)
{
	ip_pMatrix.uninitialize();

	CEBMLBaseEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CStreamedMatrixEncoder::processHeader(void)
{
	IMatrix* l_pMatrix=ip_pMatrix;

	m_ui64MatrixBufferSize=(l_pMatrix->getDimensionCount()==0?0:1);

	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_StreamedMatrix);
	 m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_StreamedMatrix_DimensionCount);
	  m_pEBMLWriterHelper->setUIntegerAsChildData(l_pMatrix->getDimensionCount());
	 m_pEBMLWriterHelper->closeChild();
	 for(uint32 i=0; i<l_pMatrix->getDimensionCount(); i++)
	 {
	  m_ui64MatrixBufferSize*=l_pMatrix->getDimensionSize(i);
	  m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_StreamedMatrix_Dimension);
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_StreamedMatrix_Dimension_Size);
	    m_pEBMLWriterHelper->setUIntegerAsChildData(l_pMatrix->getDimensionSize(i));
	   m_pEBMLWriterHelper->closeChild();
	   for(uint32 j=0; j<l_pMatrix->getDimensionSize(i); j++)
	   {
	    m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_StreamedMatrix_Dimension_Label);
	     m_pEBMLWriterHelper->setASCIIStringAsChildData(l_pMatrix->getDimensionLabel(i, j));
	    m_pEBMLWriterHelper->closeChild();
	   }
	  m_pEBMLWriterHelper->closeChild();
	 }
	m_pEBMLWriterHelper->closeChild();

	return true;
}

boolean CStreamedMatrixEncoder::processBuffer(void)
{
	IMatrix* l_pMatrix=ip_pMatrix;

	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Buffer_StreamedMatrix);
	 m_pEBMLWriterHelper->openChild(OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer);
	  m_pEBMLWriterHelper->setBinaryAsChildData(l_pMatrix->getBuffer(), m_ui64MatrixBufferSize*sizeof(float64));
	 m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->closeChild();

	return true;
}
