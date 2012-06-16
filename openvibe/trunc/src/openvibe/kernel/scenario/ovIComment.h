#ifndef __OpenViBE_Kernel_Scenario_IComment_H__
#define __OpenViBE_Kernel_Scenario_IComment_H__

#include "ovIAttributable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IComment
		 * \author Yann Renard INRIA
		 * \date 2010-04-27
		 * \brief A comment class for scenarios
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 *
		 * This interface can be used in order to fully describe an
		 * §OpenViBE§ comment in order to help the understanding of a
		 * given scenario.
		 */
		class OV_API IComment : public OpenViBE::Kernel::IAttributable
		{
		public:

			/** \name Comment naming and identification */
			//@{

			/**
			 * \brief Gets the identifier of this comment
			 * \return The identifier of this §OpenViBE§ comment.
			 */
			virtual OpenViBE::CIdentifier getIdentifier(void) const=0;
			/**
			 * \brief Gets the display name of this comment
			 * \return The name of this §OpenViBE§ comment.
			 */
			virtual OpenViBE::CString getText(void) const=0;
			/**
			 * \brief Changes the identifier of this comment
			 * \param rIdentifier [in] : The new identifier
			 *        this comment should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setIdentifier(
				const OpenViBE::CIdentifier& rIdentifier)=0;
			/**
			 * \brief Changes the text of this comment
			 * \param sText [in] : The text this comment should contain
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setText(
				const OpenViBE::CString& sText)=0;

			//@}
			/** \name Initialisation from prototypes etc... */
			//@{

			/**
			 * \brief Initializes the comment from an already existing comment
			 * \param rExisitingComment [in] : The existing box.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Resets the comment and initializes its text/attributes
			 * according to the existing comment.
			 */
			virtual OpenViBE::boolean initializeFromExistingComment(
				const OpenViBE::Kernel::IComment& rExisitingComment)=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Scenario_Comment)
		};

		typedef OpenViBE::Kernel::IComment IStaticCommentContext;
	};
};

#endif // __OpenViBE_Kernel_Scenario_IComment_H__
