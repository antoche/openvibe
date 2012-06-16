#ifndef __OpenViBE_Kernel_IKernelContext_H__
#define __OpenViBE_Kernel_IKernelContext_H__

#include "ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IAlgorithmManager;
		class IKernelObjectFactory;
		class IPlayerManager;
		class IPluginManager;
		class IScenarioManager;
		class ITypeManager;
		class ILogManager;
		class IVisualisationManager;

		/**
		 * \class IKernelContext
		 * \brief Kernel context interface, gives access to each manager the kernel owns
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-10-24
		 *
		 * This class simply provides access to each manager the kernel owns. This is the top
		 * level object that can be used by a custom §OpenViBE§ application and this is the common
		 * object all kernel object have in order to access all the functionnalities.
		 *
		 * See each manager's own documentation for more detail on a specific manager goal and usage.
		 */
		class OV_API IKernelContext : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Gets a reference on the kernel's algorithm manager
			 * \return a reference on the kernel's algorithm manager
			 */
			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) const=0;
			/**
			 * \brief Gets a reference on the kernel's player manager
			 * \return a reference on the kernel's player manager
			 */
			virtual OpenViBE::Kernel::IPlayerManager& getPlayerManager(void) const=0;
			/**
			 * \brief Gets a reference on the kernel's plugin manager
			 * \return a reference on the kernel's plugin manager
			 */
			virtual OpenViBE::Kernel::IPluginManager& getPluginManager(void) const=0;
			/**
			 * \brief Gets a reference on the kernel's object factory
			 * \return a reference on the kernel's object factory
			 */
			virtual OpenViBE::Kernel::IKernelObjectFactory& getKernelObjectFactory(void) const=0;
			/**
			 * \brief Gets a reference on the kernel's scenario manager
			 * \return a reference on the kernel's scenario manager
			 */
			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) const=0;
			/**
			 * \brief Gets a reference on the kernel's type manager
			 * \return a reference on the kernel's type manager
			 */
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const=0;
			/**
			 * \brief Gets a reference on the kernel's log manager
			 * \return a reference on the kernel's log manager
			 */
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const=0;
			/**
			 * \brief Gets a reference on the kernel's visualisation manager
			 * \return a reference on the kernel's visualisation manager
			 */
			virtual OpenViBE::Kernel::IVisualisationManager& getVisualisationManager(void) const=0;

			// backward compatibility
			virtual OpenViBE::Kernel::IKernelObjectFactory& getObjectFactory(void) const { return getKernelObjectFactory(); }

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_KernelContext)
		};
	};
};

#endif // __OpenViBE_Kernel_IKernel_H__
