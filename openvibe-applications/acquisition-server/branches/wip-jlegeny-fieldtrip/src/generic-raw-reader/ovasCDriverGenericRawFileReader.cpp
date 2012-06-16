#include "ovasCDriverGenericRawFileReader.h"
#include "ovasCConfigurationGenericRawReader.h"

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

CDriverGenericRawFileReader::CDriverGenericRawFileReader(IDriverContext& rDriverContext)
	:CDriverGenericRawReader(rDriverContext)
	,m_pFile(NULL)
{
	m_sFilename="/tmp/some_raw_file";
}

boolean CDriverGenericRawFileReader::configure(void)
{
	CConfigurationGenericRawReader m_oConfiguration("../share/openvibe-applications/acquisition-server/interface-Generic-RawFileReader.ui",
		m_bLimitSpeed,
		m_ui32SampleFormat,
		m_ui32SampleEndian,
		m_ui32HeaderSkip,
		m_ui32FrameSkip,
		m_sFilename);
	return m_oConfiguration.configure(m_oHeader);
}

boolean CDriverGenericRawFileReader::open(void)
{
	m_pFile=::fopen(m_sFilename.toASCIIString(), "rb");
	if(!m_pFile)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not open file [" << m_sFilename << "]\n";
		return false;
	}
	return true;
}

boolean CDriverGenericRawFileReader::close(void)
{
	if(m_pFile)
	{
		::fclose(m_pFile);
		m_pFile=NULL;
	}
	return true;
}

boolean CDriverGenericRawFileReader::read(void)
{
	if(!m_pFile)
	{
		return false;
	}
	return ::fread(m_pDataFrame, 1, m_ui32DataFrameSize, m_pFile)==m_ui32DataFrameSize;
}
