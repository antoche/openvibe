#include "ovpCAlgorithmAddition.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;

boolean CAlgorithmAddition::initialize(void)
{
	m_oParameter1.initialize(getInputParameter(CIdentifier(0,1)));
	m_oParameter2.initialize(getInputParameter(CIdentifier(0,2)));
	m_oParameter3.initialize(getOutputParameter(CIdentifier(0,3)));

	return true;
}

boolean CAlgorithmAddition::uninitialize(void)
{
	m_oParameter3.uninitialize();
	m_oParameter2.uninitialize();
	m_oParameter1.uninitialize();

	return true;
}

boolean CAlgorithmAddition::process(void)
{
	m_oParameter3=m_oParameter1+m_oParameter2;

	return true;
}
