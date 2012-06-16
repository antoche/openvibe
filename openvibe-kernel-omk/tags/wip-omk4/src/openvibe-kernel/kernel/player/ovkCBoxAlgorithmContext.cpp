#include "ovkCBoxAlgorithmContext.h"
#include "ovkCPlayerContext.h"
#include "../visualisation/ovkCVisualisationContext.h"
#include "ovkPsSimulatedBox.h"
#include "../../tools/ovk_bridge_bind_function.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

namespace OpenViBE
{
	namespace Kernel
	{
		namespace
		{
			class CBoxBridge : public TKernelObject<IBox>
			{
			public:

				CBoxBridge(const IKernelContext& rKernelContext, const IBox* pBox)
					:TKernelObject<IBox>(rKernelContext)
					,m_pBox(pBox)
				{
				}

				// IAttributables

				virtual __BridgeImplFunc2__((*m_pBox), boolean, addAttribute, , return false, const CIdentifier&, rAttributeIdentifier, const CString&, sAttributeValue);
				virtual __BridgeImplFunc1__((*m_pBox), boolean, removeAttribute, , return false, const CIdentifier&, rAttributeIdentifier);
				virtual __BridgeImplFunc0__((*m_pBox), boolean, removeAllAttributes, , return false);
				virtual __BridgeBindFunc1__((*m_pBox), CString, getAttributeValue, const, const CIdentifier&, rAttributeIdentifier);
				virtual __BridgeImplFunc2__((*m_pBox), boolean, setAttributeValue, , return false, const CIdentifier&, rAttributeIdentifier, const CString&, sAttributeValue);
				virtual __BridgeBindFunc1__((*m_pBox), boolean, hasAttribute, const, const CIdentifier&, rAttributeIdentifier);
				virtual __BridgeBindFunc0__((*m_pBox), boolean, hasAttributes, const);
				virtual __BridgeBindFunc1__((*m_pBox), CIdentifier, getNextAttributeIdentifier, const, const CIdentifier&, rPreviousIdentifier);

				// IBox

				virtual __BridgeBindFunc0__((*m_pBox), CIdentifier, getIdentifier, const);
				virtual __BridgeBindFunc0__((*m_pBox), CString, getName, const);
				virtual __BridgeBindFunc0__((*m_pBox), CIdentifier, getAlgorithmClassIdentifier, const);
				virtual __BridgeBindFunc0__((*m_pBox), CIdentifier, getProcessingUnitIdentifier, const);
				virtual __BridgeImplFunc1__((*m_pBox), boolean, setIdentifier, , return false, const CIdentifier&, rIdentifier);
				virtual __BridgeImplFunc1__((*m_pBox), boolean, setName, , return false, const CString&, sName);
				virtual __BridgeImplFunc1__((*m_pBox), boolean, setAlgorithmClassIdentifier, , return false, const CIdentifier&, rAlgorithmClassIdentifier);
				virtual __BridgeImplFunc1__((*m_pBox), boolean, setProcessingUnitIdentifier, , return false, const CIdentifier&, rProcessingUnitIdentifier);

				virtual __BridgeImplFunc1__((*m_pBox), boolean, initializeFromAlgorithmClassIdentifier, , return false, const CIdentifier&, rAlgorithmClassIdentifier);
				virtual __BridgeImplFunc1__((*m_pBox), boolean, initializeFromExistingBox, , return false, const IBox&, rExisitingBox);

				virtual __BridgeImplFunc2__((*m_pBox), boolean, addInput, , return false, const CString&, sName, const CIdentifier&, rTypeIdentifier);
				virtual __BridgeImplFunc1__((*m_pBox), boolean, removeInput, , return false, const uint32, ui32InputIndex);
				virtual __BridgeBindFunc0__((*m_pBox), uint32, getInputCount, const);
				virtual __BridgeBindFunc2__((*m_pBox), boolean, getInputType, const, const uint32, ui32InputIndex, CIdentifier&, rTypeIdentifier);
				virtual __BridgeBindFunc2__((*m_pBox), boolean, getInputName, const, const uint32, ui32InputIndex, CString&, rName);
				virtual __BridgeImplFunc2__((*m_pBox), boolean, setInputType, , return false, const uint32, ui32InputIndex, const CIdentifier&, rTypeIdentifier);
				virtual __BridgeImplFunc2__((*m_pBox), boolean, setInputName, , return false, const uint32, ui32InputIndex, const CString&, rName);

