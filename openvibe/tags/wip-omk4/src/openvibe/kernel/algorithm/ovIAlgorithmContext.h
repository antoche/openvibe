#ifndef __OpenViBE_Kernel_Algorithm_IAlgorithmContext_H__
#define __OpenViBE_Kernel_Algorithm_IAlgorithmContext_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IParameter;

		/**
		 * \class IAlgorithmContext
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-11-06
		 * \brief Exectution context for algorithm objects
		 * \ingroup Group_Algorithm
		 * \ingroup Group_Kernel
		 * \ingroup Group_Extend
		 * \sa OpenViBE::Plugins::IAlgorithm
		 */
		class OV_API IAlgorithmContext : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Gets a reference on the current log manager
			 * \return a reference on the current log manager
			 */
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void)=0;

			/** \name Input parameters */
			//@{

			/**
			 * \brief Gets next input parameter identifier given the previous input parameter
			 * \param rPreviousInputParameterIdentifier [in] : the previous input parameter
			 * \return the next input parameter identifier if existing.
			 * \return \c OV_UndefinedIdentifier if the previous was the last input parameter identifier.
			 * \note Passing \c OV_UndefinedIdentifier as \c rPreviousInputParameterIdentifier will
			 *       cause this function to return the firs input parameter identifier.
			 */
			virtual OpenViBE::CIdentifier getNextInputParameterIdentifier(
				const OpenViBE::CIdentifier& rPreviousInputParameterIdentifier) const=0;
			/**
			 * \brief Gets the parameter details of a specific input parameter
			 * \param rInputParameterIdentifier [in] : the identifier of the parameter which details should be returned
			 * \return the parameter pointer that corresponds to the provided identifier.
			 * \return \c NULL in case of error.
			 */
			virtual OpenViBE::Kernel::IParameter* getInputParameter(
				const OpenViBE::CIdentifier& rInputParameterIdentifier)=0;

			//@}
			/** \name Output parameters */
			//@{

			/**
			 * \brief Gets next output parameter identifier given the previous output parameter
			 * \param rPreviousOutputParameterIdentifier [in] : the previous output parameter
			 * \return the next output parameter identifier if existing.
			 * \return \c OV_UndefinedIdentifier if the previous was the last output parameter identifier.
			 * \note Passing \c OV_UndefinedIdentifier as \c rPreviousOutputParameterIdentifier will
			 *       cause this function to return the firs output parameter identifier.
			 */
			virtual OpenViBE::CIdentifier getNextOutputParameterIdentifier(
				const OpenViBE::CIdentifier& rPreviousOutputParameterIdentifier) const=0;
			/**
			 * \brief Gets the parameter details of a specific output parameter
			 * \param rOutputParameterIdentifier [in] : the identifier of the parameter which details should be returned
			 * \return the parameter pointer that corresponds to the provided identifier.
			 * \return \c NULL in case of error.
			 */
			virtual OpenViBE::Kernel::IParameter* getOutputParameter(
				const OpenViBE::CIdentifier& rOutputParameterIdentifier)=0;

			//@}
			/** \name Trigger management */
			//@{

			/**
			 * \brief Tests whether an input trigger is activated or not.
			 * \param rInputTriggerIdentifier [in] : the identifier of the input trigger which activation status should be returned
			 * \return \e true if the provided trigger is currently active.
			 * \return \e false if the provided trigger is not currently active or does not exist.
			 */
			virtual OpenViBE::boolean isInputTriggerActive(
				const OpenViBE::CIdentifier& rInputTriggerIdentifier) const=0;
			/**
			 * \brief Activates an output trigger before calling the processing function.
			 * \param rOutputTriggerIdentifier [in] : the identifier of the output to activate
			 * \param bTriggerState [in]: the new trigger state this output trigger should take
			 *                            (\e true for active, \e false for inactive)
			 * \return \e true in case the state was correctly changed.
			 * \return \e false in case the state was not changed or the provided trigger identifier does not exist.
			 */
			virtual OpenViBE::boolean activateOutputTrigger(
				const OpenViBE::CIdentifier& rOutputTriggerIdentifier,
				const OpenViBE::boolean bTriggerState)=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Algorithm_AlgorithmContext)
		};
	};
};

#endif // __OpenViBE_Kernel_Algorithm_IAlgorithmContext_H__
