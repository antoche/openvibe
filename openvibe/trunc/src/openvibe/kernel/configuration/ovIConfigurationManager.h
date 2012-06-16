#ifndef __OpenViBE_Kernel_Configuration_IConfigurationManager_H__
#define __OpenViBE_Kernel_Configuration_IConfigurationManager_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IConfigurationManager
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2008-11-05
		 * \brief Manager for all kind of configuration
		 * \ingroup Group_Configuration
		 * \ingroup Group_Kernel
		 */
		class OV_API IConfigurationManager : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Clears the content of this configuration manager
			 */
			virtual void clear(void)=0;

			/**
			 * \brief Parses a configuration file and adds its content as token configuration
			 * \param rFileNameWildCard [in] : a wildcard of the files to parse & add
			 * \return \e true in case of success
			 * \return \e false in case of error
			 * \note In case the wildcard matches several filenames, it is up to this
			 *       configuration manager to choose the order it parses the files in.
			 * \sa IConfigurationManager::createConfigurationToken
			 */
			virtual OpenViBE::boolean addConfigurationFromFile(
				const OpenViBE::CString& rFileNameWildCard)=0;

			/**
			 * \brief Creates a new configuration token in this configuration manager
			 * \param rConfigurationTokenName [in] : the name of the configuration token
			 * \param rConfigurationTokenValue [in] the value of the configuration token
			 * \return the identifier of the newly created token in case of success
			 * \return \c OV_UndefinedIdentifier in case of error
			 * \sa IConfigurationManager::releaseConfigurationToken
			 * \sa IConfigurationManager::createConfigurationToken
			 */
			virtual OpenViBE::CIdentifier createConfigurationToken(
				const OpenViBE::CString& rConfigurationTokenName,
				const OpenViBE::CString& rConfigurationTokenValue)=0;
			/**
			 * \brief Removes an existing configuration token
			 * \param rConfigurationTokenIdentifier [in] : the identifier of the token to remove
			 * \return \e true in case of success
			 * \return \e false in case of error
			 * \sa IConfigurationManager::addConfigurationFromFile
			 * \sa IConfigurationManager::createConfigurationToken
			 */
			virtual OpenViBE::boolean releaseConfigurationToken(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier)=0;
			/**
			 * \brief Iterates on the existing configuration tokens of this configuration manager
			 * \param rPreviousConfigurationTokenIdentifier [in] : the identifier of the token to start the iteration from
			 * \return the identifier of the next configuration token in case of success
			 * \return \c OV_UndefinedIdentifier in case there is no more token to iterate on
			 *
			 * Typicall use of this function is :
			 * \code
			 * CIdentifier l_oTokenIdentifier; // defaults to OV_UndefinedIdentifier
			 * while((l_oTokenIdentifier=l_pConfigurationManager->getNextConfigurationTokenIdentifier(l_oTokenIdentifier)) != OV_UndefinedIdentifier)
			 * {
			 *     // do some stuff with current token identified with l_oTokenIdentifier
			 * }
			 * \endcode
			 */
			virtual OpenViBE::CIdentifier getNextConfigurationTokenIdentifier(
				const OpenViBE::CIdentifier& rPreviousConfigurationTokenIdentifier) const=0;

			/**
			 * \brief Gets a token's name from its identifier
			 * \param rConfigurationTokenIdentifier [in] : the token identifier which name should be returned
			 * \return the name of the token in case of success
			 * \return an empty string in case of error
			 * \sa IConfigurationManager::getConfigurationTokenValue
			 */
			virtual OpenViBE::CString getConfigurationTokenName(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier) const=0;
			/**
			 * \brief Gets a token's value from its identifier
			 * \param rConfigurationTokenIdentifier [in] : the token identifier which value should be returned
			 * \return the value (unexapanded) of the token in case of success
			 * \return an empty string in case of error
			 * \sa IConfigurationManager::getConfigurationTokenName
			 * \sa IConfigurationManager::expand and others
			 * \note the returned value is not expanded by this configuration manager ; only the
			 *       value that was passed at creation time is returned. If you want to expand
			 *       things, please use IConfigurationManager::expand
			 */
			virtual OpenViBE::CString getConfigurationTokenValue(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier) const=0;

			/**
			 * \brief Changes the name of an exisiting token
			 * \param rConfigurationTokenIdentifier [in] : the identifier of the token which name should be changed
			 * \param rConfigurationTokenName [in] : the new name of the configuration token
			 * \return \e true in case of success
			 * \return \e false in case of error
			 * \note it is not valid to add a token using this function
			 * \sa IConfigurationManager::createConfigurationToken
			 * \sa IConfigurationManager::addConfigurationFromFile
			 * \sa IConfigurationManager::setConfigurationTokenValue
			 */
			virtual OpenViBE::boolean setConfigurationTokenName(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier,
				const OpenViBE::CString& rConfigurationTokenName)=0;
			/**
			 * \brief Changes the value of an exisiting token
			 * \param rConfigurationTokenIdentifier [in] : the identifier of the token which value should be changed
			 * \param rConfigurationTokenValue [in] : the new value of the configuration token
			 * \return \e true in case of success
			 * \return \e false in case of error
			 * \note it is not valid to add a token using this function
			 * \sa IConfigurationManager::createConfigurationToken
			 * \sa IConfigurationManager::addConfigurationFromFile
			 * \sa IConfigurationManager::setConfigurationTokenName
			 */
			virtual OpenViBE::boolean setConfigurationTokenValue(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier,
				const OpenViBE::CString& rConfigurationTokenValue)=0;

			/**
			 * \brief Searches the identifier of a token with a given name
			 * \param rConfigurationTokenName [in] : the name of the token which identifier should be found
			 * \param bRecursive [in] : when set to true, asks this configuration manager to propagate
			 *                          the request to parent configuration manager (if any).
			 * \return the identifier of the token with the actual name in case of success
			 * \return \c OV_UndefinedIdentifier in case of error
			 * \note if \c bRecursive is set to \e true then the returned identifier should
			 *       not be considered as the identifier of an existing token in this configuration manager
			 *       as it may have been returned from a parent configuration manager. Instead, one must consider
			 *       the returned identifier as a kind of boolean value : suck token actually exists or such
			 *       token does not exists.
			 */
			virtual OpenViBE::CIdentifier lookUpConfigurationTokenIdentifier(
				const OpenViBE::CString& rConfigurationTokenName,
				const OpenViBE::boolean bRecursive=false) const=0;
			/**
			 * \brief Searches the value of a token with a given name
			 * \param rConfigurationTokenName [in] : the name of the token which value should be found
			 * \return the value of the token with the actual name in case of success
			 * \note This function differs of \c getConfigurationTokenName in the sense that it
			 *       recursively requests a token value to parent configuration managers until it
			 *       finds one (if any). It also differs from the \c expand function in the sense that
			 *       it takes a token name as input but does not expand its value when it finds it.
			 */
			virtual OpenViBE::CString lookUpConfigurationTokenValue(
				const OpenViBE::CString& rConfigurationTokenName) const=0;
			/**
			 * \brief Expands a string to an expanded string based on its use of configuration tokens
			 * \param rExpression [in] : the string that you want to expan
			 * \return the expanded string
			 * \sa IConfigurationManager::expandAsFloat
			 * \sa IConfigurationManager::expandAsInteger
			 * \sa IConfigurationManager::expandAsUInteger
			 * \sa IConfigurationManager::expandAsBoolean
			 * \sa IConfigurationManager::expandAsEnumerationEntryValue
			 *
			 * Typical use of this function is :
			 * \code
			 * l_pConfigurationManager->expand("${TokenName}")
			 * \endcode
			 */
			virtual OpenViBE::CString expand(
				const OpenViBE::CString& rExpression) const=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Configuration_ConfigurationManager);

			/**
			 * \brief Expands a string to a floating point value based on its use of configuration tokens
			 * \param rExpression [in] : the string that you want to expand
			 * \param f64FallbackValue [in] : a fall back value to return in case the expanded
			 *                                string can not be parsed as a floating point value
			 * \return the expanded value
			 * \sa IConfigurationManager::expand
			 */
			virtual OpenViBE::float64 expandAsFloat(
				const OpenViBE::CString& rExpression,
				const OpenViBE::float64 f64FallbackValue=0) const=0;
			/**
			 * \brief Expands a string to an integer value based on its use of configuration tokens
			 * \param rExpression [in] : the string that you want to expand
			 * \param i64FallbackValue [in] : a fall back value to return in case the expanded
			 *                                string can not be parsed as an integer value
			 * \return the expanded value
			 * \sa IConfigurationManager::expand
			 */
			virtual OpenViBE::int64 expandAsInteger(
				const OpenViBE::CString& rExpression,
				const OpenViBE::int64 i64FallbackValue=0) const=0;
			/**
			 * \brief Expands a string to an unsigned integer value based on its use of configuration tokens
			 * \param rExpression [in] : the string that you want to expand
			 * \param ui64FallbackValue [in] : a fall back value to return in case the expanded
			 *                                 string can not be parsed as an unsigned integer value
			 * \return the expanded value
			 * \sa IConfigurationManager::expand
			 */
			virtual OpenViBE::uint64 expandAsUInteger(
				const OpenViBE::CString& rExpression,
				const OpenViBE::uint64 ui64FallbackValue=0) const=0;
			/**
			 * \brief Expands a string to a boolean value based on its use of configuration tokens
			 * \param rExpression [in] : the string that you want to expand
			 * \param bFallbackValue [in] : a fall back value to return in case the expanded
			 *                              string can not be parsed as a boolean value
			 * \return the expanded value
			 * \sa IConfigurationManager::expand
			 */
			virtual OpenViBE::boolean expandAsBoolean(
				const OpenViBE::CString& rExpression,
				const OpenViBE::boolean bFallbackValue=true) const=0;
			/**
			 * \brief Expands a string to an enumeration entry value based on its use of configuration tokens
			 * \param rExpression [in] : the string that you want to expand
			 * \param rEnumerationTypeIdentifier [in] : the enumeration type to use
			 * \param ui64FallbackValue [in] : a fall back value to return in case the expanded
			 *                                 string can not be parsed as an enumeration entry value
			 * \return the expanded value
			 * \sa IConfigurationManager::expand
			 * \sa ITypeManager
			 */
			virtual OpenViBE::uint64 expandAsEnumerationEntryValue(
				const OpenViBE::CString& rExpression,
				const OpenViBE::CIdentifier& rEnumerationTypeIdentifier,
				const OpenViBE::uint64 ui64FallbackValue=0) const=0;
		};
	};
};

#endif // __OpenViBE_Kernel_Configuration_IConfigurationManager_H__
