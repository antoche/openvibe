#include "ovpCBoxAlgorithmCSVFileReader.h"
#include <iostream>
#include <sstream>
#include <map>
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

namespace
{
	std::vector < std::string > split(const std::string& sString, const std::string c)
	{
		std::vector < std::string > l_vResult;
		std::string::size_type i=0;
		std::string::size_type j=0;
		while((j=sString.find(c,i))!=std::string::npos)
		{

			l_vResult.push_back(std::string(sString,i,j-i));
			i=j+c.size();
		}
		//the last element without the \n character
		l_vResult.push_back(std::string(sString,i,sString.size()-1-i));

		return l_vResult;
	}

	void clearMatrix(std::vector<std::vector<std::string> >& vMatrix)
	{
		for(uint32 i=0;i<vMatrix.size();i++)
		{
			vMatrix[i].clear();
		}
		vMatrix.clear();
	}
};

CBoxAlgorithmCSVFileReader::CBoxAlgorithmCSVFileReader(void)
	: m_pFile(NULL),
	m_ui64SamplingRate(0),
	m_fpRealProcess(NULL),
	m_bHeaderSent(false)
{
}

uint64 CBoxAlgorithmCSVFileReader::getClockFrequency(void)
{
	return 128LL<<32; // the box clock frequency
}

