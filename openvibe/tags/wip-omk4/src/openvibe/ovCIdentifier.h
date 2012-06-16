#ifndef __OpenViBE_CIdentifier_H__
#define __OpenViBE_CIdentifier_H__

#include "ov_base.h"
#include "ovCString.h"

#define OV_UndefinedIdentifier OpenViBE::CIdentifier(0xffffffff, 0xffffffff)

namespace OpenViBE
{
	/**
	 * \class CIdentifier
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2006-06-16
	 * \brief Globally used identifiaction class
	 * \ingroup Group_Base
	 *
	 * This class is the basic class to use in order to identify
	 * objects in the §OpenViBE§ platform. It can be used for class
	 * identification, for object identification and any user needed
	 * identification process. 
	 *
	 * The identification of the §OpenViBE§ platform is based on
	 * 64 bits integers. This allows more than
	 * 1 800 000 000 000 000 000 identifiers to exist... I won't
	 * write that this should be enough ;) (this is (c) Bill Gates)
	 * but at least... it may fit our today needs !
	 *
	 * This class is heavily used in the OpenViBE::IObject class. Also,
	 * the §OpenViBE§ specification gives serveral already defined
	 * class identifiers the developper should know of. For this,
	 * let you have a look to the documentation of ov_defines.h !
	 *
	 * \sa ov_defines.h
	 */
	class OV_API CIdentifier
	{
	public:

		/** \name Constructors */
		//@{

		/**
		 * \brief Default constructor
		 *
		 * Builds up the 64 bits identifier intialized to
		 * \c OV_UndefinedIdentifier.
		 */
		CIdentifier(void);
		/**
		 * \brief 32 bits integer based constructor
		 * \param ui32Identifier1 [in] : the first part of the identifier
		 * \param ui32Identifier2 [in] : the second part of the identifier
		 *
		 * Builds up the 64 bits identifier given its two 32 bits
		 * components.
		 */
		CIdentifier(
			const OpenViBE::uint32 ui32Identifier1,
			const OpenViBE::uint32 ui32Identifier2);
		/**
		 * \brief 64 bits integer based constructor
		 * \param ui64Identifier [in] : The identifier
		 */
		CIdentifier(
			const OpenViBE::uint64 ui64Identifier);
		/**
		 * \brief Copy constructor
		 * \param rIdentifier [in] : the identifier to initialize
		 *        this identifier from
		 *
		 * Builds up the 64 bits identifier exacly the same as
		 * given rIdentifier parameter.
		 */
		CIdentifier(
			const OpenViBE::CIdentifier& rIdentifier);

		//@}
		/** \name Operators */
		//@{

