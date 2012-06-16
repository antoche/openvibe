#include "ovpCSignalEncoder.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

boolean CSignalEncoder::initialize(void)
{
	CStreamedMatrixEncoder::initialize();

	ip_ui64SamplingRate.initialize(getInputParameter(OVP_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));

	return true;
}

boolean CSignalEncoder::uninitialize(void)
{
	ip_ui64SamplingRate.uninitialize();

	CStreamedMatrixEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CSignalEncoder::processHeader(void)
{
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_Signal);
	 m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_Signal_SamplingRate);
	  m_pEBMLWriterHelper->setUIntegerAsChildData(ip_ui64SamplingRate);
	 m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->closeChild();

	CStreamedMatrixEncoder::processHeader();

	return true;
}
