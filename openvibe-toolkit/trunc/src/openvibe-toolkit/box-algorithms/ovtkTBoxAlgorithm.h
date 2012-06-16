#ifndef __OpenViBEToolkit_TBoxAlgorithm_H__
#define __OpenViBEToolkit_TBoxAlgorithm_H__

#include "../ovtkIObject.h"

namespace OpenViBEToolkit
{
	template <class CBoxAlgorithmParentClass>
	class TBoxAlgorithm : public CBoxAlgorithmParentClass
	{
	public:

		TBoxAlgorithm(void)
			:m_pBoxAlgorithmContext(NULL)
		{
		}

// ====================================================================================================================================

	private:

		virtual OpenViBE::uint64 getClockFrequency(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return getClockFrequency();
		}

		virtual OpenViBE::boolean initialize(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return initialize();
		}

		virtual OpenViBE::boolean uninitialize(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return uninitialize();
		}

		virtual OpenViBE::boolean processEvent(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext, OpenViBE::CMessageEvent& rMessageEvent)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return processEvent(rMessageEvent);
		}

		virtual OpenViBE::boolean processSignal(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext, OpenViBE::CMessageSignal& rMessageSignal)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return processSignal(rMessageSignal);
		}

		virtual OpenViBE::boolean processClock(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext, OpenViBE::CMessageClock& rMessageClock)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return processClock(rMessageClock);
		}

		virtual OpenViBE::boolean processInput(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext, OpenViBE::uint32 ui32InputIndex)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return processInput(ui32InputIndex);
		}

		virtual OpenViBE::boolean process(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return process();
		}

// ====================================================================================================================================

	public:

		virtual OpenViBE::uint64 getClockFrequency(void)                                  { return 0; }
		virtual OpenViBE::boolean initialize(void)                                        { return true; }
		virtual OpenViBE::boolean uninitialize(void)                                      { return true; }
		virtual OpenViBE::boolean processEvent(OpenViBE::CMessageEvent& rMessageEvent)    { return false; }
		virtual OpenViBE::boolean processSignal(OpenViBE::CMessageSignal& rMessageSignal) { return false; }
		virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock)    { return false; }
		virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex)           { return false; }
		virtual OpenViBE::boolean process(void)=0;

// ====================================================================================================================================

		virtual OpenViBE::Kernel::IBoxAlgorithmContext* getBoxAlgorithmContext(void) { return m_pBoxAlgorithmContext; }
		// virtual OpenViBE::Kernel::IBoxAlgorithmContext& getBoxAlgorithmContext(void) { return *m_pBoxAlgorithmContext; } this one should replace !
		virtual OpenViBE::Kernel::IBox& getStaticBoxContext(void) { return *m_pBoxAlgorithmContext->getStaticBoxContext(); }
		virtual OpenViBE::Kernel::IBoxIO& getDynamicBoxContext(void) { return *m_pBoxAlgorithmContext->getDynamicBoxContext(); }
		virtual OpenViBE::Kernel::IPlayerContext& getPlayerContext(void) { return *m_pBoxAlgorithmContext->getPlayerContext(); }
		virtual OpenViBE::Kernel::IVisualisationContext& getVisualisationContext(void) { return *m_pBoxAlgorithmContext->getVisualisationContext(); }

		virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) { return getPlayerContext().getAlgorithmManager(); }
		virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void) { return getPlayerContext().getConfigurationManager(); }
		virtual OpenViBE::Kernel::ILogManager& getLogManager(void) { return getPlayerContext().getLogManager(); }
		virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) { return getPlayerContext().getScenarioManager(); }
		virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) { return getPlayerContext().getTypeManager(); }