		/**
		 * \brief Affectation operator
		 * \param rIdentifier [in] : the identifier to initialize
		 *        this identifier from
		 * \return this identifier
		 *
		 * Reinitializes the 64 bits identifier exactly the same as
		 * given rIdentifier parameter.
		 */
		OpenViBE::CIdentifier& operator=(
			const OpenViBE::CIdentifier& rIdentifier);
		/**
		 * \brief Increments this identifier by 1
		 * \return this identifer
		 * \note if this identifier is \c OV_UndefinedIdentifier, it is not incremented
		 * \note if this idenfitier is not \c OV_UndefinedIdentifier, it can not becomre \c OV_UndefinedIdentifier after being incremented
		 */
		OpenViBE::CIdentifier& operator++(void);
		/**
		 * \brief Decrements this identifier by 1
		 * \return this identifer
		 * \note if this identifier is \c OV_UndefinedIdentifier, it is not decremented
		 * \note if this idenfitier is not \c OV_UndefinedIdentifier, it can not becomre \c OV_UndefinedIdentifier after being decremented
		 */
		OpenViBE::CIdentifier& operator--(void);
		/**
		 * \brief Equality test operator
		 * \param rIdentifier1 [in] : the first identifier to compare
		 * \param rIdentifier2 [in] : the second identifier to compare
		 * \return \e true if the two identifiers are equal,
		 * \return \e false if the two identifiers are different
		 *
		 * Compares both 32 bits parts of the two identifiers and
		 * checks if those are equal or not.
		 *
		 * \sa operator!=
		 */
		friend OV_API OpenViBE::boolean operator==(
			const OpenViBE::CIdentifier& rIdentifier1,
			const OpenViBE::CIdentifier& rIdentifier2);
		/**
		 * \brief Difference test operator
		 * \param rIdentifier1 [in] : the first identifier to compare
		 * \param rIdentifier2 [in] : the second identifier to compare
		 * \return \e true if the two identifiers are different,
		 * \return \e false if the two identifiers are equal
		 *
		 * Compares both 32 bits parts of the two identifiers and
		 * checks if those are equal or not.
		 *
		 * \sa operator==
		 */
		friend OV_API OpenViBE::boolean operator!=(
			const OpenViBE::CIdentifier& rIdentifier1,
			const OpenViBE::CIdentifier& rIdentifier2);
		/**
		 * \brief Order test operator
		 * \param rIdentifier1 [in] : the first identifier to compare
		 * \param rIdentifier2 [in] : the second identifier to compare
		 * \return \e true if the first identifier is less than the second one
		 * \return \e false if the first identifier is greater or equal to the second one
		 *
		 * Compares both 32 bits parts of the two identifiers.
		 *
		 * \sa operator>
		 * \sa operator==
		 */
		friend OV_API OpenViBE::boolean operator<(
			const OpenViBE::CIdentifier& rIdentifier1,
			const OpenViBE::CIdentifier& rIdentifier2);
		/**
		 * \brief Order test operator
		 * \param rIdentifier1 [in] : the first identifier to compare
		 * \param rIdentifier2 [in] : the second identifier to compare
		 * \return \e true if the first identifier is greater than the second one
		 * \return \e false if the first identifier is less or equal to the second one
		 *
		 * Compares both 32 bits parts of the two identifiers.
		 *
		 * \sa operator<
		 * \sa operator==
		 */
		friend OV_API OpenViBE::boolean operator>(
			const OpenViBE::CIdentifier& rIdentifier1,
			const OpenViBE::CIdentifier& rIdentifier2);
		/**
		 * \brief Order test operator
		 * \param rIdentifier1 [in] : the first identifier to compare
		 * \param rIdentifier2 [in] : the second identifier to compare
		 * \return \e true if the first identifier is less or equal than the second one
		 * \return \e false if the first identifier is greater to the second one
		 *
		 * Compares both 32 bits parts of the two identifiers.
		 *
		 * \sa operator>
		 * \sa operator==
		 */
		friend OV_API OpenViBE::boolean operator<=(
			const OpenViBE::CIdentifier& rIdentifier1,
			const OpenViBE::CIdentifier& rIdentifier2)
		{
			return !(rIdentifier1>rIdentifier2);
		}
		/**
		 * \brief Order test operator
		 * \param rIdentifier1 [in] : the first identifier to compare
		 * \param rIdentifier2 [in] : the second identifier to compare
		 * \return \e true if the first identifier is greater or equal than the second one
		 * \return \e false if the first identifier is less to the second one
		 *
		 * Compares both 32 bits parts of the two identifiers.
		 *
		 * \sa operator<
		 * \sa operator==
		 */
		friend OV_API OpenViBE::boolean operator>=(
			const OpenViBE::CIdentifier& rIdentifier1,
			const OpenViBE::CIdentifier& rIdentifier2)
		{
			return !(rIdentifier1<rIdentifier2);
		}

		//@}

		/**
		 * \brief Converts this identifier into an §OpenViBE§ string
		 * \return This identifier represented as an §OpenViBE§ string
		 */
		OpenViBE::CString toString(void) const;
		/**
		 * \brief Reads a an §OpenViBE§ string to extract this identifier
		 * \param rString [in] : the string to convert
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		OpenViBE::boolean fromString(
			const OpenViBE::CString& rString);
		/**
		 * \brief Creates a random identifier
		 * \return a random identifier
		 * \note The returned identifier can not be \c OV_UndefinedIdentifier
		 */
		static OpenViBE::CIdentifier random(void);

	protected:

		OpenViBE::uint64 m_ui64Identifier; ///< the 64 bit identifier value
	};
};

#endif // __OpenViBE_CIdentifier_H__