				virtual __BridgeImplFunc2__((*m_pBox), boolean, addOutput, , return false, const CString&, sName, const CIdentifier&, rTypeIdentifier);
				virtual __BridgeImplFunc1__((*m_pBox), boolean, removeOutput, , return false, const uint32, ui32OutputIndex);
				virtual __BridgeBindFunc0__((*m_pBox), uint32, getOutputCount, const);
				virtual __BridgeBindFunc2__((*m_pBox), boolean, getOutputType, const, const uint32, ui32OutputIndex, CIdentifier&, rTypeIdentifier);
				virtual __BridgeBindFunc2__((*m_pBox), boolean, getOutputName, const, const uint32, ui32OutputIndex, CString&, rName);
				virtual __BridgeImplFunc2__((*m_pBox), boolean, setOutputType, , return false, const uint32, ui32OutputIndex, const CIdentifier&, rTypeIdentifier);
				virtual __BridgeImplFunc2__((*m_pBox), boolean, setOutputName, , return false, const uint32, ui32OutputIndex, const CString&, rName);

				virtual __BridgeImplFunc3__((*m_pBox), boolean, addSetting, , return false, const CString&, sName, const CIdentifier&, rTypeIdentifier, const CString&, sDefaultValue);
				virtual __BridgeImplFunc1__((*m_pBox), boolean, removeSetting, , return false, const uint32, ui32Index);
				virtual __BridgeBindFunc0__((*m_pBox), uint32, getSettingCount, const);
				virtual __BridgeBindFunc2__((*m_pBox), boolean, getSettingType, const, const uint32, ui32SettingIndex, CIdentifier&, rTypeIdentifier);
				virtual __BridgeBindFunc2__((*m_pBox), boolean, getSettingName, const, const uint32, ui32SettingIndex, CString&, rName);
				virtual __BridgeBindFunc2__((*m_pBox), boolean, getSettingDefaultValue, const, const uint32, ui32SettingIndex, CString&, rDefaultValue);
				virtual __BridgeBindFunc2__((*m_pBox), boolean, getSettingValue, const, const uint32, ui32SettingIndex, CString&, rValue);
				virtual __BridgeImplFunc2__((*m_pBox), boolean, setSettingType, , return false, const uint32, ui32SettingIndex, const CIdentifier&, rTypeIdentifier);
				virtual __BridgeImplFunc2__((*m_pBox), boolean, setSettingName, , return false, const uint32, ui32SettingIndex, const CString&, rName);
				virtual __BridgeImplFunc2__((*m_pBox), boolean, setSettingDefaultValue, , return false, const uint32, ui32SettingIndex, const CString&, rDefaultValue);
				virtual __BridgeImplFunc2__((*m_pBox), boolean, setSettingValue, , return false, const uint32, ui32SettingIndex, const CString&, rValue);

				_IsDerivedFromClass_Final_(TKernelObject<IBox>, OVK_ClassId_Kernel_Player_StaticBoxContext)

			protected:

				const IBox* m_pBox;
			};

			class CBoxIOBridge : public TKernelObject<IBoxIO>
			{
			public:

				CBoxIOBridge(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox)
					:TKernelObject<IBoxIO>(rKernelContext)
					,m_pSimulatedBox(pSimulatedBox)
				{
				}

				// IBoxIO

