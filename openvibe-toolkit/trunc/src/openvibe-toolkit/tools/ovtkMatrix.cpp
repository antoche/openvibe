#include "ovtkMatrix.h"

#include <system/Memory.h>

#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cfloat>

#if defined OVTK_OS_Windows
 #define isnan _isnan
 #define isinf !_finite
#elif defined OVTK_OS_MacOS
 #define isnan std::isnan
 #define isinf std::isinf
#endif

using namespace OpenViBE;
using namespace OpenViBEToolkit;
using namespace OpenViBEToolkit::Tools;

boolean OpenViBEToolkit::Tools::Matrix::copy(IMatrix& rDestinationMatrix, const IMatrix& rSourceMatrix)
{
	if(&rDestinationMatrix==&rSourceMatrix)
	{
		return true;
	}

	if(!copyDescription(rDestinationMatrix, rSourceMatrix))
	{
		return false;
	}
	if(!copyContent(rDestinationMatrix, rSourceMatrix))
	{
		return false;
	}
	return true;
}

boolean OpenViBEToolkit::Tools::Matrix::copyDescription(IMatrix& rDestinationMatrix, const IMatrix& rSourceMatrix)
{
	if(&rDestinationMatrix==&rSourceMatrix)
	{
		return true;
	}

	uint32 l_ui32DimensionCount=rSourceMatrix.getDimensionCount();
	uint32 l_ui32DimensionSize=0;
	if(!rDestinationMatrix.setDimensionCount(l_ui32DimensionCount))
	{
		return false;
	}
	for(uint32 i=0; i<l_ui32DimensionCount; i++)
	{
		l_ui32DimensionSize=rSourceMatrix.getDimensionSize(i);
		if(!rDestinationMatrix.setDimensionSize(i, l_ui32DimensionSize))
		{
			return false;
		}
		for(uint32 j=0; j<l_ui32DimensionSize; j++)
		{
			if(!rDestinationMatrix.setDimensionLabel(i, j, rSourceMatrix.getDimensionLabel(i, j)))
			{
				return false;
			}
		}
	}
	return true;
}

boolean OpenViBEToolkit::Tools::Matrix::copyContent(IMatrix& rDestinationMatrix, const IMatrix& rSourceMatrix)
{
	if(&rDestinationMatrix==&rSourceMatrix)
	{
		return true;
	}

	uint32 l_ui32SourceElementCount=rSourceMatrix.getBufferElementCount();
	uint32 l_ui32DestinationElementCount=rDestinationMatrix.getBufferElementCount();
	if(l_ui32DestinationElementCount != l_ui32SourceElementCount)
	{
		return false;
	}
	const float64* l_pSourceBuffer=rSourceMatrix.getBuffer();
	float64* l_pDestinationBuffer=rDestinationMatrix.getBuffer();
	System::Memory::copy(l_pDestinationBuffer, l_pSourceBuffer, l_ui32SourceElementCount*sizeof(float64));
	return true;
}

boolean OpenViBEToolkit::Tools::Matrix::clearContent(IMatrix& rMatrix)
{
	System::Memory::set(rMatrix.getBuffer(), rMatrix.getBufferElementCount()*sizeof(float64), 0);
	return true;
}

boolean OpenViBEToolkit::Tools::Matrix::isDescriptionSimilar(const IMatrix& rSourceMatrix1, const IMatrix& rSourceMatrix2, const boolean bCheckLabels)
{
	if(&rSourceMatrix1==&rSourceMatrix2)
	{
		return true;
	}

	if(rSourceMatrix1.getDimensionCount() != rSourceMatrix2.getDimensionCount())
	{
		return false;
	}

	for(uint32 i=0; i<rSourceMatrix1.getDimensionCount(); i++)
	{
		if(rSourceMatrix1.getDimensionSize(i) != rSourceMatrix2.getDimensionSize(i))
		{
			return false;
		}
	}

	if(bCheckLabels)
	{
		for(uint32 i=0; i<rSourceMatrix1.getDimensionCount(); i++)
		{
			for(uint32 j=0; j<rSourceMatrix1.getDimensionSize(i); j++)
			{
				if(strcmp(rSourceMatrix1.getDimensionLabel(i, j), rSourceMatrix2.getDimensionLabel(i, j))!=0)
				{
					return false;
				}
			}
		}
	}

	return true;
}

boolean OpenViBEToolkit::Tools::Matrix::isContentSimilar(const IMatrix& rSourceMatrix1, const IMatrix& rSourceMatrix2)
{
	if(&rSourceMatrix1==&rSourceMatrix2)
	{
		return true;
	}

	if(rSourceMatrix1.getBufferElementCount() != rSourceMatrix2.getBufferElementCount())
	{
		return false;
	}

	return ::memcmp(rSourceMatrix1.getBuffer(), rSourceMatrix2.getBuffer(), rSourceMatrix1.getBufferElementCount()*sizeof(float64)) == 0;
}

boolean OpenViBEToolkit::Tools::Matrix::isContentValid(const IMatrix& rSourceMatrix, const boolean bCheckNotANumber, const boolean bCheckInfinity)
{
	const float64* l_pBuffer=rSourceMatrix.getBuffer();
	const float64* l_pBufferEnd=rSourceMatrix.getBuffer()+rSourceMatrix.getBufferElementCount();
	while(l_pBuffer!=l_pBufferEnd)
	{
		if(bCheckNotANumber && isnan(*l_pBuffer)) return false;
		if(bCheckInfinity && isinf(*l_pBuffer)) return false;
		l_pBuffer++;
	}
	return true;
}