boolean CBoxAlgorithmCSVFileReader::initialize(void)
{
	this->getStaticBoxContext().getOutputType(0,m_oTypeIdentifier);

	m_sFilename= FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	CString l_sSeparator=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_sSeparator=l_sSeparator.toASCIIString();
	m_bNotUseTimer= FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_ui32SamplesPerBuffer=1;
	if(this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_ChannelLocalisation))
	{
		CString l_sParam;
		this->getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(3,l_sParam);
		m_ui32ChannelNumberPerBuffer=static_cast<uint32>(atoi((const char*)l_sParam));

	}
	else if(!this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Stimulations)
			&&!this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Spectrum))
	{
		CString l_sParam;
		this->getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(3,l_sParam);
		m_ui32SamplesPerBuffer=static_cast<uint32>(atoi((const char*)l_sParam));
	}
	//open file
	m_pFile=::fopen(m_sFilename.toASCIIString(), "rb");
	if(!m_pFile)
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Could not open file [" << m_sFilename << "]\n";
		return false;
	}

	//read the header
	char* l_pLine=new char[500];
	::fgets(l_pLine,500,m_pFile);
	m_vHeaderFile=split(std::string(l_pLine),m_sSeparator);
	m_ui32NbColumn=m_vHeaderFile.size();

	if(this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_ChannelLocalisation))
	{
		m_pAlgorithmEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_ChannelLocalisationStreamEncoder));
		m_pAlgorithmEncoder->initialize();
		op_pMemoryBuffer.initialize(m_pAlgorithmEncoder->getOutputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		ip_pMatrix.initialize(m_pAlgorithmEncoder->getInputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Matrix));
		ip_pDynamic.initialize(m_pAlgorithmEncoder->getInputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Dynamic));

		//number of column without the column contains the dynamic parameter
		//m_ui32NbColumn-=1;
		m_fpRealProcess=&CBoxAlgorithmCSVFileReader::process_channelLocalisation;
	}
	else if(this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_FeatureVector))
	{
		m_pAlgorithmEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
		m_pAlgorithmEncoder->initialize();
		op_pMemoryBuffer.initialize(m_pAlgorithmEncoder->getOutputParameter(OVP_GD_Algorithm_FeatureVectorStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		ip_pMatrix.initialize(m_pAlgorithmEncoder->getInputParameter(OVP_GD_Algorithm_FeatureVectorStreamEncoder_InputParameterId_Matrix));
		m_fpRealProcess=&CBoxAlgorithmCSVFileReader::process_featureVector;
	}
	else if(this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Spectrum))
	{
		m_pAlgorithmEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
		m_pAlgorithmEncoder->initialize();
		op_pMemoryBuffer.initialize(m_pAlgorithmEncoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		ip_pMinMaxFrequencyBands.initialize(m_pAlgorithmEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands));
		ip_pMatrix.initialize(m_pAlgorithmEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_Matrix));
		m_fpRealProcess=&CBoxAlgorithmCSVFileReader::process_spectrum;

		//number of column without columns contains min max frequency bands parameters
		m_ui32NbColumn-=2;
	}
	else if(this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Signal))
	{
		m_pAlgorithmEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
		m_pAlgorithmEncoder->initialize();
		op_pMemoryBuffer.initialize(m_pAlgorithmEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		ip_pMatrix.initialize(m_pAlgorithmEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
		ip_pSamplingRate.initialize(m_pAlgorithmEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));
		m_fpRealProcess=&CBoxAlgorithmCSVFileReader::process_signal;

		//find the sampling rate
		::fgets(l_pLine,500,m_pFile);
		m_ui64SamplingRate=atoi(split(std::string(l_pLine),m_sSeparator)[m_ui32NbColumn-1].c_str());

		::rewind(m_pFile);
		::fgets(l_pLine,500,m_pFile);

		//number of column without the column contains the sampling rate parameters
		m_ui32NbColumn-=1;
	}
	else if(this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_StreamedMatrix))
	{
		m_pAlgorithmEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
		m_pAlgorithmEncoder->initialize();
		op_pMemoryBuffer.initialize(m_pAlgorithmEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		ip_pMatrix.initialize(m_pAlgorithmEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
		m_fpRealProcess=&CBoxAlgorithmCSVFileReader::process_streamedMatrix;
	}
	else if(this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Stimulations))
	{
		m_pAlgorithmEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
		m_pAlgorithmEncoder->initialize();
		op_pMemoryBuffer.initialize(m_pAlgorithmEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		ip_pStimulationSet.initialize(m_pAlgorithmEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
		m_fpRealProcess=&CBoxAlgorithmCSVFileReader::process_stimulation;
	}
	else
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Invalid input type identifier " << this->getTypeManager().getTypeName(m_oTypeIdentifier) << "\n";
		return false;
	}
	getLogManager()<< LogLevel_Trace <<"number of column without parameters: "<<m_ui32NbColumn<<"\n";

	m_bUseCompression=false;
	delete[] l_pLine;
	getLogManager() << LogLevel_Trace << "use the file time: "<<(!m_bNotUseTimer)<<"\n";
	m_f64NextTime=0.;
	return true;
}

boolean CBoxAlgorithmCSVFileReader::uninitialize(void)
{
	if(m_pFile)
	{
		::fclose(m_pFile);
		m_pFile=NULL;

		m_pAlgorithmEncoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pAlgorithmEncoder);
		m_pAlgorithmEncoder=NULL;

		op_pMemoryBuffer.uninitialize();
		ip_pMatrix.uninitialize();
		ip_pDynamic.uninitialize();
		ip_pMinMaxFrequencyBands.uninitialize();
		ip_pSamplingRate.uninitialize();
		ip_pStimulationSet.uninitialize();
	}
	return true;
}

