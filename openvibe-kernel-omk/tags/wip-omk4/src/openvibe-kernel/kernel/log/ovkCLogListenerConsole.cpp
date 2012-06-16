#include "ovkCLogListenerConsole.h"

#include <iostream>

#if defined OVK_OS_Windows
 #include <windows.h>
 #define boolean OpenViBE::boolean
#endif

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

CLogListenerConsole::CLogListenerConsole(const IKernelContext& rKernelContext)
	:TKernelObject<ILogListener>(rKernelContext)
	,m_eLogColor(LogColor_Default)
{
}

boolean CLogListenerConsole::isActive(ELogLevel eLogLevel)
{
	map<ELogLevel, boolean>::iterator itLogLevel=m_vActiveLevel.find(eLogLevel);
	if(itLogLevel==m_vActiveLevel.end())
	{
		return true;
	}
	return itLogLevel->second;
}

boolean CLogListenerConsole::activate(ELogLevel eLogLevel, boolean bActive)
{
	m_vActiveLevel[eLogLevel]=bActive;
	return true;
}

void CLogListenerConsole::log(const uint64 ui64Value)
{
	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat=cout.flags();
	cout << dec << ui64Value << " (0x" << hex << ui64Value << ")";
	cout.flags(l_oFormat);
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const uint32 ui32Value)
{
	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat=cout.flags();
	cout << dec << ui32Value << " (0x" << hex << ui32Value << ")";
	cout.flags(l_oFormat);
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const uint16 ui16Value)
{
	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat=cout.flags();
	cout << dec << ui16Value << " (0x" << hex << ui16Value << ")";
	cout.flags(l_oFormat);
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const uint8 ui8Value)
{
	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat=cout.flags();
	cout << dec << ui8Value << " (0x" << hex << ui8Value << ")";
	cout.flags(l_oFormat);
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const int64 i64Value)
{
	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat=cout.flags();
	cout << dec << i64Value << " (0x" << hex << i64Value << ")";
	cout.flags(l_oFormat);
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const int32 i32Value)
{	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat=cout.flags();
	cout << dec << i32Value << " (0x" << hex << i32Value << ")";
	cout.flags(l_oFormat);
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const int16 i16Value)
{
	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat=cout.flags();
	cout << dec << i16Value << " (0x" << hex << i16Value << ")";
	cout.flags(l_oFormat);
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const int8 i8Value)
{
	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat=cout.flags();
	cout << dec << i8Value << " (0x" << hex << i8Value << ")";
	cout.flags(l_oFormat);
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const float64 f64Value)
{
	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	cout << f64Value;
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const float32 f32Value)
{
	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	cout << f32Value;
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const boolean bValue)
{
	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	cout << (bValue?"true":"false");
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const CIdentifier& rValue)
{
	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	cout << rValue.toString();
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const CString& rValue)
{
	log(LogColor_PushStateBit);
	log(LogColor_ForegroundMagenta);
	cout << rValue;
	log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const char* pValue)
{
	cout << pValue;
}

void CLogListenerConsole::log(const ELogLevel eLogLevel)
{
	switch(eLogLevel)
	{
		case LogLevel_Debug:
			cout << "[";
			log(LogColor_PushStateBit);
			log(LogColor_ForegroundBlue);
			cout << " DEBUG ";
			log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_Benchmark:
			cout << "[";
			log(LogColor_PushStateBit);
			log(LogColor_ForegroundMagenta);
			cout << " BENCH ";
			log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_Trace:
			cout << "[";
			log(LogColor_PushStateBit);
			log(LogColor_ForegroundYellow);
			cout << " TRACE ";
			log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_Info:
			cout << "[";
			log(LogColor_PushStateBit);
			log(LogColor_ForegroundGreen);
			cout << "  INF  ";
			log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_Warning:
			cout << "[";
			log(LogColor_PushStateBit);
			log(LogColor_ForegroundCyan);
			cout << "WARNING";
			log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_ImportantWarning:
			cout << "[";
			log(LogColor_PushStateBit);
			log(LogColor_ForegroundRed);
			cout << "WARNING";
			log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_Error:
			cout << "[";
			log(LogColor_PushStateBit);
			log(LogColor_ForegroundRed);
			cout << " ERROR ";
			log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_Fatal:
			cout << "[";
			log(LogColor_PushStateBit);
			log(LogColor_ForegroundRed);
			cout << " FATAL ";
			log(LogColor_PopStateBit);
			cout << "]";
			break;

		default:
			cout << "[UNKNOWN]";
			break;
	}
	cout << " ";
}

