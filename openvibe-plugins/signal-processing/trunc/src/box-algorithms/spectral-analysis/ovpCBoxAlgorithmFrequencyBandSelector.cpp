#include "ovpCBoxAlgorithmFrequencyBandSelector.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

#include <vector>
#include <string>
#include <cstdio>

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)<(b)?(b):(a))

namespace
{
	std::vector < std::string > split(const std::string& sString, const char c)
	{
		std::vector < std::string > l_vResult;
		std::string::size_type i=0;
		std::string::size_type j=0;
		while(i<sString.length())
		{
			j=i;
			while(j<sString.length() && sString[j]!=c)
			{
				j++;
			}
			if(i!=j)
			{
				l_vResult.push_back(std::string(sString, i, j-i));
			}
			i=j+1;
		}
		return l_vResult;
	}
};

boolean CBoxAlgorithmFrequencyBandSelector::initialize(void)
{
	CString l_sSettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	std::vector < std::string > l_vSetting=::split(l_sSettingValue.toASCIIString(), OV_Value_EnumeratedStringSeparator);
	std::vector < std::string > l_vSettingRange;
	std::vector < std::string >::const_iterator it;
	std::vector < std::string >::const_iterator itRange;
	for(it=l_vSetting.begin(); it!=l_vSetting.end(); it++)
	{
		boolean l_bGood=false;
		l_vSettingRange=::split(*it, OV_Value_RangeStringSeparator);
		if(l_vSettingRange.size() == 1)
		{
			double l_dValue;
			if(::sscanf(l_vSettingRange[0].c_str(), "%lf", &l_dValue)==1)
			{
				m_vSelected.push_back(std::pair < float64, float64 >(l_dValue, l_dValue));
				l_bGood=true;
			}
		}
		else if(l_vSettingRange.size() == 2)
		{
			double l_dLowValue;
			double l_dHighValue;
			if(::sscanf(l_vSettingRange[0].c_str(), "%lf", &l_dLowValue)==1 && ::sscanf(l_vSettingRange[1].c_str(), "%lf", &l_dHighValue)==1)
			{
				m_vSelected.push_back(std::pair < float64, float64 >(min(l_dLowValue, l_dHighValue), max(l_dLowValue, l_dHighValue)));
				l_bGood=true;
			}
		}
		if(!l_bGood)
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Ignored invalid frequency band : " << CString(it->c_str()) << "\n";
		}
	}

	m_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
	m_pStreamDecoder->initialize();

	ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pMatrix.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_Matrix));
	op_pBands.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));

	m_pStreamEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
	m_pStreamEncoder->initialize();

	ip_pMatrix.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_Matrix));
	ip_pBands.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands));
	op_pMemoryBuffer.initialize(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	ip_pBands.setReferenceTarget(op_pBands);
	ip_pMatrix=&m_oMatrix;
	op_pMatrix=&m_oMatrix;

	return true;
}

boolean CBoxAlgorithmFrequencyBandSelector::uninitialize(void)
{
	op_pMemoryBuffer.uninitialize();
	ip_pBands.uninitialize();
	ip_pMatrix.uninitialize();

	m_pStreamEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	m_pStreamEncoder=NULL;

	op_pBands.uninitialize();
	op_pMatrix.uninitialize();
	ip_pMemoryBuffer.uninitialize();

	m_pStreamDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
	m_pStreamDecoder=NULL;

	return true;
}

boolean CBoxAlgorithmFrequencyBandSelector::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmFrequencyBandSelector::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);

		m_pStreamDecoder->process();
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_vSelectionFactor.clear();
			for(uint32 j=0; j<ip_pBands->getDimensionSize(1); j++)
			{
				boolean l_bSelected=false;
				for(size_t k=0; k<m_vSelected.size(); k++)
				{
					float64 l_f64Min=ip_pBands->getBuffer()[j*2+0];
					float64 l_f64Max=ip_pBands->getBuffer()[j*2+1];
					float64 l_f64HalfStep=(l_f64Max-l_f64Min)*.5;
					if(m_vSelected[k].first < l_f64Max+l_f64HalfStep && l_f64Min+l_f64HalfStep <= m_vSelected[k].second)
					{
						l_bSelected=true;
					}
				}
				m_vSelectionFactor.push_back(l_bSelected?1.:0.);
			}

			m_pStreamEncoder->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeHeader);
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			uint32 l_ui32Offset=0;
			for(uint32 j=0; j<m_oMatrix.getDimensionSize(0); j++)
			{
				for(uint32 k=0; k<m_oMatrix.getDimensionSize(1); k++)
				{
					m_oMatrix.getBuffer()[l_ui32Offset]=m_vSelectionFactor[k]*m_oMatrix.getBuffer()[l_ui32Offset];
					l_ui32Offset++;
				}
			}

			m_pStreamEncoder->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeBuffer);
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeEnd);
		}

		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
