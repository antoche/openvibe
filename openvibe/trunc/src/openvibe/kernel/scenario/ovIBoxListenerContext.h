#ifndef __OpenViBE_Kernel_Scenario_IBoxListenerContext_H__
#define __OpenViBE_Kernel_Scenario_IBoxListenerContext_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IBoxListenerContext
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-11-06
		 * \brief Exectution context for box algorithm descriptors
		 * \ingroup Group_Kernel
		 * \ingroup Group_Extend
		 * \sa OpenViBE::Plugins::IBoxAlgorithmDesc
		 * \sa OpenViBE::Plugins::IBoxAlgorithm
		 */
		class OV_API IBoxListenerContext : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/** \name Manager accessors */
			//@{

			/**
			 * \brief Gets a reference on the current algorithm manager
			 * \return a reference on the current algorithm manager
			 */
			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) const=0;
			/**
			 * \brief Gets a reference on the current player manager
			 * \return a reference on the current player manager
			 */
			virtual OpenViBE::Kernel::IPlayerManager& getPlayerManager(void) const=0;
			/**
			 * \brief Gets a reference on the current plugin manager
			 * \return a reference on the current plugin manager
			 */
			virtual OpenViBE::Kernel::IPluginManager& getPluginManager(void) const=0;
			/**
			 * \brief Gets a reference on the current scenario manager
			 * \return a reference on the current scenario manager
			 */
			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) const=0;
			/**
			 * \brief Gets a reference on the current type manager
			 * \return a reference on the current type manager
			 */
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const=0;
			/**
			 * \brief Gets a reference on the current log manager
			 * \return a reference on the current log manager
			 */
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const=0;
			/**
			 * \brief Gets a reference on the current visualisation manager
			 * \return a reference on the current visualisation manager
			 */
			virtual OpenViBE::Kernel::IVisualisationManager& getVisualisationManager(void) const=0;

			//@}
			/** \name Box specific stuffs */

			/**
			 * \brief Gets a reference to the considered box
			 * \return a reference to the considered box
			 */
			virtual OpenViBE::Kernel::IBox& getBox(void) const=0;
			/**
			 * \brief Gets a reference to the scenario owning the considered box
			 * \return a reference to the scenario owning the considered box
			 */
			virtual OpenViBE::Kernel::IScenario& getScenario(void) const=0;
			/**
			 * \brief Gets the index of the concerned callback message
			 * \return the index of the concerned callback message in case it makes sense
			 * \return \c 0xffffffff in case it does not make any sense
			 */
			virtual OpenViBE::uint32 getIndex(void) const=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_BoxListenerContext)
		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_IBoxListenerContext_H__
