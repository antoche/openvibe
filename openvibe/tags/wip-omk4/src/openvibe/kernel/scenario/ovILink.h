#ifndef __OpenViBE_Kernel_Scenario_ILink_H__
#define __OpenViBE_Kernel_Scenario_ILink_H__

#include "ovIAttributable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class ILink
		 * \author Yann Renard (IRISA/INRIA)
		 * \date 2006-08-16
		 * \brief Link information between §OpenViBE§ box
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 *
		 * This class collects informations between §OpenViBE§
		 * boxes : box identifiers and input / output indices.
		 */
		class OV_API ILink : public OpenViBE::Kernel::IAttributable
		{
		public:

			/** \name Identification */
			//@{

			/**
			 * \brief Changes this link's identifier
			 * \param rIdentifier [in] : The new identifier
			 *        this link should have
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setIdentifier(
				const OpenViBE::CIdentifier& rIdentifier)=0;
			/**
			 * \brief Gets the identifier of this link
			 * \return The identifier of this link.
			 */
			virtual OpenViBE::CIdentifier getIdentifier(void) const=0;

			//@}
			/** \name Source / Target management */
			//@{

			/**
			 * \brief Sets the source of this link
			 * \param rBoxIdentifier [in] : The identifier of the
			 *        source box
			 * \param ui32BoxOutputIndex [in] : The index of the
			 *        output to use on the source box
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSource(
				const OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::uint32 ui32BoxOutputIndex)=0;
			/**
			 * \brief Sets the target of this link
			 * \param rBoxIdentifier [in] : The identifier of the
			 *        target box
			 * \param ui32BoxInputIndex [in] : The index of the
			 *        input to use on the target box
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setTarget(
				const OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::uint32 ui32BoxInputIndex)=0;
			/**
			 * \brief Gets the source information for this link
			 * \param rBoxIdentifier [out] : The identifier of
			 *        of the source box
			 * \param ui32BoxOutputIndex [out] : The output index
			 *        of the source box
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean getSource(
				OpenViBE::CIdentifier& rBoxIdentifier,
				OpenViBE::uint32& ui32BoxOutputIndex) const=0;
			/**
			 * \brief Gets the source box identifier for this link
			 * \return the source box identifier for thit link
			 */
			virtual OpenViBE::CIdentifier getSourceBoxIdentifier(void) const=0;
			/**
			 * \brief Gets the source box output index for this link
			 * \return the source box output index for this link
			 */
			virtual OpenViBE::uint32 getSourceBoxOutputIndex(void) const=0;
			/**
			 * \brief Gets the target information for this link
			 * \param rTargetBoxIdentifier [out] : The identifier
			 *        of the target box
			 * \param ui32BoxInputIndex [out] : The input index
			 *        of the target box
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean getTarget(
				OpenViBE::CIdentifier& rTargetBoxIdentifier,
				OpenViBE::uint32& ui32BoxInputIndex) const=0;
			/**
			 * \brief Gets the target box identifier for this link
			 * \return the target box identifier for this link
			 */
			virtual OpenViBE::CIdentifier getTargetBoxIdentifier(void) const=0;
			/**
			 * \brief Gets the target box input index for this link
			 * \return the target box input index for this link
			 */
			virtual OpenViBE::uint32 getTargetBoxInputIndex(void) const=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Scenario_Link)
		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_ILink_H__
