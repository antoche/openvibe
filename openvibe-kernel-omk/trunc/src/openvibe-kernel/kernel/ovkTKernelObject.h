#ifndef __OpenViBEKernel_Kernel_TKernelObject_H__
#define __OpenViBEKernel_Kernel_TKernelObject_H__

#include "../ovk_base.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		template <class T>
		class TKernelObject : public T
		{
		public:

			TKernelObject(const OpenViBE::Kernel::IKernelContext& rKernelContext)
				:m_rKernelContext(rKernelContext)
			{
			}

			const OpenViBE::Kernel::IKernelContext& getKernelContext(void) const
			{
				return m_rKernelContext;
			}

			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) const
			{
				return m_rKernelContext.getAlgorithmManager();
			}

			virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void) const
			{
				return m_rKernelContext.getConfigurationManager();
			}

			virtual OpenViBE::Kernel::IKernelObjectFactory& getKernelObjectFactory(void) const
			{
				return m_rKernelContext.getKernelObjectFactory();
			}

			virtual OpenViBE::Kernel::IPlayerManager& getPlayerManager(void) const
			{
				return m_rKernelContext.getPlayerManager();
			}

			virtual OpenViBE::Kernel::IPluginManager& getPluginManager(void) const
			{
				return m_rKernelContext.getPluginManager();
			}

			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) const
			{
				return m_rKernelContext.getScenarioManager();
			}

			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const
			{
				return m_rKernelContext.getTypeManager();
			}

			virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const
			{
				return m_rKernelContext.getLogManager();
			}

			virtual OpenViBE::Kernel::IVisualisationManager& getVisualisationManager(void) const
			{
				return m_rKernelContext.getVisualisationManager();
			}

			_IsDerivedFromClass_(T, OVK_ClassId_Kernel_KernelObjectT)

		private:

			const OpenViBE::Kernel::IKernelContext& m_rKernelContext;

			// TKernelObject(void);
		};
	};
};

#endif // __OpenViBEKernel_Kernel_TKernelObject_H__
