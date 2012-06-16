#ifndef __EBML_CIdentifier_H__
#define __EBML_CIdentifier_H__

#include "defines.h"

namespace EBML
{
	/**
	 * \class CIdentifier
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2006-08-07
	 * \brief Base class to work with EBML identifiers
	 *
	 * This class is used in order to work on EBML identifier.
	 * It handles 64 bits values for now, but could easily be
	 * changed to handle bigger values if needed.
	 *
	 * Be sure to look at http://ebml.sourceforge.net/specs/ in
	 * order to understand what an identifier is and how it works
	 */
	class EBML_API CIdentifier
	{
	public:

		/** \name Constructors */
		//@{

		/**
		 * \brief Basic constructor
		 *
		 * Initializes the identifier to 0.
		 */
		CIdentifier(void);
		/**
		 * \brief Integer based constructor
		 * \param ui64Identifier [in] : The value to use
		 *
		 * Initializes the identifier to the given 64 bits value.
		 */
		CIdentifier(const EBML::uint64 ui64Identifier);
		/**
		 * \brief 32 bits integer based constructor
		 * \param ui32Identifier1 [in] : the first part of the identifier
		 * \param ui32Identifier2 [in] : the second part of the identifier
		 *
		 * Builds up the 64 bits identifier given its two 32 bits
		 * components.
		 */
		CIdentifier(const EBML::uint32 ui32Identifier1, const EBML::uint32 ui32Identifier2);
		/**
		 * \brief Copy constructor
		 * \param rIdentifier [in] : The source identifier to use
		 *
		 * Initializes this identifier to the same value as
		 * the given source identifier.
		 */
		CIdentifier(const EBML::CIdentifier& rIdentifier);

		//@}
		/** \name Operators */
		//@{

		/**
		 * \brief Copy operator
		 * \param rIdentifier [in] : The source identifier to copy from
		 * \return a const reference on this identifier
		 *
		 * Initializes this identifier to the same value as
		 * the given source identifier.
		 */
		const EBML::CIdentifier& operator=(const EBML::CIdentifier& rIdentifier);
		/**
		 * \brief Equality comparison operator
		 * \param rIdentifier1 [in] : The first identifier to compare
		 * \param rIdentifier2 [in] : The second identifier to compare
		 * \return \e true when the identifiers are equal, \e false
		 *         when they are different.
		 *
		 * This function compares the two 64 bits values.
		 */
		friend EBML_API bool operator==(const EBML::CIdentifier& rIdentifier1, const EBML::CIdentifier& rIdentifier2);
		/**
		 * \brief Difference comparison operator
		 * \param rIdentifier1 [in] : The first identifier to compare
		 * \param rIdentifier2 [in] : The second identifier to compare
		 * \return \e true when the identifiers are different, \e false
		 *         when they are equal.
		 *
		 * This function compares the two 64 bits values.
		 */
		friend EBML_API bool operator!=(const EBML::CIdentifier& rIdentifier1, const EBML::CIdentifier& rIdentifier2);
		/**
		 * \brief Ordering comparison operator
		 * \param rIdentifier1 [in] : The first identifier to compare
		 * \param rIdentifier2 [in] : The second identifier to compare
		 * \return \e true when the first identifier is lesser or equal
		 *         to the second identifier, \e false in other cases.
		 *
		 * This function compares the two 64 bits values.
		 */
		friend EBML_API bool operator<=(const EBML::CIdentifier& rIdentifier1, const EBML::CIdentifier& rIdentifier2);
		/**
		 * \brief Ordering comparison operator
		 * \param rIdentifier1 [in] : The first identifier to compare
		 * \param rIdentifier2 [in] : The second identifier to compare
		 * \return \e true when the first identifier is greater or equal
		 *         to the second identifier, \e false in other cases.
		 *
		 * This function compares the two 64 bits values.
		 */
		friend EBML_API bool operator>=(const EBML::CIdentifier& rIdentifier1, const EBML::CIdentifier& rIdentifier2);
		/**
		 * \brief Ordering comparison operator
		 * \param rIdentifier1 [in] : The first identifier to compare
		 * \param rIdentifier2 [in] : The second identifier to compare
		 * \return \e true when the first identifier is lesser and not equal
		 *         to the second identifier, \e false in other cases.
		 *
		 * This function compares the two 64 bits values.
		 */
		friend EBML_API bool operator<(const EBML::CIdentifier& rIdentifier1, const EBML::CIdentifier& rIdentifier2);
		/**
		 * \brief Ordering comparison operator
		 * \param rIdentifier1 [in] : The first identifier to compare
		 * \param rIdentifier2 [in] : The second identifier to compare
		 * \return \e true when the first identifier is greater and not equal
		 *         to the second identifier, \e false in other cases.
		 *
		 * This function compares the two 64 bits values.
		 */
		friend EBML_API bool operator>(const EBML::CIdentifier& rIdentifier1, const EBML::CIdentifier& rIdentifier2);
		/**
		 * \brief Cast operator
		 * \return \e the 64 bits value contained by this identifier.
		 */
		operator EBML::uint64(void) const;

		//@}

	protected:

		EBML::uint64 m_ui64Identifier; ///< The 64 bits value of this identifier
	};
};

#endif // __EBML_CIdentifier_H__