boolean CBoxAlgorithmCSVFileReader::processClock(IMessageClock& rMessageClock)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmCSVFileReader::process(void)
{

	//line buffer
	char* l_pLine=new char[500];
	bool l_bStimulationSend=false;
	float64 l_f64currentTime=(getPlayerContext().getCurrentTime()>>16)/65536.0;
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	//if no line was read, read the first data line.
	if(m_vLastLineSplit.size() == 0)
	{
		//next line
		uint32 l_ui32NbSamples=0;
		while(!feof(m_pFile) && l_ui32NbSamples<m_ui32SamplesPerBuffer && fgets(l_pLine, 500, m_pFile) != NULL )
		{
			m_vLastLineSplit=split(std::string(l_pLine),m_sSeparator);
			l_ui32NbSamples++;
			if(m_bNotUseTimer && getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Signal))
			{
				std::stringstream l_sNextTime;
				l_sNextTime<<m_f64NextTime;
				m_vLastLineSplit[0]=l_sNextTime.str();
				m_f64NextTime+=1./m_ui64SamplingRate;
			}

			if(!this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Stimulations)
					&&!this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Spectrum)
					&&!this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_ChannelLocalisation))
			{
				m_vDataMatrix.push_back(m_vLastLineSplit);
			}

		}

	}

	l_bStimulationSend=m_vLastLineSplit.size()!=0
			&& atof(m_vLastLineSplit[0].c_str())<l_f64currentTime;


	if(this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Stimulations)
			||this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_ChannelLocalisation)
			|| this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Spectrum))
	{
		while(m_vLastLineSplit.size()!=0 && atof(m_vLastLineSplit[0].c_str())<l_f64currentTime)
		{
			m_vDataMatrix.push_back(m_vLastLineSplit);

			l_bStimulationSend=true;

			if(!feof(m_pFile) && fgets(l_pLine, 500, m_pFile) != NULL)
			{
				m_vLastLineSplit=split(std::string(l_pLine),m_sSeparator);

			}
			else
			{
				m_vLastLineSplit.clear();
			}
		}
	}

	//convert data to the good output type

	if(l_bStimulationSend )
	{
		op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
		if(!(this->*m_fpRealProcess)())
		{
			return false;
		}
		if(!this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Spectrum)
				&&!this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_ChannelLocalisation))
		{
			//get the date of the first bloc samples to send
			uint64 l_ui64StartTime=atof(m_vDataMatrix[0][0].c_str())*65536.0;
			l_ui64StartTime=l_ui64StartTime<<16;
			l_rDynamicBoxContext.markOutputAsReadyToSend(0,l_ui64StartTime,this->getPlayerContext().getCurrentTime());

		}

		//for the stimulation the line contains in m_vLastLineSplit isn't process.
		if(!this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Stimulations)
				&&!this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_Spectrum)
				&&!this->getTypeManager().isDerivedFromStream(m_oTypeIdentifier,OV_TypeId_ChannelLocalisation))
		{
			m_vLastLineSplit.clear();
		}

		//clear the Data Matrix.
		clearMatrix(m_vDataMatrix);
	}
	delete[] l_pLine;
	return true;

}

boolean CBoxAlgorithmCSVFileReader::process_streamedMatrix(void)
{
	CMatrix l_oMatrix;
	ip_pMatrix=&l_oMatrix;

	convertVectorDataToMatrix(ip_pMatrix);

	if(!m_bHeaderSent)
	{
		for(uint32 i=1;i<m_ui32NbColumn;i++)
		{
			ip_pMatrix->setDimensionLabel(0,i-1,m_vHeaderFile[i].c_str());
		}

		// Produces header
		m_pAlgorithmEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);

		// Turn flag off
		m_bHeaderSent=true;

	}
	m_pAlgorithmEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);

	return true;
}

