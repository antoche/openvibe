#include "ovtkCFeatureVectorSet.hpp"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEToolkit;

// ____________________________________________________________________________________________________________________________________
//

CInternalFeatureVector::CInternalFeatureVector(void)
	:m_pMatrix(NULL)
	,m_ui32DimensionIndex(0)
	,m_ui32BufferElementCount(0)
	,m_pBuffer(NULL)
{
}

uint32 CInternalFeatureVector::getSize(void) const
{
	return m_ui32BufferElementCount;
}

boolean CInternalFeatureVector::setSize(const uint32 ui32Size)
{
	return false;
}

float64* CInternalFeatureVector::getBuffer(void)
{
	return NULL;
}

const float64* CInternalFeatureVector::getBuffer(void) const
{
	return m_pBuffer;
}

const char* CInternalFeatureVector::getElementLabel(const uint32 ui32Index) const
{
	return m_pMatrix->getDimensionLabel(m_ui32DimensionIndex, ui32Index);
}

boolean CInternalFeatureVector::setElementLabel(const uint32 ui32Index, const char* sElementLabel)
{
	return false;
}

float64 CInternalFeatureVector::getLabel(void) const
{
	return m_pBuffer[m_ui32BufferElementCount];
}

boolean CInternalFeatureVector::setLabel(const float64 f64Label)
{
	return false;
}

// ____________________________________________________________________________________________________________________________________
//

CFeatureVectorSet::CFeatureVectorSet(const IMatrix& rMatrix)
	:m_rMatrix(rMatrix)
{
	if(rMatrix.getDimensionCount()!=2)
	{
		throw;
	}

	for(uint32 i=0; i<rMatrix.getDimensionSize(0); i++)
	{
		m_vFeatureVector[i].m_pMatrix=&rMatrix;
		m_vFeatureVector[i].m_ui32DimensionIndex=i;
		m_vFeatureVector[i].m_ui32BufferElementCount=rMatrix.getDimensionSize(1)-1;
		m_vFeatureVector[i].m_pBuffer=rMatrix.getBuffer()+i*rMatrix.getDimensionSize(1);
	}
}

uint32 CFeatureVectorSet::getFeatureVectorCount(void) const
{
	return m_rMatrix.getDimensionSize(0);
}

boolean CFeatureVectorSet::setFeatureVectorCount(const uint32 ui32FeatureVectorCount)
{
	return false;
}

boolean CFeatureVectorSet::addFeatureVector(const IFeatureVector& rFeatureVector)
{
	return false;
}

IFeatureVector& CFeatureVectorSet::getFeatureVector(const uint32 ui32Index)
{
	std::map < uint32, CInternalFeatureVector >::iterator itFeatureVector=m_vFeatureVector.find(ui32Index);
	return itFeatureVector->second;
}

const IFeatureVector& CFeatureVectorSet::getFeatureVector(const uint32 ui32Index) const
{
	std::map < uint32, CInternalFeatureVector >::const_iterator itFeatureVector=m_vFeatureVector.find(ui32Index);
	return itFeatureVector->second;
}

uint32 CFeatureVectorSet::getLabelCount(void) const
{
	std::map < float64, boolean > l_vLabel;
	std::map < uint32, CInternalFeatureVector >::const_iterator itFeatureVector;
	for(itFeatureVector=m_vFeatureVector.begin(); itFeatureVector!=m_vFeatureVector.end(); itFeatureVector++)
	{
		l_vLabel[itFeatureVector->second.getLabel()]=true;
	}
	return l_vLabel.size();
}
