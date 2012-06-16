#include "ovpCAlgorithmBrainampFileReader.h"

#include <system/Memory.h>

#include <sstream>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

// using namespace std;

boolean CAlgorithmBrainampFileReader::initialize(void)
{
	ip_sFilename.initialize(getInputParameter(OVP_Algorithm_BrainampFileReader_InputParameterId_Filename));
	ip_f64EpochDuration.initialize(getInputParameter(OVP_Algorithm_BrainampFileReader_InputParameterId_EpochDuration));
	ip_ui64SeekTime.initialize(getInputParameter(OVP_Algorithm_BrainampFileReader_InputParameterId_SeekTime));

	op_ui64CurrentStartTime.initialize(getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_CurrentStartTime));
	op_ui64CurrentEndTime.initialize(getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_CurrentEndTime));
	op_ui64SamplingRate.initialize(getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_SamplingRate));
	op_pSignalMatrix.initialize(getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_SignalMatrix));
	op_pStimulations.initialize(getOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_Stimulations));

	m_pBuffer=NULL;

	return true;
}

boolean CAlgorithmBrainampFileReader::uninitialize(void)
{
	op_pStimulations.uninitialize();
	op_pSignalMatrix.uninitialize();
	op_ui64SamplingRate.uninitialize();
	op_ui64CurrentEndTime.uninitialize();
	op_ui64CurrentStartTime.uninitialize();

	ip_ui64SeekTime.uninitialize();
	ip_f64EpochDuration.uninitialize();
	ip_sFilename.uninitialize();

	delete [] m_pBuffer;
	m_pBuffer=NULL;

	return true;
}

