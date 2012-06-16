#ifndef __OpenViBEKernel_Kernel_Player_COgreWindow_H__
#define __OpenViBEKernel_Kernel_Player_COgreWindow_H__

#include <Ogre.h>
#include <openvibe/ov_all.h>

class COgreScene;
class COgreVisualisation;
class COgreFrameListener;

/**
 * \class COgreWindow
 * \author Vincent Delannoy (INRIA/IRISA)
 * \date 2008-06
 * \brief An OgreWindow visualises a 3D scene
 * This class manages a RenderWindow which contents are visualised
 * in a single viewport.
 */
class COgreWindow
{
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
	 * \brief Constructor
	 * \param[in] rKernelContext OpenViBE kernel context
	 * \param[in] rName Window name
	 * \param[in] pOgreVis Pointer to Ogre visualisation
	 * \param[in] pOgreScene Pointer to the scene this window is a view of
	 */
	COgreWindow(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		const Ogre::String& rName,
		COgreVisualisation* pOgreVis,
		COgreScene* pOgreScene);

	/**
	 * \brief Destructor
	 */
	~COgreWindow();

	/**
	 * \brief Get window name
	 */
	const Ogre::String& getName();

	/**
	 * \brief Create a RenderWindow
	 * \param rExternalHandle External window handle of window in which to embed the RenderWindow
	 * \param uiWidth Width of RenderWindow to be created
	 * \param uiHeight Height of RenderWindow to be created
	 * \return
	 */
	OpenViBE::boolean createRenderWindow(
		const std::string& rExternalHandle,
		unsigned int uiWidth,
		unsigned int uiHeight);

	/**
	 * \brief Refresh 3D window contents
	 */
	void update();

	/** \name Event handling */
	//@{
	OpenViBE::boolean handleRealizeEvent();
	OpenViBE::boolean handleUnrealizeEvent();
	OpenViBE::boolean handleSizeAllocateEvent(
		unsigned int uiWidth,
		unsigned int uiHeight);
	OpenViBE::boolean handleExposeEvent();
	OpenViBE::boolean handleMotionEvent(
		int iX,
		int iY);
	OpenViBE::boolean handleButtonPressEvent(
		unsigned int uiButton,
		int iX,
		int iY);
	OpenViBE::boolean handleButtonReleaseEvent(
		unsigned int uiButton,
		int iX,
		int iY);
	//@}

	/** \name 3D API */
	//@{
	OpenViBE::boolean setBackgroundColor(
		OpenViBE::float32 f32ColorRed,
		OpenViBE::float32 f32ColorGreen,
		OpenViBE::float32 f32ColorBlue);
	OpenViBE::boolean setCameraToEncompassObjects();
	OpenViBE::boolean setCameraSphericalCoordinates(
		OpenViBE::float32 f32Theta,
		OpenViBE::float32 f32Phi,
		OpenViBE::float32 f32Radius);
	OpenViBE::boolean getCameraSphericalCoordinates(
		OpenViBE::float32& rTheta,
		OpenViBE::float32& rPhi,
		OpenViBE::float32& rRadius);
	//@}

private:
	OpenViBE::boolean createView();
	OpenViBE::boolean destroyView();
	/// \brief Compute translation factor based on scene size
	OpenViBE::boolean computeTranslationFactor(
		OpenViBE::float32 f32SceneSize);
	/// \brief Get value of an angle in degrees in the [0, 360[ range
	OpenViBE::float32 get360Angle(
		OpenViBE::float32 f32Angle);
	/// \brief Get value of an angle in degrees in the [0, 180[ range
	OpenViBE::float32 get180Angle(
		OpenViBE::float32 f32Angle);
	/// \brief Set theta/phi camera angles
	OpenViBE::boolean setCameraSphericalAngles(
		OpenViBE::float32 f32Theta,
		OpenViBE::float32 f32Phi);
	/// \brief Updates camera from current settings
	OpenViBE::boolean updateCamera();
	/// \brief Generate a unique identifier
	//OpenViBE::CIdentifier getUnusedIdentifier() const;

private:
	/// OpenViBE kernel context
	const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
	/// Name of this window
	Ogre::String m_sName;
	/// Pointer to Ogre visualisation
	COgreVisualisation* m_pOgreVis;
	/// Pointer to the scene visualised by this window
	COgreScene* m_pOgreScene;
	/// Pointer to the associated RenderWindow
	Ogre::RenderWindow* m_pRenderWindow;
	/**
	 * \brief Pointer to the scene manager whose contents are visualised in this window
	 * \remarks This object is created and destroyed by a COgreScene instance
	 */
	Ogre::SceneManager* m_pSceneManager;
	/// Camera used by this window
	Ogre::Camera* m_pCamera;
	/// Near/far clip planes ratio
	OpenViBE::float32 m_f32NearFarClipRatio;
	//eCameraModel m_oCameraModel;
	/// Viewport displayed in this window
	Ogre::Viewport* m_pViewport;
	//eViewportType m_oViewportType;
	COgreFrameListener* m_pFrameListener;

	/** \name Mouse button states */
	//@{
	/// Left mouse button state
	OpenViBE::boolean m_bLeftButtonPressed;
	/// Middle mouse button state
	OpenViBE::boolean m_bMiddleButtonPressed;
	/// Right mouse button state
	OpenViBE::boolean m_bRightButtonPressed;
	//@}

	/** \ name Mouse cursor coordinates */
	//@{
	/**
	 * \brief X cursor coordinate
	 * \remarks May be negative when a button is kept pressed and the cursor subsequently leaves the RenderWindow area
	 */
	OpenViBE::int32 m_i32X;
	/**
	 * \brief Y cursor coordinate
	 * \remarks May be negative when a button is kept pressed and the cursor subsequently leaves the RenderWindow area
	 */
	OpenViBE::int32 m_i32Y;
	//@}

	/** \ name Camera move settings */
	//@{
	//default translation speed : translate camera of m_f32DefaultTranslationFactor units in a unitary scene (fitting in a unit cube)
	OpenViBE::float32 m_f32DefaultTranslationFactor;
	OpenViBE::float32 m_f32TranslationFactor;
	OpenViBE::float32 m_f32RotationFactor;
	//@}

	/** \ name 'Free' camera model */
	//@{
	Ogre::Vector3 m_oRight;
	Ogre::Vector3 m_oUp;
	Ogre::Vector3 m_oLook;
	Ogre::Vector3 m_oPosition;
	//@}

	/** \ name 'Rotate around look-at point' camera model */
	//@{
	/** \brief Look at point
	 * \remarks By default, this is the center of the AABB encompassing all scene objects
	 */
	Ogre::Vector3 m_oLookAt;
	/// Theta angle of camera in a spherical coordinate system
	OpenViBE::float32 m_f32Theta;
	/// Phi angle of camera in a spherical coordinate system
	OpenViBE::float32 m_f32Phi;
	/// Radius of sphere centered at the look at point and encompassing the camera
	OpenViBE::float32 m_f32Radius;
	//@}
};

#endif // __OpenViBEKernel_Kernel_Player_COgreWindow_H__
