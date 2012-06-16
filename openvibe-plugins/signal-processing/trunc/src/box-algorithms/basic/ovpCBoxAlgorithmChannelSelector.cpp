#include "ovpCBoxAlgorithmChannelSelector.h"

#include <system/Memory.h>

#include <cstdio>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

using namespace OpenViBEToolkit;

namespace
{
	inline uint32 _find_channel_(const IMatrix& rMatrix, const CString& rChannel, const CIdentifier& rMatchMethodIdentifier, uint32 uiStart=0)
	{
		uint32 i, l_ui32Result=uint32(-1);

		if(rMatchMethodIdentifier==OVP_TypeId_MatchMethod_Name)
		{
			for(i=uiStart; i<rMatrix.getDimensionSize(0); i++)
			{
				if(OpenViBEToolkit::Tools::String::isAlmostEqual(rMatrix.getDimensionLabel(0, i), rChannel, false))
				{
					l_ui32Result=i;
				}
			}
		}
		else if(rMatchMethodIdentifier==OVP_TypeId_MatchMethod_Index)
		{
			unsigned int value;
			if(::sscanf(rChannel.toASCIIString(), "%u", &value)==1)
			{
				value--; // => makes it 0-indexed !
				if(uiStart <= uint32(value) && uint32(value) < rMatrix.getDimensionSize(0))
				{
					l_ui32Result=uint32(value);
				}
			}
		}
		else if(rMatchMethodIdentifier==OVP_TypeId_MatchMethod_Smart)
		{
			if(l_ui32Result==uint32(-1)) l_ui32Result=_find_channel_(rMatrix, rChannel, OVP_TypeId_MatchMethod_Name, uiStart);
			if(l_ui32Result==uint32(-1)) l_ui32Result=_find_channel_(rMatrix, rChannel, OVP_TypeId_MatchMethod_Index, uiStart);
		}

		return l_ui32Result;
	}
};

boolean CBoxAlgorithmChannelSelector::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	CIdentifier l_oTypeIdentifier;
	l_rStaticBoxContext.getOutputType(0, l_oTypeIdentifier);

	m_pDecoder=NULL;
	m_pEncoder=NULL;

	if(l_oTypeIdentifier==OV_TypeId_Signal)
	{
		TSignalEncoder < CBoxAlgorithmChannelSelector >* l_pEncoder=new TSignalEncoder < CBoxAlgorithmChannelSelector >;
		TSignalDecoder < CBoxAlgorithmChannelSelector >* l_pDecoder=new TSignalDecoder < CBoxAlgorithmChannelSelector >;
		l_pEncoder->initialize(*this);
		l_pDecoder->initialize(*this);
		l_pEncoder->getInputSamplingRate().setReferenceTarget(l_pDecoder->getOutputSamplingRate());
		m_pDecoder=l_pDecoder;
		m_pEncoder=l_pEncoder;
		m_pInputMatrix=l_pDecoder->getOutputMatrix();
		m_pOutputMatrix=l_pEncoder->getInputMatrix();
	}
	else if(l_oTypeIdentifier==OV_TypeId_Spectrum)
	{
		TSpectrumEncoder < CBoxAlgorithmChannelSelector >* l_pEncoder=new TSpectrumEncoder < CBoxAlgorithmChannelSelector >;
		TSpectrumDecoder < CBoxAlgorithmChannelSelector >* l_pDecoder=new TSpectrumDecoder < CBoxAlgorithmChannelSelector >;
		l_pEncoder->initialize(*this);
		l_pDecoder->initialize(*this);
		l_pEncoder->getInputMinMaxFrequencyBands().setReferenceTarget(l_pDecoder->getOutputMinMaxFrequencyBands());
		m_pDecoder=l_pDecoder;
		m_pEncoder=l_pEncoder;
		m_pInputMatrix=l_pDecoder->getOutputMatrix();
		m_pOutputMatrix=l_pEncoder->getInputMatrix();
	}
	else
	{
		this->getLogManager() << LogLevel_Error << "Unhandled type of streamed matrix [" << l_oTypeIdentifier << "]\n";
		return false;
	}

	m_vLookup.clear();
	return true;
}

boolean CBoxAlgorithmChannelSelector::uninitialize(void)
{
	if(m_pDecoder)
	{
		m_pDecoder->uninitialize();
		delete m_pDecoder;
	}
	if(m_pEncoder)
	{
		m_pEncoder->uninitialize();
		delete m_pEncoder;
	}

	return true;
}

