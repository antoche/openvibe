#include "ovkCLogListenerFile.h"

#include <cstdio>
#include <sstream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

#define _do_stuff_on_file_(f, wm, stuff) \
	FILE* f=fopen(m_sLogFilename.toASCIIString(), wm); \
	if(f) \
	{ \
		stuff; \
		fclose(f); \
	}

CLogListenerFile::CLogListenerFile(const IKernelContext& rKernelContext, const CString& sApplicationName, const CString& sLogFilename)
	:TKernelObject<ILogListener>(rKernelContext)
	,m_sApplicationName(sApplicationName)
	,m_sLogFilename(sLogFilename)
{
	_do_stuff_on_file_(l_pFile, "wt", );
}

boolean CLogListenerFile::isActive(ELogLevel eLogLevel)
{
	map<ELogLevel, boolean>::iterator itLogLevel=m_vActiveLevel.find(eLogLevel);
	if(itLogLevel==m_vActiveLevel.end())
	{
		return true;
	}
	return itLogLevel->second;
}

boolean CLogListenerFile::activate(ELogLevel eLogLevel, boolean bActive)
{
	m_vActiveLevel[eLogLevel]=bActive;
	return true;
}

boolean CLogListenerFile::activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, boolean bActive)
{
	for(int i=eStartLogLevel; i<=eEndLogLevel; i++)
	{
		m_vActiveLevel[ELogLevel(i)]=bActive;
	}
	return true;
}

boolean CLogListenerFile::activate(boolean bActive)
{
	return activate(LogLevel_First, LogLevel_Last, bActive);
}

void CLogListenerFile::log(const time64 time64Value)
{
	if(getKernelContext().getConfigurationManager().expandAsBoolean("${Kernel_FileLogTimeInSecond}",false))
	{
		uint64 l_ui64Precision = getKernelContext().getConfigurationManager().expandAsUInteger("${Kernel_FileLogTimePrecision}",3);
		float64 l_f64Time=(time64Value.m_ui64TimeValue>>22)/1024.;
		std::stringstream ss;
		ss.precision(l_ui64Precision);
		ss.setf(std::ios::fixed,std::ios::floatfield);
		ss << l_f64Time;
		ss << " sec";
		if(getConfigurationManager().expandAsBoolean("${Kernel_FileLogWithHexa}",true))
		{
			ss << " (0x" << hex << time64Value.m_ui64TimeValue << ")";
		}

		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, ss.str().c_str());
		);
	}
	else
	{
		if(getConfigurationManager().expandAsBoolean("${Kernel_FileLogWithHexa}",true))
		{
			_do_stuff_on_file_(l_pFile, "at",
				fprintf(l_pFile, "%llu (0x%llx)", time64Value.m_ui64TimeValue, time64Value.m_ui64TimeValue);
			);
		}
		else
		{
			_do_stuff_on_file_(l_pFile, "at",
				fprintf(l_pFile, "%llu", time64Value.m_ui64TimeValue);
			);
		}
	}
}

void CLogListenerFile::log(const uint64 ui64Value)
{
	if(getConfigurationManager().expandAsBoolean("${Kernel_FileLogWithHexa}",true))
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%llu (0x%llx)", ui64Value, ui64Value);
		);
	}
	else
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%llu", ui64Value);
		);
	}
}

void CLogListenerFile::log(const uint32 ui32Value)
{
	if(getConfigurationManager().expandAsBoolean("${Kernel_FileLogWithHexa}",true))
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%u (0x%x)", ui32Value, ui32Value);
		);
	}
	else
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%u", ui32Value);
		);
	}
}

void CLogListenerFile::log(const uint16 ui16Value)
{
	if(getConfigurationManager().expandAsBoolean("${Kernel_FileLogWithHexa}",true))
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%u (0x%x)", ui16Value, ui16Value);
		);
	}
	else
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%u", ui16Value);
		);
	}
}

void CLogListenerFile::log(const uint8 ui8Value)
{
	if(getConfigurationManager().expandAsBoolean("${Kernel_FileLogWithHexa}",true))
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%u (0x%x)", ui8Value, ui8Value);
		);
	}
	else
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%u", ui8Value);
		);
	}
}

