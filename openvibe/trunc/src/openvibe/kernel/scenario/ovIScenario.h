#ifndef __OpenViBE_Kernel_Scenario_IScenario_H__
#define __OpenViBE_Kernel_Scenario_IScenario_H__

#include "ovIAttributable.h"

namespace OpenViBE
{
	class IMemoryBuffer;

	namespace Kernel
	{
		class IBox;
		class ILink;
		class IProcessingUnit;

		/**
		 * \class IScenario
		 * \author Yann Renard (IRISA/INRIA)
		 * \date 2006-08-16
		 * \brief A static §OpenViBE§ scenario
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 *
		 * This class is a static scenario description.
		 * It is used to manipulate an §OpenViBE§
		 * box/connection collection...
		 *
		 * \todo Add meta information for this scenario
		 */
		class OV_API IScenario : public OpenViBE::Kernel::IAttributable
		{
		public:

			/** \name General purpose functions */
			//@{

			/**
			 * \brief Clears the scenario
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean clear(void)=0;
			/**
			 * \brief Merges this scenario with an other existing scenario
			 * \param rScenario [in] : a reference to the scenario to merge this scenario with
			 * \return \e true in case of success
			 * \return \e false in case of error
			 */
			virtual OpenViBE::boolean merge(
				const OpenViBE::Kernel::IScenario& rScenario)=0;

			//@{
			/** \name Visualisation tree management */
			//@{

			/**
			 * \brief Sets identifier of IVisualisationTree object corresponding to this scenario
			 * \param rVisualisationTreeIdentifier [in] : identifier to be stored
			 */
			virtual OpenViBE::boolean setVisualisationTreeIdentifier(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier)=0;
			/**
			 * \brief Returns identifier of IVisualisationTree object corresponding to this scenario
			 */
			virtual OpenViBE::CIdentifier getVisualisationTreeIdentifier(void) const=0;
			/**
			 * \brief Gets the details of the visualisation tree of this scenario
			 * \return The visualisation tree details
			 */
			virtual const OpenViBE::Kernel::IVisualisationTree& getVisualisationTreeDetails() const=0;
			/// \copydoc getVisualisationTreeDetails()const
			virtual OpenViBE::Kernel::IVisualisationTree& getVisualisationTreeDetails()=0;
			//@}

			//@{
			/** \name Box management */
			//@{

			/**
			 * \brief Gets next box identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding box
			 * \return The identifier of the next box in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first box
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextBoxIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;
			/**
			 * \brief Gets next box identifier running on a given process unit
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding box
			 * \param rProcessingUnitIdentifier [in] : The identifier
			 *        for the processing unit which the box should
			 *        be ran on.
			 * \return The identifier of the next box in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first box
			 *       identifier.
			 * \note Giving \c OV_UndefinedIdentifier as \c rProcessingUnitIdentifier
			 *       will cause this function to return an unaffected box
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextBoxIdentifierOnProcessingUnit(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rProcessingUnitIdentifier) const=0;
			/**
			 * \brief Tests whether a given identifier is a box or not
			 * \param rBoxIdentifier [in] : the identifier to test
			 * \return \e true if the identified object is a box
			 * \return \e false if the identified object is not a box
			 * \note Requesting a bad identifier returns \e false
			 */
			virtual OpenViBE::boolean isBox(
				const OpenViBE::CIdentifier& rBoxIdentifier) const=0;
			/**
			 * \brief Gets the details for a specific box
			 * \param rBoxIdentifier [in] : The identifier
			 *        of the box which details should be
			 *        sent.
			 * \return The box details
			 */
			virtual const OpenViBE::Kernel::IBox* getBoxDetails(
				const OpenViBE::CIdentifier& rBoxIdentifier) const=0;
			/// \copydoc getBoxDetails(const OpenViBE::CIdentifier&)const
			virtual OpenViBE::Kernel::IBox* getBoxDetails(
				const OpenViBE::CIdentifier& rBoxIdentifier)=0;
			/**
			 * \brief Adds a new box in the scenario
			 * \param rBoxIdentifier [out] : The identifier of
			 *        the created box
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rBoxIdentifier remains unchanged.
			 * \note This produces an empty and unconfigured box !
			 */
			virtual OpenViBE::boolean addBox(
				OpenViBE::CIdentifier& rBoxIdentifier)=0;
			/**
			 * \brief Adds a new box in the scenario based on an existing box
			 * \param rBox [in] : the box to copy in this scenario
			 * \param rBoxIdentifier [out] : The identifier of
			 *        the created box
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rBoxIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean addBox(
				const OpenViBE::Kernel::IBox& rBox,
				OpenViBE::CIdentifier& rBoxIdentifier)=0;
			/**
			 * \brief Adds a new box in the scenario
			 * \param rBoxAlgorithmClassIdentifier [in] : The
			 *        class identifier of the algorithm for
			 *        this box
			 * \param rBoxIdentifier [out] : The identifier of
			 *        the created box
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rBoxIdentifier remains unchanged.
			 * \note This function prepares the box according
			 *       to its algorithm class identifier !
			 */
			virtual OpenViBE::boolean addBox(
				const OpenViBE::CIdentifier& rBoxAlgorithmClassIdentifier,
				OpenViBE::CIdentifier& rBoxIdentifier)=0;
			/**
			 * \brief Removes a box of the scenario
			 * \param rBoxIdentifier [in] : The box identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note Each link related to this box is also removed
			 */
			virtual OpenViBE::boolean removeBox(
				const OpenViBE::CIdentifier& rBoxIdentifier)=0;

