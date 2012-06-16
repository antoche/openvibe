#ifndef __OpenViBE_Kernel_Visualisation_CVisualisationContext_H__
#define __OpenViBE_Kernel_Visualisation_CVisualisationContext_H__

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class CVisualisationContext
		 * \author Vincent Delannoy (INRIA/IRISA)
		 * \date 2008-06
		 * \brief Visualisation context made available to plugins and allowing them to interact with a 3D scene
		 * This class offers a simplified, library independent 3D API to be used by plugin developers.
		 */
		class CVisualisationContext : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IVisualisationContext>
		{
		public:
			/**
			 * \brief Constructor
			 */
			CVisualisationContext(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::CSimulatedBox* pSimulatedBox);

			/**
			 * \brief Destructor
			 */
			virtual ~CVisualisationContext(void);

			virtual OpenViBE::boolean setToolbar(
				::GtkWidget* pToolbarWidget);

			virtual OpenViBE::boolean setWidget(
				::GtkWidget* pTopmostWidget);

			virtual OpenViBE::CIdentifier create3DWidget(
				::GtkWidget*& p3DWidget);

			virtual OpenViBE::boolean is3DWidgetRealized(
				const OpenViBE::CIdentifier& rWidgetIdentifier) const;

			virtual OpenViBE::boolean createResourceGroup(
				OpenViBE::CIdentifier& rResourceGroupIdentifier,
				const OpenViBE::CString& rResourceGroupName);

			virtual OpenViBE::boolean addResourceLocation(
				const OpenViBE::CIdentifier& rResourceGroupIdentifier,
				const OpenViBE::CString& rPath,
				OpenViBE::Kernel::EResourceType type,
				bool bRecursive);

			virtual OpenViBE::boolean addResourceLocationFromFile(
				const OpenViBE::CIdentifier& rResourceGroupIdentifier,
				const OpenViBE::CString& rResourcesFileName);

			virtual OpenViBE::boolean initializeResourceGroup(
				const OpenViBE::CIdentifier& rResourceGroupIdentifier);

			virtual OpenViBE::boolean destroyResourceGroup(
				const OpenViBE::CIdentifier& rResourceGroupIdentifier);

			virtual OpenViBE::boolean update3DWidget(
				const OpenViBE::CIdentifier& rWidgetIdentifier);

			virtual OpenViBE::boolean setBackgroundColor(
				const OpenViBE::CIdentifier& rWidgetIdentifier,
				OpenViBE::float32 f32ColorRed,
				OpenViBE::float32 f32ColorGreen,
				OpenViBE::float32 f32ColorBlue);

			virtual OpenViBE::boolean setCameraToEncompassObjects(
				const OpenViBE::CIdentifier& rWidgetIdentifier);

			virtual OpenViBE::boolean setCameraSphericalCoordinates(
				const OpenViBE::CIdentifier& rWindowIdentifier,
				OpenViBE::float32 f32Theta,
				OpenViBE::float32 f32Phi,
				OpenViBE::float32 f32Radius);

			virtual OpenViBE::CIdentifier createObject(
				const OpenViBE::CString& rObjectFileName,
				const CNameValuePairList* pObjectParams);

			virtual OpenViBE::CIdentifier createObject(
				const OpenViBE::Kernel::EStandard3DObject eStandard3DObject,
				const CNameValuePairList* pObjectParams);

			virtual OpenViBE::boolean removeObject(
				const OpenViBE::CIdentifier& rIdentifier);

			virtual OpenViBE::boolean setObjectVisible(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::boolean bVisible);

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

			virtual OpenViBE::boolean setObjectScale(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32ScaleX,
				OpenViBE::float32 f32ScaleY,
				OpenViBE::float32 f32ScaleZ);

			virtual OpenViBE::boolean setObjectScale(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32Scale);

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
				OpenViBE::uint32 ui32VertexCount);

			virtual OpenViBE::boolean setObjectVertexPositionArray(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32 ui32VertexPositionCount,
				const OpenViBE::float32* pVertexPositionArray);

			virtual OpenViBE::boolean setObjectVertexColorArray(
				const OpenViBE::CIdentifier& rIdentifier,
				uint32 ui32VertexColorCount,
				const OpenViBE::float32* pVertexColorArray);

			virtual OpenViBE::boolean setObjectTriangleCount(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32 ui32TriangleCount);

			virtual OpenViBE::boolean setObjectTriangleIndexArray(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32 ui32TriangleCount,
				const OpenViBE::uint32* pTriangleIndexArray);

			// =====================================

			virtual OpenViBE::boolean getCameraSphericalCoordinates(
				const OpenViBE::CIdentifier& rWindowIdentifier,
				OpenViBE::float32& rTheta,
				OpenViBE::float32& rPhi,
				OpenViBE::float32& rRadius);

			virtual OpenViBE::boolean getObjectAxisAlignedBoundingBox(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32* pMinimum,
				OpenViBE::float32* pMaximum);

			virtual OpenViBE::boolean getObjectVertexCount(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32VertexCount) const;

			virtual OpenViBE::boolean getObjectVertexPositionArray(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32 ui32VertexPositionCount,
				OpenViBE::float32* pVertexPositionArray) const;

			virtual OpenViBE::boolean getObjectVertexColorArray(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32 ui32VertexColorCount,
				OpenViBE::float32* pVertexColorArray) const;

			virtual OpenViBE::boolean getObjectTriangleCount(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32TriangleCount) const;

			virtual OpenViBE::boolean getObjectTriangleIndexArray(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32 ui32TriangleCount,
				OpenViBE::uint32* pTriangleIndexArray) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IVisualisationContext>, OVK_ClassId_Kernel_Visualisation_VisualisationContext)

		protected:

			OpenViBE::Kernel::CSimulatedBox* m_pSimulatedBox;
			OpenViBE::Kernel::IVisualisationManager* m_pVisualisationManagerBridge;
		};
	};
};

#endif // __OpenViBE_Kernel_Visualisation_CVisualisationContext_H__
