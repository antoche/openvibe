#include "ovtkCBoxAlgorithmSpectrumOutputWriterPart.inl"
#include <iostream>

using namespace OpenViBEToolkit;
using namespace OpenViBE;

// ________________________________________________________________________________________________________________
//

CBoxAlgorithmSpectrumOutputWriterPart::CBoxAlgorithmSpectrumOutputWriterPart(void)
	:m_ui32FrequencyBandCount(0)
{
}

// ________________________________________________________________________________________________________________
//

boolean CBoxAlgorithmSpectrumOutputWriterPart::setFrequencyBandCount(const OpenViBE::uint32 ui32FrequencyBandCount)
{
	m_ui32FrequencyBandCount=ui32FrequencyBandCount;
	m_vFrequencyBandStart.clear();
	m_vFrequencyBandStop.clear();
	return m_ui32FrequencyBandCount!=0;
}

boolean CBoxAlgorithmSpectrumOutputWriterPart::setFrequencyBandStart(const uint32 ui32FrequencyBandIndex, const float64 f64FrequencyBandStart)
{
	if(ui32FrequencyBandIndex>=m_ui32FrequencyBandCount)
	{
		return false;
	}
	m_vFrequencyBandStart[ui32FrequencyBandIndex]=f64FrequencyBandStart;
	return true;
}

boolean CBoxAlgorithmSpectrumOutputWriterPart::setFrequencyBandStop(const uint32 ui32FrequencyBandIndex, const float64 f64FrequencyBandStop)
{
	if(ui32FrequencyBandIndex>=m_ui32FrequencyBandCount)
	{
		return false;
	}
	m_vFrequencyBandStop[ui32FrequencyBandIndex]=f64FrequencyBandStop;
	return true;
}


// ________________________________________________________________________________________________________________
//

boolean CBoxAlgorithmSpectrumOutputWriterPart::writeHeaderStart(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.openChild(OVTK_NodeId_Header_Spectrum);
	  for(uint32 i=0; i<m_ui32FrequencyBandCount; i++)
	  {
	   m_oWriterHelper.openChild(OVTK_NodeId_Header_Spectrum_FrequencyBand);
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_Spectrum_FrequencyBand_Start);
	     m_oWriterHelper.setFloat64AsChildData(m_vFrequencyBandStart[i]);
	    m_oWriterHelper.closeChild();
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop);
	     m_oWriterHelper.setFloat64AsChildData(m_vFrequencyBandStop[i]);
	    m_oWriterHelper.closeChild();
	   m_oWriterHelper.closeChild();
	  }

	m_oWriterHelper.disconnect();
	return true;
}


boolean CBoxAlgorithmSpectrumOutputWriterPart::writeHeaderStop(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();
	return true;
}
