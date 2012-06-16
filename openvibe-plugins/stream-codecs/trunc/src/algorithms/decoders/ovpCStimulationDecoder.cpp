#include "ovpCStimulationDecoder.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

// ________________________________________________________________________________________________________________
//

boolean CStimulationDecoder::initialize(void)
{
	CEBMLBaseDecoder::initialize();

	op_pStimulationSet.initialize(getOutputParameter(OVP_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	return true;
}

boolean CStimulationDecoder::uninitialize(void)
{
	op_pStimulationSet.uninitialize();

	CEBMLBaseDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

EBML::boolean CStimulationDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	     if(rIdentifier==OVTK_NodeId_Buffer_Stimulation)                        { return true; }
	else if(rIdentifier==OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)   { return false; }
	else if(rIdentifier==OVTK_NodeId_Buffer_Stimulation_Stimulation)            { return true; }
	else if(rIdentifier==OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier) { return false; }
	else if(rIdentifier==OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)       { return false; }
	else if(rIdentifier==OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration)   { return false; }
	return CEBMLBaseDecoder::isMasterChild(rIdentifier);
}

void CStimulationDecoder::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Buffer_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration))
	{
	}
	else
	{
		CEBMLBaseDecoder::openChild(rIdentifier);
	}
}

void CStimulationDecoder::processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Buffer_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration))
	{
		if(l_rTop==OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)   { op_pStimulationSet->setStimulationCount(m_pEBMLReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize)); m_ui64StimulationIndex=0; }
		if(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier) { op_pStimulationSet->setStimulationIdentifier(m_ui64StimulationIndex, m_pEBMLReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize)); }
		if(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)       { op_pStimulationSet->setStimulationDate(m_ui64StimulationIndex, m_pEBMLReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize)); }
		if(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration)   { op_pStimulationSet->setStimulationDuration(m_ui64StimulationIndex, m_pEBMLReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize)); }
	}
	else
	{
		CEBMLBaseDecoder::processChildData(pBuffer, ui64BufferSize);
	}
}

void CStimulationDecoder::closeChild(void)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Buffer_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)
	 ||(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration))
	{
		if(l_rTop==OVTK_NodeId_Buffer_Stimulation_Stimulation) { m_ui64StimulationIndex++; }
	}
	else
	{
		CEBMLBaseDecoder::closeChild();
	}

	m_vNodes.pop();
}
