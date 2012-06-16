#ifndef __OpenViBEKernel_Kernel_Log_CLogListenerFile_H__
#define __OpenViBEKernel_Kernel_Log_CLogListenerFile_H__

#include "../ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerFile : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>
		{
		public:

			CLogListenerFile(const OpenViBE::Kernel::IKernelContext& rKernelContext, const CString& sApplicationName, const OpenViBE::CString& sLogFilename);

			virtual OpenViBE::boolean isActive(OpenViBE::Kernel::ELogLevel eLogLevel);
			virtual OpenViBE::boolean activate(OpenViBE::Kernel::ELogLevel eLogLevel, OpenViBE::boolean bActive);
			virtual OpenViBE::boolean activate(OpenViBE::Kernel::ELogLevel eStartLogLevel, OpenViBE::Kernel::ELogLevel eEndLogLevel, OpenViBE::boolean bActive);
			virtual OpenViBE::boolean activate(OpenViBE::boolean bActive);

			virtual void log(const OpenViBE::time64 time64Value);

			virtual void log(const OpenViBE::uint64 ui64Value);
			virtual void log(const OpenViBE::uint32 ui32Value);
			virtual void log(const OpenViBE::uint16 ui16Value);
			virtual void log(const OpenViBE::uint8 ui8Value);

			virtual void log(const OpenViBE::int64 i64Value);
			virtual void log(const OpenViBE::int32 i32Value);
			virtual void log(const OpenViBE::int16 i16Value);
			virtual void log(const OpenViBE::int8 i8Value);

			virtual void log(const OpenViBE::float64 f64Value);
			virtual void log(const OpenViBE::float32 f32Value);

			virtual void log(const OpenViBE::boolean bValue);

			virtual void log(const OpenViBE::CIdentifier& rValue);
			virtual void log(const OpenViBE::CString& rValue);
			virtual void log(const char* pValue);

			virtual void log(const OpenViBE::Kernel::ELogLevel eLogLevel);
			virtual void log(const OpenViBE::Kernel::ELogColor eLogColor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerFile);

		protected:

			std::map<OpenViBE::Kernel::ELogLevel, OpenViBE::boolean> m_vActiveLevel;
			OpenViBE::CString m_sApplicationName;
			OpenViBE::CString m_sLogFilename;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Log_CLogListenerFile_H__
