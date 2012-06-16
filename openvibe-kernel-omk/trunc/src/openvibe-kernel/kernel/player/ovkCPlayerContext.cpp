#include "ovkCPlayerContext.h"
#include "ovkCSimulatedBox.h"
#include "ovkCScheduler.h"
#include "ovkCPlayer.h"

#include "../../tools/ovk_bridge_bind_function.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

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

				__BridgeBindFunc1__(getKernelContext().getAlgorithmManager(), CIdentifier, createAlgorithm, , const CIdentifier&, rAlgorithmClassIdentifier)
				__BridgeBindFunc1__(getKernelContext().getAlgorithmManager(), boolean, releaseAlgorithm, , const CIdentifier&, rAlgorithmIdentifier)
				__BridgeBindFunc1__(getKernelContext().getAlgorithmManager(), boolean, releaseAlgorithm, , IAlgorithmProxy&, rAlgorithm)
				__BridgeBindFunc1__(getKernelContext().getAlgorithmManager(), IAlgorithmProxy&, getAlgorithm, , const CIdentifier&, rAlgorithmIdentifier)
				__BridgeBindFunc1__(getKernelContext().getAlgorithmManager(), CIdentifier, getNextAlgorithmIdentifier, const, const CIdentifier&, rPreviousIdentifier)

				_IsDerivedFromClass_Final_(TKernelObject<IAlgorithmManager>, OV_UndefinedIdentifier);

			protected:

				CSimulatedBox* m_pSimulatedBox;
			};

			class CConfigurationManagerBridge : public TKernelObject<IConfigurationManager>
			{
			public:

				CConfigurationManagerBridge(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox) : TKernelObject<IConfigurationManager>(rKernelContext), m_pSimulatedBox(pSimulatedBox) { }

				__BridgeBindFunc0__(getKernelContext().getConfigurationManager(), void, clear, );

				__BridgeBindFunc1__(getKernelContext().getConfigurationManager(), boolean, addConfigurationFromFile, , const CString&, rFileNameWildCard);

				__BridgeBindFunc2__(getKernelContext().getConfigurationManager(), CIdentifier, createConfigurationToken, , const CString&, rConfigurationTokenName, const CString&, rConfigurationTokenValue);
				__BridgeBindFunc1__(getKernelContext().getConfigurationManager(), boolean, releaseConfigurationToken, , const CIdentifier&, rConfigurationTokenIdentifier);
				__BridgeBindFunc1__(getKernelContext().getConfigurationManager(), CIdentifier, getNextConfigurationTokenIdentifier, const, const CIdentifier&, rPreviousConfigurationTokenIdentifier);

				__BridgeBindFunc1__(getKernelContext().getConfigurationManager(), CString, getConfigurationTokenName, const, const CIdentifier&, rConfigurationTokenIdentifier);
				__BridgeBindFunc1__(getKernelContext().getConfigurationManager(), CString, getConfigurationTokenValue, const, const CIdentifier&, rConfigurationTokenIdentifier);

				__BridgeBindFunc2__(getKernelContext().getConfigurationManager(), boolean, setConfigurationTokenName, , const CIdentifier&, rConfigurationTokenIdentifier, const CString&, rConfigurationTokenName);
				__BridgeBindFunc2__(getKernelContext().getConfigurationManager(), boolean, setConfigurationTokenValue, , const CIdentifier&, rConfigurationTokenIdentifier, const CString&, rConfigurationTokenValue);

				__BridgeBindFunc2__(getKernelContext().getConfigurationManager(), CIdentifier, lookUpConfigurationTokenIdentifier, const, const CString&, rConfigurationTokenName, const boolean, bRecursive);
				__BridgeBindFunc1__(getKernelContext().getConfigurationManager(), CString, lookUpConfigurationTokenValue, const, const CString&, rConfigurationTokenName);
				__BridgeBindFunc1__(getKernelContext().getConfigurationManager(), CString, expand, const, const CString&, rExpression);


				__BridgeBindFunc2__(getKernelContext().getConfigurationManager(), float64, expandAsFloat, const, const CString&, rExpression, const float64, f64FallbackValue);
				__BridgeBindFunc2__(getKernelContext().getConfigurationManager(), int64, expandAsInteger, const, const CString&, rExpression, const int64, i64FallbackValue);
				__BridgeBindFunc2__(getKernelContext().getConfigurationManager(), uint64, expandAsUInteger, const, const CString&, rExpression, const uint64, ui64FallbackValue);
				__BridgeBindFunc2__(getKernelContext().getConfigurationManager(), boolean, expandAsBoolean, const, const CString&, rExpression, const boolean, bFallbackValue);
				__BridgeBindFunc3__(getKernelContext().getConfigurationManager(), uint64, expandAsEnumerationEntryValue, const, const CString&, rExpression, const CIdentifier&, rEnumerationTypeIdentifier, const uint64, ui64FallbackValue);

				_IsDerivedFromClass_Final_(TKernelObject<IConfigurationManager>, OV_UndefinedIdentifier);

			protected:

				CSimulatedBox* m_pSimulatedBox;
			};

			class CLogManagerBridge : public TKernelObject<ILogManager>
			{
			public:

				CLogManagerBridge(const IKernelContext& rKernelContext, IPlayerContext& rPlayerContext, CSimulatedBox* pSimulatedBox) : TKernelObject<ILogManager>(rKernelContext), m_rPlayerContext(rPlayerContext), m_pSimulatedBox(pSimulatedBox) { }

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
						<< "At time "
						<< time64(m_rPlayerContext.getCurrentTime())
						<< " <"
						<< LogColor_PushStateBit
						<< LogColor_ForegroundBlue
						<< "Box algorithm"
						<< LogColor_PopStateBit
						<< "::"
						<< m_pSimulatedBox->getName()
						<< "> ";
				}

				_IsDerivedFromClass_Final_(TKernelObject<ILogManager>, OV_UndefinedIdentifier);

			protected:

				IPlayerContext& m_rPlayerContext;
				CSimulatedBox* m_pSimulatedBox;
			};

			class CScenarioManagerBridge : public TKernelObject<IScenarioManager>
			{
			public:

				CScenarioManagerBridge(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox) : TKernelObject<IScenarioManager>(rKernelContext), m_pSimulatedBox(pSimulatedBox) { }

				__BridgeBindFunc1__(getKernelContext().getScenarioManager(), CIdentifier, getNextScenarioIdentifier, const, const CIdentifier&, rPreviousIdentifier)
				__BridgeBindFunc1__(getKernelContext().getScenarioManager(), boolean, createScenario, , CIdentifier&, rScenarioIdentifier)
				__BridgeBindFunc1__(getKernelContext().getScenarioManager(), boolean, releaseScenario, , const CIdentifier&, rScenarioIdentifier)
				__BridgeBindFunc1__(getKernelContext().getScenarioManager(), IScenario&, getScenario, , const CIdentifier&, rScenarioIdentifier)

				_IsDerivedFromClass_Final_(TKernelObject<IScenarioManager>, OV_UndefinedIdentifier);

			protected:

				CSimulatedBox* m_pSimulatedBox;
			};

			class CTypeManagerBridge : public TKernelObject<ITypeManager>
			{
			public:

				CTypeManagerBridge(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox) : TKernelObject<ITypeManager>(rKernelContext), m_pSimulatedBox(pSimulatedBox) { }

				__BridgeBindFunc1__(getKernelContext().getTypeManager(), CIdentifier, getNextTypeIdentifier, const, const CIdentifier&, rPreviousIdentifier);
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), boolean, registerType, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName)
				__BridgeBindFunc3__(getKernelContext().getTypeManager(), boolean, registerStreamType, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName, const CIdentifier&, rParentTypeIdentifier)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), boolean, registerEnumerationType, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName)
				__BridgeBindFunc3__(getKernelContext().getTypeManager(), boolean, registerEnumerationEntry, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName, const uint64, ui64ValueIndex)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), boolean, registerBitMaskType, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName)
				__BridgeBindFunc3__(getKernelContext().getTypeManager(), boolean, registerBitMaskEntry, , const CIdentifier&, rTypeIdentifier, const CString&, sTypeName, const uint64, ui64ValueIndex)

				__BridgeBindFunc1__(getKernelContext().getTypeManager(), boolean, isRegistered, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc1__(getKernelContext().getTypeManager(), boolean, isStream, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), boolean, isDerivedFromStream, const, const CIdentifier&, rTypeIdentifier, const CIdentifier&, rParentTypeIdentifier)
				__BridgeBindFunc1__(getKernelContext().getTypeManager(), boolean, isEnumeration, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc1__(getKernelContext().getTypeManager(), boolean, isBitMask, const, const CIdentifier&, rTypeIdentifier)

				__BridgeBindFunc1__(getKernelContext().getTypeManager(), CString, getTypeName, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc1__(getKernelContext().getTypeManager(), CIdentifier, getStreamParentType, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc1__(getKernelContext().getTypeManager(), uint64, getEnumerationEntryCount, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc4__(getKernelContext().getTypeManager(), boolean, getEnumerationEntry, const, const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryIndex, CString&, sEntryName, uint64&, ui64EntryValue)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), CString, getEnumerationEntryNameFromValue, const, const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryValue)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), uint64, getEnumerationEntryValueFromName, const, const CIdentifier&, rTypeIdentifier, const CString&, sEntryName)
				__BridgeBindFunc1__(getKernelContext().getTypeManager(), uint64, getBitMaskEntryCount, const, const CIdentifier&, rTypeIdentifier)
				__BridgeBindFunc4__(getKernelContext().getTypeManager(), boolean, getBitMaskEntry, const, const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryIndex, CString&, sEntryName, uint64&, ui64EntryValue)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), CString, getBitMaskEntryNameFromValue, const, const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryValue)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), uint64, getBitMaskEntryValueFromName, const, const CIdentifier&, rTypeIdentifier, const CString&, sEntryName)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), CString, getBitMaskEntryCompositionNameFromValue, const, const CIdentifier&, rTypeIdentifier, const uint64, ui64EntryCompositionValue)
				__BridgeBindFunc2__(getKernelContext().getTypeManager(), uint64, getBitMaskEntryCompositionValueFromName, const, const CIdentifier&, rTypeIdentifier, const CString&, sEntryCompositionName)

				_IsDerivedFromClass_Final_(TKernelObject<ITypeManager>, OV_UndefinedIdentifier);

			protected:

				CSimulatedBox* m_pSimulatedBox;
			};
		};
	};
};

