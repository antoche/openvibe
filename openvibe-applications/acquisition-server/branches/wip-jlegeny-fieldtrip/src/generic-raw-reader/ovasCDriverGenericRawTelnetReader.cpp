#include "ovasCDriverGenericRawTelnetReader.h"
#include "ovasCConfigurationGenericRawReader.h"

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

CDriverGenericRawTelnetReader::CDriverGenericRawTelnetReader(IDriverContext& rDriverContext)
	:CDriverGenericRawReader(rDriverContext)
	,m_pConnection(NULL)
{
	m_sHostName="localhost";
	m_ui32HostPort=1337;
}

boolean CDriverGenericRawTelnetReader::configure(void)
{
	CString l_sFilename;
	CConfigurationGenericRawReader m_oConfiguration("../share/openvibe-applications/acquisition-server/interface-Generic-RawTelnetReader.ui",
		m_bLimitSpeed,
		m_ui32SampleFormat,
		m_ui32SampleEndian,
		m_ui32HeaderSkip,
		m_ui32FrameSkip,
		l_sFilename);
	m_oConfiguration.setHostName(m_sHostName);
	m_oConfiguration.setHostPort(m_ui32HostPort);
	if(!m_oConfiguration.configure(m_oHeader))
	{
		return false;
	}

	m_sHostName=m_oConfiguration.getHostName();
	m_ui32HostPort=m_oConfiguration.getHostPort();
	return true;
}

boolean CDriverGenericRawTelnetReader::open(void)
{
	m_pConnection=Socket::createConnectionClient();
	if(!m_pConnection)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not create client connection\n";
		return false;
	}
	if(!m_pConnection->connect(m_sHostName.toASCIIString(), m_ui32HostPort))
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not connect to server [" << m_sHostName << ":" << m_ui32HostPort << "]\n";
		return false;
	}
	return true;
}

boolean CDriverGenericRawTelnetReader::close(void)
{
	if(m_pConnection)
	{
		m_pConnection->close();
		m_pConnection->release();
		m_pConnection=NULL;
	}
	return true;
}

boolean CDriverGenericRawTelnetReader::read(void)
{
	if(!m_pConnection)
	{
		return false;
	}
	return m_pConnection->receiveBufferBlocking(m_pDataFrame, m_ui32DataFrameSize);
}
