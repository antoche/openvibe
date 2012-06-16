#include "ovtkColorGradient.h"

#include <cstdio>
#include <string>
#include <map>

using namespace OpenViBE;

namespace
{
	typedef struct
	{
		float64 fPercent;
		float64 fRed;
		float64 fGreen;
		float64 fBlue;
	} SColor;
};

boolean OpenViBEToolkit::Tools::ColorGradient::parse(IMatrix& rColorGradient, const CString& rString)
{
	std::string l_sString(rString.toASCIIString());
	std::string::size_type l_iStart=0;
	std::string::size_type l_iEnd;

	std::map < float64, SColor > l_vColorGradient;

	do
	{
		l_iEnd=l_sString.find(OV_Value_EnumeratedStringSeparator, l_iStart);
		if(l_iEnd==std::string::npos)
		{
			l_iEnd=l_sString.length();
		}

		std::string l_sColor;
		l_sColor.assign(l_sString, l_iStart, l_iEnd-l_iStart);

		int p,r,g,b;
		if(sscanf(l_sColor.c_str(), "%i:%i,%i,%i", &p, &r, &g, &b) == 4)
		{
			SColor l_oColor;
			l_oColor.fPercent=p;
			l_oColor.fRed=r;
			l_oColor.fGreen=g;
			l_oColor.fBlue=b;
			l_vColorGradient[l_oColor.fPercent]=l_oColor;
		}

		l_iStart=l_iEnd+1;
	}
	while(l_iStart<l_sString.length());

	rColorGradient.setDimensionCount(2);
	rColorGradient.setDimensionSize(0, 4);
	rColorGradient.setDimensionSize(1, l_vColorGradient.size());

	uint32 i=0;
	std::map < float64, SColor > ::const_iterator it;
	for(it=l_vColorGradient.begin(); it!=l_vColorGradient.end(); it++, i++)
	{
		rColorGradient[i*4  ]=it->second.fPercent;
		rColorGradient[i*4+1]=it->second.fRed;
		rColorGradient[i*4+2]=it->second.fGreen;
		rColorGradient[i*4+3]=it->second.fBlue;
	}

	return true;
}

boolean OpenViBEToolkit::Tools::ColorGradient::format(CString& rString, const IMatrix& rColorGradient)
{
	if(rColorGradient.getDimensionCount() != 2)
	{
		return false;
	}

	if(rColorGradient.getDimensionSize(0) != 4)
	{
		return false;
	}

	std::string l_sSeparator("  ");
	l_sSeparator[0]=OV_Value_EnumeratedStringSeparator;

	std::string l_sResult;
	for(uint32 i=0; i<rColorGradient.getDimensionSize(1); i++)
	{
		char l_sBuffer[1024];
		sprintf(
			l_sBuffer,
			"%.0lf:%i,%i,%i",
			rColorGradient[i*4],
			(int)rColorGradient[i*4+1],
			(int)rColorGradient[i*4+2],
			(int)rColorGradient[i*4+3]);
		l_sResult+=(i==0?"":l_sSeparator);
		l_sResult+=l_sBuffer;
	}

	rString=l_sResult.c_str();
	return true;
}

boolean OpenViBEToolkit::Tools::ColorGradient::interpolate(IMatrix& rInterpolatedColorGradient, const IMatrix& rColorGradient, const uint32 ui32Steps)
{
	uint32 i;

	if(rColorGradient.getDimensionCount() != 2)
	{
		return false;
	}

	if(rColorGradient.getDimensionSize(0) != 4)
	{
		return false;
	}

	if(ui32Steps <= 1)
	{
		return false;
	}

	rInterpolatedColorGradient.setDimensionCount(2);
	rInterpolatedColorGradient.setDimensionSize(0, 4);
	rInterpolatedColorGradient.setDimensionSize(1, ui32Steps);

	std::map < float64, SColor > l_vColors;

	for(i=0; i<rColorGradient.getDimensionSize(1); i++)
	{
		SColor l_oColor;
		l_oColor.fPercent=rColorGradient[i*4];
		l_oColor.fRed    =rColorGradient[i*4+1];
		l_oColor.fGreen  =rColorGradient[i*4+2];
		l_oColor.fBlue   =rColorGradient[i*4+3];
		l_vColors[l_oColor.fPercent]=l_oColor;
	}

	if(l_vColors.find(0) == l_vColors.end())
	{
		SColor l_oColor;
		l_oColor=l_vColors.begin()->second;
		l_oColor.fPercent=0;
		l_vColors[0]=l_oColor;
	}

	if(l_vColors.find(100) == l_vColors.end())
	{
		SColor l_oColor;
		l_oColor=l_vColors.rbegin()->second;
		l_oColor.fPercent=100;
		l_vColors[100]=l_oColor;
	}

	std::map < float64, SColor >::const_iterator it1=l_vColors.begin();
	std::map < float64, SColor >::const_iterator it2=l_vColors.begin();
	it2++;

	for(i=0; i<ui32Steps; i++)
	{
		float64 t=i*100/(ui32Steps-1);
		while(it2->first < t)
		{
			it1++;
			it2++;
		}

		float64 a=it2->first-t;
		float64 b=t-it1->first;
		float64 d=it2->first-it1->first;

		rInterpolatedColorGradient[i*4  ]=t;
		rInterpolatedColorGradient[i*4+1]=(it1->second.fRed   * a + it2->second.fRed   * b) / d;
		rInterpolatedColorGradient[i*4+2]=(it1->second.fGreen * a + it2->second.fGreen * b) / d;
		rInterpolatedColorGradient[i*4+3]=(it1->second.fBlue  * a + it2->second.fBlue  * b) / d;
	}

	return true;
}
