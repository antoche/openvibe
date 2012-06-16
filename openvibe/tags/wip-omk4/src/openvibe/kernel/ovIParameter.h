#ifndef __OpenViBE_Kernel_IParameter_H__
#define __OpenViBE_Kernel_IParameter_H__

#include "ovIKernelObject.h"
#include "ovIConfigurable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IParameter
		 * \brief Generic parameter handler for configurable objects
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-11-21
		 * \ingroup Group_Kernel
		 * \sa IConfigurable
		 * \sa EParameterType
		 * \sa TParameterHandler
		 *
		 * Instances of this interface encapsulate a specific type of object as a
		 * so called "parameter". The idea behind this parameter interface is to allow
		 * to override the parameter content directly pointing to another parameter
		 * or another value. In order to be as generic as possible, this interface
		 * uses \c void* pointers in conjunction with data size values. It can be
		 * hard/tricky to understand for unexperienced developers. Thus I recommand
		 * the use of the TParameterHandler object which dramatically eases the use
		 * of the IParameter interface.
		 */
		class OV_API IParameter : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/** \name information */
			//@{

			/**
			 * \brief Gets the size of this parameter's content
			 * \return the size of this parameter's content.
			 */
			virtual OpenViBE::uint64 getParameterSize(void) const                                        { return 0; }
			/**
			 * \brief Gets the type of this parameter's content
			 * \return the type of this parameter's content.
			 */
			virtual OpenViBE::Kernel::EParameterType getType(void) const                                 { return ParameterType_None; }

			//@}
			/** \name Reinitialization */
			//@{

			/**
			 * \brief Clears all reference targets for this paramter
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \sa getReferenceTarget
			 * \sa setReferenceTarget
			 */
			virtual OpenViBE::boolean clearReferenceTarget(void)                                         { return false; }

			//@}
			/** \name Reference target management */
			//@{

			/**
			 * \brief Gets the parameter reference target
			 * \param value [out] : a pointer to the reference target parameter if existing, \c NULL if not
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean getReferenceTarget(
				OpenViBE::Kernel::IParameter*& value) const                                              { return false; }
			/**
			 * \brief Sets the parameter reference target for this parameter
			 * \param value [in] : the new reference target to use
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note Setting \c NULL simply removes the parameter reference target
			 * \warning calling this function overrives previous value reference target !
			 *
			 * After this function has been called, any call to \c getValue or \c setValue will be
			 * directly redirected to the specified reference target parameter. The internal value
			 * handled by this parameter won't be affected anymore until the reference target is
			 * cleared.
			 */
			virtual OpenViBE::boolean setReferenceTarget(
				OpenViBE::Kernel::IParameter* value)                                                     { return false; }
			/**
			 * \brief Gets the value reference target
			 * \param value [out] : a pointer to the value target parameter if existing, \c NULL if not
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean getReferenceTarget(
				void* value) const                                                                       { return false; }
			/**
			 * \brief Sets the value reference target for this parameter
			 * \param value [in] : the new reference target to use
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note Setting \c NULL simply removes the value reference target
			 * \warning calling this function overrives previous parameter reference target !
			 *
			 * After this function has been called, any call to \c getValue or \c setValue will be
			 * directly redirected to the specified value reference target. The internal value
			 * handled by this parameter won't be affected anymore until the reference target is
			 * cleared.
			 */
			virtual OpenViBE::boolean setReferenceTarget(
				const void* value)                                                                       { return false; }

			//@}
			/** \name Value access */
			//@{

			/**
			 * \brief Gets the value for this parameter
			 * \param value [out] : a pointer to where the value should be copied
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note the function assumes enough memory has been allocated in \c value to
			 *       copy the value itself.
			 * \note if a parameter or value reference target has been created, then
			 *       either this target's \c getValue is automatically called, either
			 *       the value target is directly copied
			 * \sa getParameterSize
			 */
			virtual OpenViBE::boolean getValue(
				void* value) const                                                                       { return false; }
			/**
			 * \brief Sets the value for this parameter
			 * \param value [in] : a pointer from where the value should be copied
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \sa getParameterSize
			 */
			virtual OpenViBE::boolean setValue(
				const void* value)                                                                       { return false; }

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Parameter);
		};
	};
};

#endif // __OpenViBE_Kernel_IParameter_H__
