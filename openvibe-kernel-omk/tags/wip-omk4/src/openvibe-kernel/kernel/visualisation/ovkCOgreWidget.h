// Copyright (c) 2006 Christian Lindequist Larsen
//
// This software is provided 'as-is', without any express or implied warranty. In no event will
// the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose, including commercial
// applications, and to alter it and redistribute it freely, subject to the following
// restrictions:
//
//     1. The origin of this software must not be misrepresented; you must not claim that you
//        wrote the original software. If you use this software in a product, an acknowledgment
//        in the product documentation would be appreciated but is not required.
//
//     2. Altered source versions must be plainly marked as such, and must not be
//        misrepresented as being the original software.
//
//     3. This notice may not be removed or altered from any source distribution.
//
#ifndef __GTK_OGRE_WIDGET_H__
#define __GTK_OGRE_WIDGET_H__

#include <openvibe/ov_all.h>

#include "OgrePrerequisites.h"
#include "OMKOgreVis.h"
#include "../ovkTKernelObject.h"
#include <gtk/gtkwidget.h>

class GtkOgre
{
private:
	/**
	\brief This member MUST be the first declared in this class, so as to allow this structure to be used as a GtkWidget
	*/
	GtkWidget m_oWidget;

	/**
	\brief Use the gtk_ogre_new() global function to create a GtkOgre instance
	*/
	GtkOgre();

	/**
	\brief Use the destroy event to delete a GtkOgre instance
	*/
	~GtkOgre();

public:
	/*enum eViewportType
	{
		ViewportType_Perspective,
		ViewportType_RIGHT, //or X
		ViewportType_TOP, //or Y
		ViewportType_FRONT //or Z
	};*/

	/*enum eCameraModel
	{
		CameraModel_Free, //free moving camera
		CameraModel_LookAt
	};
	*/

	/**
	\brief Initializes the GtkOgre widget (acts as a constructor).
	Called automatically upon widget creation
	*/
	void init();

	/**
	\brief Initializes the GtkOgre widget (acts as a destructor).
	Called automatically upon widget destruction
	*/
	void destroy();

	//setters/getters
	OpenViBE::boolean setName(
		const Ogre::String& rName);
	OpenViBE::boolean setOwner(
		OMK::SimulatedObject* pOwner);

	//event handling
	OpenViBE::boolean handleDestroyEvent();
	OpenViBE::boolean handleRealizeEvent();
	OpenViBE::boolean handleUnrealizeEvent();
	OpenViBE::boolean handleSizeRequestEvent(
		GtkRequisition* pRequisition);
	OpenViBE::boolean handleSizeAllocateEvent(
		GtkAllocation* pAllocation);
	OpenViBE::boolean handleExposeEvent();
	OpenViBE::boolean handleMotionEvent(
		::GdkEventButton* bevent);
	OpenViBE::boolean handleButtonPressEvent(
		::GdkEventButton *bevent);
	OpenViBE::boolean handleButtonReleaseEvent(
		::GdkEventButton *bevent);

	//3D API
	OpenViBE::boolean setBackgroundColor(
		OpenViBE::float32 f32ColorRed,
		OpenViBE::float32 f32ColorGreen,
		OpenViBE::float32 f32ColorBlue);
	OpenViBE::boolean setCameraToEncompassObjects();
	OpenViBE::CIdentifier createObject(
		const OpenViBE::CString& rObjectFileName);
	OpenViBE::CIdentifier createObject(
		const OpenViBE::Kernel::EStandard3DObject eStandard3DObject);
	OpenViBE::boolean removeObject(
		const OpenViBE::CIdentifier& rIdentifier);
	OpenViBE::boolean setObjectPosition(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32 f32PositionX,
		OpenViBE::float32 f32PositionY,
		OpenViBE::float32 f32PositionZ);
	OpenViBE::boolean setObjectOrientation(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32 f32OrientationX,
		OpenViBE::float32 f32OrientationY,
		OpenViBE::float32 f32OrientationZ,
		OpenViBE::float32 f32OrientationW);
	OpenViBE::boolean setObjectScale(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32 f32ScaleX,
		OpenViBE::float32 f32ScaleY,
		OpenViBE::float32 f32ScaleZ);
	OpenViBE::boolean setObjectScale(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32 f32Scale);
	OpenViBE::boolean setObjectColor(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32 f32ColorRed,
		OpenViBE::float32 f32ColorGreen,
		OpenViBE::float32 f32ColorBlue);
	OpenViBE::boolean setObjectTransparency(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32 f32Transparency);
	OpenViBE::boolean setObjectVertexColorArray(
		const OpenViBE::CIdentifier& rIdentifier,
		const OpenViBE::uint32 ui32VertexColorCount,
		const OpenViBE::float32* pVertexColorArray);
	OpenViBE::boolean getObjectVertexCount(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::uint32& ui32VertexCount) const;
	OpenViBE::boolean getObjectVertexPositionArray(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32* pVertexPositionArray) const;

	//object callbacks
	static void gtk_ogre_destroy(
		GtkObject* pObject);