OpenViBE::boolean CBoxAlgorithmCSVFileReader::process_stimulation(void){

	CStimulationSet l_oStimulationSet;
	ip_pStimulationSet=&l_oStimulationSet;

	for(uint32 i=0;i<m_vDataMatrix.size();i++)
	{
		//stimulation date
		uint64 l_ui64StimulationDate=atof(m_vDataMatrix[i][0].c_str())*65536.0;
		l_ui64StimulationDate=l_ui64StimulationDate<<16;

		//stimulation indices
		uint64 l_ui64Stimulation=atof(m_vDataMatrix[i][1].c_str());

		//stimulation duration
		uint64 l_ui64StimulationDuration=atof(m_vDataMatrix[i][2].c_str())*65536.0;
		l_ui64StimulationDuration=l_ui64StimulationDuration<<16;

		getLogManager()<<LogLevel_Trace<<"  Stimulation "<<l_ui64Stimulation<<" start at time: "<<l_ui64StimulationDate<<"("<<((l_ui64StimulationDate>>16)/65536.0)<<" s)"<<" end during:"<<l_ui64StimulationDuration<<"("<<((l_ui64StimulationDuration>>16)/65536.0)<<" s)\n";

		l_oStimulationSet.appendStimulation(l_ui64Stimulation,l_ui64StimulationDate,l_ui64StimulationDuration);

	}
	if(!m_bHeaderSent)
	{
		// Produces header
		m_pAlgorithmEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);

		// Turn flag off
		m_bHeaderSent=true;
	}

	m_pAlgorithmEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);

	return true;
}
OpenViBE::boolean CBoxAlgorithmCSVFileReader::process_signal(void)
{
	CMatrix l_oMatrix;
	ip_pMatrix=&l_oMatrix;


	convertVectorDataToMatrix(&l_oMatrix);


	if(!m_bHeaderSent)
	{
		ip_pSamplingRate=m_ui64SamplingRate;
		for(uint32 i=1;i<m_ui32NbColumn;i++)
		{
			l_oMatrix.setDimensionLabel(0,i-1,m_vHeaderFile[i].c_str());
		}
		m_pAlgorithmEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);

		m_bHeaderSent=true;

		getLogManager()<<LogLevel_Trace<<"Sampling Rate:"<<ip_pSamplingRate<<"\n";

	}

	m_pAlgorithmEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);

	return true;
}
OpenViBE::boolean CBoxAlgorithmCSVFileReader::process_channelLocalisation(void)
{
	CMatrix l_oMatrix;
	ip_pMatrix=&l_oMatrix;
	op_pMemoryBuffer=this->getDynamicBoxContext().getOutputChunk(0);

	std::vector<std::vector<std::string> > l_vChannelBloc;
	for(uint32 i=0;i<m_vDataMatrix.size();i++)
	{
		l_vChannelBloc.push_back(m_vDataMatrix[i]);
	}

	//clear matrix
	clearMatrix(m_vDataMatrix);

	for(uint64 i=0;i<l_vChannelBloc.size();i++)
	{
		m_vDataMatrix.push_back(l_vChannelBloc[i]);

		//send the current bloc if the next data hasn't the same date
		if(i>=l_vChannelBloc.size()-1 || l_vChannelBloc[i+1][0]!=m_vDataMatrix[0][0])
		{
			convertVectorDataToMatrix(&l_oMatrix);

			//Header
			if(!m_bHeaderSent)
			{
				ip_pDynamic=false;//atoi(m_vDataMatrix[0][m_ui32NbColumn].c_str());
				for(uint32 i=1;i<m_ui32NbColumn;i++)
				{
					l_oMatrix.setDimensionLabel(0,i-1,m_vHeaderFile[i].c_str());
				}
				m_pAlgorithmEncoder->process(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeHeader);

				m_bHeaderSent=true;

			}

			m_pAlgorithmEncoder->process(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeBuffer);
			uint64 l_ui64Date=atof(m_vDataMatrix[0][0].c_str())*65536.0;
			l_ui64Date=l_ui64Date<<16;
			this->getDynamicBoxContext().markOutputAsReadyToSend(0,l_ui64Date,l_ui64Date);

			//clear matrix
			clearMatrix(m_vDataMatrix);
		}

	}

	//clear matrix
	clearMatrix(l_vChannelBloc);

	return true;
}

