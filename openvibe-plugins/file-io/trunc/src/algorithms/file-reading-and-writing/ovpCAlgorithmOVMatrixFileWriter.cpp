#include "ovpCAlgorithmOVMatrixFileWriter.h"

#include <system/Memory.h>

#include <sstream>
#if defined OVP_OS_Windows
#include <cerrno>
#endif

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

boolean CAlgorithmOVMatrixFileWriter::initialize(void)
{
	ip_sFilename.initialize(getInputParameter(OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Filename));
	ip_pMatrix.initialize(getInputParameter(OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Matrix));

	return true;
}

boolean CAlgorithmOVMatrixFileWriter::uninitialize(void)
{
	ip_sFilename.uninitialize();
	ip_pMatrix.uninitialize();

	return true;
}

boolean CAlgorithmOVMatrixFileWriter::process(void)
{
	m_oDataFile.open(ip_sFilename->toASCIIString(), std::ios_base::out | std::ios_base::trunc );
	if(!m_oDataFile.good())
	{
		getLogManager() << LogLevel_Error << "Opening " << *ip_sFilename << " failed\n";
		return false;
	}

	dumpHeader();

	uint32 l_ui32ElementIndex = 0;
	dumpBuffer(0, l_ui32ElementIndex);

	getLogManager() << LogLevel_Trace << "Opening " << *ip_sFilename << " succeeded\n";
	m_oDataFile.close();

	return true;
}

boolean CAlgorithmOVMatrixFileWriter::dumpHeader()
{
	//header start
	m_oDataFile << "[\n";

	//dump labels for each dimension
	for(uint32 i=0; i<ip_pMatrix->getDimensionCount(); i++)
	{
		m_oDataFile << "\t[";

		for(uint32 j=0; j<ip_pMatrix->getDimensionSize(i); j++)
		{
			m_oDataFile << " \"" << ip_pMatrix->getDimensionLabel(i, j) << "\"";
		}

		m_oDataFile << " ]\n";
	}

	//header end
	m_oDataFile << "]\n";

	return true;
}

boolean CAlgorithmOVMatrixFileWriter::dumpBuffer(uint32 ui32DimensionIndex, uint32& ui32ElementIndex)
{
	//are we in innermost dimension?
	if(ui32DimensionIndex == ip_pMatrix->getDimensionCount()-1)
	{
		//dimension start
		for(uint32 j=0; j<ui32DimensionIndex; j++)
		{
			m_oDataFile << "\t";
		}
		m_oDataFile << "[";

		//dump current cell contents
		for(uint32 j=0; j<ip_pMatrix->getDimensionSize(ui32DimensionIndex); j++, ui32ElementIndex++)
		{
			m_oDataFile << " " << (*ip_pMatrix)[ui32ElementIndex];
		}

		//dimension end
		m_oDataFile << " ]\n";
	}
	else
	{
		//dump all entries in current dimension
		for(uint32 i=0; i<ip_pMatrix->getDimensionSize(ui32DimensionIndex); i++)
		{
			//dimension start
			for(uint32 j=0; j<ui32DimensionIndex; j++)
			{
				m_oDataFile << "\t";
			}
			m_oDataFile << "[\n";

			dumpBuffer(ui32DimensionIndex+1, ui32ElementIndex);

			//dimension end
			for(uint32 j=0; j<ui32DimensionIndex; j++)
			{
				m_oDataFile << "\t";
			}
			m_oDataFile << "]\n";
		}
	}

	return true;
}
