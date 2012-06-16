#ifndef __OpenViBE_Kernel_Plugins_IPluginModuleContext_H__
#define __OpenViBE_Kernel_Plugins_IPluginModuleContext_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class ILogManager;
		class ITypeManager;
		class IScenarioManager;

		/**
		 * \class IPluginModuleContext
		 * \brief Plugin context
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-06-20
		 * \ingroup Group_Plugins
		 * \ingroup Group_Kernel
		 * \ingroup Group_Extend
		 */
		class OV_API IPluginModuleContext : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Gets the current scenario manager
			 * \return a reference on the current scenario manager
			 */
			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) const=0;
			/**
			 * \brief Gets the current type manager
			 * \return a reference on the current type manager
			 */
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const=0;
			/**
			 * \brief Gets the current log manager
			 * \return a reference on the current log manager
			 */
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Plugins_PluginModuleContext)
		};
	};
};

#endif // __OpenViBE_Kernel_Plugins_IPluginModuleContext_H__
