#ifndef __OpenViBE_CString_H__
#define __OpenViBE_CString_H__

#include "ov_base.h"

namespace OpenViBE
{
	typedef struct CStringImpl CStringImpl;

	/**
	 * \class CString
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2006-08-10
	 * \brief String class to avoid std::string in the interface
	 * \ingroup Group_Base
	 *
	 * This class helps avoiding std::string being present in exposed
	 * C++ interface, eventually resulting in compile/link errors when
	 * dynamically loading modules.
	 *
	 * \note The implementation uses std::string of course :)
	 */
	class OV_API CString
	{
	public:

		/** \name Constructor / Destructor */
		//@{

		/**
		 * \brief Default constructor
		 *
		 * Initializes the string to an empty string.
		 */
		CString(void);
		/**
		 * \brief Copy constructor
		 * \param rString [in] : The string to copy
		 *
		 * Copies the content of \c rString into the new string.
		 */
		CString(const OpenViBE::CString& rString);
		/**
		 * \brief Constructor based on ASCII strings
		 * \param pString [in] : The string to copy
		 *
		 * Copies the content of \c pString into the new string.
		 */
		CString(const char* pString);
		/**
		 * \brief Destructor
		 *
		 * The destructor releases the std::string implementation !
		 */
		virtual ~CString(void);

		//@}
		/** \name Operators */
		//@{

		/**
		 * \brief ASCII string cast operator
		 * \return The string formated as standard ASCII string used in C.
		 *
		 * The implementation simply calls \c c_str().
		 */
		operator const char* (void) const;
		/**
		 * \brief Affectation operator (copy)
		 * \param rString [in] : The string to copy
		 * \return This string.
		 */
		OpenViBE::CString& operator=(
			const OpenViBE::CString& rString);
		/**
		 * \brief Addition operator
		 * \param rString1 [in] : The first part of the resulting string
		 * \param rString2 [in] : The second part of the resulting string
		 * \return The concatenation of \c rString1 and \c rString2.
		 */
		friend OV_API const OpenViBE::CString operator+(
			const OpenViBE::CString& rString1,
			const OpenViBE::CString& rString2);
		/**
		 * \brief Equality comparison operator
		 * \param rString1 [in] : The first part of the resulting string
		 * \param rString2 [in] : The second part of the resulting string
		 * \return \e true is \c rString1 is exactly rString2.
		 * \return \e false in other case.
		 * \note This is case sensitive !
		 */
		friend OV_API OpenViBE::boolean operator==(
			const OpenViBE::CString& rString1,
			const OpenViBE::CString& rString2);
		/**
		 * \brief Inequality comparison operator
		 * \param rString1 [in] : The first part of the resulting string
		 * \param rString2 [in] : The second part of the resulting string
		 * \return \e false is \c rString1 is exactly rString2.
		 * \return \e true in other case.
		 * \note This is case sensitive !
		 */
		friend OV_API OpenViBE::boolean operator!=(
			const OpenViBE::CString& rString1,
			const OpenViBE::CString& rString2);

/**
		 * \brief Order comparison operator (necessary to use CString as a key in a stl map)
		 * \param rString1 [in] : The first part of the resulting string
		 * \param rString2 [in] : The second part of the resulting string
		 * \return \e false is \c rString1 is exactly rString2.
		 * \return \e true in other case.
		 * \note This is case sensitive !
		 */
		friend OV_API OpenViBE::boolean operator<(
			const OpenViBE::CString& rString1,
			const OpenViBE::CString& rString2);
		//@}

		/**
		 * \brief Initializes this string from another §OpenViBE§ string
		 * \param rString [in] : the §OpenViBE§ string to initialize this string from
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual OpenViBE::boolean set(
			const OpenViBE::CString& rString);
		/**
		 * \brief Initializes this string from an ANSI/ASCII string
		 * \param pString [in] : the ANSI/ASCII string to initialize this string from
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual OpenViBE::boolean set(
			const char* pString);
		/**
		 * \brief Converts this string to an ANSI/ASCII string
		 * \return the ANSI/ASCII converted string.
		 */
		virtual const char* toASCIIString(void) const;

	protected:

		OpenViBE::CStringImpl* m_pStringImpl; ///< The string implementation
	};
};

#endif // __OpenViBE_CString_H__
