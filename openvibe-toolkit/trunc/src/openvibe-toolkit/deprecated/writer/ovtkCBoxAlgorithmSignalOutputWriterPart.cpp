#include "ovtkCBoxAlgorithmSignalOutputWriterPart.inl"

using namespace OpenViBEToolkit;
using namespace OpenViBE;

// ________________________________________________________________________________________________________________
//

CBoxAlgorithmSignalOutputWriterPart::CBoxAlgorithmSignalOutputWriterPart(void)
	:m_ui32SamplingRate(0)
{
}

// ________________________________________________________________________________________________________________
//

OpenViBE::boolean CBoxAlgorithmSignalOutputWriterPart::setSamplingRate(const OpenViBE::uint32 ui32SamplingRate)
{
	m_ui32SamplingRate=ui32SamplingRate;
	return m_ui32SamplingRate!=0;
}

// ________________________________________________________________________________________________________________
//

OpenViBE::boolean CBoxAlgorithmSignalOutputWriterPart::writeHeaderStart(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.openChild(OVTK_NodeId_Header_Signal);
	  m_oWriterHelper.openChild(OVTK_NodeId_Header_Signal_SamplingRate);
	   m_oWriterHelper.setUIntegerAsChildData(m_ui32SamplingRate);
	  m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();
	return true;
}


OpenViBE::boolean CBoxAlgorithmSignalOutputWriterPart::writeHeaderStop(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();
	return true;
}