OpenViBE::boolean CBoxAlgorithmCSVFileReader::process_featureVector(void)
{
	CMatrix l_oMatrix;
	ip_pMatrix=&l_oMatrix;

	convertVectorDataToMatrix(&l_oMatrix);

	if(!m_bHeaderSent)
	{
		for(uint32 i=1;i<m_ui32NbColumn;i++)
		{
			l_oMatrix.setDimensionLabel(0,i-1,m_vHeaderFile[i].c_str());
		}
		m_pAlgorithmEncoder->process(OVP_GD_Algorithm_FeatureVectorStreamEncoder_InputTriggerId_EncodeHeader);

		m_bHeaderSent=true;

	}
	m_pAlgorithmEncoder->process(OVP_GD_Algorithm_FeatureVectorStreamEncoder_InputTriggerId_EncodeBuffer);

	return true;
}
OpenViBE::boolean CBoxAlgorithmCSVFileReader::process_spectrum(void)
{
	CMatrix l_oMinMaxFrequencyBands;
	CMatrix l_oMatrix;
	ip_pMinMaxFrequencyBands=&l_oMinMaxFrequencyBands;
	ip_pMatrix=&l_oMatrix;
	op_pMemoryBuffer=this->getDynamicBoxContext().getOutputChunk(0);

	std::vector<std::vector<std::string> > l_vSpectrumBloc;
	for(uint32 i=0;i<m_vDataMatrix.size();i++)
	{
		l_vSpectrumBloc.push_back(m_vDataMatrix[i]);
	}

	//clear matrix
	clearMatrix(m_vDataMatrix);

	for(uint64 i=0;i<l_vSpectrumBloc.size();i++)
	{
		m_vDataMatrix.push_back(l_vSpectrumBloc[i]);
		//send the current bloc if the next data hasn't the same date
		if(i>=l_vSpectrumBloc.size()-1 || l_vSpectrumBloc[i+1][0]!=m_vDataMatrix[0][0])
		{
			convertVectorDataToMatrix(&l_oMatrix);

			//Header
			if(!m_bHeaderSent)
			{
				for(uint32 i=1;i<m_ui32NbColumn;i++)
				{
					l_oMatrix.setDimensionLabel(0,i-1,m_vHeaderFile[i].c_str());
				}
				l_oMinMaxFrequencyBands.setDimensionCount(3);
				l_oMinMaxFrequencyBands.setDimensionSize(0,m_ui32NbColumn-1);
				l_oMinMaxFrequencyBands.setDimensionSize(1,m_vDataMatrix.size());
				l_oMinMaxFrequencyBands.setDimensionSize(2,2);
				for(uint32 j=0;j<m_ui32NbColumn-1;j++)
				{
					for(uint32 i=0;i<m_vDataMatrix.size();i++)
					{
						l_oMinMaxFrequencyBands.getBuffer()[j*m_vDataMatrix.size()*2+i*2]=atof(m_vDataMatrix[i][m_ui32NbColumn].c_str());
						l_oMinMaxFrequencyBands.getBuffer()[j*m_vDataMatrix.size()*2+i*2+1]=atof(m_vDataMatrix[i][m_ui32NbColumn+1].c_str());
						std::stringstream l_sLabel;
						l_sLabel<<(16*i)<<"-"<<(16*(i+1));
						l_oMinMaxFrequencyBands.setDimensionLabel(1,i,l_sLabel.str().c_str());
					}
				}

				m_pAlgorithmEncoder->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeHeader);

				m_bHeaderSent=true;

			}

			m_pAlgorithmEncoder->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeBuffer);
			uint64 l_ui64Date=atof(m_vDataMatrix[0][0].c_str())*65536.0;
			l_ui64Date=l_ui64Date<<16;
			this->getDynamicBoxContext().markOutputAsReadyToSend(0,l_ui64Date-1,l_ui64Date);

			//clear matrix
			clearMatrix(m_vDataMatrix);
		}

	}

	//clear matrix
	clearMatrix(l_vSpectrumBloc);
	return true;
}

void CBoxAlgorithmCSVFileReader::convertVectorDataToMatrix(IMatrix* matrix)
{
		matrix->setDimensionCount(2);
		matrix->setDimensionSize(0,m_ui32NbColumn-1);
		matrix->setDimensionSize(1,m_vDataMatrix.size());

		std::stringstream  l_sMatrix;
		for(uint32 i=0;i<m_vDataMatrix.size();i++)
		{
			l_sMatrix<<"at time ("<<m_vDataMatrix[i][0].c_str()<<"):";
			for(uint32 j=0;j<m_ui32NbColumn-1;j++)
			{
				matrix->getBuffer()[j*m_vDataMatrix.size()+i]=atof(m_vDataMatrix[i][j+1].c_str());
				l_sMatrix<<matrix->getBuffer()[j*m_vDataMatrix.size()+i]<<";";
			}
			l_sMatrix<<"\n";
		}
		getLogManager()<<LogLevel_Debug<<"Matrix:\n"<<l_sMatrix.str().c_str();
}
