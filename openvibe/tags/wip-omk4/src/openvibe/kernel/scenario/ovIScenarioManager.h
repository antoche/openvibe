#ifndef __OpenViBE_Kernel_Scenario_IScenarioManager_H__
#define __OpenViBE_Kernel_Scenario_IScenarioManager_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IScenario;

		/**
		 * \class IScenarioManager
		 * \author Yann Renard (IRISA/INRIA)
		 * \date 2006-10-05
		 * \brief The scenario manager
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 *
		 * This manager is responsible to organize and handle
		 * all the scenarios of the kernel.
		 */
		class OV_API IScenarioManager : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Gets next scenario identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding scenario
			 * \return The identifier of the next scenario in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first scenario
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextScenarioIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;
			/**
			 * \brief Creates a new scenario
			 * \param rScenarioIdentifier [out] : the identifier of
			 *        the newly created scenario
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean createScenario(
				OpenViBE::CIdentifier& rScenarioIdentifier)=0;
			/**
			 * \brief Releases an existing scenario
			 * \param rScenarioIdentifier [in] : the existing scenario identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean releaseScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier)=0;
			/**
			 * \brief Gets details on a specific scenario
			 * \param rScenarioIdentifier [in] : the scenario identifier which details should be returned
			 * \return the corresponding scenario reference.
			 * \warning Calling this function with a bad identifier causes a crash
			 */
			virtual OpenViBE::Kernel::IScenario& getScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_ScenarioManager);
		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_IScenarioManager_H__