void CLogListenerFile::log(const int64 i64Value)
{
	if(getConfigurationManager().expandAsBoolean("${Kernel_FileLogWithHexa}",true))
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%lli (0x%llx)", i64Value, i64Value);
		);
	}
	else
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%lli", i64Value);
		);
	}
	
}

void CLogListenerFile::log(const int32 i32Value)
{
	if(getConfigurationManager().expandAsBoolean("${Kernel_FileLogWithHexa}",true))
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%i (0x%x)", i32Value, i32Value);
		);
	}
	else
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%i", i32Value);
		);
	}
}

void CLogListenerFile::log(const int16 i16Value)
{
	if(getConfigurationManager().expandAsBoolean("${Kernel_FileLogWithHexa}",true))
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%i (0x%x)", i16Value, i16Value);
		);
	}
	else
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%i", i16Value);
		);
	}
}

void CLogListenerFile::log(const int8 i8Value)
{
	if(getConfigurationManager().expandAsBoolean("${Kernel_FileLogWithHexa}",true))
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%i (0x%x)", i8Value, i8Value);
		);
	}
	else
	{
		_do_stuff_on_file_(l_pFile, "at",
			fprintf(l_pFile, "%i", i8Value);
		);
	}
}

void CLogListenerFile::log(const float32 f32Value)
{
	_do_stuff_on_file_(l_pFile, "at",
		fprintf(l_pFile, "%f", f32Value);
	);
}

void CLogListenerFile::log(const float64 f64Value)
{
	_do_stuff_on_file_(l_pFile, "at",
		fprintf(l_pFile, "%lf", f64Value);
	);
}

void CLogListenerFile::log(const boolean bValue)
{
	_do_stuff_on_file_(l_pFile, "at",
		fprintf(l_pFile, "%s", (bValue?"true":"false"));
	);
}

void CLogListenerFile::log(const CIdentifier& rValue)
{
	CString l_sValue=rValue.toString();
	_do_stuff_on_file_(l_pFile, "at",
		fprintf(l_pFile, "%s", (const char*)l_sValue);
	);
}

void CLogListenerFile::log(const CString& rValue)
{
	_do_stuff_on_file_(l_pFile, "at",
		fprintf(l_pFile, "%s", (const char*)rValue);
	);
}

void CLogListenerFile::log(const char* pValue)
{
	_do_stuff_on_file_(l_pFile, "at",
		fprintf(l_pFile, "%s", pValue);
	);
}

void CLogListenerFile::log(const ELogLevel eLogLevel)
{
	switch(eLogLevel)
	{
		case LogLevel_Debug:
			{ _do_stuff_on_file_(l_pFile, "at", fprintf(l_pFile, "[ DEBUG ] "); ); }
			break;

		case LogLevel_Benchmark:
			{ _do_stuff_on_file_(l_pFile, "at", fprintf(l_pFile, "[ BENCH ] "); ); }
			break;

		case LogLevel_Trace:
			{ _do_stuff_on_file_(l_pFile, "at", fprintf(l_pFile, "[ TRACE ] "); ); }
			break;

		case LogLevel_Info:
			{ _do_stuff_on_file_(l_pFile, "at", fprintf(l_pFile, "[  INF  ] "); ); }
			break;

		case LogLevel_Warning:
			{ _do_stuff_on_file_(l_pFile, "at", fprintf(l_pFile, "[WARNING] "); ); }
			break;

		case LogLevel_ImportantWarning:
			{ _do_stuff_on_file_(l_pFile, "at", fprintf(l_pFile, "[WARNING] "); ); }
			break;

		case LogLevel_Error:
			{ _do_stuff_on_file_(l_pFile, "at", fprintf(l_pFile, "[ ERROR ] "); ); }
			break;

		case LogLevel_Fatal:
			{ _do_stuff_on_file_(l_pFile, "at", fprintf(l_pFile, "[ FATAL ] "); ); }
			break;

		default:
			{ _do_stuff_on_file_(l_pFile, "at", fprintf(l_pFile, "[UNKNOWN] "); ); }
			break;
	}
}

void CLogListenerFile::log(const ELogColor eLogColor)
{
}