boolean CAlgorithmBrainampFileReader::process(void)
{
	if(this->isInputTriggerActive(OVP_Algorithm_BrainampFileReader_InputTriggerId_Open))
	{
		std::string l_sMarkerFilename;
		std::string l_sDataFilename;
		uint64 l_ui64SamplingInterval;

		m_ui32BinaryFormat=BinaryFormat_Integer16;
		m_ui32ChannelCount=0;
		m_ui64SamplingInterval=0;
		m_ui64StartSampleIndex=0;
		m_ui64EndSampleIndex=0;
		m_ui64SampleCountPerBuffer=0;
		m_vChannelScale.clear();
		delete [] m_pBuffer;
		m_pBuffer=NULL;
		m_ui32Endianness=Endianness_LittleEndian;

		m_oHeaderFile.open(ip_sFilename->toASCIIString(), std::ios::binary);
		if(!m_oHeaderFile.good())
		{
			getLogManager() << LogLevel_Error << "Could not open file [" << *ip_sFilename << "]\n";
		}
		else
		{
			getLogManager() << LogLevel_Trace << "Opening " << *ip_sFilename << " succeeded\n";

			op_pSignalMatrix->setDimensionCount(2);

			uint32 l_ui32ChannelIndex=0;
			uint32 l_ui32Status=Status_Nothing;
			do
			{
				std::string l_sWhat;
				std::getline(m_oHeaderFile, l_sWhat, '\n');
				getLogManager() << LogLevel_Debug << CString(l_sWhat.c_str()) << "\n";

				// optionally removes ending cariage return for windows / linux compatibility
				if(l_sWhat.length()!=0)
				{
					if(l_sWhat[l_sWhat.length()-1]=='\r')
					{
						l_sWhat.erase(l_sWhat.length()-1, 1);
					}
				}

				if(l_sWhat.length()!=0)
				{
					std::string::size_type l_iEqualPosition;
					if(l_sWhat[0] == ';') // comments
					{
					}
					else if(l_sWhat.length() > 2 && l_sWhat[0]=='[') // section start
					{
						std::string l_sSectionName;
						l_sSectionName.assign(l_sWhat, 1, l_sWhat.length()-2);
						if(l_sSectionName=="Common Infos")
						{
							getLogManager() << LogLevel_Trace << "Found section " << CString(l_sWhat.c_str()) << "\n";
							l_ui32Status=Status_CommonInfos;
						}
						else if(l_sSectionName=="Binary Infos")
						{
							getLogManager() << LogLevel_Trace << "Found section " << CString(l_sWhat.c_str()) << "\n";
							l_ui32Status=Status_BinrayInfos;
						}
						else if(l_sSectionName=="Channel Infos")
						{
							getLogManager() << LogLevel_Trace << "Found section " << CString(l_sWhat.c_str()) << "\n";
							l_ui32Status=Status_ChannelInfos;
						}
						else if(l_sSectionName=="Comment")
						{
							getLogManager() << LogLevel_Trace << "Found section " << CString(l_sWhat.c_str()) << "\n";
							l_ui32Status=Status_Comment;
						}
						else
						{
							getLogManager() << LogLevel_Warning << "{" << CString(l_sWhat.c_str()) << "} looked like a new section in the header file but is not know of this parser. Therefore anything after this line will be skipped until a new section is found\n";
							l_ui32Status=Status_Nothing;
						}
					}
					else if((l_iEqualPosition=l_sWhat.find("="))!=std::string::npos && l_ui32Status!=Status_Comment) // Option value
					{
						std::string l_sOptionName;
						std::string l_sOptionValue;
						l_sOptionName.assign(l_sWhat, 0, l_iEqualPosition);
						l_sOptionValue.assign(l_sWhat, l_iEqualPosition+1, l_sWhat.length()-l_iEqualPosition-1);

						getLogManager() << LogLevel_Trace << "| Found option " << CString(l_sOptionName.c_str()) << " with value " << CString(l_sOptionValue.c_str()) << "\n";

						switch(l_ui32Status)
						{
							case Status_CommonInfos:
								if(l_sOptionName=="DataFormat")
								{
									if(l_sOptionValue!="BINARY")
									{
										getLogManager() << LogLevel_ImportantWarning << "Only binary data is supported\n";
									}
								}
								else if(l_sOptionName=="DataOrientation")
								{
									if(l_sOptionValue!="MULTIPLEXED")
									{
										getLogManager() << LogLevel_ImportantWarning << "Only multiplexed data is supported\n";
									}
								}
								else if(l_sOptionName=="DataType")
								{
									if(l_sOptionValue!="TIMEDOMAIN")
									{
										getLogManager() << LogLevel_ImportantWarning << "Only time domain data is supported\n";
									}
								}
								else if(l_sOptionName=="Codepage")
								{
									if(l_sOptionValue!="ANSI")
									{
										getLogManager() << LogLevel_Warning << "Header specifies code page as " << CString(l_sOptionValue.c_str()) << " but it will be forced back to " << CString("ANSI") << "\n";
									}
								}
								else if(l_sOptionName=="DataFile")
								{
									l_sDataFilename=l_sOptionValue;
								}
								else if(l_sOptionName=="MarkerFile")
								{
									l_sMarkerFilename=l_sOptionValue;
								}
								else if(l_sOptionName=="NumberOfChannels")
								{
									m_ui32ChannelCount=atoi(l_sOptionValue.c_str());
									m_vChannelScale.clear();
									m_vChannelScale.resize(m_ui32ChannelCount, 1);
									op_pSignalMatrix->setDimensionSize(0, m_ui32ChannelCount);
								}
								else if(l_sOptionName=="SamplingInterval")
								{
									l_ui64SamplingInterval=atoi(l_sOptionValue.c_str());

									m_ui64SampleCountPerBuffer=((int64)((ip_f64EpochDuration*1000000.)/l_ui64SamplingInterval)); // $$$ Casted in (int64) because of Ubuntu 7.10 crash !
									op_pSignalMatrix->setDimensionSize(1, (uint32)m_ui64SampleCountPerBuffer);
									op_ui64SamplingRate=1000000/l_ui64SamplingInterval;

									// TODO warn if approximated sampling rate
									getLogManager() << LogLevel_Trace << "| -> Calculated sampling frequency " << op_ui64SamplingRate << "Hz\n";

									m_ui64SamplingInterval=(l_ui64SamplingInterval<<32)/1000000;
								}
								else
								{
									getLogManager() << LogLevel_Warning << "Skipped option " << CString(l_sOptionName.c_str()) << " with value " << CString(l_sOptionValue.c_str()) << "\n";
								}
								break;

							case Status_BinrayInfos:
								if(l_sOptionName=="BinaryFormat")
								{
									if(l_sOptionValue=="INT_16")
									{
										m_ui32BinaryFormat=BinaryFormat_Integer16;
									}
									else if(l_sOptionValue=="UINT_16")
									{
										m_ui32BinaryFormat=BinaryFormat_UnsignedInteger16;
									}
									else if(l_sOptionValue=="FLOAT_32")
									{
										m_ui32BinaryFormat=BinaryFormat_Float32;
									}
									else
									{
										m_ui32BinaryFormat=BinaryFormat_Integer16;
										getLogManager() << LogLevel_ImportantWarning << "Unsupported binary format option value \"" << CString(l_sOptionValue.c_str()) << "\"... Switched back to 16 bits integer (default)\n";
									}
								}
								else if(l_sOptionName=="UseBigEndianOrder")
								{
									if(l_sOptionValue=="YES")
									{
										m_ui32Endianness=Endianness_BigEndian;
									}
									else if(l_sOptionValue=="NO")
									{
										m_ui32Endianness=Endianness_LittleEndian;
									}
									else
									{
										m_ui32Endianness=Endianness_LittleEndian;
										getLogManager() << LogLevel_ImportantWarning << "Unsupported use big endian order option value \"" << CString(l_sOptionValue.c_str()) << "\"... Switched back to little endian (default)\n";
									}
								}
								else
								{
									getLogManager() << LogLevel_Warning << "Skipped option " << CString(l_sOptionName.c_str()) << " with value " << CString(l_sOptionValue.c_str()) << "\n";
								}
								break;

							case Status_ChannelInfos:
								{
									std::stringstream l_sStringStream(l_sOptionValue);
									std::string l_sChannelName;
									std::string l_sReferenceChannelName;
									std::string l_sResolutionInUnit;
									std::string l_sUnitName;

									std::getline(l_sStringStream, l_sChannelName, ',');
									std::getline(l_sStringStream, l_sReferenceChannelName, ',');
									std::getline(l_sStringStream, l_sResolutionInUnit, ',');
									std::getline(l_sStringStream, l_sUnitName, ',');

									op_pSignalMatrix->setDimensionLabel(0, l_ui32ChannelIndex, l_sChannelName.c_str());
									m_vChannelScale[l_ui32ChannelIndex]=atof(l_sResolutionInUnit.c_str());
									l_ui32ChannelIndex++;
								}
								break;
						}
					}
				}
			}
			while(m_oHeaderFile.good());

			// Changing file location
			std::string l_sFullPath=ip_sFilename->toASCIIString();
			std::string l_sPath;
			std::string::size_type l_iSlashLocation=l_sFullPath.rfind("/");
			if(l_iSlashLocation!=std::string::npos)
			{
				l_sPath.assign(l_sFullPath, 0, l_iSlashLocation+1);
				l_sMarkerFilename=l_sPath+l_sMarkerFilename;
				l_sDataFilename=l_sPath+l_sDataFilename;
			}

			// Opens data file
			m_oDataFile.open(l_sDataFilename.c_str(), std::ios::binary);
			if(!m_oDataFile.good())
			{
				getLogManager() << LogLevel_Error << "Could not open file [" << CString(l_sDataFilename.c_str()) << "]\n";
			}
			else
			{
				getLogManager() << LogLevel_Trace << "Opening " << CString(l_sDataFilename.c_str()) << " succeeded\n";
			}

			// Opens marker file
			m_oMarkerFile.open(l_sMarkerFilename.c_str(), std::ios::binary);
			if(!m_oMarkerFile.good())
			{
				getLogManager() << LogLevel_Error << "Could not open file [" << CString(l_sMarkerFilename.c_str()) << "]\n";
			}
			else
			{
				getLogManager() << LogLevel_Trace << "Opening " << CString(l_sMarkerFilename.c_str()) << " succeeded\n";

				uint32 l_ui32Status=Status_Nothing;
				do
				{
					std::string l_sWhat;
					std::getline(m_oMarkerFile, l_sWhat, '\n');
					getLogManager() << LogLevel_Debug << CString(l_sWhat.c_str()) << "\n";

					// optionnaly removes ending cariage return for windows / linux compatibility
					if(l_sWhat.length()!=0)
					{
						if(l_sWhat[l_sWhat.length()-1]=='\r')
						{
							l_sWhat.erase(l_sWhat.length()-1, 1);
						}
					}

					if(l_sWhat.length()!=0)
					{
						std::string::size_type l_iEqualPosition;
						if(l_sWhat[0] == ';') // comments
						{
						}
						else if(l_sWhat.length() > 2 && l_sWhat[0]=='[') // section start
						{
							std::string l_sSectionName;
							l_sSectionName.assign(l_sWhat, 1, l_sWhat.length()-2);
							if(l_sSectionName=="Common Infos")
							{
								getLogManager() << LogLevel_Trace << "Found section " << CString(l_sWhat.c_str()) << "\n";
								l_ui32Status=Status_CommonInfos;
							}
							else if(l_sSectionName=="Marker Infos")
							{
								getLogManager() << LogLevel_Trace << "Found section " << CString(l_sWhat.c_str()) << "\n";
								l_ui32Status=Status_MarkerInfos;
							}
							else
							{
								getLogManager() << LogLevel_Warning << "{" << CString(l_sWhat.c_str()) << "} looked like a new section in the marker file but is not know of this parser. Therefore anything after this line will be skipped until a new section is found\n";
								l_ui32Status=Status_Nothing;
							}
						}
						else if((l_iEqualPosition=l_sWhat.find("="))!=std::string::npos && l_ui32Status!=Status_Comment) // Option value
						{
							std::string l_sOptionName;
							std::string l_sOptionValue;
							l_sOptionName.assign(l_sWhat, 0, l_iEqualPosition);
							l_sOptionValue.assign(l_sWhat, l_iEqualPosition+1, l_sWhat.length()-l_iEqualPosition-1);

							getLogManager() << LogLevel_Trace << "| Found option " << CString(l_sOptionName.c_str()) << " with value " << CString(l_sOptionValue.c_str()) << "\n";

							switch(l_ui32Status)
							{
								case Status_CommonInfos:
									break;

								case Status_MarkerInfos:
									{
										std::stringstream l_sStringStream(l_sOptionValue);
										std::string l_sMarkerType;
										std::string l_sMarkerDescription;
										std::string l_sMarkerPosition;
										std::string l_sMarkerDuration;
										std::string l_sMarkerChannelIndex;
										std::string l_sMarkerDate;

										std::getline(l_sStringStream, l_sMarkerType, ',');
										std::getline(l_sStringStream, l_sMarkerDescription, ',');
										std::getline(l_sStringStream, l_sMarkerPosition, ',');
										std::getline(l_sStringStream, l_sMarkerDuration, ',');
										std::getline(l_sStringStream, l_sMarkerChannelIndex, ',');
										std::getline(l_sStringStream, l_sMarkerDate, ',');

										getLogManager() << LogLevel_Trace << "| -> Found marker "
											<< CString(l_sMarkerType.c_str()) << ","
											<< CString(l_sMarkerDescription.c_str()) << ","
											<< CString(l_sMarkerPosition.c_str()) << ","
											<< CString(l_sMarkerDuration.c_str()) << ","
											<< CString(l_sMarkerChannelIndex.c_str()) << ","
											<< CString(l_sMarkerDate.c_str()) << "\n";

										if(l_sMarkerType=="Stimulus" && l_sMarkerDescription.length()>2)
										{
											if(l_sMarkerDescription[0]=='S' && l_sMarkerDescription[1]==' ')
											{
												if(l_sMarkerChannelIndex!="0")
												{
													getLogManager() << LogLevel_Warning << "Marker [" << CString(l_sMarkerType.c_str()) << ":" << CString(l_sMarkerDescription.c_str()) << "] is not marked on channel 0 and OpenViBE only supports global scope stimulations. Therefore it will this marker will be considered as global\n";
												}

												std::string l_sStimulationIdentifier;
												l_sStimulationIdentifier.assign(l_sMarkerDescription, 2, l_sWhat.length()-2);

												SStimulation l_oStimulation;
												l_oStimulation.m_ui64Identifier=OVTK_StimulationId_Label(::atoi(l_sStimulationIdentifier.c_str()));
												l_oStimulation.m_ui64StartIndex=::atoi(l_sMarkerPosition.c_str());
												l_oStimulation.m_ui64Duration=::atoi(l_sMarkerDuration.c_str());
												l_oStimulation.m_sName=l_sMarkerDescription;
												m_vStimulation.push_back(l_oStimulation);

												getLogManager() << LogLevel_Trace << "Found stimulation " << l_oStimulation.m_ui64Identifier << " at sample index [" << l_oStimulation.m_ui64StartIndex << ":" << l_oStimulation.m_ui64Duration << "]\n";
											}
										}
										else
										{
											getLogManager() << LogLevel_Warning << "Marker [" << CString(l_sMarkerType.c_str()) << ":" << CString(l_sMarkerDescription.c_str()) << "] is not supported. Therefore it will be ignored\n";
										}
									}
									break;
							}
						}
					}
				}
				while(m_oMarkerFile.good());
			}
		}
	}

	if(this->isInputTriggerActive(OVP_Algorithm_BrainampFileReader_InputTriggerId_Seek))
	{
		getLogManager() << LogLevel_ImportantWarning << "This has not been implemented yet\n";
	}

	if(this->isInputTriggerActive(OVP_Algorithm_BrainampFileReader_InputTriggerId_Next))
	{
		if(!m_pBuffer)
		{
			m_pBuffer=new uint8[op_pSignalMatrix->getBufferElementCount()*sizeof(int16)];
		}

		uint8* l_pFileBuffer=m_pBuffer;
		float64* l_pSignalMatrixBuffer=op_pSignalMatrix->getBuffer();

#define _do_it_with_type_(T) \
		{ \
			T l_tValue; \
			m_oDataFile.read((char*)l_pFileBuffer, op_pSignalMatrix->getBufferElementCount()*sizeof(T)); \
			boolean (*l_fpFileToHost)(const uint8*, T*); \
			if(m_ui32Endianness==Endianness_LittleEndian) \
			{ \
				l_fpFileToHost=System::Memory::littleEndianToHost; \
			} \
			else \
			{ \
				l_fpFileToHost=System::Memory::bigEndianToHost; \
			} \
			for(uint32 j=0; j<m_ui64SampleCountPerBuffer; j++) \
			{ \
				for(uint32 i=0; i<m_ui32ChannelCount; i++, l_pFileBuffer+=sizeof(T)) \
				{ \
					(*l_fpFileToHost)(l_pFileBuffer, &l_tValue); \
					l_pSignalMatrixBuffer[i*m_ui64SampleCountPerBuffer+j]=m_vChannelScale[i]*l_tValue; \
				} \
			} \
		}

		if(m_ui32BinaryFormat==BinaryFormat_Integer16)
		{
			_do_it_with_type_(int16);
		}
		if(m_ui32BinaryFormat==BinaryFormat_UnsignedInteger16)
		{
			_do_it_with_type_(uint16);
		}
		if(m_ui32BinaryFormat==BinaryFormat_Float32)
		{
			_do_it_with_type_(float32);
		}

		m_ui64StartSampleIndex=m_ui64EndSampleIndex;
		m_ui64EndSampleIndex+=m_ui64SampleCountPerBuffer;

		op_ui64CurrentStartTime=m_ui64StartSampleIndex*m_ui64SamplingInterval;
		op_ui64CurrentEndTime=m_ui64EndSampleIndex*m_ui64SamplingInterval;

		// find stimulations in this range
		uint64 l_ui64StimulationCount=0;
		std::vector < SStimulation >::const_iterator itStimulation;
		for(itStimulation=m_vStimulation.begin(); itStimulation!=m_vStimulation.end(); itStimulation++)
		{
			if(m_ui64StartSampleIndex <= itStimulation->m_ui64StartIndex && itStimulation->m_ui64StartIndex < m_ui64EndSampleIndex)
			{
				l_ui64StimulationCount++;
			}
		}

		op_pStimulations->setStimulationCount(l_ui64StimulationCount);
		l_ui64StimulationCount=0;
		for(itStimulation=m_vStimulation.begin(); itStimulation!=m_vStimulation.end(); itStimulation++)
		{
			if(m_ui64StartSampleIndex <= itStimulation->m_ui64StartIndex && itStimulation->m_ui64StartIndex < m_ui64EndSampleIndex)
			{
				op_pStimulations->setStimulationIdentifier(l_ui64StimulationCount, itStimulation->m_ui64Identifier);
				op_pStimulations->setStimulationDate(l_ui64StimulationCount, itStimulation->m_ui64StartIndex*m_ui64SamplingInterval);
				op_pStimulations->setStimulationDuration(l_ui64StimulationCount, itStimulation->m_ui64Duration*m_ui64SamplingInterval);
				l_ui64StimulationCount++;
			}
		}

		this->activateOutputTrigger(OVP_Algorithm_BrainampFileReader_OutputTriggerId_DataProduced, true);
	}

	if(this->isInputTriggerActive(OVP_Algorithm_BrainampFileReader_InputTriggerId_Close))
	{
		delete [] m_pBuffer;
		m_pBuffer=NULL;

		m_oMarkerFile.close();
		m_oDataFile.close();
		m_oHeaderFile.close();
	}

	return true;
}
