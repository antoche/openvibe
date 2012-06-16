#ifndef __OpenViBE_Kernel_Visualisation_CVisualisationContext_H__
#define __OpenViBE_Kernel_Visualisation_CVisualisationContext_H__

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CSimulatedBox;

		class CVisualisationContext : virtual public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IVisualisationContext>
		{
		public:

			CVisualisationContext(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::CSimulatedBox* pSimulatedBox);

			virtual ~CVisualisationContext(void);

			virtual OpenViBE::boolean setWidgets(
				::GtkWidget* pWidget,
				::GtkWidget* pToolbarWidget);

			virtual OpenViBE::boolean setBackgroundColor(
				OpenViBE::float32 f32ColorRed,
				OpenViBE::float32 f32ColorGreen,
				OpenViBE::float32 f32ColorBlue);

			virtual OpenViBE::boolean setCameraToEncompassObjects();

			virtual OpenViBE::CIdentifier createObject(
				const OpenViBE::CString& rObjectFileName);

			virtual OpenViBE::CIdentifier createObject(
				const OpenViBE::Kernel::EStandard3DObject eStandard3DObject);

			virtual OpenViBE::boolean removeObject(
				const OpenViBE::CIdentifier& rIdentifier);

			virtual OpenViBE::boolean setObjectScale(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32ScaleX,
				OpenViBE::float32 f32ScaleY,
				OpenViBE::float32 f32ScaleZ);

			virtual OpenViBE::boolean setObjectScale(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32Scale);

			virtual OpenViBE::boolean setObjectPosition(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32PositionX,
				OpenViBE::float32 f32PositionY,
				OpenViBE::float32 f32PositionZ);

			virtual OpenViBE::boolean setObjectOrientation(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32OrientationX,
				OpenViBE::float32 f32OrientationY,
				OpenViBE::float32 f32OrientationZ,
				OpenViBE::float32 f32OrientationW);

			virtual OpenViBE::boolean setObjectColor(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32ColorRed,
				OpenViBE::float32 f32ColorGreen,
				OpenViBE::float32 f32ColorBlue);

			virtual OpenViBE::boolean setObjectTransparency(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32Transparency);

			virtual OpenViBE::boolean setObjectVertexCount(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::uint32 ui32VertexCount);

			virtual OpenViBE::boolean setObjectVertexPositionArray(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::float32* pVertexPositionArray);

			virtual OpenViBE::boolean setObjectVertexColorArray(
				const OpenViBE::CIdentifier& rIdentifier,
				const uint32 ui32VertexColorCount,
				const OpenViBE::float32* pVertexColorArray);

			virtual OpenViBE::boolean setObjectTriangleCount(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::uint32 ui32TriangleCount);

			virtual OpenViBE::boolean setObjectTriangleIndexArray(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::uint32* pTriangleIndexArray);

			// =====================================

			virtual OpenViBE::boolean getObjectVertexCount(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32VertexCount) const;

			virtual OpenViBE::boolean getObjectVertexPositionArray(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32* pVertexPositionArray) const;

			virtual OpenViBE::boolean getObjectVertexColorArray(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32* pVertexColorArray) const;

			virtual OpenViBE::boolean getObjectTriangleCount(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32TriangleCount) const;

			virtual OpenViBE::boolean getObjectTriangleIndexArray(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32* pTriangleIndexArray) const;

			// =====================================

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IVisualisationContext>, OVK_ClassId_Kernel_Visualisation_VisualisationContext)

		protected:

			OpenViBE::Kernel::CSimulatedBox* m_pSimulatedBox;
			OpenViBE::Kernel::IVisualisationManager* m_pVisualisationManagerBridge;
		};
	};
};

#endif // __OpenViBE_Kernel_Visualisation_CVisualisationContext_H__