void CLogListenerConsole::log(const ELogColor eLogColor)
{
	// Tests 'push state' bit
	if(eLogColor&LogColor_PushStateBit)
	{
		m_vLogColor.push(m_eLogColor);
	}

	// Tests 'pop state' bit
	if(eLogColor&LogColor_PopStateBit)
	{
		if(m_vLogColor.size())
		{
			m_eLogColor=m_vLogColor.top();
			m_vLogColor.pop();
		}
		else
		{
			m_eLogColor=LogColor_Default;
		}
	}

	// Tests 'reset' bit
	if(eLogColor&LogColor_ResetBit)
	{
		m_eLogColor=LogColor_Default;
	}

	// Tests 'foreground' bit
	if(eLogColor&LogColor_ForegroundBit)
	{
		// Tests 'color' bit
		if(eLogColor&LogColor_ForegroundColorBit)
		{
			ELogColor l_eColorMask=ELogColor(LogColor_ForegroundColorRedBit|LogColor_ForegroundColorGreenBit|LogColor_ForegroundColorBlueBit);
			m_eLogColor=ELogColor(m_eLogColor&(~l_eColorMask));
			m_eLogColor=ELogColor(m_eLogColor|(eLogColor&l_eColorMask)|LogColor_ForegroundBit|LogColor_ForegroundColorBit);
		}

		// Test 'light' bit
		if(eLogColor&LogColor_ForegroundLightBit)
		{
			ELogColor l_eLightMask=ELogColor(LogColor_ForegroundLightBit|LogColor_ForegroundLightStateBit);
			m_eLogColor=ELogColor(m_eLogColor&(~l_eLightMask));
			m_eLogColor=ELogColor(m_eLogColor|(eLogColor&l_eLightMask)|LogColor_ForegroundBit);
		}

		// Test 'blink' bit
		if(eLogColor&LogColor_ForegroundBlinkBit)
		{
			ELogColor l_eBlinkMask=ELogColor(LogColor_ForegroundBlinkBit|LogColor_ForegroundBlinkStateBit);
			m_eLogColor=ELogColor(m_eLogColor&(~l_eBlinkMask));
			m_eLogColor=ELogColor(m_eLogColor|(eLogColor&l_eBlinkMask)|LogColor_ForegroundBit);
		}

		// Test 'bold' bit
		if(eLogColor&LogColor_ForegroundBoldBit)
		{
			ELogColor l_eBoldMask=ELogColor(LogColor_ForegroundBoldBit|LogColor_ForegroundBoldStateBit);
			m_eLogColor=ELogColor(m_eLogColor&(~l_eBoldMask));
			m_eLogColor=ELogColor(m_eLogColor|(eLogColor&l_eBoldMask)|LogColor_ForegroundBit);
		}

		// Test 'underline' bit
		if(eLogColor&LogColor_ForegroundUnderlineBit)
		{
			ELogColor l_eUnderlineMask=ELogColor(LogColor_ForegroundBlinkBit|LogColor_ForegroundBlinkStateBit);
			m_eLogColor=ELogColor(m_eLogColor&(~l_eUnderlineMask));
			m_eLogColor=ELogColor(m_eLogColor|(eLogColor&l_eUnderlineMask)|LogColor_ForegroundBit);
		}
	}

	// Tests 'background' bit
	if(eLogColor&LogColor_BackgroundBit)
	{
		// Tests 'color' bit
		if(eLogColor&LogColor_BackgroundColorBit)
		{
			ELogColor l_eColorMask=ELogColor(LogColor_BackgroundColorRedBit|LogColor_BackgroundColorGreenBit|LogColor_BackgroundColorBlueBit);
			m_eLogColor=ELogColor(m_eLogColor&(~l_eColorMask));
			m_eLogColor=ELogColor(m_eLogColor|(eLogColor&l_eColorMask)|LogColor_BackgroundBit|LogColor_BackgroundColorBit);
		}

		// Test 'light' bit
		if(eLogColor&LogColor_BackgroundLightBit)
		{
			ELogColor l_eLightMask=ELogColor(LogColor_BackgroundLightBit|LogColor_BackgroundLightStateBit);
			m_eLogColor=ELogColor(m_eLogColor&(~l_eLightMask));
			m_eLogColor=ELogColor(m_eLogColor|(eLogColor&l_eLightMask)|LogColor_BackgroundBit);
		}

		// Test 'blink' bit
		if(eLogColor&LogColor_BackgroundBlinkBit)
		{
			ELogColor l_eBlinkMask=ELogColor(LogColor_BackgroundBlinkBit|LogColor_BackgroundBlinkStateBit);
			m_eLogColor=ELogColor(m_eLogColor&(~l_eBlinkMask));
			m_eLogColor=ELogColor(m_eLogColor|(eLogColor&l_eBlinkMask)|LogColor_BackgroundBit);
		}
	}

	// Finally applies current color
	applyColor();
}

