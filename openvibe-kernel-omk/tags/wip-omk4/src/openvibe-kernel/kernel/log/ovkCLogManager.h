#ifndef __OpenViBEKernel_Kernel_Log_CLogManager_h__
#define __OpenViBEKernel_Kernel_Log_CLogManager_h__

#include "../ovkTKernelObject.h"

#include <vector>
#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogManager : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogManager>
		{
		public:

			CLogManager(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual OpenViBE::boolean isActive(OpenViBE::Kernel::ELogLevel eLogLevel);
			virtual OpenViBE::boolean activate(OpenViBE::Kernel::ELogLevel eLogLevel, OpenViBE::boolean bActive);

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

			virtual OpenViBE::boolean addListener(OpenViBE::Kernel::ILogListener* pListener);
			virtual OpenViBE::boolean removeListener(OpenViBE::Kernel::ILogListener* pListener);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogManager>, OVK_ClassId_Kernel_Log_LogManager);

		protected:

			template <class T> void logForEach(T tValue)
			{
				if(m_eCurrentLogLevel!=LogLevel_None && this->isActive(m_eCurrentLogLevel))
				{
					std::vector<OpenViBE::Kernel::ILogListener*>::iterator i;
					for(i=m_vListener.begin(); i!=m_vListener.end(); i++)
					{
						if((*i)->isActive(m_eCurrentLogLevel))
						{
							(*i)->log(tValue);
						}
					}
				}
			}

		protected:

			std::vector<OpenViBE::Kernel::ILogListener*> m_vListener;
			std::map<OpenViBE::Kernel::ELogLevel, OpenViBE::boolean> m_vActiveLevel;
			ELogLevel m_eCurrentLogLevel;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Log_CLogManager_h__
