#include "ovpCAlgorithmOVMatrixFileReader.h"

#include <system/Memory.h>

#include <sstream>
#include <locale>
#if defined OVP_OS_Windows
#include <cerrno>
#endif

#include <cstring>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

boolean CAlgorithmOVMatrixFileReader::initialize(void)
{
	ip_sFilename.initialize(getInputParameter(OVP_Algorithm_OVMatrixFileReader_InputParameterId_Filename));

	op_pMatrix.initialize(getOutputParameter(OVP_Algorithm_OVMatrixFileReader_OutputParameterId_Matrix));

	return true;
}

boolean CAlgorithmOVMatrixFileReader::uninitialize(void)
{
	op_pMatrix.uninitialize();

	ip_sFilename.uninitialize();

	return true;
}

boolean CAlgorithmOVMatrixFileReader::process(void)
{
	//if(this->isInputTriggerActive(OVP_Algorithm_OVMatrixFileReader_InputTriggerId_Open))
	{
		m_oDataFile.open(ip_sFilename->toASCIIString(), std::ios_base::in);
		if(!m_oDataFile.good())
		{
			getLogManager() << LogLevel_Warning << "Opening " << *ip_sFilename << " failed\n";
			return false;
		}

		getLogManager() << LogLevel_Trace << "Opening " << *ip_sFilename << " succeeded\n";

		//determine matrix dimension count and size
		if(parseFile(false) == false)
		{
			getLogManager() << LogLevel_Warning << "Parsing " << *ip_sFilename << " failed\n";
			m_oDataFile.close();
			return false;
		}
	}

	//if(this->isInputTriggerActive(OVP_Algorithm_OVMatrixFileReader_InputTriggerId_Load))
	{
		//go back to start of file
		m_oDataFile.seekg(0, std::ios_base::beg);

		//store labels and values
		if(parseFile(true) == false)
		{
			getLogManager() << LogLevel_Warning << "Parsing " << *ip_sFilename << " failed\n";
			m_oDataFile.close();
			return false;
		}

		this->activateOutputTrigger(OVP_Algorithm_OVMatrixFileReader_OutputTriggerId_DataProduced, true);
	}

	//if(this->isInputTriggerActive(OVP_Algorithm_OVMatrixFileReader_InputTriggerId_Close))
	{
		getLogManager() << LogLevel_Trace << "Parsing " << *ip_sFilename << " succeeded\n";
		m_oDataFile.close();
	}

	return true;
}

