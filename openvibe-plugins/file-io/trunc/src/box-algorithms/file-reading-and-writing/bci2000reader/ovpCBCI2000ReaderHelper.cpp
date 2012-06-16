#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <boost/regex.hpp>
#include <boost/detail/endian.hpp>

#include "ovpCBCI2000ReaderHelper.h"

using namespace OpenViBE;
using namespace BCI2000;

void CBCI2000ReaderHelper::printInfo(std::ostream &os)
{
	if (m_bGood)
	{
		os << "version:              " << m_f32BCI2000version << std::endl;
		os << "header length:        " << m_i32HeaderLength << std::endl;
		os << "source channels:      " << m_i32SourceChannelCount << std::endl;
		os << "state vector length:  " << m_i32StateVectorLength << std::endl;
		os << "data format:          " << m_sDataFormat << std::endl;
		os << "samples:              " << m_i32NbSamples << std::endl;
		os << "samples left:         " << m_i32SamplesLeft << std::endl;
		os << "sample size:          " << m_i32SampleSize << std::endl;
		os << "rate:                 " << getRate() << std::endl;
	}
	else
	{
		os << "bad file (unreadable or bad header)" << std::endl;
	}
}

boolean CBCI2000ReaderHelper::parseMeta(CString &rMeta)
{
	static const boost::regex l_oReMeta(
		"BCI2000V= ([0-9.]+) HeaderLen= ([0-9]+) SourceCh= ([0-9]+) StateVectorLen(gth)?= ([0-9]+) DataFormat= ([a-z0-9]+)\r?$",
		boost::regex::perl|boost::regex::icase
	);

	static const boost::regex l_oReOldMeta(
		"HeaderLen= +([0-9]+) SourceCh= +([0-9]+) StatevectorLen= +([0-9]+)\r$",
		boost::regex::perl|boost::regex::icase
	);

	boost::smatch l_oMatch;

	if (regex_match(std::string(rMeta),l_oMatch,l_oReMeta))
	{
		m_f32BCI2000version=(float32)atof(l_oMatch.str(1).c_str());
		m_i32HeaderLength=(int32) atoi(l_oMatch.str(2).c_str());
		m_i32SourceChannelCount=(int32) atoi(l_oMatch.str(3).c_str());
		// 4 is dropped (could be used for len/length syntax check)
		m_i32StateVectorLength=(int32) atoi(l_oMatch.str(5).c_str());
		m_sDataFormat=l_oMatch.str(6).c_str();
	}
	else
	{
		if (regex_match(std::string(rMeta),l_oMatch,l_oReOldMeta))
		{
			m_f32BCI2000version=1.0;
			m_i32HeaderLength=(int32) atoi(l_oMatch.str(1).c_str());
			m_i32SourceChannelCount=(int32) atoi(l_oMatch.str(2).c_str());
			m_i32StateVectorLength=(int32) atoi(l_oMatch.str(3).c_str());
			m_sDataFormat="int16";
		}
		else
		{
			std::cerr << " *** bci2000helper error: cannot parse l_sMeta information header" << std::endl;
			return false;
		}
	}
	return true;
}


