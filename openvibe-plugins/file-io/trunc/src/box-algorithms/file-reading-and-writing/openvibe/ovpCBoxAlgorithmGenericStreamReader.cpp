#include "ovpCBoxAlgorithmGenericStreamReader.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

CBoxAlgorithmGenericStreamReader::CBoxAlgorithmGenericStreamReader(void)
	:m_oReader(*this)
	,m_pFile(NULL)
{
}

uint64 CBoxAlgorithmGenericStreamReader::getClockFrequency(void)
{
	return 128LL<<32; // the box clock frequency
}

boolean CBoxAlgorithmGenericStreamReader::initialize(void)
{
	getStaticBoxContext().getSettingValue(0, m_sFilename);

	m_pFile=::fopen(m_sFilename.toASCIIString(), "rb");
	if(!m_pFile)
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Could not open file [" << m_sFilename << "]\n";
		return false;
	}

	m_bPending=false;
	m_bUseCompression=false;

	m_vStreamIndexToOutputIndex.clear();
	m_vStreamIndexToTypeIdentifier.clear();

	return true;
}

boolean CBoxAlgorithmGenericStreamReader::uninitialize(void)
{
	if(m_pFile)
	{
		::fclose(m_pFile);
		m_pFile=NULL;
	}

	return true;
}

boolean CBoxAlgorithmGenericStreamReader::processClock(IMessageClock& rMessageClock)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmGenericStreamReader::process(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	uint64 l_ui64Time=this->getPlayerContext().getCurrentTime();
	boolean l_bFinished=false;

	while(!l_bFinished && (!::feof(m_pFile) || m_bPending))
	{
		if(m_bPending)
		{
			if(m_ui64EndTime<=l_ui64Time)
			{
				if(m_ui32OutputIndex>=l_rStaticBoxContext.getOutputCount())
				{
					this->getLogManager() << LogLevel_Warning << "Stream index " << m_ui32OutputIndex << " can not be output from this box because it does not have enough outputs\n";
				}
				else
				{
					l_rDynamicBoxContext.getOutputChunk(m_ui32OutputIndex)->append(m_oPendingChunk);
					l_rDynamicBoxContext.markOutputAsReadyToSend(m_ui32OutputIndex, m_ui64StartTime, m_ui64EndTime);
				}
				m_bPending=false;
			}
			else
			{
				l_bFinished=true;
			}
		}
		else
		{
			boolean l_bJustStarted=true;
			while(!::feof(m_pFile) && m_oReader.getCurrentNodeIdentifier()==EBML::CIdentifier())
			{
				uint8 l_ui8Byte;
				size_t s=::fread(&l_ui8Byte, sizeof(uint8), 1, m_pFile);

				if(s!=1 && !l_bJustStarted)
				{
					this->getLogManager() << LogLevel_ImportantWarning << "Unexpected end of file " << m_sFilename << "\n";
					return false;
				}

				m_oReader.processData(&l_ui8Byte, sizeof(l_ui8Byte));
				l_bJustStarted=false;
			}
			if(!::feof(m_pFile) && m_oReader.getCurrentNodeSize()!=0)
			{
				m_oSwap.setSize(m_oReader.getCurrentNodeSize(), true);
				size_t s=::fread(m_oSwap.getDirectPointer(), sizeof(uint8), m_oSwap.getSize(), m_pFile);

				if(s!=m_oSwap.getSize())
				{
					this->getLogManager() << LogLevel_ImportantWarning << "Unexpected end of file " << m_sFilename << "\n";
					return false;
				}

				m_oPendingChunk.setSize(0, true);
				m_ui64StartTime=(uint64)-1;
				m_ui64EndTime=(uint64)-1;
				m_ui32OutputIndex=(uint32)-1;

				m_oReader.processData(m_oSwap.getDirectPointer(), m_oSwap.getSize());

				// FIXME
				if(m_bUseCompression)
				{
					return false;
				}
			}
		}
	}

	return true;
}

EBML::boolean CBoxAlgorithmGenericStreamReader::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	if(rIdentifier==OVP_NodeId_OpenViBEStream_Header              ) return true;
	if(rIdentifier==OVP_NodeId_OpenViBEStream_Header_Compression  ) return false;
	if(rIdentifier==OVP_NodeId_OpenViBEStream_Header_ChannelType  ) return false;
	if(rIdentifier==OVP_NodeId_OpenViBEStream_Buffer              ) return true;
	if(rIdentifier==OVP_NodeId_OpenViBEStream_Buffer_ChannelIndex ) return false;
	if(rIdentifier==OVP_NodeId_OpenViBEStream_Buffer_StartTime    ) return false;
	if(rIdentifier==OVP_NodeId_OpenViBEStream_Buffer_EndTime      ) return false;
	if(rIdentifier==OVP_NodeId_OpenViBEStream_Buffer_Content      ) return false;
	return false;
}

void CBoxAlgorithmGenericStreamReader::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if(l_rTop==OVP_NodeId_OpenViBEStream_Header)
	{
		m_vStreamIndexToOutputIndex.clear();
		m_vStreamIndexToTypeIdentifier.clear();
	}
}