	//widget callbacks
	static void gtk_ogre_realize(
		GtkWidget* pWidget);
	static void gtk_ogre_unrealize(
		GtkWidget* pWidget);
	static void gtk_ogre_size_request(
		GtkWidget *pWidget,
		GtkRequisition* pRequisition);
	static void gtk_ogre_size_allocate(
		GtkWidget* pWidget,
		GtkAllocation* pAllocation);
	static gint gtk_ogre_expose(
		GtkWidget *pWidget,
		GdkEventExpose *pEvent);

private:
	/// \brief Create scene manager to be used by this widget
	OpenViBE::boolean createSceneManager();
	/// \brief Create default scene
	OpenViBE::boolean createDefaultScene();
	/// \brief Compute translation factor based on scene size
	OpenViBE::boolean computeTranslationFactor(
		OpenViBE::float32 f32SceneSize);
	/// \brief Get value of an angle in degrees in the [0, 360[ range
	void get360Angle(OpenViBE::float32& f32Angle);
	/// \brief Get value of an angle in degrees in the [0, 180[ range
	void get180Angle(OpenViBE::float32& f32Angle);
	/// \brief Updates camera from current settings
	OpenViBE::boolean updateCamera();
	/// \brief Generate a unique identifier
	OpenViBE::CIdentifier getUnusedIdentifier() const;
	/// \brief Retrieve first entity in Ogre hierarchy from node identified by rIdentifier
	OpenViBE::boolean getFirstEntityInHierarchy(
		const OpenViBE::CIdentifier& rIdentifier,
		Ogre::Entity*& pEntity) const;
	/// \brief Retrieve first entity in Ogre hierarchy from pNode
	OpenViBE::boolean getFirstEntityInHierarchy(
		Ogre::SceneNode* pNode,
		Ogre::Entity*& pEntity) const;
	/// \brief Retrieve first SubMesh in Entity
	OpenViBE::boolean getFirstSubMesh(
		Ogre::Entity* l_pEntity,
		Ogre::SubMesh*& l_pSubMesh) const;
	/// \brief Get world bounding box of Ogre hierarchy from node identified by rIdentifier
	OpenViBE::boolean getObjectWorldBoundingBox(
		const OpenViBE::CIdentifier& rIdentifier,
		OpenViBE::float32& rMinX,
		OpenViBE::float32& rMinY,
		OpenViBE::float32& rMinZ,
		OpenViBE::float32& rMaxX,
		OpenViBE::float32& rMaxY,
		OpenViBE::float32& rMaxZ);

	/// \brief Button press event callback
	static gboolean button_press_event_in_frame_cb(
		GtkWidget *widget,
		GdkEventButton *bevent,
		gpointer user_data);
	/// \brief Mouse motion event callback
	static gboolean motion_notify_event_in_frame_cb(
		GtkWidget *widget,
		GdkEventButton *bevent,
		gpointer user_data);
	/// \brief Button release event callback
	static gboolean button_release_event_in_frame_cb(
		GtkWidget *widget,
		GdkEventButton *bevent,
		gpointer user_data);

private:
	//WARNING : constructor doesn't get called, so any member that requires its constructor
	//to be called upon object creation won't work properly. This is why e.g. the map of simulated
	//objects is allocated dynamically in init().

	/**
	\brief Flag set to true upon first widget creation
	Upon first widget realization, Ogre initialisation must be completed, and only then.
	*/
	//static OpenViBE::boolean ms_bFirst3DWidgetRealized;

	/**
	\brief Map of simulated object IDs and corresponding simulated object Names.
	Simulated Object names must not contain any space, otherwise they'll get truncated
	when unpacked by OpenMASK upon object creation. Thus, objects are named after
	their OpenViBE identifier converted to a string without space characters.
	*/
	std::map<OpenViBE::CIdentifier, OpenViBE::CString>* m_pSimulatedObjects;

	//For now, there is only one VO per SO and they share the same name
	//std::map<OpenViBE::CIdentifier, OpenViBE::CString>* m_pVisualObjects;

	Ogre::String* m_sName;
	OMK::SimulatedObject* m_pOwner;
	OMK::Vis::OgreVis* m_pOgreVis;
	OpenViBE::uint32 m_ui32RenderWindowId;
	Ogre::RenderWindow* m_pRenderWindow;
	Ogre::SceneManager* m_pSceneManager;

	Ogre::Light* m_pLight;

	Ogre::Camera* m_pCamera;
	//eCameraModel m_oCameraModel;

	Ogre::Viewport* m_pViewport;
	//eViewportType m_oViewportType;

	//mouse buttons state
	OpenViBE::boolean m_bLeftButtonPressed;
	OpenViBE::boolean m_bMiddleButtonPressed;
	OpenViBE::boolean m_bRightButtonPressed;

	//last cursor coordinates
	OpenViBE::uint32 m_ui32X;
	OpenViBE::uint32 m_ui32Y;

	//default translation speed : translate camera of m_f32DefaultTranslationFactor units in a unitary scene (fitting in a unit cube)
	OpenViBE::float32 m_f32DefaultTranslationFactor;
	OpenViBE::float32 m_f32TranslationFactor;
	OpenViBE::float32 m_f32RotationFactor;

	//free camera model
	//-----------------
	Ogre::Vector3 m_oRight;
	Ogre::Vector3 m_oUp;
	Ogre::Vector3 m_oLook;
	Ogre::Vector3 m_oPosition;

	//rotate around look at point camera model
	//----------------------------------------
	Ogre::Vector3 m_oLookAt; //center of scene objects AABB
	OpenViBE::float32 m_f32Theta;
	OpenViBE::float32 m_f32Phi;
	OpenViBE::float32 m_f32Radius;
	//
};

/**
\brief Creates a new GtkOgre widget
\param rName name of the widget
\param pOgreVis pointer to Ogre visualisation object in OpenMASK
\return pointer to newly created GtkOgre widget, or NULL if failed
*/
GtkOgre* gtk_ogre_new(const Ogre::String& rName, OMK::SimulatedObject* pOwner);

#endif // __GTK_OGRE_WIDGET_H__
