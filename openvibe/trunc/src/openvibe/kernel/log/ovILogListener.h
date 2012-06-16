#ifndef __OpenViBE_Kernel_Log_ILogListener_h__
#define __OpenViBE_Kernel_Log_ILogListener_h__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \brief Log levels
		 */
		enum ELogLevel
		{
			LogLevel_First,
			LogLevel_None,
			LogLevel_Debug,
			LogLevel_Benchmark,
			LogLevel_Trace,
			LogLevel_Info,
			LogLevel_Warning,
			LogLevel_ImportantWarning,
			LogLevel_Error,
			LogLevel_Fatal,
			LogLevel_Last,
		};

		/**
		 * \brief Log colors
		 */
		enum ELogColor
		{
			LogColor_ForegroundColorRedBit      =0x00000001,
			LogColor_ForegroundColorGreenBit    =0x00000002,
			LogColor_ForegroundColorBlueBit     =0x00000004,
			LogColor_ForegroundColorBit         =0x00000008,
			LogColor_ForegroundLightStateBit    =0x00000010,
			LogColor_ForegroundLightBit         =0x00000020,
			LogColor_ForegroundBlinkStateBit    =0x00000040,
			LogColor_ForegroundBlinkBit         =0x00000080,
			LogColor_ForegroundBoldStateBit     =0x00000100,
			LogColor_ForegroundBoldBit          =0x00000200,
			LogColor_ForegroundUnderlineStateBit=0x00000400,
			LogColor_ForegroundUnderlineBit     =0x00000800,

			LogColor_BackgroundColorRedBit      =0x00001000,
			LogColor_BackgroundColorGreenBit    =0x00002000,
			LogColor_BackgroundColorBlueBit     =0x00004000,
			LogColor_BackgroundColorBit         =0x00008000,
			LogColor_BackgroundLightStateBit    =0x00010000,
			LogColor_BackgroundLightBit         =0x00020000,
			LogColor_BackgroundBlinkStateBit    =0x00040000,
			LogColor_BackgroundBlinkBit         =0x00080000,

			LogColor_ForegroundBit              =0x00100000,
			LogColor_BackgroundBit              =0x00200000,
			LogColor_PushStateBit               =0x00400000,
			LogColor_PopStateBit                =0x00800000,
			LogColor_ResetBit                   =0x80000000,

			LogColor_Default                    =0x00000000,

			LogColor_ForegroundBlack            =0x00100008,
			LogColor_ForegroundRed              =0x00100009,
			LogColor_ForegroundGreen            =0x0010000A,
			LogColor_ForegroundYellow           =0x0010000B,
			LogColor_ForegroundBlue             =0x0010000C,
			LogColor_ForegroundMagenta          =0x0010000D,
			LogColor_ForegroundCyan             =0x0010000E,
			LogColor_ForegroundWhite            =0x0010000F,
			LogColor_ForegroundLightOff         =0x00100020,
			LogColor_ForegroundLightOn          =0x00100030,
			LogColor_ForegroundBlinkOff         =0x00100080,
			LogColor_ForegroundBlinkOn          =0x001000C0,
			LogColor_ForegroundBoldOff          =0x00100200,
			LogColor_ForegroundBoldOn           =0x00100300,
			LogColor_ForegroundUnderlineOff     =0x00100800,
			LogColor_ForegroundUnderlineOn      =0x00100C00,

			LogColor_BackgroundBlack            =0x00208000,
			LogColor_BackgroundRed              =0x00209000,
			LogColor_BackgroundGreen            =0x0020A000,
			LogColor_BackgroundYellow           =0x0020B000,
			LogColor_BackgroundBlue             =0x0020C000,
			LogColor_BackgroundMagenta          =0x0020D000,
			LogColor_BackgroundCyan             =0x0020E000,
			LogColor_BackgroundWhite            =0x0020F000,
			LogColor_BackgroundLightOff         =0x00220000,
			LogColor_BackgroundLightOn          =0x00230000,
			LogColor_BackgroundBlinkOff         =0x00280000,
			LogColor_BackgroundBlinkOn          =0x002C0000,
		};

		/**
		 * \class ILogListener
		 * \brief Log manager's listener interface
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-06-03
		 * \ingroup Group_Log
		 * \ingroup Group_Kernel
		 *
		 * The log listener is derived and implemented such as it can
		 * effectively process the log action. It could do it in a
		 * file, in a notification area, or whatever fits your needs.
		 * Objects to log are sent to it thanks to the log manager
		 * after a listener has been registered to it.
		 */
		class OV_API ILogListener : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/** \name Log level activation */
			//@{

			/**
			 * \brief Tests whether a log level is active or not
			 * \param eLogLevel [in] : the log level which has to be tested
			 * \return \e true if this log level is active.
			 * \return \e false if this log level is not active.
			 */
			virtual OpenViBE::boolean isActive(OpenViBE::Kernel::ELogLevel eLogLevel)=0;
			/**
			 * \brief Changes the activation status of a specific log level
			 * \param eLogLevel [in] : the log level which status has to be changed
			 * \param bActive [in] : a boolean telling whether this level should be active or not
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean activate(OpenViBE::Kernel::ELogLevel eLogLevel, OpenViBE::boolean bActive)=0;
			/**
			 * \brief Changes the activation status of a specific range of log level
			 * \param eStartLogLevel [in] : the first log level which status has to be changed
			 * \param eEndLogLevel [in] : the last log level which status has to be changed
			 * \param bActive [in] : a boolean telling whether these levels should be active or not
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean activate(OpenViBE::Kernel::ELogLevel eStartLogLevel, OpenViBE::Kernel::ELogLevel eEndLogLevel, OpenViBE::boolean bActive)=0;
			/**
			 * \brief Changes the activation status of all log levels at once
			 * \param bActive [in] : a boolean telling whether the levels should be active or not
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean activate(OpenViBE::boolean bActive)=0;

			//@}
			/** \name Logging function */
			//@{

			/**
			 * \brief Logs a formated time value (64 bits unsigned integer)
			 * \param ui64Value [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::time64 time64Value)=0;
			/**
			 * \brief Logs a 64 bits unsigned integer
			 * \param ui64Value [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::uint64 ui64Value)=0;
			/**
			 * \brief Logs a 32 bits unsigned integer
			 * \param ui32Value [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::uint32 ui32Value)=0;
			/**
			 * \brief Logs a 16 bits unsigned integer
			 * \param ui16Value [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::uint16 ui16Value)=0;
			/**
			 * \brief Logs a 8 bits unsigned integer
			 * \param ui8Value [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::uint8 ui8Value)=0;
			/**
			 * \brief Logs a 64 bits signed integer
			 * \param i64Value [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::int64 i64Value)=0;
			/**
			 * \brief Logs a 32 bits signed integer
			 * \param i32Value [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::int32 i32Value)=0;
			/**
			 * \brief Logs a 16 bits signed integer
			 * \param i16Value [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::int16 i16Value)=0;
			/**
			 * \brief Logs a 8 bits signed integer
			 * \param i8Value [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::int8 i8Value)=0;

			/**
			 * \brief Logs a 64 bits floating point value
			 * \param f64Value [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::float64 f64Value)=0;
			/**
			 * \brief Logs a 32 bits floating point value
			 * \param f32Value [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::float32 f32Value)=0;
			/**
			 * \brief Logs a boolean value
			 * \param bValue [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::boolean bValue)=0;
			/**
			 * \brief Logs an identifier value
			 * \param rValue [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::CIdentifier& rValue)=0;
			/**
			 * \brief Logs an §OpenViBE§ string value
			 * \param rValue [in] : the value that should be logged
			 */
			virtual void log(const OpenViBE::CString& rValue)=0;
			/**
			 * \brief Logs an ASCII string value
			 * \param rValue [in] : the value that should be logged
			 */
			virtual void log(const char* rValue)=0;

			//@}
			/** \name Manipulators */
			//@{

			/**
			 * \brief Changes the log level
			 * \param eLogLevel [in] : the new log level
			 */
			virtual void log(const OpenViBE::Kernel::ELogLevel eLogLevel)=0;
			/**
			 * \brief Changes the log color
			 * \param eLogColor [in] : the new log color
			 */
			virtual void log(const OpenViBE::Kernel::ELogColor eLogColor)=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Log_LogListener);
		};
	};
};

#endif // __OpenViBE_Kernel_Log_ILogListener_h__