boolean CBoxAlgorithmChannelSelector::processInput(uint32 ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmChannelSelector::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	uint32 i, j, k;

	for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		m_pDecoder->decode(0, i);
		if(m_pDecoder->isHeaderReceived())
		{
			CString l_sSettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
			uint64 l_ui64SelectionMethodIdentifier=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
			uint64 l_ui64MatchMethodIdentifier=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

			// ______________________________________________________________________________________________________________________________________________________
			//
			// Splits the channel list in order to build up the look up table
			// The look up table is later used to fill in the matrix content
			// ______________________________________________________________________________________________________________________________________________________
			//

			std::vector < CString > l_sToken;
			uint32 l_ui32TokenCount=OpenViBEToolkit::Tools::String::split(l_sSettingValue, OpenViBEToolkit::Tools::String::TSplitCallback < std::vector < CString > >(l_sToken), OV_Value_EnumeratedStringSeparator);
			for(j=0; j<l_ui32TokenCount; j++)
			{
				std::vector < CString > l_sSubToken;

				// Checks if the token is a range
				if(OpenViBEToolkit::Tools::String::split(l_sToken[j], OpenViBEToolkit::Tools::String::TSplitCallback < std::vector < CString > >(l_sSubToken), OV_Value_RangeStringSeparator)==2)
				{
					// Finds the first & second part of the range (only index based)
					uint32 l_ui32RangeStartIndex=::_find_channel_(*m_pInputMatrix, l_sSubToken[0], OVP_TypeId_MatchMethod_Index);
					uint32 l_ui32RangeEndIndex=::_find_channel_(*m_pInputMatrix, l_sSubToken[1], OVP_TypeId_MatchMethod_Index);

					// When first or second part is not found but associated token is empty, don't consider this as an error
					if(l_ui32RangeStartIndex==uint32(-1) && l_sSubToken[0]==CString("")) l_ui32RangeStartIndex=0;
					if(l_ui32RangeEndIndex  ==uint32(-1) && l_sSubToken[1]==CString("")) l_ui32RangeEndIndex=m_pInputMatrix->getDimensionSize(0)-1;

					 // After these corections, if either first or second token were not found, or if start index is greater than start index, consider this an error and invalid range
					if(l_ui32RangeStartIndex==uint32(-1) || l_ui32RangeEndIndex  ==uint32(-1) || l_ui32RangeStartIndex>l_ui32RangeEndIndex)
					{
						this->getLogManager() << LogLevel_Warning << "Invalid channel range [" << l_sToken[j] << "] - splitted as [" << l_sSubToken[0] << "][" << l_sSubToken[1] << "]\n";
					}
					else
					{
						// The range is valid so selects all the channels in this range
						this->getLogManager() << LogLevel_Trace << "For range [" << l_sToken[j] << "] :\n";
						for(k=l_ui32RangeStartIndex; k<=l_ui32RangeEndIndex; k++)
						{
							m_vLookup.push_back(k);
							this->getLogManager() << LogLevel_Trace << "  Selected channel [" << k+1 << "]\n";
						}
					}
				}
				else
				{
					// This is not a range, so we can consider the whole token as a single token name
					uint32 l_bFound=false;
					uint32 l_ui32Index=uint32(-1);

					// Looks for all the channels with this name
					while((l_ui32Index=::_find_channel_(*m_pInputMatrix, l_sToken[j], l_ui64MatchMethodIdentifier, l_ui32Index+1))!=uint32(-1))
					{
						l_bFound=true;
						m_vLookup.push_back(l_ui32Index);
						this->getLogManager() << LogLevel_Trace << "Selected channel [" << l_ui32Index+1 << "]\n";
					}

					// When no channel was found, consider it a missing channel
					if(!l_bFound)
					{
						this->getLogManager() << LogLevel_Warning << "Invalid channel [" << l_sToken[j] << "]\n";
						m_vLookup.push_back(uint32(-1));
					}
				}
			}

			// ______________________________________________________________________________________________________________________________________________________
			//
			// When selection method is set to reject
			// We have to revert the selection building up a new look up table and replacing the old one
			// ______________________________________________________________________________________________________________________________________________________
			//

			if(l_ui64SelectionMethodIdentifier==OVP_TypeId_SelectionMethod_Reject)
			{
				std::vector < uint32 > l_vInversedLookup;
				for(j=0; j<m_pInputMatrix->getDimensionSize(0); j++)
				{
					boolean l_bSelected=false;
					for(k=0; k<m_vLookup.size(); k++)
					{
						l_bSelected|=(m_vLookup[k]==j);
					}
					if(!l_bSelected)
					{
						l_vInversedLookup.push_back(j);
					}
				}
				m_vLookup=l_vInversedLookup;
			}

			// ______________________________________________________________________________________________________________________________________________________
			//
			// Now we have the exact topology of the output matrix :)
			// ______________________________________________________________________________________________________________________________________________________
			//

			m_pOutputMatrix->setDimensionCount(2);
			m_pOutputMatrix->setDimensionSize(0, m_vLookup.size());
			m_pOutputMatrix->setDimensionSize(1, m_pInputMatrix->getDimensionSize(1));
			Tools::Matrix::clearContent(*m_pOutputMatrix);
			for(j=0; j<m_vLookup.size(); j++)
			{
				if(m_vLookup[j] < m_pInputMatrix->getDimensionSize(0))
				{
					m_pOutputMatrix->setDimensionLabel(0, j, m_pInputMatrix->getDimensionLabel(0, m_vLookup[j]));
				}
				else
				{
					m_pOutputMatrix->setDimensionLabel(0, j, "Missing channel");
				}
			}
			m_pEncoder->encodeHeader(0);
		}
		if(m_pDecoder->isBufferReceived())
		{
			// ______________________________________________________________________________________________________________________________________________________
			//
			// When a buffer is received, just copy the channel content depending on the look up table
			// ______________________________________________________________________________________________________________________________________________________
			//

			uint32 l_ui32SampleCount=m_pOutputMatrix->getDimensionSize(1);
			for(j=0; j<m_vLookup.size(); j++)
			{
				if(m_vLookup[j] < m_pInputMatrix->getDimensionSize(0))
				{
					System::Memory::copy(
						m_pOutputMatrix->getBuffer()+j*l_ui32SampleCount,
						m_pInputMatrix->getBuffer()+m_vLookup[j]*l_ui32SampleCount,
						l_ui32SampleCount*sizeof(float64));
				}
			}
			m_pEncoder->encodeBuffer(0);
		}
		if(m_pDecoder->isEndReceived())
		{
			m_pEncoder->encodeEnd(0);
		}
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
	}

	return true;
}
