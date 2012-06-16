#ifndef __OpenViBE_IObjectVisitor_H__
#define __OpenViBE_IObjectVisitor_H__

#include "ovIObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IObjectVisitorContext;

		class IBox;
		class ILink;
		class IProcessingUnit;
		class IScenario;

		class IVisualisationTree;
		class IVisualisationWidget;
	};

	class OV_API IObjectVisitor : public OpenViBE::IObject
	{
	public:

		virtual OpenViBE::boolean processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IBox& rBox)                                 { return true; }
		virtual OpenViBE::boolean processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::ILink& rLink)                               { return true; }
		virtual OpenViBE::boolean processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IProcessingUnit& rProcessingUnit)           { return true; }
		virtual OpenViBE::boolean processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IScenario& rScenario)                       { return true; }

		virtual OpenViBE::boolean processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IVisualisationTree& rVisualisationTree)     { return true; }
		virtual OpenViBE::boolean processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IVisualisationWidget& rVisualisationWidget) { return true; }

		virtual OpenViBE::boolean processEnd  (OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IBox& rBox)                                 { return true; }
		virtual OpenViBE::boolean processEnd  (OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::ILink& rLink)                               { return true; }
		virtual OpenViBE::boolean processEnd  (OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IProcessingUnit& rProcessingUnit)           { return true; }
		virtual OpenViBE::boolean processEnd  (OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IScenario& rScenario)                       { return true; }

		virtual OpenViBE::boolean processEnd  (OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IVisualisationTree& rVisualisationTree)     { return true; }
		virtual OpenViBE::boolean processEnd  (OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IVisualisationWidget& rVisualisationWidget) { return true; }

		_IsDerivedFromClass_(OpenViBE::IObject, OV_ClassId_ObjectVisitor);
	};
};

#endif // __OpenViBE_IObjectVisitor_H__