				virtual __BridgeBindFunc1__((*m_pSimulatedBox), uint32, getInputChunkCount, const, const uint32, ui32InputIndex);
				virtual __BridgeBindFunc6__((*m_pSimulatedBox), boolean, getInputChunk, const, const uint32, ui32InputIndex, const uint32, ui32ChunkIndex, uint64&, rStartTime, uint64&, rEndTime, uint64&, rChunkSize, const uint8*&, rpChunkBuffer);
				virtual __BridgeBindFunc2__((*m_pSimulatedBox), const IMemoryBuffer*, getInputChunk, const, const uint32, ui32InputIndex, const uint32, ui32ChunkIndex);
				virtual __BridgeBindFunc2__((*m_pSimulatedBox), uint64, getInputChunkStartTime, const, const uint32, ui32InputIndex, const uint32, ui32ChunkIndex);
				virtual __BridgeBindFunc2__((*m_pSimulatedBox), uint64, getInputChunkEndTime, const, const uint32, ui32InputIndex, const uint32, ui32ChunkIndex);
				virtual __BridgeBindFunc2__((*m_pSimulatedBox), boolean, markInputAsDeprecated, , const uint32, ui32InputIndex, const uint32, ui32ChunkIndex);

				virtual __BridgeBindFunc1__((*m_pSimulatedBox), uint64, getOutputChunkSize, const, const uint32, ui32OutputIndex);
				virtual __BridgeBindFunc3__((*m_pSimulatedBox), boolean, setOutputChunkSize, , const uint32, ui32OutputIndex, const uint64, ui64Size, const boolean, bDiscard);
				virtual __BridgeBindFunc1__((*m_pSimulatedBox), uint8*, getOutputChunkBuffer, , const uint32, ui32OutputIndex);
				virtual __BridgeBindFunc3__((*m_pSimulatedBox), boolean, appendOutputChunkData, , const uint32, ui32OutputIndex, const uint8*, pBuffer, const uint64, ui64BufferSize);
				virtual __BridgeBindFunc1__((*m_pSimulatedBox), IMemoryBuffer*, getOutputChunk, , const uint32, ui32OutputIndex);
				virtual __BridgeBindFunc3__((*m_pSimulatedBox), boolean, markOutputAsReadyToSend, , const uint32, ui32OutputIndex, const uint64, ui64StartTime, const uint64, ui64EndTime);

				_IsDerivedFromClass_Final_(TKernelObject<IBoxIO>, OVK_ClassId_Kernel_Player_DynamicBoxContext)

			protected:

				CSimulatedBox *m_pSimulatedBox;
			};
		};
	};
};

#define boolean OpenViBE::boolean

CBoxAlgorithmContext::CBoxAlgorithmContext(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox, const IBox* pBox)
	:TKernelObject<IBoxAlgorithmContext>(rKernelContext)
	,m_pStaticBoxContext(NULL)
	,m_pDynamicBoxContext(NULL)
	,m_pPlayerContext(NULL)
	,m_pVisualisationContext(NULL)
	,m_bReadyToProcess(false)
{
	m_pStaticBoxContext=new CBoxBridge(rKernelContext, pBox);
	m_pDynamicBoxContext=new CBoxIOBridge(rKernelContext, pSimulatedBox);
	m_pPlayerContext=new CPlayerContext(rKernelContext, pSimulatedBox);
	m_pVisualisationContext=new CVisualisationContext(rKernelContext, pSimulatedBox);
}

CBoxAlgorithmContext::~CBoxAlgorithmContext(void)
{
	delete m_pVisualisationContext;
	delete m_pPlayerContext;
	delete m_pDynamicBoxContext;
	delete m_pStaticBoxContext;
}

IBox* CBoxAlgorithmContext::getStaticBoxContext(void)
{
	return m_pStaticBoxContext;
}

IBoxIO* CBoxAlgorithmContext::getDynamicBoxContext(void)
{
	return m_pDynamicBoxContext;
}

IPlayerContext* CBoxAlgorithmContext::getPlayerContext(void)
{
	return m_pPlayerContext;
}

IVisualisationContext* CBoxAlgorithmContext::getVisualisationContext(void)
{
	return m_pVisualisationContext;
}

boolean CBoxAlgorithmContext::markAlgorithmAsReadyToProcess(void)
{
	m_bReadyToProcess=true;
	return true;
}

boolean CBoxAlgorithmContext::isAlgorithmReadyToProcess(void)
{
	return m_bReadyToProcess;
}
