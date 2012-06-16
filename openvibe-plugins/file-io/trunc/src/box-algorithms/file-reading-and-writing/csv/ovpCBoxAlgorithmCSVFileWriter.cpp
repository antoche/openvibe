#include "ovpCBoxAlgorithmCSVFileWriter.h"

#include <string>
#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

CBoxAlgorithmCSVFileWriter::CBoxAlgorithmCSVFileWriter(void)
	:m_pFile(NULL)
	,m_fpRealProcess(NULL)
	,m_pStreamDecoder(NULL)
	,m_pMatrix(NULL)
	,m_bDeleteMatrix(false)
{
}

boolean CBoxAlgorithmCSVFileWriter::initialize(void)
{
	this->getStaticBoxContext().getInputType(0, m_oTypeIdentifier);

	CString l_sFilename=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_sSeparator=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_bUseCompression=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

	m_pFile=::fopen(l_sFilename.toASCIIString(), "wb");
	if(!m_pFile)
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Could not open file [" << l_sFilename << "]\n";
		return false;
	}

	if(this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier, OV_TypeId_StreamedMatrix))
	{
		if(m_oTypeIdentifier==OV_TypeId_Signal)
		{
			m_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
			m_pStreamDecoder->initialize();
			op_ui64SamplingFrequency.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
		}
		else if(m_oTypeIdentifier==OV_TypeId_Spectrum)
		{
			m_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
			m_pStreamDecoder->initialize();
			op_pMinMaxFrequencyBand.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));
		}
		else
		{
			m_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
			m_pStreamDecoder->initialize();
		}

		ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
		op_pMatrix.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
		m_fpRealProcess=&CBoxAlgorithmCSVFileWriter::process_streamedMatrix;
	}
	else if(m_oTypeIdentifier==OV_TypeId_Stimulations)
	{
		m_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
		m_pStreamDecoder->initialize();
		ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
		op_pStimulationSet.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
		m_fpRealProcess=&CBoxAlgorithmCSVFileWriter::process_stimulation;
	}
	else
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Invalid input type identifier " << this->getTypeManager().getTypeName(m_oTypeIdentifier) << "\n";
		return false;
	}

	if(m_bUseCompression)
	{
		this->getLogManager() << LogLevel_Warning << "Compression flag not used yet, the file will be flagged uncompressed and stored as is\n";
	}

	m_bFirstBuffer=true;
	return true;
}

boolean CBoxAlgorithmCSVFileWriter::uninitialize(void)
{
	if(m_pFile)
	{
		::fclose(m_pFile);
		m_pFile=NULL;
	}
	if(m_bDeleteMatrix)
	{
		delete m_pMatrix;
	}
	op_pStimulationSet.uninitialize();
	op_pMatrix.uninitialize();
	ip_pMemoryBuffer.uninitialize();

	if(m_pStreamDecoder)
	{
		m_pStreamDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
		m_pStreamDecoder=NULL;
	}

	return true;
}

boolean CBoxAlgorithmCSVFileWriter::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmCSVFileWriter::process(void)
{
	return (this->*m_fpRealProcess)();
}

