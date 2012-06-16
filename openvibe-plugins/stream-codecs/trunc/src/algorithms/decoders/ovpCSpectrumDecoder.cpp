#include "ovpCSpectrumDecoder.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

// ________________________________________________________________________________________________________________
//

boolean CSpectrumDecoder::initialize(void)
{
	CStreamedMatrixDecoder::initialize();

	op_pMinMaxFrequencyBands.initialize(getOutputParameter(OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));

	return true;
}

boolean CSpectrumDecoder::uninitialize(void)
{
	op_pMinMaxFrequencyBands.uninitialize();

	CStreamedMatrixDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

EBML::boolean CSpectrumDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	     if(rIdentifier==OVTK_NodeId_Header_Spectrum)                              { return true; }
	else if(rIdentifier==OVTK_NodeId_Header_Spectrum_FrequencyBand)                { return true; }
	else if(rIdentifier==OVTK_NodeId_Header_Spectrum_FrequencyBand_Start)          { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop)           { return false; }
	return CStreamedMatrixDecoder::isMasterChild(rIdentifier);
}

void CSpectrumDecoder::openChild(const EBML::CIdentifier& rIdentifier)
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
			op_pMinMaxFrequencyBands->setDimensionCount(2);
			op_pMinMaxFrequencyBands->setDimensionSize(0, 2);
			op_pMinMaxFrequencyBands->setDimensionSize(1, op_pMatrix->getDimensionSize(1));

			m_ui32FrequencyBandIndex=0;
		}
	}
	else
	{
		CStreamedMatrixDecoder::openChild(rIdentifier);
	}
}

void CSpectrumDecoder::processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_Spectrum)
	 ||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand)
	 ||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Start)
	 ||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop))
	{
		if(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Start) op_pMinMaxFrequencyBands->getBuffer()[m_ui32FrequencyBandIndex*2  ]=m_pEBMLReaderHelper->getFloatFromChildData(pBuffer, ui64BufferSize);
		if(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop)  op_pMinMaxFrequencyBands->getBuffer()[m_ui32FrequencyBandIndex*2+1]=m_pEBMLReaderHelper->getFloatFromChildData(pBuffer, ui64BufferSize);
	}
	else
	{
		CStreamedMatrixDecoder::processChildData(pBuffer, ui64BufferSize);
	}
}

void CSpectrumDecoder::closeChild(void)
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
		CStreamedMatrixDecoder::closeChild();
	}

	m_vNodes.pop();
}