			//@}
			/** \name Connection management */
			//@{

			/**
			 * \brief Gets next link identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding link
			 * \return The identifier of the next link in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first link
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;
			/**
			 * \brief Gets next link identifier from fixed box
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding link
			 * \param rBoxIdentifier [in] : The box identifier
			 *        which the link should end to
			 * \return The identifier of the next link in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first link
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifierFromBox(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier) const=0;
			/**
			 * \brief Gets next link identifier from fixed box output
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding link
			 * \param rBoxIdentifier [in] : The box identifier
			 *        which the link should end to
			 * \param ui32OutputIndex [in] : The input index
			 *        which the link should end to
			 * \return The identifier of the next link in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first link
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifierFromBoxOutput(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::uint32 ui32OutputIndex) const=0;
			/**
			 * \brief Gets next link identifier from fixed box
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding link
			 * \param rBoxIdentifier [in] : The box identifier
			 *        which the link should start from
			 * \return The identifier of the next link in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first link
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifierToBox(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier) const=0;
			/**
			 * \brief Gets next link identifier from fixed box input
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding link
			 * \param rBoxIdentifier [in] : The box identifier
			 *        which the link should start from
			 * \param ui32InputInex [in] : The input index
			 *        which the link should start from
			 * \return The identifier of the next link in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first link
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifierToBoxInput(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::uint32 ui32InputInex) const=0;
			/**
			 * \brief Tests whether a given identifier is a link or not
			 * \param rIdentifier [in] : the identifier to test
			 * \return \e true if the identified object is a link
			 * \return \e false if the identified object is not a link
			 * \note Requesting a bad identifier returns \e false
			 */
			virtual OpenViBE::boolean isLink(
				const OpenViBE::CIdentifier& rIdentifier) const=0;
			/**
			 * \brief Gets the details for a specific link
			 * \param rLinkIdentifier [in] : The identifier
			 *        of the link which details should be
			 *        sent.
			 * \return The link details
			 */
			virtual const OpenViBE::Kernel::ILink* getLinkDetails(
				const OpenViBE::CIdentifier& rLinkIdentifier) const=0;
			/// \copydoc getLinkDetails(const OpenViBE::CIdentifier&)const
			virtual OpenViBE::Kernel::ILink* getLinkDetails(
				const OpenViBE::CIdentifier& rLinkIdentifier)=0;
			/**
			 * \brief Creates a connection between two boxes
			 * \param rSourceBoxIdentifier [in] : The source
			 *        box identifier
			 * \param ui32SourceBoxOutputIndex [in] : The output
			 *        index for the given source box
			 * \param rTargetBoxIdentifier [in] : The target
			 *        box identifier
			 * \param ui32TargetBoxInputIndex [in] : The input
			 *        index for the given target box
			 * \param rLinkIdentifier [out] : The created link
			 *        identifier.
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         rLinkIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean connect(
				const OpenViBE::CIdentifier& rSourceBoxIdentifier,
				const OpenViBE::uint32 ui32SourceBoxOutputIndex,
				const OpenViBE::CIdentifier& rTargetBoxIdentifier,
				const OpenViBE::uint32 ui32TargetBoxInputIndex,
				OpenViBE::CIdentifier& rLinkIdentifier)=0;
			/**
			 * \brief Deletes a connection between two boxes
			 * \param rSourceBoxIdentifier [in] : The source
			 *        box identifier
			 * \param ui32SourceBoxOutputIndex [in] : The output
			 *        index for the given source box
			 * \param rTargetBoxIdentifier [in] : The target
			 *        box identifier
			 * \param ui32TargetBoxInputIndex [in] : The input
			 *        index for the given target box
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean disconnect(
				const OpenViBE::CIdentifier& rSourceBoxIdentifier,
				const OpenViBE::uint32 ui32SourceBoxOutputIndex,
				const OpenViBE::CIdentifier& rTargetBoxIdentifier,
				const OpenViBE::uint32 ui32TargetBoxInputIndex)=0;
			/**
			 * \brief Deletes a connection between two boxes
			 * \param rLinkIdentifier [out] : The identifier
			 *        for the link to be deleted
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean disconnect(
				const OpenViBE::CIdentifier& rLinkIdentifier)=0;

			//@}
			/** \name Comment management */
			//@{

