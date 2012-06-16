#include "ovpCBoxAlgorithmAdditionTest.h"

#include <cstdlib>
#include <cmath>

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

CBoxAlgorithmAdditionTest::CBoxAlgorithmAdditionTest(void)
	:m_iInt64_1(0)
	,m_iInt64_2(0)
	,m_iInt64_3(0)
	,m_iInt64_4(0)
{
}

uint64 CBoxAlgorithmAdditionTest::getClockFrequency(void)
{
	return ((uint64)1LL)<<36;
}

boolean CBoxAlgorithmAdditionTest::initialize(void)
{
	CString l_sLogLevel;
	getStaticBoxContext().getSettingValue(0, l_sLogLevel);
	m_eLogLevel=static_cast<ELogLevel>(getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_LogLevel, l_sLogLevel));

	m_pProxy1=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_AlgorithmAddition));
	m_pProxy2=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_AlgorithmAddition));
	m_pProxy3=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_AlgorithmAddition));

	m_pProxy1->initialize();
	m_pProxy2->initialize();
	m_pProxy3->initialize();

	m_pProxy1->getInputParameter(CIdentifier(0,1))->setReferenceTarget(&m_iInt64_1);
	m_pProxy1->getInputParameter(CIdentifier(0,2))->setReferenceTarget(&m_iInt64_2);
	m_pProxy2->getInputParameter(CIdentifier(0,1))->setReferenceTarget(&m_iInt64_3);
	m_pProxy2->getInputParameter(CIdentifier(0,2))->setReferenceTarget(&m_iInt64_4);
	m_pProxy3->getInputParameter(CIdentifier(0,1))->setReferenceTarget(m_pProxy1->getOutputParameter(CIdentifier(0,3)));
	m_pProxy3->getInputParameter(CIdentifier(0,2))->setReferenceTarget(m_pProxy2->getOutputParameter(CIdentifier(0,3)));

	return true;
}

boolean CBoxAlgorithmAdditionTest::uninitialize(void)
{
	m_pProxy1->uninitialize();
	m_pProxy2->uninitialize();
	m_pProxy3->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_pProxy1);
	getAlgorithmManager().releaseAlgorithm(*m_pProxy2);
	getAlgorithmManager().releaseAlgorithm(*m_pProxy3);

	return true;
}

boolean CBoxAlgorithmAdditionTest::processClock(IMessageClock& rMessageClock)
{
	m_iInt64_1=(rand()%100);
	m_iInt64_2=(rand()%100)*100;
	m_iInt64_3=(rand()%100)*10000;
	m_iInt64_4=(rand()%100)*1000000;

	m_pProxy1->process();
	m_pProxy2->process();
	m_pProxy3->process();

	TParameterHandler < int64 > l_oParameter_1_1(m_pProxy1->getInputParameter(CIdentifier(0,1)));
	TParameterHandler < int64 > l_oParameter_1_2(m_pProxy1->getInputParameter(CIdentifier(0,2)));
	TParameterHandler < int64 > l_oParameter_1_3(m_pProxy1->getOutputParameter(CIdentifier(0,3)));

	TParameterHandler < int64 > l_oParameter_2_1(m_pProxy2->getInputParameter(CIdentifier(0,1)));
	TParameterHandler < int64 > l_oParameter_2_2(m_pProxy2->getInputParameter(CIdentifier(0,2)));
	TParameterHandler < int64 > l_oParameter_2_3(m_pProxy2->getOutputParameter(CIdentifier(0,3)));

	TParameterHandler < int64 > l_oParameter_3_1(m_pProxy3->getInputParameter(CIdentifier(0,1)));
	TParameterHandler < int64 > l_oParameter_3_2(m_pProxy3->getInputParameter(CIdentifier(0,2)));
	TParameterHandler < int64 > l_oParameter_3_3(m_pProxy3->getOutputParameter(CIdentifier(0,3)));

	getLogManager() << m_eLogLevel << "l_oParameter_1_1 = " << l_oParameter_1_1 << "\n";
	getLogManager() << m_eLogLevel << "l_oParameter_1_2 = " << l_oParameter_1_2 << "\n";
	getLogManager() << m_eLogLevel << "l_oParameter_1_3 = " << l_oParameter_1_3 << "\n";
	getLogManager() << m_eLogLevel << "l_oParameter_2_1 = " << l_oParameter_2_1 << "\n";
	getLogManager() << m_eLogLevel << "l_oParameter_2_2 = " << l_oParameter_2_2 << "\n";
	getLogManager() << m_eLogLevel << "l_oParameter_2_3 = " << l_oParameter_2_3 << "\n";
	getLogManager() << m_eLogLevel << "l_oParameter_3_1 = " << l_oParameter_3_1 << "\n";
	getLogManager() << m_eLogLevel << "l_oParameter_3_2 = " << l_oParameter_3_2 << "\n";
	getLogManager() << m_eLogLevel << "l_oParameter_3_3 = " << l_oParameter_3_3 << "\n";
	getLogManager() << m_eLogLevel << "------------------\n";

	return true;
}

boolean CBoxAlgorithmAdditionTest::process(void)
{
	return true;
}
