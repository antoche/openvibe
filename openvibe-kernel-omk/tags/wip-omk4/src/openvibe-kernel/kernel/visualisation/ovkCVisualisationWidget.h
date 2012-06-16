#ifndef __OpenViBEKernel_Kernel_Visualisation_CVisualisationWidget_H__
#define __OpenViBEKernel_Kernel_Visualisation_CVisualisationWidget_H__

#include "../ovkTKernelObject.h"

#include "../scenario/ovkTAttributable.h"

#include <vector>
#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CVisualisationWidget : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::TAttributable<OpenViBE::Kernel::IVisualisationWidget> >
		{
		public:
			CVisualisationWidget(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual ~CVisualisationWidget(void);

			virtual OpenViBE::boolean initialize(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::CString& rName,
				OpenViBE::Kernel::EVisualisationWidgetType oType,
				const OpenViBE::CIdentifier& rParentIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				OpenViBE::uint32 ui32NbChildren);

			virtual OpenViBE::CIdentifier getIdentifier(void) const;

			virtual const OpenViBE::CString& getName(void) const;

			virtual void setName(
				const OpenViBE::CString& rName);

			virtual EVisualisationWidgetType getType(void) const;

			virtual OpenViBE::CIdentifier getParentIdentifier(void) const;

			virtual void setParentIdentifier(
				const OpenViBE::CIdentifier& rParentIdentifier);

			virtual OpenViBE::CIdentifier getBoxIdentifier(void) const;

			virtual OpenViBE::uint32 getNbChildren(void) const;

			virtual OpenViBE::boolean getChildIndex(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32Index) const;

			//for windows, the number of children is unknown a priori
			virtual OpenViBE::boolean addChild(
				const OpenViBE::CIdentifier& rChildIdentifier);

			virtual OpenViBE::boolean removeChild(
				const OpenViBE::CIdentifier& rIdentifier);

			virtual OpenViBE::boolean getChildIdentifier(
				OpenViBE::uint32 ui32ChildIndex,
				OpenViBE::CIdentifier& rIdentifier) const;

			virtual OpenViBE::boolean setChildIdentifier(
				OpenViBE::uint32 ui32ChildIndex,
				const OpenViBE::CIdentifier& rIdentifier);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::TAttributable<OpenViBE::Kernel::IVisualisationWidget> >, OVK_ClassId_Kernel_Visualisation_VisualisationWidget)

		private:

			OpenViBE::CIdentifier m_oIdentifier; //unique identifier
			OpenViBE::CString m_oName;
			OpenViBE::Kernel::EVisualisationWidgetType m_oType;
			OpenViBE::CIdentifier m_oParentIdentifier; //VisualisationWidget
			OpenViBE::CIdentifier m_oBoxIdentifier; //id of an existing CBox
			std::vector<OpenViBE::CIdentifier> m_vChildren;
			void* m_pParentWidget;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Visualisation_CVisualisationWidget_H__
