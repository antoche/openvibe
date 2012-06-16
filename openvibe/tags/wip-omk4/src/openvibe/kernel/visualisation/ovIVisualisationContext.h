#ifndef __OpenViBE_Kernel_Visualisation_IVisualisationContext_H__
#define __OpenViBE_Kernel_Visualisation_IVisualisationContext_H__

#include "../ovIKernelObject.h"

typedef struct _GtkWidget GtkWidget;

namespace OpenViBE
{
	namespace Kernel
	{
		typedef union
		{
			struct
			{
				OpenViBE::float32 x;
				OpenViBE::float32 y;
				OpenViBE::float32 z;
			};
			OpenViBE::float32 v[3];
		} SVertex3;

		typedef union
		{
			struct
			{
				OpenViBE::float32 r;
				OpenViBE::float32 g;
				OpenViBE::float32 b;
				OpenViBE::float32 a;
			};
			OpenViBE::float32 v[4];
		} SColor4;

		typedef union
		{
			struct
			{
				OpenViBE::uint32 i;
				OpenViBE::uint32 j;
				OpenViBE::uint32 k;
			};
			OpenViBE::uint32 t[3];
		} STriangle;

		/**
		\brief Standard 3D objects
		*/
		enum EStandard3DObject
		{
			Standard3DObject_Invalid = -1,
			Standard3DObject_Sphere,
			Standard3DObject_Cone/*,
			Standard3DObject_Cube,*/
		};

		/**
		 * \class IVisualisationContext
		 * \author Vincent Delannoy (INRIA/IRISA)
		 * \date 2007-11
		 * \brief Visualisation manager interface for plugin objects
		 */
		class OV_API IVisualisationContext : virtual public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Sets the GtkWidgets corresponding to a visualisation plugin's main window and toolbar
			 * \param pWidget [in] : main widget of the plugin
			 * \param pToolbarWidget [in] : toolbar widget
			 *
			 * \return \e true in case of success, \e false otherwise.
			 */
			virtual OpenViBE::boolean setWidgets(
				::GtkWidget* pWidget,
				::GtkWidget* pToolbarWidget) = 0;

			virtual OpenViBE::boolean setBackgroundColor(
				OpenViBE::float32 f32ColorRed,
				OpenViBE::float32 f32ColorGreen,
				OpenViBE::float32 f32ColorBlue) = 0;

			virtual OpenViBE::boolean setCameraToEncompassObjects() = 0;

			/**
			\param rObjectFileName [in] : file name without extension
			*/
			virtual OpenViBE::CIdentifier createObject(
				const OpenViBE::CString& rObjectFileName) = 0;

			virtual OpenViBE::CIdentifier createObject(
				const OpenViBE::Kernel::EStandard3DObject eStandard3DObject) = 0;

			virtual OpenViBE::boolean removeObject(
				const OpenViBE::CIdentifier& rIdentifier) = 0;

			virtual OpenViBE::boolean setObjectScale(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32ScaleX,
				OpenViBE::float32 f32ScaleY,
				OpenViBE::float32 f32ScaleZ) = 0;

			virtual OpenViBE::boolean setObjectScale(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32Scale) = 0;

			virtual OpenViBE::boolean setObjectPosition(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32PositionX,
				OpenViBE::float32 f32PositionY,
				OpenViBE::float32 f32PositionZ) = 0;

			virtual OpenViBE::boolean setObjectOrientation(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32OrientationX,
				OpenViBE::float32 f32OrientationY,
				OpenViBE::float32 f32OrientationZ,
				OpenViBE::float32 f32OrientationW) = 0;

			virtual OpenViBE::boolean setObjectColor(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32ColorRed,
				OpenViBE::float32 f32ColorGreen,
				OpenViBE::float32 f32ColorBlue) = 0;

			virtual OpenViBE::boolean setObjectTransparency(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32 f32Transparency) = 0;

			// =====================================

			virtual OpenViBE::boolean setObjectVertexCount(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::uint32 ui32VertexCount) = 0;

			virtual OpenViBE::boolean setObjectVertexPositionArray(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::float32* pVertexPositionArray) = 0; // xyz

			virtual OpenViBE::boolean setObjectVertexColorArray(
				const OpenViBE::CIdentifier& rIdentifier,
				const uint32 ui32VertexColorCount,
				const OpenViBE::float32* pVertexColorArray) = 0; // rgba

			virtual OpenViBE::boolean setObjectTriangleCount(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::uint32 ui32TriangleCount) = 0;

			virtual OpenViBE::boolean setObjectTriangleIndexArray(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::uint32* pTriangleIndexArray) = 0;

			// =====================================

			virtual OpenViBE::boolean getObjectVertexCount(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32VertexCount) const = 0;

			virtual OpenViBE::boolean getObjectVertexPositionArray(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32* pVertexPositionArray) const = 0; // xyz

			virtual OpenViBE::boolean getObjectVertexColorArray(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::float32* pVertexColorArray) const = 0; // rgba

			virtual OpenViBE::boolean getObjectTriangleCount(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32TriangleCount) const = 0;

			virtual OpenViBE::boolean getObjectTriangleIndexArray(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32* pTriangleIndexArray) const = 0;

			// =====================================

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Visualisation_VisualisationContext)
		};
	};
};

#endif // __OpenViBE_Kernel_Visualisation_IVisualisationContext_H__