#if defined OVK_OS_Linux

void CLogListenerConsole::applyColor(void)
{
	int l_iGotACommand=0;

	#define _command_separator_ (l_iGotACommand++?";":"")

	cout << "\033[00m";

	if(m_eLogColor!=LogColor_Default)
	{
		cout << "\033[";

		if(m_eLogColor&LogColor_ForegroundBit)
		{
			if(m_eLogColor&LogColor_ForegroundLightBit && m_eLogColor&LogColor_ForegroundLightStateBit)
			{
				// No function to do that
			}

			if(m_eLogColor&LogColor_ForegroundBoldBit && m_eLogColor&LogColor_ForegroundBoldStateBit)
			{
				cout << _command_separator_ << "01";
			}

			if(m_eLogColor&LogColor_ForegroundUnderlineBit && m_eLogColor&LogColor_ForegroundUnderlineStateBit)
			{
				cout << _command_separator_ << "04";
			}

			if(m_eLogColor&LogColor_ForegroundBlinkBit && m_eLogColor&LogColor_ForegroundBlinkStateBit)
			{
				cout << _command_separator_ << "05";
			}

			if(m_eLogColor&LogColor_ForegroundColorBit)
			{
				ELogColor l_eLogColor=ELogColor(m_eLogColor&(LogColor_ForegroundBit|LogColor_ForegroundColorBit|LogColor_ForegroundColorRedBit|LogColor_ForegroundColorGreenBit|LogColor_ForegroundColorBlueBit));
				switch(l_eLogColor)
				{
					case LogColor_ForegroundBlack:    cout << _command_separator_ << "30"; break;
					case LogColor_ForegroundRed:      cout << _command_separator_ << "31"; break;
					case LogColor_ForegroundGreen:    cout << _command_separator_ << "32"; break;
					case LogColor_ForegroundYellow:   cout << _command_separator_ << "33"; break;
					case LogColor_ForegroundBlue:     cout << _command_separator_ << "34"; break;
					case LogColor_ForegroundMagenta:  cout << _command_separator_ << "35"; break;
					case LogColor_ForegroundCyan:     cout << _command_separator_ << "36"; break;
					case LogColor_ForegroundWhite:    cout << _command_separator_ << "37"; break;
					default: break;
				}
			}
		}

		if(m_eLogColor&LogColor_BackgroundBit)
		{
			if(m_eLogColor&LogColor_BackgroundColorBit)
			{
				ELogColor l_eLogColor=ELogColor(m_eLogColor&(LogColor_BackgroundBit|LogColor_BackgroundColorBit|LogColor_BackgroundColorRedBit|LogColor_BackgroundColorGreenBit|LogColor_BackgroundColorBlueBit));
				switch(l_eLogColor)
				{
					case LogColor_BackgroundBlack:    cout << _command_separator_ << "40"; break;
					case LogColor_BackgroundRed:      cout << _command_separator_ << "41"; break;
					case LogColor_BackgroundGreen:    cout << _command_separator_ << "42"; break;
					case LogColor_BackgroundYellow:   cout << _command_separator_ << "43"; break;
					case LogColor_BackgroundBlue:     cout << _command_separator_ << "44"; break;
					case LogColor_BackgroundMagenta:  cout << _command_separator_ << "45"; break;
					case LogColor_BackgroundCyan:     cout << _command_separator_ << "46"; break;
					case LogColor_BackgroundWhite:    cout << _command_separator_ << "47"; break;
					default: break;
				}
			}

			if(m_eLogColor&LogColor_BackgroundLightBit && m_eLogColor&LogColor_BackgroundLightStateBit)
			{
				// No function to do that
			}

			if(m_eLogColor&LogColor_BackgroundBlinkBit && m_eLogColor&LogColor_BackgroundBlinkStateBit)
			{
				// No function to do that
			}
		}

		cout << "m";
	}

	#undef _command_separator_
}

