#include "ovCMatrix.h"

#include <vector>
#include <string>
#include <iostream>

using namespace OpenViBE;
using namespace std;

namespace OpenViBE
{
	namespace
	{
		class CMatrixImpl : public IMatrix
		{
		public:

			CMatrixImpl(void);
			virtual ~CMatrixImpl(void);

			virtual const uint32 getDimensionCount(void) const;
			virtual const uint32 getDimensionSize(const uint32 ui32DimensionIndex) const;
			virtual const char* getDimensionLabel(const uint32 ui32DimensionIndex, const uint32 ui32DimensionEntryIndex) const;
			virtual const float64* getBuffer(void) const;
			virtual const uint32 getBufferElementCount(void) const;

			virtual boolean setDimensionCount(const uint32 ui32DimensionCount);
			virtual boolean setDimensionSize(const uint32 ui32DimensionIndex, const uint32 ui32DimensionSize);
			virtual boolean setDimensionLabel(const uint32 ui32DimensionIndex, const uint32 ui32DimensionEntryIndex, const char* sDimensionLabel);
			virtual float64* getBuffer(void);

			_IsDerivedFromClass_Final_(IMatrix, OV_ClassId_MatrixImpl);

		private:

			boolean refreshInternalBuffer(void) const;

		protected:

			mutable float64* m_pBuffer;
			mutable uint32 m_ui32BufferElementCount;

			std::vector < uint32 > m_vDimensionSize;
			std::vector < std::vector < std::string > > m_vDimensionLabel;
		};
	};
};

// ________________________________________________________________________________________________________________
//

CMatrixImpl::CMatrixImpl(void)
	:m_pBuffer(NULL)
	,m_ui32BufferElementCount(0)
{
}

CMatrixImpl::~CMatrixImpl(void)
{
	delete [] m_pBuffer;
}

const uint32 CMatrixImpl::getDimensionCount(void) const
{
	return m_vDimensionSize.size();
}

const uint32 CMatrixImpl::getDimensionSize(const uint32 ui32DimensionIndex) const
{
	if(ui32DimensionIndex>=m_vDimensionSize.size())
	{
		return 0;
	}
	return m_vDimensionSize[ui32DimensionIndex];
}

const char* CMatrixImpl::getDimensionLabel(const uint32 ui32DimensionIndex, const uint32 ui32DimensionEntryIndex) const
{
	if(ui32DimensionIndex>=m_vDimensionSize.size())
	{
		return "";
	}
	if(ui32DimensionEntryIndex>=m_vDimensionSize[ui32DimensionIndex])
	{
		return "";
	}
	return m_vDimensionLabel[ui32DimensionIndex][ui32DimensionEntryIndex].c_str();
}

const float64* CMatrixImpl::getBuffer(void) const
{
	if(!m_pBuffer)
	{
		this->refreshInternalBuffer();
	}

	return m_pBuffer;
}

const uint32 CMatrixImpl::getBufferElementCount(void) const
{
	if(!m_ui32BufferElementCount)
	{
		this->refreshInternalBuffer();
	}

	return m_ui32BufferElementCount;
}

boolean CMatrixImpl::setDimensionCount(const uint32 ui32DimensionCount)
{
	if(ui32DimensionCount==0)
	{
		return false;
	}

	delete [] m_pBuffer;
	m_pBuffer=NULL;

	m_vDimensionSize.clear();
	m_vDimensionSize.resize(ui32DimensionCount);

	m_vDimensionLabel.clear();
	m_vDimensionLabel.resize(ui32DimensionCount);

	return true;
}

boolean CMatrixImpl::setDimensionSize(const uint32 ui32DimensionIndex, const uint32 ui32DimensionSize)
{
	if(ui32DimensionIndex>=m_vDimensionSize.size())
	{
		return false;
	}

	delete [] m_pBuffer;
	m_pBuffer=NULL;

	m_vDimensionSize[ui32DimensionIndex]=ui32DimensionSize;
	m_vDimensionLabel[ui32DimensionIndex].clear();
	m_vDimensionLabel[ui32DimensionIndex].resize(ui32DimensionSize);
	return true;
}

boolean CMatrixImpl::setDimensionLabel(const uint32 ui32DimensionIndex, const uint32 ui32DimensionEntryIndex, const char* sDimensionLabel)
{
	if(ui32DimensionIndex>=m_vDimensionSize.size())
	{
		return false;
	}
	if(ui32DimensionEntryIndex>=m_vDimensionSize[ui32DimensionIndex])
	{
		return false;
	}
	m_vDimensionLabel[ui32DimensionIndex][ui32DimensionEntryIndex]=sDimensionLabel;
	return true;
}

float64* CMatrixImpl::getBuffer(void)
{
	if(!m_pBuffer)
	{
		this->refreshInternalBuffer();
	}

	return m_pBuffer;
}

boolean CMatrixImpl::refreshInternalBuffer(void) const
{
	if(m_pBuffer)
	{
		return false;
	}

	if(m_vDimensionSize.size()==0)
	{
		return false;
	}

	m_ui32BufferElementCount=1;
	for(size_t i=0; i<m_vDimensionSize.size(); i++)
	{
		m_ui32BufferElementCount*=m_vDimensionSize[i];
	}

	if(m_ui32BufferElementCount==0)
	{
		return false;
	}

	m_pBuffer=new float64[m_ui32BufferElementCount];
	if(!m_pBuffer)
	{
		m_ui32BufferElementCount=0;
		return false;
	}

	return true;
}

// ________________________________________________________________________________________________________________
//

CMatrix::CMatrix(void)
{
	m_pMatrixImpl=new CMatrixImpl();
}

CMatrix::~CMatrix(void)
{
	delete m_pMatrixImpl;
}

const uint32 CMatrix::getDimensionCount(void) const
{
	return m_pMatrixImpl->getDimensionCount();
}

const uint32 CMatrix::getDimensionSize(const uint32 ui32DimensionIndex) const
{
	return m_pMatrixImpl->getDimensionSize(ui32DimensionIndex);
}

const char* CMatrix::getDimensionLabel(const uint32 ui32DimensionIndex, const uint32 ui32DimensionEntryIndex) const
{
	return m_pMatrixImpl->getDimensionLabel(ui32DimensionIndex, ui32DimensionEntryIndex);
}

const float64* CMatrix::getBuffer(void) const
{
	return m_pMatrixImpl->getBuffer();
}

const uint32 CMatrix::getBufferElementCount(void) const
{
	return m_pMatrixImpl->getBufferElementCount();
}

boolean CMatrix::setDimensionCount(const uint32 ui32DimensionCount)
{
	return m_pMatrixImpl->setDimensionCount(ui32DimensionCount);
}

boolean CMatrix::setDimensionSize(const uint32 ui32DimensionIndex, const uint32 ui32DimensionSize)
{
	return m_pMatrixImpl->setDimensionSize(ui32DimensionIndex, ui32DimensionSize);
}

boolean CMatrix::setDimensionLabel(const uint32 ui32DimensionIndex, const uint32 ui32DimensionEntryIndex, const char* sDimensionLabel)
{
	return m_pMatrixImpl->setDimensionLabel(ui32DimensionIndex, ui32DimensionEntryIndex, sDimensionLabel);
}

float64* CMatrix::getBuffer(void)
{
	return m_pMatrixImpl->getBuffer();
}
