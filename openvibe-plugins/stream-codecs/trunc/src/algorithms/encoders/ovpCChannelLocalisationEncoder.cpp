#include "ovpCChannelLocalisationEncoder.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

boolean CChannelLocalisationEncoder::initialize(void)
{
	CStreamedMatrixEncoder::initialize();

	ip_bDynamic.initialize(getInputParameter(OVP_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Dynamic));

	return true;
}

boolean CChannelLocalisationEncoder::uninitialize(void)
{
	ip_bDynamic.uninitialize();

	CStreamedMatrixEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CChannelLocalisationEncoder::processHeader(void)
{
	CStreamedMatrixEncoder::processHeader();

	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ChannelLocalisation);
	 m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ChannelLocalisation_Dynamic);
	  m_pEBMLWriterHelper->setUIntegerAsChildData(ip_bDynamic?1:0);
	 m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->closeChild();

	return true;
}
