#include "ovpCSpectrumEncoder.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

boolean CSpectrumEncoder::initialize(void)
{
	CStreamedMatrixEncoder::initialize();

	ip_pMinMaxFrenquencyBands.initialize(getInputParameter(OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands));

	return true;
}

boolean CSpectrumEncoder::uninitialize(void)
{
	ip_pMinMaxFrenquencyBands.uninitialize();

	CStreamedMatrixEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CSpectrumEncoder::processHeader(void)
{
	// ip_pMinMaxFrenquencyBands dimmension count should be 2
	// ip_pMinMaxFrenquencyBands dimmension size 0 should be 2
	// ip_pMinMaxFrenquencyBands dimmension size 1 should be the same as streamed matrix dimmension size 1

	IMatrix* l_pFrequencyBandMinMax=ip_pMinMaxFrenquencyBands;

	CStreamedMatrixEncoder::processHeader();

	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_Spectrum);
	 for(uint32 i=0; i<l_pFrequencyBandMinMax->getDimensionSize(1); i++)
	 {
	  m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_Spectrum_FrequencyBand);
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_Spectrum_FrequencyBand_Start);
	    m_pEBMLWriterHelper->setFloat64AsChildData(l_pFrequencyBandMinMax->getBuffer()[i*2+0]);
	   m_pEBMLWriterHelper->closeChild();
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop);
	    m_pEBMLWriterHelper->setFloat64AsChildData(l_pFrequencyBandMinMax->getBuffer()[i*2+1]);
	   m_pEBMLWriterHelper->closeChild();
	  m_pEBMLWriterHelper->closeChild();
	 }
	m_pEBMLWriterHelper->closeChild();

	return true;
}