CPlayerContext::CPlayerContext(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox)
	:TKernelObject<IPlayerContext>(rKernelContext)
	,m_pSimulatedBox(pSimulatedBox)
	,m_pAlgorithmManagerBridge(NULL)
	,m_pConfigurationManagerBridge(NULL)
	,m_pLogManagerBridge(NULL)
	,m_pScenarioManagerBridge(NULL)
	,m_pTypeManagerBridge(NULL)
{
	m_pAlgorithmManagerBridge=new CAlgorithmManagerBridge(rKernelContext, pSimulatedBox);
	m_pConfigurationManagerBridge=new CConfigurationManagerBridge(rKernelContext, pSimulatedBox);
	m_pLogManagerBridge=new CLogManagerBridge(rKernelContext, *this, pSimulatedBox);
	m_pScenarioManagerBridge=new CScenarioManagerBridge(rKernelContext, pSimulatedBox);
	m_pTypeManagerBridge=new CTypeManagerBridge(rKernelContext, pSimulatedBox);
}

CPlayerContext::~CPlayerContext(void)
{
	delete m_pTypeManagerBridge;
	delete m_pScenarioManagerBridge;
	delete m_pLogManagerBridge;
	delete m_pConfigurationManagerBridge;
	delete m_pAlgorithmManagerBridge;
}