void CBoxAlgorithmGenericStreamReader::processChildData(const void* pBuffer, const uint64 ui64BufferSize)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if(l_rTop==OVP_NodeId_OpenViBEStream_Header_Compression)
	{
		if(m_oReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize)!=0)
		{
			m_bUseCompression=true;
			this->getLogManager() << LogLevel_ImportantWarning << "File decompression not yet implemented\n";
		}
		else
		{
			m_bUseCompression=false;
		}
	}
	if(l_rTop==OVP_NodeId_OpenViBEStream_Header_ChannelType)
	{
		m_vStreamIndexToTypeIdentifier[m_vStreamIndexToTypeIdentifier.size()]=m_oReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize);
	}

	if(l_rTop==OVP_NodeId_OpenViBEStream_Buffer_ChannelIndex)
	{
		uint32 l_ui32StreamIndex=(uint32)m_oReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize);
		if(m_vStreamIndexToTypeIdentifier.find(l_ui32StreamIndex)!=m_vStreamIndexToTypeIdentifier.end())
		{
			m_ui32OutputIndex=m_vStreamIndexToOutputIndex[l_ui32StreamIndex];
		}
		else
		{
			this->getLogManager() << LogLevel_Trace << "Discarded buffer on stream " << l_ui32StreamIndex << " that has no corresponding output\n";
		}
	}
	if(l_rTop==OVP_NodeId_OpenViBEStream_Buffer_StartTime)
	{
		m_ui64StartTime=m_oReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize);
	}
	if(l_rTop==OVP_NodeId_OpenViBEStream_Buffer_EndTime)
	{
		m_ui64EndTime=m_oReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize);
	}
	if(l_rTop==OVP_NodeId_OpenViBEStream_Buffer_Content)
	{
		m_oPendingChunk.setSize(0, true);
		m_oPendingChunk.append(reinterpret_cast<const EBML::uint8*>(pBuffer), ui64BufferSize);
	}
}

void CBoxAlgorithmGenericStreamReader::closeChild(void)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if(l_rTop==OVP_NodeId_OpenViBEStream_Header)
	{
		IBox& l_rStaticBoxContext=this->getStaticBoxContext();

		std::map < uint32, CIdentifier >::const_iterator it;
		std::map < uint32, uint32 > l_vOutputIndexToStreamIndex;

		boolean l_bLostStreams=false;
		boolean l_bLastOutputs=false;

		// Go on each stream of the file
		for(it=m_vStreamIndexToTypeIdentifier.begin(); it!=m_vStreamIndexToTypeIdentifier.end(); it++)
		{
			CIdentifier l_oOutputTypeIdentifier;
			uint32 l_ui32Index=(uint32)-1;

			// Find the first box output with this type that has no file stream connected
			for(uint32 i=0; i<l_rStaticBoxContext.getOutputCount() && l_ui32Index==(uint32)-1; i++)
			{
				if(l_rStaticBoxContext.getOutputType(i, l_oOutputTypeIdentifier))
				{
					if(l_vOutputIndexToStreamIndex.find(i)==l_vOutputIndexToStreamIndex.end())
					{
						if(l_oOutputTypeIdentifier==it->second)
						{
							this->getLogManager() << LogLevel_Trace << "Found output " << i << " for stream " << it->first << " with corresponding type identifier " << l_oOutputTypeIdentifier << "\n";
							l_ui32Index=i;
						}
					}
				}
			}

			// In case no suiting output was found, look for a derived one
			for(uint32 i=0; i<l_rStaticBoxContext.getOutputCount() && l_ui32Index==(uint32)-1; i++)
			{
				if(l_rStaticBoxContext.getOutputType(i, l_oOutputTypeIdentifier))
				{
					if(l_vOutputIndexToStreamIndex.find(i)==l_vOutputIndexToStreamIndex.end())
					{
						if(this->getTypeManager().isDerivedFromStream(l_oOutputTypeIdentifier, it->second))
						{
							this->getLogManager() << LogLevel_Trace << "Found output " << i << " for stream " << it->first << " with corresponding derived type identifier " << l_oOutputTypeIdentifier << " " << it->second << "\n";
							l_ui32Index=i;
						}
					}
				}
			}

			// In case it was not find
			if(l_ui32Index==(uint32)-1)
			{
				CString l_sTypeName=this->getTypeManager().getTypeName(it->second);
				this->getLogManager() << LogLevel_Warning << "Did not find output for stream " << it->first << " of type identifier " << it->second << " (type name is [" << l_sTypeName << "])\n";
				m_vStreamIndexToOutputIndex[it->first]=(uint32)-1;
				l_bLostStreams=true;
			}
			else
			{
				m_vStreamIndexToOutputIndex[it->first]=l_ui32Index;
				l_vOutputIndexToStreamIndex[l_ui32Index]=it->first;
			}
		}

		// Warns for output with no stream connected to them
		for(uint32 i=0; i<l_rStaticBoxContext.getOutputCount(); i++)
		{
			if(l_vOutputIndexToStreamIndex.find(i)==l_vOutputIndexToStreamIndex.end())
			{
				this->getLogManager() << LogLevel_Warning << "Output " << i << " did not find a stream candidate from the input file\n";
				l_bLastOutputs=true;
			}
		}

		// When both outputs and streams were lost, there most probably was a damn mistake
		if(l_bLastOutputs && l_bLostStreams)
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Current configuration has both file stream(s) with no output connector and output connector(s) with no file stream, you probably should check your box configuration\n";
		}
	}

	if(l_rTop==OVP_NodeId_OpenViBEStream_Buffer)
	{
		m_bPending=((m_ui32OutputIndex!=(uint32)-1) && (m_ui64StartTime!=(uint64)-1) && (m_ui64EndTime!=(uint64)-1));
	}

	m_vNodes.pop();
}
