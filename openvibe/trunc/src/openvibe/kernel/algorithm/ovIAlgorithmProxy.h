#ifndef __OpenViBE_Kernel_Algorithm_IAlgorithmProxy_H__
#define __OpenViBE_Kernel_Algorithm_IAlgorithmProxy_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IParameter;

		/**
		 * \class IAlgorithmProxy
		 * \brief Application interface to an algorithm plugin
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-11-21
		 * \ingroup Group_Algorithm
		 * \ingroup Group_Kernel
		 * \sa Plugins::IAlgorithm
		 *
		 * This class is an easier to user interface to an IAlgorihtm instanciated
		 * object. Its purpose is to automatically handle input / output trigger
		 * activation and to help in calling processing methods.
		 */
		class OV_API IAlgorithmProxy : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/** \name Input parameters */
			//@{

			/**
			 * \brief Gets next input parameter identifier given the previous input parameter
			 * \param rPreviousInputParameterIdentifier [in] : the previous input parameter
			 * \return the next input parameter identifier if existing.
			 * \return \c OV_UndefinedIdentifier if the previous was the last input parameter identifier.
			 * \note Passing \c OV_UndefinedIdentifier as \c rPreviousInputParameterIdentifier will
			 *       cause this function to return the first input parameter identifier.
			 */
			virtual OpenViBE::CIdentifier getNextInputParameterIdentifier(
				const OpenViBE::CIdentifier& rPreviousInputParameterIdentifier) const=0;
			/**
			 * \brief Gets the name of a specific input parameter
			 * \param rInputParameterIdentifier [in] : the identifier of the parameter which name should be returned
			 * \return The name of the specified input parameter on success
			 * \return En empty string on error
			 */
			virtual OpenViBE::CString getInputParameterName(
				const OpenViBE::CIdentifier& rInputParameterIdentifier) const=0;
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
			 *       cause this function to return the first output parameter identifier.
			 */
			virtual OpenViBE::CIdentifier getNextOutputParameterIdentifier(
				const OpenViBE::CIdentifier& rPreviousOutputParameterIdentifier) const=0;
			/**
			 * \brief Gets the name of a specific input parameter
			 * \param rOutputParameterIdentifier [in] : the identifier of the parameter which name should be returned
			 * \return The name of the specified input parameter on success
			 * \return En empty string on error
			 */
			virtual OpenViBE::CString getOutputParameterName(
				const OpenViBE::CIdentifier& rOutputParameterIdentifier) const=0;
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
			 * \brief Gets next output trigger identifier given the previous output trigger
			 * \param rPreviousOutputTriggerIdentifier [in] : the previous output trigger
			 * \return the next output trigger identifier if existing.
			 * \return \c OV_UndefinedIdentifier if the previous was the last output trigger identifier.
			 * \note Passing \c OV_UndefinedIdentifier as \c rPreviousOutputTriggerIdentifier will
			 *       cause this function to return the first output trigger identifier.
			 */
			virtual OpenViBE::CIdentifier getNextOutputTriggerIdentifier(
				const OpenViBE::CIdentifier& rPreviousOutputTriggerIdentifier) const=0;
			/**
			 * \brief Gets the name of a specific output trigger
			 * \param rOutputTriggerIdentifier [in] : the identifier of the trigger which name should be returned
			 * \return The name of the specified output trigger on success
			 * \return En empty string on error
			 */
			virtual OpenViBE::CString getOutputTriggerName(
				const OpenViBE::CIdentifier& rOutputTriggerIdentifier) const=0;
			/**
			 * \brief Tests whether an output trigger is activated or not.
			 * \param rOutputTriggerIdentifier [in] : the identifier of the output trigger which activation status should be returned
			 * \return \e true if the provided trigger is currently active.
			 * \return \e false if the provided trigger is not currently active or does not exist.
			 */
			virtual OpenViBE::boolean isOutputTriggerActive(
				const OpenViBE::CIdentifier& rOutputTriggerIdentifier) const=0;

			/**
			 * \brief Gets next input trigger identifier given the previous input trigger
			 * \param rPreviousInputTriggerIdentifier [in] : the previous input trigger
			 * \return the next input trigger identifier if existing.
			 * \return \c OV_UndefinedIdentifier if the previous was the last input trigger identifier.
			 * \note Passing \c OV_UndefinedIdentifier as \c rPreviousInputTriggerIdentifier will
			 *       cause this function to return the first input trigger identifier.
			 */
			virtual OpenViBE::CIdentifier getNextInputTriggerIdentifier(
				const OpenViBE::CIdentifier& rPreviousInputTriggerIdentifier) const=0;
			/**
			 * \brief Gets the name of a specific input trigger
			 * \param rInputTriggerIdentifier [in] : the identifier of the trigger which name should be returned
			 * \return The name of the specified input trigger on success
			 * \return En empty string on error
			 */
			virtual OpenViBE::CString getInputTriggerName(
				const OpenViBE::CIdentifier& rInputTriggerIdentifier) const=0;
			/**
			 * \brief Activates an input trigger before calling the processing function.
			 * \param rInputTriggerIdentifier [in] : the identifier of the input to activate
			 * \param bTriggerState [in]: the new trigger state this input trigger should take
			 *                            (\e true for active, \e false for inactive)
			 * \return \e true in case the state was correctly changed.
			 * \return \e false in case the state was not changed or the provided trigger identifier does not exist.
			 */
			virtual OpenViBE::boolean activateInputTrigger(
				const OpenViBE::CIdentifier& rInputTriggerIdentifier,
				const OpenViBE::boolean bTriggerState)=0;

			//@}
			/** \name Algorithm functions */
			//@{

			/**
			 * \brief Initializes the algorithm
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note The algorithm context is managed internally
			 */
			virtual OpenViBE::boolean initialize(void)=0;
			/**
			 * \brief Uninitializes the algorithm
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note The algorithm context is managed internally
			 */
			virtual OpenViBE::boolean uninitialize(void)=0;
			/**
			 * \brief Calls the processing method of the algorithm
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note The algorithm context is managed internally
			 */
			virtual OpenViBE::boolean process(void)=0;
			/**
			 * \brief Activates an input trigger and immediatly calls the processing method
			 * \param rTriggerIdentifier [in] : the identifier of the input trigger to activate
			 * \return \e true in case of success.
			 * \return \e false in case the trigger does not exist or the processing did not succeed.
			 * \note The algorithm context is managed internally
			 */
			virtual OpenViBE::boolean process(
				const OpenViBE::CIdentifier& rTriggerIdentifier)=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Algorithm_AlgorithmProxy)
		};
	};
};

#endif // __OpenViBE_Kernel_Algorithm_IAlgorithmProxy_H__
