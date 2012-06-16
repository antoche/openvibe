#include "ovasCDriverGenericSawTooth.h"
#include "../ovasCConfigurationBuilder.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <system/Time.h>

#include <cmath>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

//___________________________________________________________________//
//                                                                   //

CDriverGenericSawTooth::CDriverGenericSawTooth(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_pSample(NULL)
	,m_ui32TotalSampleCount(0)
	,m_ui32StartTime(0)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGenericSawTooth::CDriverGenericSawTooth\n";

	m_oHeader.setSamplingFrequency(512);
	m_oHeader.setChannelCount(1);
}

void CDriverGenericSawTooth::release(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGenericSawTooth::release\n";

	delete this;
}

const char* CDriverGenericSawTooth::getName(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGenericSawTooth::getName\n";

	return "Generic Saw Tooth";
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverGenericSawTooth::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGenericSawTooth::initialize\n";

	if(m_rDriverContext.isConnected()) { return false; }

	if(!m_oHeader.isChannelCountSet()
	 ||!m_oHeader.isSamplingFrequencySet())
	{
		return false;
	}

	m_pSample=new float32[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock];
	if(!m_pSample)
	{
		delete [] m_pSample;
		m_pSample=NULL;
		return false;
	}

	m_pCallback=&rCallback;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;

	return true;
}

boolean CDriverGenericSawTooth::start(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGenericSawTooth::start\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	m_ui32TotalSampleCount=0;
	m_ui32StartTime=System::Time::getTime();

	return true;
}

boolean CDriverGenericSawTooth::loop(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Debug << "CDriverGenericSawTooth::loop\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return true; }

	uint32 l_ui32CurrentTime=System::Time::getTime();

	if(l_ui32CurrentTime-m_ui32StartTime > (1000*(m_ui32TotalSampleCount+m_ui32SampleCountPerSentBlock))/(m_oHeader.getSamplingFrequency()))
	{
		for(uint32 j=0; j<m_oHeader.getChannelCount(); j++)
		{
			for(uint32 i=0; i<m_ui32SampleCountPerSentBlock; i++)
			{
				m_pSample[j*m_ui32SampleCountPerSentBlock+i]=float32(i)/(m_ui32SampleCountPerSentBlock-1);
			}
		}

		CStimulationSet l_oStimulationSet;
		l_oStimulationSet.appendStimulation((l_ui32CurrentTime-m_ui32StartTime)/1000, 0, 0);
		l_oStimulationSet.appendStimulation((l_ui32CurrentTime-m_ui32StartTime)/1000, (uint64(m_ui32SampleCountPerSentBlock-1)<<32)/m_oHeader.getSamplingFrequency(), 0);

		m_pCallback->setSamples(m_pSample);
		// m_pCallback->setStimulationSet(l_oStimulationSet);
		m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());

		m_ui32TotalSampleCount+=m_ui32SampleCountPerSentBlock;
	}

	return true;
}

boolean CDriverGenericSawTooth::stop(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGenericSawTooth::stop\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }
	return true;
}

boolean CDriverGenericSawTooth::uninitialize(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGenericSawTooth::uninitialize\n";

	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	delete [] m_pSample;
	m_pSample=NULL;
	m_pCallback=NULL;

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverGenericSawTooth::isConfigurable(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGenericSawTooth::isConfigurable\n";

	return false;
}

boolean CDriverGenericSawTooth::configure(void)
{
	m_rDriverContext.getLogManager() << LogLevel_Trace << "CDriverGenericSawTooth::configure\n";

	return false;
}