#elif defined OVK_OS_Windows

void CLogListenerConsole::applyColor(void)
{
	WORD l_dwTextAttribute=0;

	if(m_eLogColor&LogColor_ForegroundBit)
	{
		if(m_eLogColor&LogColor_ForegroundColorBit)
		{
			l_dwTextAttribute|=(m_eLogColor&LogColor_ForegroundColorRedBit?FOREGROUND_RED:0);
			l_dwTextAttribute|=(m_eLogColor&LogColor_ForegroundColorGreenBit?FOREGROUND_GREEN:0);
			l_dwTextAttribute|=(m_eLogColor&LogColor_ForegroundColorBlueBit?FOREGROUND_BLUE:0);
		}
		else
		{
			l_dwTextAttribute|=FOREGROUND_RED;
			l_dwTextAttribute|=FOREGROUND_GREEN;
			l_dwTextAttribute|=FOREGROUND_BLUE;
		}

		if(m_eLogColor&LogColor_ForegroundLightBit && m_eLogColor&LogColor_ForegroundLightStateBit)
		{
			l_dwTextAttribute|=FOREGROUND_INTENSITY;
		}

		if(m_eLogColor&LogColor_ForegroundBoldBit && m_eLogColor&LogColor_ForegroundBoldStateBit)
		{
			// No function to do that
		}

		if(m_eLogColor&LogColor_ForegroundUnderlineBit && m_eLogColor&LogColor_ForegroundUnderlineStateBit)
		{
			l_dwTextAttribute|=COMMON_LVB_UNDERSCORE;
		}

		if(m_eLogColor&LogColor_ForegroundBlinkBit && m_eLogColor&LogColor_ForegroundBlinkStateBit)
		{
			// No function to do that
		}
	}
	else
	{
		l_dwTextAttribute|=FOREGROUND_RED;
		l_dwTextAttribute|=FOREGROUND_GREEN;
		l_dwTextAttribute|=FOREGROUND_BLUE;
	}

	if(m_eLogColor&LogColor_BackgroundBit)
	{
		if(m_eLogColor&LogColor_BackgroundColorBit)
		{
			l_dwTextAttribute|=(m_eLogColor&LogColor_BackgroundColorRedBit?BACKGROUND_RED:0);
			l_dwTextAttribute|=(m_eLogColor&LogColor_BackgroundColorGreenBit?BACKGROUND_GREEN:0);
			l_dwTextAttribute|=(m_eLogColor&LogColor_BackgroundColorBlueBit?BACKGROUND_BLUE:0);
		}

		if(m_eLogColor&LogColor_BackgroundLightBit && m_eLogColor&LogColor_BackgroundLightStateBit)
		{
			l_dwTextAttribute|=BACKGROUND_INTENSITY;
		}

		if(m_eLogColor&LogColor_BackgroundBlinkBit && m_eLogColor&LogColor_BackgroundBlinkStateBit)
		{
			// No function to do that
		}
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), l_dwTextAttribute);
}

#else

void CLogListenerConsole::applyColor(void)
{
}

#endif
