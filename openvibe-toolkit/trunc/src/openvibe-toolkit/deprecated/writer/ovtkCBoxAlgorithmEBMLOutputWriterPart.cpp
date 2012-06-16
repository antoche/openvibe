#include "ovtkCBoxAlgorithmEBMLOutputWriterPart.inl"

using namespace OpenViBEToolkit;
using namespace OpenViBE;

boolean CBoxAlgorithmEBMLOutputWriterPart::writeHeaderStart(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.openChild(OVTK_NodeId_Header);
	  m_oWriterHelper.openChild(OVTK_NodeId_Header_StreamType);
	   m_oWriterHelper.setUIntegerAsChildData(0);
	  m_oWriterHelper.closeChild();
	  m_oWriterHelper.openChild(OVTK_NodeId_Header_StreamVersion);
	   m_oWriterHelper.setUIntegerAsChildData(0);
	  m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();
	return true;
}

boolean CBoxAlgorithmEBMLOutputWriterPart::writeHeaderStop(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();
	return true;
}

boolean CBoxAlgorithmEBMLOutputWriterPart::writeBufferStart(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.openChild(OVTK_NodeId_Buffer);

	m_oWriterHelper.disconnect();
	return true;
}

boolean CBoxAlgorithmEBMLOutputWriterPart::writeBufferStop(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();
	return true;
}

boolean CBoxAlgorithmEBMLOutputWriterPart::writeEndStart(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.openChild(OVTK_NodeId_End);

	m_oWriterHelper.disconnect();
	return true;
}

boolean CBoxAlgorithmEBMLOutputWriterPart::writeEndStop(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();
	return true;
}
