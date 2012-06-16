#ifndef __OpenViBE_CNameValuePairList_H__
#define __OpenViBE_CNameValuePairList_H__

#include "ov_base.h"

namespace OpenViBE
{
	typedef struct CNameValuePairListImpl CNameValuePairListImpl;

	/**
	 * \class CNameValuePairList
	 * \author Vincent Delannoy (INRIA/IRISA)
	 * \date 2008-07
	 * \brief This class handles a list of name/value pairs.
	 * It handles a (hidden) map associating string keys to string values.
	 * \ingroup Group_Base
	 *
	 * This class avoids the need to include stl's map header file in the C++ interface,
	 * thus preventing potential compile/link errors when dynamically loading modules.
	 *
	 * \note Implementation based on std::map<std::string, std::string>
	 */
	class OV_API CNameValuePairList
	{
	public:

		/** \name Constructor / Destructor */
		//@{

		/**
		 * \brief Default constructor
		 *
		 * Initializes the list.
		 */
		CNameValuePairList(void);
		/**
		 * \brief Copy constructor
		 * \param rNameValuePairList [in] : The list to copy
		 *
		 * Copies the contents of \c rNameValuePairList into the new list.
		 */
		CNameValuePairList(const OpenViBE::CNameValuePairList& rNameValuePairList);
		/**
		 * \brief Destructor
		 *
		 * The destructor releases the std::map implementation !
		 */
		virtual ~CNameValuePairList(void);

		//@}
		/** \name Operators */
		//@{

		/**
		 * \brief Affectation operator (copy)
		 * \param rNameValuePairList [in] : The list to copy
		 * \return This list.
		 */
		OpenViBE::CNameValuePairList& operator=(
			const OpenViBE::CNameValuePairList& rNameValuePairList);
		//@}

		/**
		 * \brief Insert a name/value pair
		 * \param [in] rName Name to add to the list
		 * \param [in] rValue Value to associate with the name
		 * \return True if pair could be inserted, false otherwise
		 */
		OpenViBE::boolean setValue(
			const OpenViBE::CString& rName,
			const OpenViBE::CString& rValue);

		/**
		 * \brief Insert a name/value pair
		 * \param [in] rName Name to add to the list
		 * \param [in] pValue Value to associate with the name
		 * \return True if pair could be inserted, false otherwise
		 */
		OpenViBE::boolean setValue(
			const OpenViBE::CString& rName,
			const char* pValue);

		/**
		 * \brief Insert a name/value pair where value is a float64
		 * \param [in] rName Name to add to the list
		 * \param [in] rValue Float64 value to associate with the name
		 * \return True if pair could be inserted, false otherwise
		 */
		OpenViBE::boolean setValue(
			const OpenViBE::CString& rName,
			const OpenViBE::float64& rValue);

		/**
		 * \brief Insert a name/value pair where value is a boolean
		 * \param [in] rName Name to add to the list
		 * \param [in] bValue Boolean value to associate with the name
		 * \return True if pair could be inserted, false otherwise
		 */
		OpenViBE::boolean setValue(
			const OpenViBE::CString& rName,
			OpenViBE::boolean bValue);

		/**
		 * \brief Retrieve a value from the list
		 * \param [in] rName Name whose value is to be retrieved
		 * \param [out] rValue Value to be retrieved from the list
		 * \return True if value could be retrieved, false otherwise
		 */
		OpenViBE::boolean getValue(
			const OpenViBE::CString& rName,
			OpenViBE::CString& rValue) const;

		/**
		 * \brief Retrieve a float64 value from the list
		 * \param [in] rName Name whose value is to be retrieved
		 * \param [out] rValue Float64 value to be retrieved
		 * \return True if a float64 value could be retrieved, false otherwise
		 */
		OpenViBE::boolean getValue(
			const OpenViBE::CString& rName,
			OpenViBE::float64& rValue) const;

		/**
		 * \brief Retrieve a boolean value from the list
		 * In the current implementation a value evaluates to true if its string
		 * equals "1" and to false if it equals "0".
		 * \param rValue String to evaluate
		 * \return True if string evaluates to a boolean, false otherwise
		 */
		OpenViBE::boolean getValue(
			const OpenViBE::CString& rName,
			OpenViBE::boolean& rValue) const;

	protected:

		OpenViBE::CNameValuePairListImpl* m_pNameValuePairListImpl; ///< The list implementation
	};
};

#endif // __OpenViBE_CNameValuePairList_H__
