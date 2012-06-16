#include "ovpCBoxAlgorithmDifferentialIntegral.h"

#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

boolean CBoxAlgorithmDifferentialIntegral::initialize(void)
{
	// Signal stream decoder
	m_oSignalDecoder.initialize(*this);
	// Signal stream encoder
	m_oSignalEncoder.initialize(*this);
	
	// If you need to, you can manually set the reference targets to link the codecs input and output. To do so, you can use :
	m_oSignalEncoder.getInputMatrix().setReferenceTarget(m_oSignalDecoder.getOutputMatrix());
	m_oSignalEncoder.getInputSamplingRate().setReferenceTarget(m_oSignalDecoder.getOutputSamplingRate());
	
	// If you need to retrieve setting values, use the FSettingValueAutoCast function.
	// For example :
	// - CString setting at index 0 in the setting list :
	// CString l_sSettingValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	// - unsigned int64 setting at index 1 in the setting list :
	m_ui64Operation = (uint64)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_ui64FilterOrder = (uint64)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmDifferentialIntegral::uninitialize(void)
{
	m_oSignalDecoder.uninitialize();
	m_oSignalEncoder.uninitialize();

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmDifferentialIntegral::processInput(uint32 ui32InputIndex)
{
	// some pre-processing code if needed...

	// ready to process !
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
/*******************************************************************************/

float64 CBoxAlgorithmDifferentialIntegral::operation(float64 a, float64 b)
{
	if (m_ui64Operation == OVP_TypeId_DifferentialIntegralOperation_Differential)
	{
		return a-b;
	}

	if (m_ui64Operation == OVP_TypeId_DifferentialIntegralOperation_Integral)
	{
		return a+b;
	}

	return 0;

}


boolean CBoxAlgorithmDifferentialIntegral::process(void)
{
	
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	//iterate over all chunk on input 0
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		// decode the chunk i on input 0
		m_oSignalDecoder.decode(0,i);

		if(m_oSignalDecoder.isHeaderReceived())
		{
			// initialize the past data array
			IMatrix* l_pMatrix = m_oSignalDecoder.getOutputMatrix(); // the StreamedMatrix of samples.


			// initialize all of the tables according to the number of channels
			m_pPastData = new OpenViBE::float64*[l_pMatrix->getDimensionSize(0)];
			m_pTmpData = new OpenViBE::float64*[l_pMatrix->getDimensionSize(0)];
			m_pStabilized = new OpenViBE::boolean[l_pMatrix->getDimensionSize(0)];
			m_pStep = new OpenViBE::uint32[l_pMatrix->getDimensionSize(0)];

			for (uint32 k = 0; k < l_pMatrix->getDimensionSize(0); k++)
			{
				m_pStabilized[k] = false;
				m_pStep[k] = 0;
				m_pPastData[k] = new OpenViBE::float64[m_ui64FilterOrder];
				m_pTmpData[k] = new OpenViBE::float64[m_ui64FilterOrder];
				m_pTmpData[k][0] = 0;
			}

			// Encode the output header
			m_oSignalEncoder.encodeHeader(0);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}

		if(m_oSignalDecoder.isBufferReceived())
		{
			IMatrix* l_pMatrix = m_oSignalDecoder.getOutputMatrix(); // the StreamedMatrix of samples.

			uint32 l_ui32ChannelCount = l_pMatrix->getDimensionSize(0);
			uint32 l_ui32SamplesPerChannel = l_pMatrix->getDimensionSize(1);

			// ... do some process on the matrix ...

			float64* l_pBuffer = l_pMatrix->getBuffer();

			for (uint32 channel = 0; channel < l_ui32ChannelCount; channel++)
			{
				for (uint32 sample = 0; sample < l_ui32SamplesPerChannel; sample++)
				{

					// save the results of the previous step in a temporary array
					for (uint32 step = 0; step < m_pStep[channel]; step++)
					{
						m_pTmpData[channel][step] = m_pPastData[channel][step];
					}

					// save the current sample as f^0(x)
					m_pPastData[channel][0] = l_pBuffer[sample + channel * l_ui32SamplesPerChannel];

					// save all of the f^n(x)
					for (uint32 step = 1; step < m_pStep[channel]; step++)
					{
						m_pPastData[channel][step] = operation( m_pPastData[channel][step - 1], m_pTmpData[channel][step - 1] );
					}

					// if the filter is not yet stabilized we increase the step and use 0 as a return value
					if (!m_pStabilized[channel])
					{
						if (m_pStep[channel] == m_ui64FilterOrder)
						{
							m_pStabilized[channel] = true;
						}
						else
						{
							m_pStep[channel]++;
						}

						l_pBuffer[sample + channel * l_ui32SamplesPerChannel] = 0;
					}
					// otherwise use f^order(x)
					else
					{
						l_pBuffer[sample + channel * l_ui32SamplesPerChannel] = operation(m_pPastData[channel][m_ui64FilterOrder - 1], m_pTmpData[channel][m_ui64FilterOrder - 1]);
					}
				}
			}

			// Encode the output buffer
			m_oSignalEncoder.encodeBuffer(0);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));

		}

		if(m_oSignalDecoder.isEndReceived())
		{
			// End of stream received. This happens only once when pressing "stop". Just pass it to the next boxes so they receive the message :
			m_oSignalEncoder.encodeEnd(0);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}

	}

	return true;
}
