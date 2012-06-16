#include "ovkCVertexData.h"

using namespace OpenViBE;
//using namespace OMK ;
//using namespace OMK::Type ;

//const Transform Transform::sk_identity;

CVertexData::CVertexData() :
	m_ui32VertexPositionCount(0)
#ifndef USE_STL_CONTAINERS
	,m_pVertexPositionArray(NULL)
#endif
	,m_ui32VertexColorCount(0)
#ifndef USE_STL_CONTAINERS
	,m_pVertexColorArray(NULL)
#endif
{
}

CVertexData::~CVertexData()
{
	//FIXME
	//cleanup();
}

void CVertexData::cleanup()
{
#ifndef USE_STL_CONTAINERS
	if(m_pVertexPositionArray != NULL)
	{
		delete m_pVertexPositionArray;
	}
	if(m_pVertexColorArray != NULL)
	{
		delete m_pVertexColorArray;
	}
#endif
}

void CVertexData::setVertexPositionArray( uint32 ui32VertexPositionCount, const float32* pVertexPositionArray )
{
	m_ui32VertexPositionCount = ui32VertexPositionCount;

#ifdef ALLOCATE_VERTEX_DATA_MEMORY
	if(m_pVertexPositionArray != NULL)
	{
		delete m_pVertexPositionArray;
	}
	m_pVertexPositionArray = new float32[3*ui32VertexPositionCount];
	memcpy(m_pVertexPositionArray, pVertexPositionArray, 3*ui32VertexPositionCount*sizeof(float32));
#endif

#ifdef SHARE_MEMORY_WITH_CALLER
	m_pVertexPositionArray = pVertexPositionArray;
#endif

#ifdef USE_STL_CONTAINERS
	m_pVertexPositionArray.resize(3*ui32VertexPositionCount);
	const float32* l_pVertexPosition = pVertexPositionArray;
	for(unsigned int i=0; i<3*ui32VertexPositionCount; i++, l_pVertexPosition++)
	{
		m_pVertexPositionArray[i] = *l_pVertexPosition;
	}
#endif
}

void CVertexData::getVertexPositionArray( uint32& ui32VertexPositionCount, const float32*& pVertexPositionArray) const
{
	ui32VertexPositionCount = m_ui32VertexPositionCount;
	if(m_ui32VertexPositionCount > 0)
		pVertexPositionArray = &m_pVertexPositionArray[0];
	else
		pVertexPositionArray = NULL;
}

void CVertexData::setVertexColorArray( uint32 ui32VertexColorCount, const float32* pVertexColorArray )
{
	m_ui32VertexColorCount = ui32VertexColorCount;
#ifdef ALLOCATE_VERTEX_DATA_MEMORY
	if(m_pVertexColorArray != NULL)
	{
		delete m_pVertexColorArray;
	}
	m_pVertexColorArray = new float32[4*ui32VertexColorCount];
	memcpy(m_pVertexColorArray, pVertexColorArray, 4*ui32VertexColorCount*sizeof(float32));
#endif

#ifdef SHARE_MEMORY_WITH_CALLER
	m_pVertexColorArray = pVertexColorArray;
#endif

#ifdef USE_STL_CONTAINERS
	m_pVertexColorArray.resize(4*ui32VertexColorCount);
	const float32* l_pVertexColor = pVertexColorArray;
	for(unsigned int i=0; i<4*ui32VertexColorCount; i++, l_pVertexColor++)
	{
		m_pVertexColorArray[i] = *l_pVertexColor;
	}
#endif
}

void CVertexData::getVertexColorArray( uint32& ui32VertexColorCount, const float32*& pVertexColorArray) const
{
	ui32VertexColorCount = m_ui32VertexColorCount;
	if(m_ui32VertexColorCount)
		pVertexColorArray = &m_pVertexColorArray[0];
	else
		pVertexColorArray = NULL;
}

bool operator == ( const CVertexData& vd0, const CVertexData& vd1 )
{
  if(vd0.m_ui32VertexPositionCount != vd1.m_ui32VertexPositionCount ||
		 vd0.m_ui32VertexColorCount != vd1.m_ui32VertexColorCount)
		return false;

	for(uint32 i = 0; i < vd0.m_ui32VertexPositionCount; i+=3)
	{
		if( vd0.m_pVertexPositionArray[i] != vd1.m_pVertexPositionArray[i])
			return false;
		if( vd0.m_pVertexPositionArray[i+1] != vd1.m_pVertexPositionArray[i+1])
			return false;
		if( vd0.m_pVertexPositionArray[i+2] != vd1.m_pVertexPositionArray[i+2])
			return false;
	}

	for(uint32 i = 0; i < vd0.m_ui32VertexColorCount; i+=4)
	{
		if( vd0.m_pVertexColorArray[i] != vd1.m_pVertexColorArray[i])
			return false;
		if( vd0.m_pVertexColorArray[i+1] != vd1.m_pVertexColorArray[i+1])
			return false;
		if( vd0.m_pVertexColorArray[i+2] != vd1.m_pVertexColorArray[i+2])
			return false;
		if( vd0.m_pVertexColorArray[i+3] != vd1.m_pVertexColorArray[i+3])
			return false;
	}

	return true;
}

bool operator != ( const CVertexData& vd0, const CVertexData& vd1 )
{
  return !( vd0 == vd1 ) ;
}