			/**
			 * \brief Gets next comment identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding comment
			 * \return The identifier of the next comment in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first comment
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextCommentIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;
			/**
			 * \brief Tests whether a given identifier is a comment or not
			 * \param rCommentIdentifier [in] : the identifier to test
			 * \return \e true if the identified object is a comment
			 * \return \e false if the identified object is not a comment
			 * \note Requesting a bad identifier returns \e false
			 */
			virtual OpenViBE::boolean isComment(
				const OpenViBE::CIdentifier& rCommentIdentifier) const=0;
			/**
			 * \brief Gets the details for a specific comment
			 * \param rCommentIdentifier [in] : The identifier
			 *        of the comment which details should be
			 *        sent.
			 * \return The comment details
			 */
			virtual const OpenViBE::Kernel::IComment* getCommentDetails(
				const OpenViBE::CIdentifier& rCommentIdentifier) const=0;
			/// \copydoc getCommentDetails(const OpenViBE::CIdentifier&)const
			virtual OpenViBE::Kernel::IComment* getCommentDetails(
				const OpenViBE::CIdentifier& rCommentIdentifier)=0;
			/**
			 * \brief Adds a new comment in the scenario
			 * \param rCommentIdentifier [out] : The identifier of
			 *        the created comment
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rCommentIdentifier remains unchanged.
			 * \note This produces an empty and unconfigured comment !
			 */
			virtual OpenViBE::boolean addComment(
				OpenViBE::CIdentifier& rCommentIdentifier)=0;
			/**
			 * \brief Adds a new comment in the scenario based on an existing comment
			 * \param rComment [in] : the comment to copy in this scenario
			 * \param rCommentIdentifier [out] : The identifier of
			 *        the created comment
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rCommentIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean addComment(
				const OpenViBE::Kernel::IComment& rComment,
				OpenViBE::CIdentifier& rCommentIdentifier)=0;
			/**
			 * \brief Removes a comment of the scenario
			 * \param rCommentIdentifier [in] : The comment identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean removeComment(
				const OpenViBE::CIdentifier& rCommentIdentifier)=0;

			//@}
			/** \name Processing units management */
			//@{

			/**
			 * \brief Gets next processing unit identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding processing unit
			 * \return The identifier of the next processing unit in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first processing unit
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextProcessingUnitIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;
			/**
			 * \brief Tests if a processing unit exists
			 * \param rIdentifier [in] : the identifier which should be tested
			 * \return \e true if the provided identifier is a processing unit identifier.
			 * \return \e false in other cases.
			 */
			virtual OpenViBE::boolean isProcessingUnit(
				const OpenViBE::CIdentifier& rIdentifier) const=0;
			/**
			 * \brief Gets processing unit details
			 * \param rProcessingUnitIdentifier [in] : the identifier of the processing unit which details should be returned
			 * \return the details for the identifier processing unit in case of success.
			 * \return \c NULL in case of error.
			 */
			virtual const OpenViBE::Kernel::IProcessingUnit* getProcessingUnitDetails(
				const OpenViBE::CIdentifier& rProcessingUnitIdentifier) const=0;
			/// \copydoc getProcessingUnitDetails(const OpenViBE::CIdentifier&)const
			virtual OpenViBE::Kernel::IProcessingUnit* getProcessingUnitDetails(
				const OpenViBE::CIdentifier& rProcessingUnitIdentifier)=0;
			/**
			 * \brief Adds a new processing unit in the scenario
			 * \param rProcessingUnitIdentifier [out] : the newly created identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean addProcessingUnit(
				OpenViBE::CIdentifier& rProcessingUnitIdentifier)=0;
			/**
			 * \brief Removes a processing unit in the scenario
			 * \param rProcessingUnitIdentifier [in] : the identifier of the processing unit to remove
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean removeProcessingUnit(
				const OpenViBE::CIdentifier& rProcessingUnitIdentifier)=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Scenario_Scenario)
		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_IScenario_H__