boolean CBoxAlgorithmCSVFileWriter::process_streamedMatrix(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		uint64 l_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(0, i);
		uint64 l_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(0, i);
		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		m_pStreamDecoder->process();
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			if(op_pMatrix->getDimensionCount() > 2 || op_pMatrix->getDimensionCount() < 1)
			{
				this->getLogManager() << LogLevel_ImportantWarning << "Input matrix does not have 1 or 2 dimensions - Could not write content in CSV file...\n";
				return false;
			}

			if( op_pMatrix->getDimensionCount() == 1 )
			{
				m_pMatrix = new CMatrix();
				m_bDeleteMatrix = true;
				m_pMatrix->setDimensionCount(2);
				m_pMatrix->setDimensionSize(0,1);
				m_pMatrix->setDimensionSize(1,op_pMatrix->getDimensionSize(0));
				for(uint32 i=0;i<op_pMatrix->getDimensionSize(0);i++)
				{
					m_pMatrix->setDimensionLabel(1,i,op_pMatrix->getDimensionLabel(0,i));
				}
			}
			else
			{
				m_pMatrix=op_pMatrix;
			}
//			std::cout<<&m_pMatrix<<" "<<&op_pMatrix<<"\n";
			::fprintf(m_pFile, "Time (s)");
			for(uint32 c=0; c<m_pMatrix->getDimensionSize(0); c++)
			{
				std::string l_sLabel(m_pMatrix->getDimensionLabel(0, c));
				while(l_sLabel.length()>0 && l_sLabel[l_sLabel.length()-1]==' ')
				{
					l_sLabel.erase(l_sLabel.length()-1);
				}
				::fprintf(m_pFile,
					"%s%s",
					m_sSeparator.toASCIIString(),
					l_sLabel.c_str());
			}

			if(m_oTypeIdentifier==OV_TypeId_Signal)
			{
				::fprintf(m_pFile,
					"%sSampling Rate",
					m_sSeparator.toASCIIString());
			}
			else if(m_oTypeIdentifier==OV_TypeId_Spectrum)
			{
				::fprintf(m_pFile,
					"%sMin frequency band",
					m_sSeparator.toASCIIString());
				::fprintf(m_pFile,
					"%sMax frequency band",
					m_sSeparator.toASCIIString());
			}
			else
			{
			}

			::fprintf(m_pFile, "\n");
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			for(uint32 s=0; s<m_pMatrix->getDimensionSize(1); s++)
			{
				if(m_oTypeIdentifier==OV_TypeId_Signal)   ::fprintf(m_pFile, "%f", ((l_ui64StartTime+((s*(l_ui64EndTime-l_ui64StartTime))/op_pMatrix->getDimensionSize(1)))>>16)/65536.);
				if(m_oTypeIdentifier==OV_TypeId_Spectrum) ::fprintf(m_pFile, "%f", (l_ui64EndTime>>16)/65536.);
				for(uint32 c=0; c<m_pMatrix->getDimensionSize(0); c++)
				{
					::fprintf(m_pFile,
						"%s%f",
						m_sSeparator.toASCIIString(),
						op_pMatrix->getBuffer()[c*m_pMatrix->getDimensionSize(1)+s]);
				}

				if(m_bFirstBuffer)
				{
					if(m_oTypeIdentifier==OV_TypeId_Signal)
					{
						::fprintf(m_pFile,
							"%s%lli",
							m_sSeparator.toASCIIString(),
							(uint64)op_ui64SamplingFrequency);

						m_bFirstBuffer=false;
					}
					else if(m_oTypeIdentifier==OV_TypeId_Spectrum)
					{
						::fprintf(m_pFile,
							"%s%f",
							m_sSeparator.toASCIIString(),
							op_pMinMaxFrequencyBand->getBuffer()[s*2+0]);
						::fprintf(m_pFile,
							"%s%f",
							m_sSeparator.toASCIIString(),
							op_pMinMaxFrequencyBand->getBuffer()[s*2+1]);
					}
					else
					{
					}
				}
				else
				{
					if(m_oTypeIdentifier==OV_TypeId_Signal)
					{
						::fprintf(m_pFile,
							"%s",
							m_sSeparator.toASCIIString());
					}
					else if(m_oTypeIdentifier==OV_TypeId_Spectrum)
					{
						::fprintf(m_pFile,
							"%s",
							m_sSeparator.toASCIIString());
						::fprintf(m_pFile,
							"%s",
							m_sSeparator.toASCIIString());
					}
					else
					{
					}
				}

				::fprintf(m_pFile, "\n");
			}

			m_bFirstBuffer=false;
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
		}
		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	return true;
}

boolean CBoxAlgorithmCSVFileWriter::process_stimulation(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		m_pStreamDecoder->process();
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			::fprintf(m_pFile,
				"Time (s)%sIdentifier%sDuration\n",
				m_sSeparator.toASCIIString(),
				m_sSeparator.toASCIIString());
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			for(uint32 j=0; j<op_pStimulationSet->getStimulationCount(); j++)
			{
				::fprintf(m_pFile,
					"%f%s%llu%s%f\n",
					(op_pStimulationSet->getStimulationDate(j)>>16)/65536.0,
					m_sSeparator.toASCIIString(),
					op_pStimulationSet->getStimulationIdentifier(j),
					m_sSeparator.toASCIIString(),
					(op_pStimulationSet->getStimulationDuration(j)>>16)/65536.0);
			}
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
		}
		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
