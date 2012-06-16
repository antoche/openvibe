#ifndef __OpenViBEKernel_CKernelContext_H__
#define __OpenViBEKernel_CKernelContext_H__

#include "ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerConsole;
		class CLogListenerFile;

		class CKernelContext : public OpenViBE::Kernel::IKernelContext
		{
		public:

			CKernelContext(const OpenViBE::Kernel::IKernelContext* pMasterKernelContext, const OpenViBE::CString& rApplicationName, const OpenViBE::CString& rConfigurationFile);
			virtual ~CKernelContext(void);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) const;
			virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void) const;
			virtual OpenViBE::Kernel::IKernelObjectFactory& getKernelObjectFactory(void) const;
			virtual OpenViBE::Kernel::IPlayerManager& getPlayerManager(void) const;
			virtual OpenViBE::Kernel::IPluginManager& getPluginManager(void) const;
			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) const;
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const;
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const;
			virtual OpenViBE::Kernel::IVisualisationManager& getVisualisationManager(void) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelContext, OVK_ClassId_Kernel_KernelContext)

		protected:

			virtual OpenViBE::Kernel::ELogLevel earlyGetLogLevel(const OpenViBE::CString& rLogLevelName);

		protected:

			const OpenViBE::Kernel::IKernelContext& m_rMasterKernelContext;

			mutable OpenViBE::Kernel::IKernelContext* m_pThis;
			mutable OpenViBE::Kernel::IAlgorithmManager* m_pAlgorithmManager;
			mutable OpenViBE::Kernel::IConfigurationManager* m_pConfigurationManager;
			mutable OpenViBE::Kernel::IKernelObjectFactory* m_pKernelObjectFactory;
			mutable OpenViBE::Kernel::IPlayerManager* m_pPlayerManager;
			mutable OpenViBE::Kernel::IPluginManager* m_pPluginManager;
			mutable OpenViBE::Kernel::IScenarioManager* m_pScenarioManager;
			mutable OpenViBE::Kernel::ITypeManager* m_pTypeManager;
			mutable OpenViBE::Kernel::ILogManager* m_pLogManager;
			mutable OpenViBE::Kernel::IVisualisationManager* m_pVisualisationManager;

			OpenViBE::boolean m_bIsInitialized;
			OpenViBE::CString m_sApplicationName;
			OpenViBE::CString m_sConfigurationFile;

			OpenViBE::Kernel::CLogListenerConsole* m_pLogListenerConsole;
			OpenViBE::Kernel::CLogListenerFile* m_pLogListenerFile;

		private:

			CKernelContext(void);
		};

		class CKernelContextBridge : public OpenViBE::Kernel::IKernelContext
		{
		public:

			CKernelContextBridge(const OpenViBE::Kernel::IKernelContext& rKernelContext)
				:m_rKernelContext(rKernelContext)
				,m_pAlgorithmManager(NULL)
				,m_pConfigurationManager(NULL)
				,m_pKernelObjectFactory(NULL)
				,m_pPlayerManager(NULL)
				,m_pPluginManager(NULL)
				,m_pScenarioManager(NULL)
				,m_pTypeManager(NULL)
				,m_pLogManager(NULL)
				,m_pVisualisationManager(NULL)
			{
			}

			virtual OpenViBE::boolean initialize(void) { return true; }
			virtual OpenViBE::boolean uninitialize(void) { return true; }

			void setAlgorithmManager(OpenViBE::Kernel::IAlgorithmManager* pAlgorithmManager) { m_pAlgorithmManager=pAlgorithmManager; }
			void setConfigurationManager(OpenViBE::Kernel::IConfigurationManager* pConfigurationManager) { m_pConfigurationManager=pConfigurationManager; }
			void setKernelObjectFactory(OpenViBE::Kernel::IKernelObjectFactory* pKernelObjectFactory) { m_pKernelObjectFactory=pKernelObjectFactory; }
			void setPlayerManager(OpenViBE::Kernel::IPlayerManager* pPlayerManager) { m_pPlayerManager=pPlayerManager; }
			void setPluginManager(OpenViBE::Kernel::IPluginManager* pPluginManager) { m_pPluginManager=pPluginManager; }
			void setScenarioManager(OpenViBE::Kernel::IScenarioManager* pScenarioManager) { m_pScenarioManager=pScenarioManager; }
			void setTypeManager(OpenViBE::Kernel::ITypeManager* pTypeManager) { m_pTypeManager=pTypeManager; }
			void setLogManager(OpenViBE::Kernel::ILogManager* pLogManager) { m_pLogManager=pLogManager; }
			void setVisualisationManager(OpenViBE::Kernel::IVisualisationManager* pVisualisationManager) { m_pVisualisationManager=pVisualisationManager; }

			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) const { return m_pAlgorithmManager?*m_pAlgorithmManager:m_rKernelContext.getAlgorithmManager(); }
			virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void) const { return m_pConfigurationManager?*m_pConfigurationManager:m_rKernelContext.getConfigurationManager(); }
			virtual OpenViBE::Kernel::IKernelObjectFactory& getKernelObjectFactory(void) const { return m_pKernelObjectFactory?*m_pKernelObjectFactory:m_rKernelContext.getKernelObjectFactory(); }
			virtual OpenViBE::Kernel::IPlayerManager& getPlayerManager(void) const { return m_pPlayerManager?*m_pPlayerManager:m_rKernelContext.getPlayerManager(); }
			virtual OpenViBE::Kernel::IPluginManager& getPluginManager(void) const { return m_pPluginManager?*m_pPluginManager:m_rKernelContext.getPluginManager(); }
			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) const { return m_pScenarioManager?*m_pScenarioManager:m_rKernelContext.getScenarioManager(); }
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const { return m_pTypeManager?*m_pTypeManager:m_rKernelContext.getTypeManager(); }
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const { return m_pLogManager?*m_pLogManager:m_rKernelContext.getLogManager(); }
			virtual OpenViBE::Kernel::IVisualisationManager& getVisualisationManager(void) const { return m_pVisualisationManager?*m_pVisualisationManager:m_rKernelContext.getVisualisationManager(); }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelContext, OVK_ClassId_Kernel_KernelContext)

		protected:

			const OpenViBE::Kernel::IKernelContext& m_rKernelContext;

		protected:

			mutable OpenViBE::Kernel::IAlgorithmManager* m_pAlgorithmManager;
			mutable OpenViBE::Kernel::IConfigurationManager* m_pConfigurationManager;
			mutable OpenViBE::Kernel::IKernelObjectFactory* m_pKernelObjectFactory;
			mutable OpenViBE::Kernel::IPlayerManager* m_pPlayerManager;
			mutable OpenViBE::Kernel::IPluginManager* m_pPluginManager;
			mutable OpenViBE::Kernel::IScenarioManager* m_pScenarioManager;
			mutable OpenViBE::Kernel::ITypeManager* m_pTypeManager;
			mutable OpenViBE::Kernel::ILogManager* m_pLogManager;
			mutable OpenViBE::Kernel::IVisualisationManager* m_pVisualisationManager;
		};
	};
};

#endif // __OpenViBEKernel_CKernelContext_H__
