#include "ovkCObjectVisitorContext.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

namespace OpenViBE
{
	namespace Kernel
	{
		namespace
		{
			class CLogManagerBridge : public TKernelObject<ILogManager>
			{
			public:

				CLogManagerBridge(const IKernelContext& rKernelContext) : TKernelObject<ILogManager>(rKernelContext) { }

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const uint64, ui64Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const uint32, ui32Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const uint16, ui16Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const uint8, ui8Value)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const int64, i64Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const int32, i32Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const int16, i16Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const int8, i8Value)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const float32, f32Value)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const float64, f64Value)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const boolean, bValue)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const CIdentifier&, rValue)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const CString&, rValue);
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const char*, rValue);

				// __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const ELogLevel, eLogLevel)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const ELogColor, eLogColor)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), boolean, addListener, , ILogListener*, pListener)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), boolean, removeListener, , ILogListener*, pListener)

				__BridgeBindFunc1__(getKernelContext().getLogManager(), boolean, isActive, , ELogLevel, eLogLevel)
				__BridgeBindFunc2__(getKernelContext().getLogManager(), boolean, activate, , ELogLevel, eLogLevel, boolean, bActive)

				void log(const ELogLevel eLogLevel)
				{
					getKernelContext().getLogManager()
						<< eLogLevel
						<< "<"
						<< LogColor_PushStateBit
						<< LogColor_ForegroundBlue
						<< "ObjectVisitor"
						<< LogColor_PopStateBit
						<< "::"
						<< "unnamed"
						<< "> ";
				}

				_IsDerivedFromClass_Final_(TKernelObject<ILogManager>, OV_UndefinedIdentifier);
			};
		};
	};
};

CObjectVisitorContext::CObjectVisitorContext(const IKernelContext& rKernelContext)
	:TKernelObject < IObjectVisitorContext >(rKernelContext)
	,m_pLogManager(NULL)
{
	m_pLogManager=new CLogManagerBridge(rKernelContext);
}

CObjectVisitorContext::~CObjectVisitorContext(void)
{
	delete m_pLogManager;
}

ILogManager& CObjectVisitorContext::getLogManager(void)
{
	return *m_pLogManager;
}
