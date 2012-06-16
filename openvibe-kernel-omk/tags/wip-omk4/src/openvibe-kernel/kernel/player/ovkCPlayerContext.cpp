#include "ovkCPlayerContext.h"
#include "ovkPsSimulatedBox.h"

#include "../../tools/ovk_bridge_bind_function.h"

namespace OpenViBE
{
	namespace Kernel
	{
		namespace
		{
			class CAlgorithmManagerBridge : public TKernelObject<IAlgorithmManager>
			{
			public:

				CAlgorithmManagerBridge(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox) : TKernelObject<IAlgorithmManager>(rKernelContext), m_pSimulatedBox(pSimulatedBox) { }

				virtual __BridgeBindFunc1__(getKernelContext().getAlgorithmManager(), CIdentifier, createAlgorithm, , const CIdentifier&, rAlgorithmClassIdentifier)
				virtual __BridgeBindFunc1__(getKernelContext().getAlgorithmManager(), boolean, releaseAlgorithm, , const CIdentifier&, rAlgorithmIdentifier)
				virtual __BridgeBindFunc1__(getKernelContext().getAlgorithmManager(), boolean, releaseAlgorithm, , IAlgorithmProxy&, rAlgorithm)
				virtual __BridgeBindFunc1__(getKernelContext().getAlgorithmManager(), IAlgorithmProxy&, getAlgorithm, , const CIdentifier&, rAlgorithmIdentifier)
				virtual __BridgeBindFunc1__(getKernelContext().getAlgorithmManager(), CIdentifier, getNextAlgorithmIdentifier, const, const CIdentifier&, rPreviousIdentifier)

				_IsDerivedFromClass_Final_(TKernelObject<IAlgorithmManager>, OV_UndefinedIdentifier);

			protected:

				CSimulatedBox* m_pSimulatedBox;
			};

			class CLogManagerBridge : public TKernelObject<ILogManager>
			{
			public:

				CLogManagerBridge(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox) : TKernelObject<ILogManager>(rKernelContext), m_pSimulatedBox(pSimulatedBox) { }

				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const uint64, ui64Value)
				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const uint32, ui32Value)
				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const uint16, ui16Value)
				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const uint8, ui8Value)

				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const int64, i64Value)
				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const int32, i32Value)
				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const int16, i16Value)
				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const int8, i8Value)

				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const float32, f32Value)
				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const float64, f64Value)

				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const boolean, bValue)

				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const CIdentifier&, rValue)
				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const CString&, rValue);
				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const char*, rValue);

				// virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const ELogLevel, eLogLevel)
				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), void, log, , const ELogColor, eLogColor)

				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), boolean, addListener, , ILogListener*, pListener)
				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), boolean, removeListener, , ILogListener*, pListener)

				virtual __BridgeBindFunc1__(getKernelContext().getLogManager(), boolean, isActive, , ELogLevel, eLogLevel)
				virtual __BridgeBindFunc2__(getKernelContext().getLogManager(), boolean, activate, , ELogLevel, eLogLevel, boolean, bActive)

				virtual void log(const ELogLevel eLogLevel)
				{
					getKernelContext().getLogManager()
						<< eLogLevel
						<< "<"
						<< LogColor_PushStateBit
						<< LogColor_ForegroundBlue
						<< "Box algorithm"
						<< LogColor_PopStateBit
						<< "::"
						<< m_pSimulatedBox->getOVName()
						<< "> ";
				}

				_IsDerivedFromClass_Final_(TKernelObject<ILogManager>, OV_UndefinedIdentifier);

			protected:

				CSimulatedBox* m_pSimulatedBox;
			};

			class CScenarioManagerBridge : public TKernelObject<IScenarioManager>
			{
			public:

				CScenarioManagerBridge(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox) : TKernelObject<IScenarioManager>(rKernelContext), m_pSimulatedBox(pSimulatedBox) { }

				virtual __BridgeBindFunc1__(getKernelContext().getScenarioManager(), CIdentifier, getNextScenarioIdentifier, const, const CIdentifier&, rPreviousIdentifier)
				virtual __BridgeBindFunc1__(getKernelContext().getScenarioManager(), boolean, createScenario, , CIdentifier&, rScenarioIdentifier)
				virtual __BridgeBindFunc1__(getKernelContext().getScenarioManager(), boolean, releaseScenario, , const CIdentifier&, rScenarioIdentifier)
				virtual __BridgeBindFunc1__(getKernelContext().getScenarioManager(), IScenario&, getScenario, , const CIdentifier&, rScenarioIdentifier)

				_IsDerivedFromClass_Final_(TKernelObject<IScenarioManager>, OV_UndefinedIdentifier);

			protected:

				CSimulatedBox* m_pSimulatedBox;
			};

			class CTypeManagerBridge : public TKernelObject<ITypeManager>
			{
			public:

				CTypeManagerBridge(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox) : TKernelObject<ITypeManager>(rKernelContext), m_pSimulatedBox(pSimulatedBox) { }

				virtual __BridgeBindFunc1__(getKernelContext().getTypeManager(), CIdentifier, getNextTypeIdentifier, const, const CIdentifier&, rPreviousIdentifier);
				virtual __BridgeBindFunc2__(getKernelContext().getTypeManager(), boolean, registerType, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName)
				virtual __BridgeBindFunc3__(getKernelContext().getTypeManager(), boolean, registerStreamType, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName, const CIdentifier&, rParentTypeIdentifier)
				virtual __BridgeBindFunc2__(getKernelContext().getTypeManager(), boolean, registerEnumerationType, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName)
				virtual __BridgeBindFunc3__(getKernelContext().getTypeManager(), boolean, registerEnumerationEntry, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName, const uint64, ui64ValueIndex)
				virtual __BridgeBindFunc2__(getKernelContext().getTypeManager(), boolean, registerBitMaskType, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName)
				virtual __BridgeBindFunc3__(getKernelContext().getTypeManager(), boolean, registerBitMaskEntry, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName, const uint64, ui64ValueIndex)

				virtual __BridgeBindFunc1__(getKernelContext().getTypeManager(), boolean, isRegistered, , const CIdentifier&, rTypeIdentifier)
				virtual __BridgeBindFunc1__(getKernelContext().getTypeManager(), boolean, isStream, , const CIdentifier&, rTypeIdentifier)
				virtual __BridgeBindFunc1__(getKernelContext().getTypeManager(), boolean, isEnumeration, , const CIdentifier&, rTypeIdentifier)
				virtual __BridgeBindFunc1__(getKernelContext().getTypeManager(), boolean, isBitMask, , const CIdentifier&, rTypeIdentifier)

				virtual __BridgeBindFunc1__(getKernelContext().getTypeManager(), CString, getTypeName, , const CIdentifier&, rTypeIdentifier)
				virtual __BridgeBindFunc1__(getKernelContext().getTypeManager(), CIdentifier, getStreamParentType, , const CIdentifier&, rTypeIdentifier)
				virtual __BridgeBindFunc1__(getKernelContext().getTypeManager(), uint64, getEnumerationEntryCount, , const CIdentifier&, rTypeIdentifier)
				virtual __BridgeBindFunc4__(getKernelContext().getTypeManager(), boolean, getEnumerationEntry, , const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryIndex, CString&, sEntryName, uint64&, ui64EntryValue)
				virtual __BridgeBindFunc2__(getKernelContext().getTypeManager(), CString, getEnumerationEntryNameFromValue, , const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryValue)
				virtual __BridgeBindFunc2__(getKernelContext().getTypeManager(), uint64, getEnumerationEntryValueFromName, , const CIdentifier&, rTypeIdentifier, const CString&, sEntryName)
				virtual __BridgeBindFunc1__(getKernelContext().getTypeManager(), uint64, getBitMaskEntryCount, , const CIdentifier&, rTypeIdentifier)
				virtual __BridgeBindFunc4__(getKernelContext().getTypeManager(), boolean, getBitMaskEntry, , const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryIndex, CString&, sEntryName, uint64&, ui64EntryValue)
				virtual __BridgeBindFunc2__(getKernelContext().getTypeManager(), CString, getBitMaskEntryNameFromValue, , const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryValue)
				virtual __BridgeBindFunc2__(getKernelContext().getTypeManager(), uint64, getBitMaskEntryValueFromName, , const CIdentifier&, rTypeIdentifier, const CString&, sEntryName)

				_IsDerivedFromClass_Final_(TKernelObject<ITypeManager>, OV_UndefinedIdentifier);

			protected:

				CSimulatedBox* m_pSimulatedBox;
			};
		};
	};
};

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
#define boolean OpenViBE::boolean

