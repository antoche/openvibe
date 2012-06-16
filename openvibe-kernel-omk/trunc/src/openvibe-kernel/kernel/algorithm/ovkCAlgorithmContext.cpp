#include "ovkCAlgorithmContext.h"
#include "ovkCAlgorithm.h"

#include "../../tools/ovk_bridge_bind_function.h"

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

				CLogManagerBridge(const IKernelContext& rKernelContext, const ::IPluginObjectDesc& rPluginObjectDesc) : TKernelObject<ILogManager>(rKernelContext), m_rPluginObjectDesc(rPluginObjectDesc) { }

				__BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const time64, time64Value)

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
				__BridgeBindFunc3__(getKernelContext().getLogManager(), boolean, activate, , ELogLevel, eStartLogLevel, ELogLevel, eEndLogLevel, boolean, bActive)
				__BridgeBindFunc1__(getKernelContext().getLogManager(), boolean, activate, , boolean, bActive)

				void log(const ELogLevel eLogLevel)
				{
					getKernelContext().getLogManager()
						<< eLogLevel
						<< "<"
						<< LogColor_PushStateBit
						<< LogColor_ForegroundBlue
						<< "Algorithm"
						<< LogColor_PopStateBit
						<< "::"
						<< m_rPluginObjectDesc.getName()
						<< "> ";
				}

				_IsDerivedFromClass_Final_(TKernelObject<ILogManager>, OV_UndefinedIdentifier);

			protected:

				const ::IPluginObjectDesc& m_rPluginObjectDesc;
			};
		};
	};
};

CAlgorithmContext::CAlgorithmContext(const IKernelContext& rKernelContext, CAlgorithm& rAlgorithm, const IPluginObjectDesc& rPluginObjectDesc)
	:TKernelObject < IAlgorithmContext >(rKernelContext)
	,m_pLogManager(NULL)
	,m_rAlgorithm(rAlgorithm)
{
	m_pLogManager=new CLogManagerBridge(rKernelContext, rPluginObjectDesc);
}

CAlgorithmContext::~CAlgorithmContext(void)
{
	delete m_pLogManager;
}

IConfigurationManager& CAlgorithmContext::getConfigurationManager(void)
{
	return getKernelContext().getConfigurationManager();
}

IAlgorithmManager& CAlgorithmContext::getAlgorithmManager(void)
{
	return getKernelContext().getAlgorithmManager();
}

ILogManager& CAlgorithmContext::getLogManager(void)
{
	return *m_pLogManager;
}

ITypeManager& CAlgorithmContext::getTypeManager(void)
{
	return getKernelContext().getTypeManager();
}

CIdentifier CAlgorithmContext::getNextInputParameterIdentifier(
	const CIdentifier& rPreviousInputParameterIdentifier) const
{
	return m_rAlgorithm.getNextInputParameterIdentifier(rPreviousInputParameterIdentifier);
}

IParameter* CAlgorithmContext::getInputParameter(
	const CIdentifier& rInputParameterIdentifier)
{
	return m_rAlgorithm.getInputParameter(rInputParameterIdentifier);
}

CIdentifier CAlgorithmContext::getNextOutputParameterIdentifier(
	const CIdentifier& rPreviousOutputParameterIdentifier) const
{
	return m_rAlgorithm.getNextOutputParameterIdentifier(rPreviousOutputParameterIdentifier);
}

IParameter* CAlgorithmContext::getOutputParameter(
	const CIdentifier& rOutputParameterIdentifier)
{
	return m_rAlgorithm.getOutputParameter(rOutputParameterIdentifier);
}

boolean CAlgorithmContext::isInputTriggerActive(
	const CIdentifier& rInputTriggerIdentifier) const
{
	return m_rAlgorithm.isInputTriggerActive(rInputTriggerIdentifier);
}

boolean CAlgorithmContext::activateOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier,
	const boolean bTriggerState)
{
	return m_rAlgorithm.activateOutputTrigger(rOutputTriggerIdentifier, bTriggerState);
}
