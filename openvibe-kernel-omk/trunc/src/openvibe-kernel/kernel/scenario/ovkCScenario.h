#ifndef __OpenViBEKernel_Kernel_Scenario_CScenario_H__
#define __OpenViBEKernel_Kernel_Scenario_CScenario_H__

#include "../ovkTKernelObject.h"

#include "ovkTAttributable.h"

#include <vector>
#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CBox;
		class CComment;
		class CLink;
		class CProcessingUnit;

		class CScenario : public OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IScenario > >
		{
		public:

			CScenario(const OpenViBE::Kernel::IKernelContext& rKernelContext, const OpenViBE::CIdentifier& rIdentifier);
			virtual ~CScenario(void);

			virtual OpenViBE::boolean clear(void);
			virtual OpenViBE::boolean merge(
				const OpenViBE::Kernel::IScenario& rScenario);

			virtual OpenViBE::boolean setVisualisationTreeIdentifier(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier);
			virtual OpenViBE::CIdentifier getVisualisationTreeIdentifier(void) const;
			virtual const OpenViBE::Kernel::IVisualisationTree& getVisualisationTreeDetails() const;
			virtual OpenViBE::Kernel::IVisualisationTree& getVisualisationTreeDetails();

			virtual OpenViBE::CIdentifier getNextBoxIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;
			virtual OpenViBE::CIdentifier getNextBoxIdentifierOnProcessingUnit(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rProcessingUnitIdentifier) const;
			virtual OpenViBE::boolean isBox(
				const OpenViBE::CIdentifier& rBoxIdentifier) const;
			virtual const OpenViBE::Kernel::IBox* getBoxDetails(
				const OpenViBE::CIdentifier& rBoxIdentifier) const;
			virtual OpenViBE::Kernel::IBox* getBoxDetails(
				const OpenViBE::CIdentifier& rBoxIdentifier);
			virtual OpenViBE::boolean addBox(
				OpenViBE::CIdentifier& rBoxIdentifier);
			virtual OpenViBE::boolean addBox(
				const OpenViBE::Kernel::IBox& rBox,
				OpenViBE::CIdentifier& rBoxIdentifier);
			virtual OpenViBE::boolean addBox(
				const OpenViBE::CIdentifier& rBoxAlgorithmIdentifier,
				OpenViBE::CIdentifier& rBoxIdentifier);
			virtual OpenViBE::boolean removeBox(
				const OpenViBE::CIdentifier& rBoxIdentifier);

			virtual OpenViBE::CIdentifier getNextCommentIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;
			virtual OpenViBE::boolean isComment(
				const OpenViBE::CIdentifier& rCommentIdentifier) const;
			virtual const OpenViBE::Kernel::IComment* getCommentDetails(
				const OpenViBE::CIdentifier& rCommentIdentifier) const;
			virtual OpenViBE::Kernel::IComment* getCommentDetails(
				const OpenViBE::CIdentifier& rCommentIdentifier);
			virtual OpenViBE::boolean addComment(
				OpenViBE::CIdentifier& rCommentIdentifier);
			virtual OpenViBE::boolean addComment(
				const OpenViBE::Kernel::IComment& rComment,
				OpenViBE::CIdentifier& rCommentIdentifier);
			virtual OpenViBE::boolean removeComment(
				const OpenViBE::CIdentifier& rCommentIdentifier);

			virtual OpenViBE::CIdentifier getNextLinkIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;
			virtual OpenViBE::CIdentifier getNextLinkIdentifierFromBox(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier) const;
			virtual OpenViBE::CIdentifier getNextLinkIdentifierFromBoxOutput(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::uint32 ui32OutputIndex) const;
			virtual OpenViBE::CIdentifier getNextLinkIdentifierToBox(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier) const;
			virtual OpenViBE::CIdentifier getNextLinkIdentifierToBoxInput(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::uint32 ui32InputInex) const;
			virtual OpenViBE::boolean isLink(
				const OpenViBE::CIdentifier& rBoxIdentifier) const;
			virtual const OpenViBE::Kernel::ILink* getLinkDetails(
				const OpenViBE::CIdentifier& rLinkIdentifier) const;
			virtual OpenViBE::Kernel::ILink* getLinkDetails(
				const OpenViBE::CIdentifier& rLinkIdentifier);
			virtual OpenViBE::boolean connect(
				const OpenViBE::CIdentifier& rSourceBoxIdentifier,
				const OpenViBE::uint32 ui32SourceBoxOutputIndex,
				const OpenViBE::CIdentifier& rTargetBoxIdentifier,
				const OpenViBE::uint32 ui32TargetBoxInputIndex,
				OpenViBE::CIdentifier& rLinkIdentifier);
			virtual OpenViBE::boolean disconnect(
				const OpenViBE::CIdentifier& rSourceBoxIdentifier,
				const OpenViBE::uint32 ui32SourceBoxOutputIndex,
				const OpenViBE::CIdentifier& rTargetBoxIdentifier,
				const OpenViBE::uint32 ui32TargetBoxInputIndex);
			virtual OpenViBE::boolean disconnect(
				const OpenViBE::CIdentifier& rLinkIdentifier);

			virtual OpenViBE::CIdentifier getNextProcessingUnitIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;
			virtual OpenViBE::boolean isProcessingUnit(
				const OpenViBE::CIdentifier& rIdentifier) const;
			virtual const OpenViBE::Kernel::IProcessingUnit* getProcessingUnitDetails(
				const OpenViBE::CIdentifier& rProcessingUnitIdentifier) const;
			virtual OpenViBE::Kernel::IProcessingUnit* getProcessingUnitDetails(
				const OpenViBE::CIdentifier& rProcessingUnitIdentifier);
			virtual OpenViBE::boolean addProcessingUnit(
				OpenViBE::CIdentifier& rProcessingUnitIdentifier);
			virtual OpenViBE::boolean removeProcessingUnit(
				const OpenViBE::CIdentifier& rProcessingUnitIdentifier);

			virtual OpenViBE::boolean acceptVisitor(
				OpenViBE::IObjectVisitor& rObjectVisitor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IScenario > >, OVK_ClassId_Kernel_Scenario_Scenario)

		protected:

			virtual OpenViBE::CIdentifier getUnusedIdentifier(void) const;

		protected:

			OpenViBE::CIdentifier m_oIdentifier;
			OpenViBE::CIdentifier m_oVisualisationTreeIdentifier;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CBox*> m_vBox;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CComment*> m_vComment;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CLink*> m_vLink;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CProcessingUnit*> m_vProcessingUnit;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CScenario_H__