CPlayerContext::CPlayerContext(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox)
	:TKernelObject<IPlayerContext>(rKernelContext)
	,m_pSimulatedBox(pSimulatedBox)
	,m_pAlgorithmManagerBridge(NULL)
	,m_pLogManagerBridge(NULL)
	,m_pScenarioManagerBridge(NULL)
	,m_pTypeManagerBridge(NULL)
{
	m_pAlgorithmManagerBridge=new CAlgorithmManagerBridge(rKernelContext, pSimulatedBox);
	m_pLogManagerBridge=new CLogManagerBridge(rKernelContext, pSimulatedBox);
	m_pScenarioManagerBridge=new CScenarioManagerBridge(rKernelContext, pSimulatedBox);
	m_pTypeManagerBridge=new CTypeManagerBridge(rKernelContext, pSimulatedBox);
}

CPlayerContext::~CPlayerContext(void)
{
	delete m_pTypeManagerBridge;
	delete m_pScenarioManagerBridge;
	delete m_pLogManagerBridge;
	delete m_pAlgorithmManagerBridge;
}

boolean CPlayerContext::sendSignal(
	const CMessageSignal& rMessageSignal)
{
	// TODO
	log() << LogLevel_Debug << "CPlayerContext::sendSignal - Not yet implemented\n";
	return false;
}

boolean CPlayerContext::sendMessage(
	const CMessageEvent& rMessageEvent,
	const CIdentifier& rTargetIdentifier)
{
	// TODO
	log() << LogLevel_Debug << "CPlayerContext::sendMessage - Not yet implemented\n";
	return false;
}

boolean CPlayerContext::sendMessage(
	const CMessageEvent& rMessageEvent,
	const CIdentifier* pTargetIdentifier,
	const uint32 ui32TargetIdentifierCount)
{
	// TODO
	log() << LogLevel_Debug << "CPlayerContext::sendMessage - Not yet implemented\n";
	return false;
}

uint64 CPlayerContext::getCurrentTime(void)
{
	return m_pSimulatedBox->getCurrentTime();
}

IAlgorithmManager& CPlayerContext::getAlgorithmManager(void)
{
	return *m_pAlgorithmManagerBridge;
}

ILogManager& CPlayerContext::getLogManager(void)
{
	return *m_pLogManagerBridge;
}

IScenarioManager& CPlayerContext::getScenarioManager(void)
{
	return *m_pScenarioManagerBridge;
}

ITypeManager& CPlayerContext::getTypeManager(void)
{
	return *m_pTypeManagerBridge;
}
