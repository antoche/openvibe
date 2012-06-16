#include "ovkCLogListenerNull.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

CLogListenerNull::CLogListenerNull(const IKernelContext& rKernelContext)
	:TKernelObject<ILogListener>(rKernelContext)
{
}

boolean CLogListenerNull::isActive(ELogLevel eLogLevel)
{
	map<ELogLevel, boolean>::iterator itLogLevel=m_vActiveLevel.find(eLogLevel);
	if(itLogLevel==m_vActiveLevel.end())
	{
		return true;
	}
	return itLogLevel->second;
}

boolean CLogListenerNull::activate(ELogLevel eLogLevel, boolean bActive)
{
	m_vActiveLevel[eLogLevel]=bActive;
	return true;
}

boolean CLogListenerNull::activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, boolean bActive)
{
	for(int i=eStartLogLevel; i<=eEndLogLevel; i++)
	{
		m_vActiveLevel[ELogLevel(i)]=bActive;
	}
	return true;
}

boolean CLogListenerNull::activate(boolean bActive)
{
	return activate(LogLevel_First, LogLevel_Last, bActive);
}

void CLogListenerNull::log(const time64 time64Value)
{
}

void CLogListenerNull::log(const uint64 ui64Value)
{
}

void CLogListenerNull::log(const uint32 ui32Value)
{
}

void CLogListenerNull::log(const uint16 ui16Value)
{
}

void CLogListenerNull::log(const uint8 ui8Value)
{
}

void CLogListenerNull::log(const int64 i64Value)
{
}

void CLogListenerNull::log(const int32 i32Value)
{
}

void CLogListenerNull::log(const int16 i16Value)
{
}

void CLogListenerNull::log(const int8 i8Value)
{
}

void CLogListenerNull::log(const float32 f32Value)
{
}

void CLogListenerNull::log(const float64 f64Value)
{
}

void CLogListenerNull::log(const boolean bValue)
{
}

void CLogListenerNull::log(const CIdentifier& rValue)
{
}

void CLogListenerNull::log(const CString& rValue)
{
}

void CLogListenerNull::log(const char* pValue)
{
}

void CLogListenerNull::log(const ELogLevel eLogLevel)
{
}

void CLogListenerNull::log(const ELogColor eLogColor)
{
}