boolean CPlayerContext::sendSignal(
	const CMessageSignal& rMessageSignal)
{
	// TODO
	this->getLogManager() << LogLevel_Debug << "CPlayerContext::sendSignal - Not yet implemented\n";
	return false;
}

boolean CPlayerContext::sendMessage(
	const CMessageEvent& rMessageEvent,
	const CIdentifier& rTargetIdentifier)
{
	// TODO
	this->getLogManager() << LogLevel_Debug << "CPlayerContext::sendMessage - Not yet implemented\n";
	return false;
}

boolean CPlayerContext::sendMessage(
	const CMessageEvent& rMessageEvent,
	const CIdentifier* pTargetIdentifier,
	const uint32 ui32TargetIdentifierCount)
{
	// TODO
	this->getLogManager() << LogLevel_Debug << "CPlayerContext::sendMessage - Not yet implemented\n";
	return false;
}

uint64 CPlayerContext::getCurrentTime(void)
{
	return m_pSimulatedBox->getCurrentTime();
}

boolean CPlayerContext::stop(void)
{
	return m_pSimulatedBox->getScheduler().getPlayer().stop();
}

boolean CPlayerContext::pause(void)
{
	return m_pSimulatedBox->getScheduler().getPlayer().pause();
}

boolean CPlayerContext::play(void)
{
	return m_pSimulatedBox->getScheduler().getPlayer().play();
}

boolean CPlayerContext::forward(void)
{
	return m_pSimulatedBox->getScheduler().getPlayer().forward();
}

EPlayerStatus CPlayerContext::getStatus(void) const
{
	return m_pSimulatedBox->getScheduler().getPlayer().getStatus();
}

IAlgorithmManager& CPlayerContext::getAlgorithmManager(void)
{
	return *m_pAlgorithmManagerBridge;
}

IConfigurationManager& CPlayerContext::getConfigurationManager(void)
{
	return *m_pConfigurationManagerBridge;
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
