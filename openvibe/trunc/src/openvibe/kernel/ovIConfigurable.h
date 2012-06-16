#ifndef __OpenViBE_Kernel_IConfigurable_H__
#define __OpenViBE_Kernel_IConfigurable_H__

#include "ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IParameter;

		/**
		 * \brief Parameter type enumeration for a configurable object
		 * \sa IConfigurable
		 * \sa IParameter
		 */
		enum EParameterType
		{
			ParameterType_None,           //!< No parameter
			ParameterType_Integer,        //!< Integer parameter
			ParameterType_UInteger,       //!< Unsigned integer parameter
			ParameterType_Enumeration,    //!< Enumeration integer parameter
			ParameterType_Boolean,        //!< Boolean parameter
			ParameterType_Float,          //!< Float parameter
			ParameterType_String,         //!< String parameter
			ParameterType_Identifier,     //!< Identifier parameter
			ParameterType_Matrix,         //!< Matrix pointer parameter
			ParameterType_StimulationSet, //!< Stimulation set pointer parameter
			ParameterType_MemoryBuffer,   //!< Memory buffer pointer parameter
			ParameterType_Object,         //!< Object pointer parameter
			ParameterType_Pointer,        //!< Raw pointer parameter
		};

		/**
		 * \class IConfigurable
		 * \brief Configurable object interface
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-11-21
		 * \sa IParameter
		 * \ingroup Group_Kernel
		 *
		 * An instance of this class is able to self create several type of parameters
		 * in order to be used by external code. The parameter handling is very abstract
		 * but its use is made easier thanks to the parameter handler. See IParameter and
		 * TParameterHandler for more details.
		 */
		class OV_API IConfigurable : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Enumerates parameter identifiers for this configurable.
			 * \param rPreviousIdentifier [in] : the identifier which next identifier has to be returned
			 * \return the parameter identifier following the provided parameter identifier.
			 * \note if \c rPreviousIdentifier is \e OV_UndefinedIdentifier , the first parameter identifier is returned
			 * \note getting \e OV_UndefinedIdentifier has result means there are no more identifier after \c rPreviousIdentifier
			 *
			 * Sample code to iterate on parameter identifiers :
			 *
			 * \code
			 * IConfigurable* l_pConfigurable= // ...
			 * CIdentifier l_oCurrentIdentifier=OV_UndefinedIdentifier;
			 * while((l_oCurrentIdentifier=l_pConfigurable->getNextParameterIdentifier(l_oCurrentIdentifier))!=OV_UndefinedIdentifier)
			 * {
			 *   IParameter* l_pParameter=l_pConfigurable->getParameter(l_oCurrentIdentifier);
			 *   // ...
			 * }
			 * \endcode
			 */
			virtual OpenViBE::CIdentifier getNextParameterIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;

			/**
			 * \brief Gets a specific parameter given its identifier
			 * \param rParameterIdentifier [in] : the identifier of the parameter to get
			 * \return a pointer to the corresponding parameter in case of success.
			 * \return \c NULL in case of error.
			 */
			virtual OpenViBE::Kernel::IParameter* getParameter(
				const OpenViBE::CIdentifier& rParameterIdentifier)=0;
			/**
			 * \brief Replaces the parameter with a client handled object
			 * \param rParameterIdentifier [in] : the identifier of the parameter to replace
			 * \param rpParameter [in] : the parameter object to put in place of the old parameter
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note The parameter should have been created before and should exist.
			 * \note Even if a new paramter is affected to a configurable, the memory management
			 *       of this parameter remains to the responsability of the client code. Thus
			 *       none of \c removeParameter nor configurable destruction will release
			 *       this parameter object. The caller should take care of this when needed.
			 */
			virtual OpenViBE::boolean setParameter(
				const OpenViBE::CIdentifier& rParameterIdentifier,
				OpenViBE::Kernel::IParameter& rpParameter)=0;
			/**
			 * \brief Creates a new parameter of a specific type
			 * \param rParameterIdentifier [in] : the parameter identifier which has to be created
			 * \param eParameterType [in] : the type of this parameter
			 * \param rSubTypeIdentifier [in] : the optional sub type of this parameter (e.g. for enumerations)
			 * \sa EParameterType
			 * \sa IParameter
			 *
			 * This function creates a new parameter with its associated object.
			 */
			virtual OpenViBE::Kernel::IParameter* createParameter(
				const OpenViBE::CIdentifier& rParameterIdentifier,
				const OpenViBE::Kernel::EParameterType eParameterType,
				const OpenViBE::CIdentifier& rSubTypeIdentifier=OV_UndefinedIdentifier)=0;
			/**
			 * \brief Removes an existing parameter
			 * \param rParameterIdentifier [in] : the identifier of the parameter to remove
			 * \return \e true in case of success
			 * \return \e false in case of error
			 * \note if the parameter object is not released if it was replaced by a custom
			 *       parameter object thanks to \c setParameter function.
			 */
			virtual OpenViBE::boolean removeParameter(
				const OpenViBE::CIdentifier& rParameterIdentifier)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Configurable);
		};
	};
};

#endif // __OpenViBE_Kernel_IConfigurable_H__
