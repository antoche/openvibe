#include "ovtkCBoxAlgorithmStimulationOutputWriterPart.inl"

#define OVTK_Version_StimulationOutputWriter 1

using namespace OpenViBEToolkit;
using namespace OpenViBE;
using namespace std;

// ________________________________________________________________________________________________________________
//

CBoxAlgorithmStimulationOutputWriterPart::CBoxAlgorithmStimulationOutputWriterPart(void)
	:m_ui32SitmulationCount(0)
{
}

// ________________________________________________________________________________________________________________
//

boolean CBoxAlgorithmStimulationOutputWriterPart::setStimulationCount(const uint32 ui32StimulationCount)
{
	m_ui32SitmulationCount=ui32StimulationCount;
	return true;
}

boolean CBoxAlgorithmStimulationOutputWriterPart::setStimulation(const uint32 ui32StimulationIndex, const uint64 ui64StimulationIdentifier, const uint64 ui64StimulationDate)
{
	m_vStimulation[ui32StimulationIndex]=pair<uint64, uint64>(ui64StimulationIdentifier, ui64StimulationDate);
	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CBoxAlgorithmStimulationOutputWriterPart::writeBufferStart(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.openChild(OVTK_NodeId_Buffer);
	  m_oWriterHelper.openChild(OVTK_NodeId_Buffer_Stimulation);
	   m_oWriterHelper.openChild(OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations);
	    m_oWriterHelper.setUIntegerAsChildData(m_ui32SitmulationCount);
	   m_oWriterHelper.closeChild();
	   for(uint32 i=0; i<m_ui32SitmulationCount; i++)
	   {
	     m_oWriterHelper.openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation);
	      m_oWriterHelper.openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier);
	       m_oWriterHelper.setUIntegerAsChildData(m_vStimulation[i].first);
	      m_oWriterHelper.closeChild();
	      m_oWriterHelper.openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation_Date);
	       m_oWriterHelper.setUIntegerAsChildData(m_vStimulation[i].second);
	      m_oWriterHelper.closeChild();
	     m_oWriterHelper.closeChild();
	    }
	  m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();
	return true;
}

boolean CBoxAlgorithmStimulationOutputWriterPart::writeBufferStop(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();
	return true;
}