boolean CBCI2000ReaderHelper::parseHeader(std::istream &rStream)
{
	static const boost::regex l_oRegexpSection("\\[ *(.*[^ ]) *\\].*");
	static const boost::regex l_oRegexpParameter("([^ ]+ [^ ]+ [^ ]+)= (.*)$"); // (section) (type) (name)= (value)
	static const boost::regex l_oRegexpField("([^ ]+) ([^ ]+) ([^ ]+) ([^ ]+) ([^ ]+)$"); // Name Length Value ByteLocation BitLocation
	std::string l_sSection="NONAME";
	std::string l_sLine;
	boost::smatch l_oMatch;
	std::getline(rStream,l_sLine);
	while (l_sLine.length()>2)
	{
		// is it a section name ?
		if (regex_match(l_sLine,l_oMatch,l_oRegexpSection))
		{
			l_sSection=l_oMatch[1];
		}
		else // not section, parse if interesting
		{
			if (l_sSection=="Parameter Definition")
			{
				if (regex_match(l_sLine,l_oMatch,l_oRegexpParameter))
				{
					m_mParameters[l_oMatch[1].str().c_str()]=l_oMatch[2].str().c_str();
					//std::cout << "parameter added : \n\t\t" << l_oMatch[1].str() << " \n\t\t" << l_oMatch[2].str() << "\n";
				}
				else
				{
					// should never happen: malformed file
					std::cerr<<" *** bci2000helper error: cannot parse Parameters"<<std::endl;
					return false;
				}
			}
			if (l_sSection=="State Vector Definition")
			{
				if (regex_match(l_sLine,l_oMatch,l_oRegexpField))
				{
					int32 l_i32Length  = atoi(l_oMatch.str(2).c_str());
					int32 l_i32Value   = atoi(l_oMatch.str(3).c_str());
					int32 l_i32BytePos = atoi(l_oMatch.str(4).c_str());
					int32 l_i32BitPos  = atoi(l_oMatch.str(5).c_str());
					m_oBitfield.addField(l_i32BytePos,l_i32BitPos,l_i32Length,l_oMatch.str(1).c_str(),l_i32Value);
				}
				else
				{
					// should never happen: malformed file
					std::cerr<<" *** bci2000helper error: cannot parse Parameters"<<std::endl;
					return false;
				}
			}
		}
		std::getline(rStream,l_sLine);
	}
	return true;
}

float32 CBCI2000ReaderHelper::getRate(void) const
{
	// Warning: the SamplingRate field is not clearly defined
	// in the BCI2000 doc; it could be an int32, or a OpenViBE::float32;
	// it should be in Section Source, but Source:Garbage:Stuff is possible
	// in some case, there is a pending "Hz" after the numbers
	// Conclusion: we should not fix this until it's clear...

	CString l_sKey="Source int SamplingRate";
	if (m_mParameters.count(l_sKey)==1)
	{
		std::istringstream l_oIStream((const char *) (m_mParameters.find(l_sKey)->second));
		float32 l_f32Rate;
		l_oIStream >> l_f32Rate;
		return l_f32Rate;
	}
	l_sKey="Source:Signal%20Properties:DataIOFilter int SamplingRate";
	if (m_mParameters.count(l_sKey)==1)
	{
		std::istringstream is((const char *) (m_mParameters.find(l_sKey)->second));
		float32 l_f32Rate;
		is >> l_f32Rate;
		return l_f32Rate;
	}
	return -1.0; // not found :-(
}

CString CBCI2000ReaderHelper::getChannelName(uint32 ui32Index) const
{
	// To be checked on the different version of format.

	CString l_sKey="Source:Signal%20Properties:DataIOFilter list ChannelNames";
	if (m_mParameters.count(l_sKey)==1)
	{
		std::istringstream is((const char *) (m_mParameters.find(l_sKey)->second));
		std::string l_sToken;
		for(uint32 i=0; i<=ui32Index+1; i++) //+1 because the channel count is in the parameter on first position (parameter:list)
		{
			l_sToken.clear();
			is >> l_sToken;
		}
		return CString(l_sToken.c_str());
	}

	char l_sChannelName[128];
	sprintf(l_sChannelName, "Channel %u", ui32Index + 1);
	return CString(l_sChannelName); 
}