boolean CAlgorithmOVMatrixFileReader::parseFile(boolean bStoreData)
{
	std::locale l_oLocale("C");
	//current string to parse
	std::string l_sWhat;
	//current parsing status
	uint32 l_ui32Status=Status_Nothing;
	//current element index (incremented every time a value is stored in matrix)
	uint32 l_ui32CurElementIndex = 0;
	//number of dimensions
	uint32 l_ui32DimensionCount = op_pMatrix->getDimensionCount();
	//current dimension index
	uint32 l_ui32CurDimensionIndex = (uint32)-1;
	//vector keeping track of dimension sizes
	std::vector<uint32> l_vDimensionSize;
	//vector keeping track of number of values found in each dimension
	std::vector<uint32> l_vValuesCount(op_pMatrix->getDimensionCount(), 0);
	//current quote-delimited string
	std::string l_sCurString;

	do
	{
		//read current line
		std::getline(m_oDataFile, l_sWhat, '\n');

		//is line empty?
		if(l_sWhat.length()==0)
		{
			//skip it
			continue;
		}

		//output line to be parsed in debug level
		getLogManager() << LogLevel_Debug << CString(l_sWhat.c_str()) << "\n";

		//remove ending carriage return (if any) for windows / linux compatibility
		if(l_sWhat[l_sWhat.length()-1]=='\r')
		{
			l_sWhat.erase(l_sWhat.length()-1, 1);
		}

		//start parsing current line
		std::string::iterator l_oIt = l_sWhat.begin();

		//parse current line
		while(l_oIt != l_sWhat.end())
		{
			switch(l_ui32Status)
			{
				//initial parsing status
				case Status_Nothing:

					//comments starting
					if(*l_oIt == '#')
					{
						//ignore rest of line by skipping to last character
						l_oIt = l_sWhat.end()-1;
					}
					//header starting
					else if(*l_oIt == '[')
					{
						//update status
						l_ui32Status = Status_ParsingHeader;
					}
					else if(std::isspace(*l_oIt, l_oLocale) == false)
					{
						getLogManager() << LogLevel_Trace << "Unexpected character found on line " << l_sWhat.c_str() << ", parsing aborted\n";
						return false;
					}
					break;

				//parse header
				case Status_ParsingHeader:

					//comments starting
					if(*l_oIt == '#')
					{
						//ignore rest of line by skipping to last character
						l_oIt = l_sWhat.end()-1;
					}
					//new dimension opened
					else if(*l_oIt == '[')
					{
						//increment dimension count
						l_vDimensionSize.resize(l_vDimensionSize.size()+1);

						//update current dimension index
						l_ui32CurDimensionIndex++;

						//update status
						l_ui32Status = Status_ParsingHeaderDimension;
					}
					//finished parsing header
					else if(*l_oIt == ']')
					{
						if(bStoreData == false)
						{
							//ensure at least one dimension was found
							if(l_vDimensionSize.size() == 0)
							{
								getLogManager() << LogLevel_Trace << "End of header section reached, found 0 dimensions : parsing aborted\n";
								return false;
							}

							//resize matrix
							op_pMatrix->setDimensionCount(l_vDimensionSize.size());
							for(size_t i=0; i<l_vDimensionSize.size(); i++)
							{
								op_pMatrix->setDimensionSize(i, l_vDimensionSize[i]);
							}

							//dump dimension count and size
							char l_pBuf[1024]={'\0'};
							for(size_t i=0; i<l_vDimensionSize.size(); i++)
							{
								if(i>0)
								{
									strcat(l_pBuf, ", ");
									sprintf(l_pBuf+strlen(l_pBuf), "%d", l_vDimensionSize[i]);
								}
								else
								{
									sprintf(l_pBuf+strlen(l_pBuf), "%d", l_vDimensionSize[i]);
								}
							}
							getLogManager() << LogLevel_Trace
								<< "End of header section reached, found " << (uint32)l_vDimensionSize.size() << " dimensions of size ["
								<< CString(l_pBuf) << "]\n";

							//stop parsing
							return true;
						}
						else
						{
							//reset current dimension index
							l_ui32CurDimensionIndex = (uint32)-1;

							//update status
							l_ui32Status = Status_ParsingBuffer;
						}
					}
					else if(std::isspace(*l_oIt, l_oLocale) == false)
					{
						getLogManager() << LogLevel_Trace << "Unexpected character found on line " << CString(l_sWhat.c_str()) << ", parsing aborted\n";
						return false;
					}
					break;

				case Status_ParsingHeaderDimension:

					//comments starting
					if(*l_oIt == '#')
					{
						//ignore rest of line by skipping to last character
						l_oIt = l_sWhat.end()-1;
					}
					//new label found
					else if(*l_oIt == '"')
					{
						//new element found in current dimension
						l_vDimensionSize[l_ui32CurDimensionIndex]++;

						//update status
						l_ui32Status = Status_ParsingHeaderLabel;
					}
					//finished parsing current dimension header
					else if(*l_oIt == ']')
					{
						//update status
						l_ui32Status = Status_ParsingHeader;
					}
					else if(std::isspace(*l_oIt, l_oLocale) == false)
					{
						getLogManager() << LogLevel_Trace << "Unexpected character found on line " << CString(l_sWhat.c_str()) << ", parsing aborted\n";
						return false;
					}
					break;

				//look for end of label (first '"' char not preceded by the '\' escape char)
				case Status_ParsingHeaderLabel:

					//found '"' char not preceded by escape char : end of label reached
					if(*l_oIt == '"' && *(l_oIt-1) != '\\')
					{
						if(bStoreData == true)
						{
							//store label in matrix
							op_pMatrix->setDimensionLabel(
								l_ui32CurDimensionIndex, //dimension index
								l_vDimensionSize[l_ui32CurDimensionIndex]-1, //label index
								l_sCurString.c_str()); //label

							//clear current string
							l_sCurString.erase();
						}

						//update status
						l_ui32Status = Status_ParsingHeaderDimension;
					}
					//otherwise, keep parsing current label
					else
					{
						if(bStoreData == true)
						{
							l_sCurString.append(1, *l_oIt);
						}
					}
					break;

				case Status_ParsingBuffer:

					//comments starting
					if(*l_oIt == '#')
					{
						//ignore rest of line by skipping to last character
						l_oIt = l_sWhat.end()-1;
					}
					//going down one dimension
					else if(*l_oIt == '[')
					{
						//update dimension index
						l_ui32CurDimensionIndex++;

						//ensure dimension count remains in allocated range
						if(l_ui32CurDimensionIndex == l_ui32DimensionCount)
						{
							getLogManager() << LogLevel_Trace << "Exceeded expected number of dimensions while parsing values, parsing aborted\n";
							return false;
						}

						//ensure values count remains in allocated range
						if(l_vValuesCount[l_ui32CurDimensionIndex] == op_pMatrix->getDimensionSize(l_ui32CurDimensionIndex))
						{
							getLogManager() << LogLevel_Trace << "Exceeded expected number of values for dimension " << l_ui32CurDimensionIndex << ", parsing aborted\n";
							return false;
						}

						//increment values count for current dimension, if it is not the innermost
						if(l_ui32CurDimensionIndex < l_ui32DimensionCount - 1)
						{
							l_vValuesCount[l_ui32CurDimensionIndex]++;
						}
					}
					//going up one dimension
					else if(*l_oIt == ']')
					{
						//if we are not in innermost dimension
						if(l_ui32CurDimensionIndex < l_ui32DimensionCount-1)
						{
							//ensure the right number of values was parsed in lower dimension
							if(l_vValuesCount[l_ui32CurDimensionIndex+1] != op_pMatrix->getDimensionSize(l_ui32CurDimensionIndex+1))
							{
								getLogManager() << LogLevel_Trace
									<< "Found " << l_vValuesCount[l_ui32CurDimensionIndex+1] << " values in dimension "
									<< l_ui32CurDimensionIndex+1 << ", expected " << op_pMatrix->getDimensionSize(l_ui32CurDimensionIndex+1) << ", parsing aborted\n";
								return false;
							}
							//reset values count of lower dimension to 0
							l_vValuesCount[l_ui32CurDimensionIndex+1] = 0;
						}
						//ensure dimension count is correct
						else if(l_ui32CurDimensionIndex == (uint32)-1)
						{
							getLogManager() << LogLevel_Trace << "Found one too many closing bracket character, parsing aborted\n";
							return false;
						}

						//go up one dimension
						l_ui32CurDimensionIndex--;
					}
					//non whitespace character found
					else if(std::isspace(*l_oIt, l_oLocale) == false)
					{
						//if we are in innermost dimension, assume a value is starting here
						if(l_ui32CurDimensionIndex == l_ui32DimensionCount-1)
						{
							//ensure values parsed so far in current dimension doesn't exceed current dimension size
							if(l_vValuesCount.back() == op_pMatrix->getDimensionSize(l_ui32CurDimensionIndex))
							{
								getLogManager() << LogLevel_Trace
									<< "Found " << l_vValuesCount.back() << " values in dimension " << l_ui32CurDimensionIndex
									<< ", expected " << op_pMatrix->getDimensionSize(l_ui32CurDimensionIndex) << ", parsing aborted\n";
								return false;
							}

							//increment values count found in innermost dimension
							l_vValuesCount[l_ui32CurDimensionIndex]++;

							//append current character to current string
							l_sCurString.append(1, *l_oIt);

							//update status
							l_ui32Status = Status_ParsingBufferValue;
						}
						else
						{
							getLogManager() << LogLevel_Trace << "Unexpected character found on line " << CString(l_sWhat.c_str()) << ", parsing aborted\n";
							return false;
						}
					}
					break;

				//look for end of value (first '"' char not preceded by the '\' escape char)
				case Status_ParsingBufferValue:

					//values end at first whitespace character or ']' character
					if(std::isspace(*l_oIt, l_oLocale) == true || *l_oIt == ']')
					{
						//if dimension closing bracket is found
						if(*l_oIt == ']')
						{
							//move back iterator by one character so that closing bracket is taken into account in Status_ParsingBuffer case
							l_oIt--;
						}

						//retrieve value
						float64 l_f64Value = atof(l_sCurString.c_str());
#if defined OVP_OS_Windows
						if(errno == ERANGE)
						{
							//string couldn't be converted to a double
							getLogManager() << LogLevel_Trace << "Couldn't convert token \"" << CString(l_sCurString.c_str()) << "\" to floating point value, parsing aborted\n";
							return false;
						}
#endif
						//store value in matrix
						(*op_pMatrix)[l_ui32CurElementIndex] = l_f64Value;

						//update element index
						l_ui32CurElementIndex++;

						//reset current string
						l_sCurString.erase();

						//update status
						l_ui32Status = Status_ParsingBuffer;
					}
					//otherwise, append current character to current string
					else
					{
						l_sCurString.append(1, *l_oIt);
					}
					break;

				default:
					break;
			} // switch(l_ui32Status)

			//increment iterator
			l_oIt++;

		} // while(l_oIt != l_sWhat.end()) (read each character of current line)

	} while(m_oDataFile.good()); //read each line in turn

	//ensure the right number of values were parsed in first dimension
	if(l_vValuesCount[0] != op_pMatrix->getDimensionSize(0))
	{
		getLogManager() << LogLevel_Trace <<
			"Found " << l_vValuesCount[0] << " values in dimension 0, expected " << op_pMatrix->getDimensionSize(0) << ", parsing aborted\n";
		return false;
	}

	return true;
}