// ====================================================================================================================================

		virtual void appendOutputChunkData(const OpenViBE::uint32 ui32OutputIndex, const void* pBuffer, const OpenViBE::uint64 ui64BufferSize)
		{
			OpenViBE::Kernel::IBoxAlgorithmContext* l_pBoxAlgorithmContext=this->getBoxAlgorithmContext();
			if(l_pBoxAlgorithmContext)
			{
				OpenViBE::Kernel::IBoxIO* l_pDynamicBoxContext=l_pBoxAlgorithmContext->getDynamicBoxContext();
				if(l_pDynamicBoxContext)
				{
					l_pDynamicBoxContext->appendOutputChunkData(ui32OutputIndex, static_cast<const OpenViBE::uint8*>(pBuffer), ui64BufferSize);
				}
			}
		}

		template <OpenViBE::uint32 ui32OutputIndex>
		void appendOutputChunkData(const void* pBuffer, const OpenViBE::uint64 ui64BufferSize)
		{
			appendOutputChunkData(ui32OutputIndex, pBuffer, ui64BufferSize);
		}

		_IsDerivedFromClass_(CBoxAlgorithmParentClass, OVTK_ClassId_);

	protected:

		class FSettingValueAutoCast
		{
		public:
			FSettingValueAutoCast(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext, const OpenViBE::uint32 ui32Index)
				:m_rLogManager(rBoxAlgorithmContext.getPlayerContext()->getLogManager())
				,m_rTypeManager(rBoxAlgorithmContext.getPlayerContext()->getTypeManager())
				,m_rConfigurationManager(rBoxAlgorithmContext.getPlayerContext()->getConfigurationManager())
			{
				rBoxAlgorithmContext.getStaticBoxContext()->getSettingValue(ui32Index, m_sSettingValue);
				rBoxAlgorithmContext.getStaticBoxContext()->getSettingType(ui32Index, m_oSettingType);
			}
			operator OpenViBE::uint64 (void)
			{
				if(m_rTypeManager.isEnumeration(m_oSettingType))
				{
					OpenViBE::uint64 l_ui64StimId=m_rTypeManager.getEnumerationEntryValueFromName(m_oSettingType, m_sSettingValue);
					if(l_ui64StimId==0xffffffffffffffffll)
					{
						m_rLogManager << OpenViBE::Kernel::LogLevel_ImportantWarning << "Did not find an enumeration value for [" << m_rTypeManager.getTypeName(m_oSettingType) << ":" << m_sSettingValue << "]\n";
					}
					return l_ui64StimId;
				}
				return m_rConfigurationManager.expandAsUInteger(m_sSettingValue);
			}
			operator OpenViBE::int64 (void)
			{
				return m_rConfigurationManager.expandAsInteger(m_sSettingValue);
			}
			operator OpenViBE::float64 (void)
			{
				return m_rConfigurationManager.expandAsFloat(m_sSettingValue);
			}
			operator OpenViBE::boolean (void)
			{
				return m_rConfigurationManager.expandAsBoolean(m_sSettingValue);
			}
			operator const OpenViBE::CString (void)
			{
				return m_sSettingValue;
			}
		private:
			OpenViBE::Kernel::ILogManager& m_rLogManager;
			OpenViBE::Kernel::ITypeManager& m_rTypeManager;
			OpenViBE::Kernel::IConfigurationManager& m_rConfigurationManager;
			OpenViBE::CString m_sSettingValue;
			OpenViBE::CIdentifier m_oSettingType;
		};

	private:

		class CScopedBoxAlgorithm
		{
		public:
			CScopedBoxAlgorithm(OpenViBE::Kernel::IBoxAlgorithmContext*& rpBoxAlgorithmContext, OpenViBE::Kernel::IBoxAlgorithmContext* pBoxAlgorithmContext)
				:m_rpBoxAlgorithmContext(rpBoxAlgorithmContext)
			{
				m_rpBoxAlgorithmContext=pBoxAlgorithmContext;
			}
			virtual ~CScopedBoxAlgorithm(void)
			{
				m_rpBoxAlgorithmContext=NULL;
			}
		protected:
			OpenViBE::Kernel::IBoxAlgorithmContext*& m_rpBoxAlgorithmContext;
		};

		OpenViBE::Kernel::IBoxAlgorithmContext* m_pBoxAlgorithmContext;
	};

	template <class CBoxListenerParentClass>
	class TBoxListener : public CBoxListenerParentClass
	{
	public:

		TBoxListener(void)
			:m_pBoxListenerContext(NULL)
		{
		}

	private:

		virtual OpenViBE::boolean initialize(
			OpenViBE::Kernel::IBoxListenerContext& rBoxListenerContext)
		{
			CScopedBoxListener l_oScopedBoxListener(m_pBoxListenerContext, &rBoxListenerContext);
			return initialize();
		}

		virtual OpenViBE::boolean uninitialize(
			OpenViBE::Kernel::IBoxListenerContext& rBoxListenerContext)
		{
			CScopedBoxListener l_oScopedBoxListener(m_pBoxListenerContext, &rBoxListenerContext);
			return uninitialize();
		}

		virtual OpenViBE::boolean process(
			OpenViBE::Kernel::IBoxListenerContext& rBoxListenerContext,
			const OpenViBE::Kernel::EBoxModification eBoxModificationType)
		{
			CScopedBoxListener l_oScopedBoxListener(m_pBoxListenerContext, &rBoxListenerContext);
			switch(eBoxModificationType)
			{
				case OpenViBE::Kernel::BoxModification_Initialized: return this->onInitialized(m_pBoxListenerContext->getBox());
				case OpenViBE::Kernel::BoxModification_NameChanged: return this->onNameChanged(m_pBoxListenerContext->getBox());
				case OpenViBE::Kernel::BoxModification_IdentifierChanged: return this->onIdentifierChanged(m_pBoxListenerContext->getBox());
				case OpenViBE::Kernel::BoxModification_AlgorithmClassIdentifierChanged: return this->onAlgorithmClassIdentifierChanged(m_pBoxListenerContext->getBox());
				case OpenViBE::Kernel::BoxModification_ProcessingUnitChanged: return this->onProcessingUnitChanged(m_pBoxListenerContext->getBox());
				case OpenViBE::Kernel::BoxModification_InputConnected: return this->onInputConnected(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_InputDisconnected: return this->onInputDisconnected(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_InputAdded: return this->onInputAdded(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_InputRemoved: return this->onInputRemoved(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_InputTypeChanged: return this->onInputTypeChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_InputNameChanged: return this->onInputNameChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputConnected: return this->onOutputConnected(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputDisconnected: return this->onOutputDisconnected(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputAdded: return this->onOutputAdded(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputRemoved: return this->onOutputRemoved(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputTypeChanged: return this->onOutputTypeChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputNameChanged: return this->onOutputNameChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingAdded: return this->onSettingAdded(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingRemoved: return this->onSettingRemoved(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingTypeChanged: return this->onSettingTypeChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingNameChanged: return this->onSettingNameChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingDefaultValueChanged: return this->onSettingDefaultValueChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingValueChanged: return this->onSettingValueChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				default: this->getLogManager() << OpenViBE::Kernel::LogLevel_Warning << "Unhandled box modification type " << (OpenViBE::uint32)eBoxModificationType << "\n";
			}
			return false;
		}

// ====================================================================================================================================

	public:

		virtual OpenViBE::boolean initialize(void) { return true; }
		virtual OpenViBE::boolean uninitialize(void) { return true; }
		virtual OpenViBE::boolean onInitialized(OpenViBE::Kernel::IBox& rBox) { return true; };
		virtual OpenViBE::boolean onNameChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
		virtual OpenViBE::boolean onIdentifierChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
		virtual OpenViBE::boolean onAlgorithmClassIdentifierChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
		virtual OpenViBE::boolean onProcessingUnitChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
		virtual OpenViBE::boolean onInputConnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onInputDisconnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onInputNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onOutputConnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onOutputDisconnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onOutputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onOutputNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onSettingAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onSettingRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onSettingTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onSettingNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onSettingDefaultValueChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
		virtual OpenViBE::boolean onSettingValueChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };

// ====================================================================================================================================

		virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) const { return m_pBoxListenerContext->getAlgorithmManager(); }
		virtual OpenViBE::Kernel::IPlayerManager& getPlayerManager(void) const { return m_pBoxListenerContext->getPlayerManager(); }
		virtual OpenViBE::Kernel::IPluginManager& getPluginManager(void) const { return m_pBoxListenerContext->getPluginManager(); }
		virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) const { return m_pBoxListenerContext->getScenarioManager(); }
		virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const { return m_pBoxListenerContext->getTypeManager(); }
		virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const { return m_pBoxListenerContext->getLogManager(); }
		virtual OpenViBE::Kernel::IVisualisationManager& getVisualisationManager(void) const { return m_pBoxListenerContext->getVisualisationManager(); }

		virtual OpenViBE::Kernel::IScenario& getScenario(void) const { return m_pBoxListenerContext->getScenario(); }

// ====================================================================================================================================

		_IsDerivedFromClass_(CBoxListenerParentClass, OVTK_ClassId_);

	private:

		class CScopedBoxListener
		{
		public:
			CScopedBoxListener(OpenViBE::Kernel::IBoxListenerContext*& rpBoxListenerContext, OpenViBE::Kernel::IBoxListenerContext* pBoxListenerContext)
				:m_rpBoxListenerContext(rpBoxListenerContext)
			{
				m_rpBoxListenerContext=pBoxListenerContext;
			}
			virtual ~CScopedBoxListener(void)
			{
				m_rpBoxListenerContext=NULL;
			}
		protected:
			OpenViBE::Kernel::IBoxListenerContext*& m_rpBoxListenerContext;
		};

		OpenViBE::Kernel::IBoxListenerContext* m_pBoxListenerContext;
	};
};

#endif // __OpenViBEToolkit_TBoxAlgorithm_H__