CBCI2000ReaderHelper::CBCI2000ReaderHelper(const char * sFilename)
{
	m_oBCIFile.open(sFilename,std::ios::binary);
	if (!m_oBCIFile.good())
	{
		m_bGood=false;
		return;
	}

	std::stringbuf l_oStringBuffer;
	m_oBCIFile.get(l_oStringBuffer);
	CString l_sMeta=CString(l_oStringBuffer.str().c_str());

	m_oBCIFile.seekg(0,std::ios::end);
	int32 l_i32FileSize=m_oBCIFile.tellg();
	m_oBCIFile.seekg(0,std::ios::beg);

	m_bGood=parseMeta(l_sMeta);
	if (!m_bGood)
	{
		return;
	}

	std::map<CString,int32> l_mSizesOfMap;
	l_mSizesOfMap["float32"]=4;
	l_mSizesOfMap["int32"]=4;
	l_mSizesOfMap["int16"]=2;
	m_i32NbSamples=(l_i32FileSize-m_i32HeaderLength)/(l_mSizesOfMap[m_sDataFormat]*m_i32SourceChannelCount+m_i32StateVectorLength);
	m_i32SampleSize=l_mSizesOfMap[m_sDataFormat]*m_i32SourceChannelCount+m_i32StateVectorLength;
	m_i32SamplesLeft=m_i32NbSamples;
	m_bGood=parseHeader(m_oBCIFile);
}

CBCI2000ReaderHelper::~CBCI2000ReaderHelper()
{
	if (m_oBCIFile)
	{
		m_oBCIFile.close();
	}
}

std::vector<OpenViBE::float32> CBCI2000ReaderHelper::readSample()
{
	std::vector<OpenViBE::float32> l_vSamples;
	if (m_i32SamplesLeft<1)
	{
		return l_vSamples; // nothing to read, empty vector returned
	}
	char *l_pData=new char[m_i32SampleSize];
	m_oBCIFile.read(l_pData,m_i32SampleSize);
	OpenViBE::float32 *l_pDataAsFloat=(float32*) l_pData;
	for (int32 i=0; i<m_i32SourceChannelCount; i++)
	{
		l_vSamples.push_back(l_pDataAsFloat[i]);
	}
	delete[] l_pData;
	m_i32SamplesLeft--;
	return l_vSamples;
}

template <class TFrom, class TTo>
int32 CBCI2000ReaderHelper::readSamplesInternal(TTo* pSamples, uint32 * pStates, int32 i32Nb)
{
	if (i32Nb>m_i32SamplesLeft)
	{
		i32Nb=m_i32SamplesLeft;
	}
	if (i32Nb<1)
	{
		return 0;
	}
	char *l_pData=new char[m_i32SampleSize*i32Nb];
	m_oBCIFile.read(l_pData,m_i32SampleSize*i32Nb);
	for (int32 i=0; i<i32Nb; i++)
	{
		if (pSamples!=NULL)
		{
			for (int32 j=0; j<m_i32SourceChannelCount; j++)
			{
				TFrom l_oSample;
				l_oSample=*(TFrom*)(l_pData+i*m_i32SampleSize+j*sizeof(TFrom));
				pSamples[i*m_i32SourceChannelCount+j]=l_oSample;
				// check endianess ?
			}
		}
		if (pStates!=NULL)
		{
			unsigned char * l_pState=(unsigned char*)l_pData+i*m_i32SampleSize+m_i32SourceChannelCount*sizeof(TFrom);
			m_oBitfield.getFields(l_pState,pStates+i*m_oBitfield.size());
		}
	//std::copy(l_pData+i*m_i32SampleSize,l_pData+i*m_i32SampleSize+m_i32SourceChannelCount*sizeof(T),samples+i*m_i32SourceChannelCount);
	}
	delete[] l_pData;
	m_i32SamplesLeft-=i32Nb;
	return i32Nb;
}

int32 CBCI2000ReaderHelper::readSamples(float64* pSamples, uint32 * pStates, int32 i32Nb)
{
	if (m_sDataFormat==CString("float32")) return readSamplesInternal<float32,double>(pSamples,pStates,i32Nb);
#if defined(BOOST_LITTLE_ENDIAN)
	if (m_sDataFormat==CString("int32")) return readSamplesInternal<int32,double>(pSamples,pStates,i32Nb);
	if (m_sDataFormat==CString("int16")) return readSamplesInternal<int16,double>(pSamples,pStates,i32Nb);
#else
	std::cerr << "*** bci2000helper error: read_samples from int16 or int32 is not implemented yet on bigendian machines" << std::endl;
	exit(EXIT_FAILURE);
#endif
	return -1; // should never happen... TODO: error checking
}
