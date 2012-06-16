#ifndef __OpenViBE_Kernel_Algorithm_IAlgorithmProto_H__
#define __OpenViBE_Kernel_Algorithm_IAlgorithmProto_H__

#include "../ovIConfigurable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IAlgorithmProto
		 * \brief Prototype interface for algorithm
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-11-21
		 * \ingroup Group_Algorithm
		 * \ingroup Group_Kernel
		 * \ingroup Group_Extend
		 * \sa OpenViBE::Plugins::IAlgorithm
		 * \sa OpenViBE::Plugins::IAlgorithmDesc
		 */
		class OV_API IAlgorithmProto : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Adds an input parameter
			 * \param rInputParameterIdentifier [in] : the identifier for this parameter
			 * \param sInputName [in] : the name for this parameter
			 * \param eParameterType [in] : the type for this parameter
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \sa IParameter
			 */
			virtual OpenViBE::boolean addInputParameter(
				const OpenViBE::CIdentifier& rInputParameterIdentifier,
				const OpenViBE::CString& sInputName,
				const OpenViBE::Kernel::EParameterType eParameterType)=0;
			/**
			 * \brief Adds an output parameter
			 * \param rOutputParameterIdentifier [in] : the identifier for this parameter
			 * \param sOutputName [in] : the name for this parameter
			 * \param eParameterType [in] : the type for this parameter
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \sa IParameter
			 */
			virtual OpenViBE::boolean addOutputParameter(
				const OpenViBE::CIdentifier& rOutputParameterIdentifier,
				const OpenViBE::CString& sOutputName,
				const OpenViBE::Kernel::EParameterType eParameterType)=0;
			/**
			 * \brief Adds an input trigger
			 * \param rInputTriggerIdentifier [in] : the identifier for this trigger
			 * \param rInputTriggerName [in] : the name for this trigger
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean addInputTrigger(
				const OpenViBE::CIdentifier& rInputTriggerIdentifier,
				const OpenViBE::CString& rInputTriggerName)=0;
			/**
			 * \brief Adds an output trigger
			 * \param rOutputTriggerIdentifier [in] : the identifier for this trigger
			 * \param rOutputTriggerName [in] : the name for this trigger
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean addOutputTrigger(
				const OpenViBE::CIdentifier& rOutputTriggerIdentifier,
				const OpenViBE::CString& rOutputTriggerName)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Algorithm_AlgorithmProto)
		};
	};
};

#endif // __OpenViBE_Kernel_Algorithm_IAlgorithmProto_H__
