#ifndef __OpenViBEKernel_Kernel_Player_COgreScene_H__
#define __OpenViBEKernel_Kernel_Player_COgreScene_H__

#include <Ogre.h>
#include <openvibe/ov_all.h>

class COgreObject;

/**
 * \class COgreScene
 * \author Vincent Delannoy (INRIA/IRISA)
 * \date 2008-06
 * \brief Management of an Ogre scene
 * This class creates a scene manager and a default 3D scene
 * which can be populated using a simplified 3D API.
 */
class COgreScene
{
public:
	/**
	 * \brief Constructor
	 * \param[in] rKernelContext OpenViBE kernel context
	 * \param[in] rName Name of the object
	 * \param[in] pOgreVis Pointer to Ogre visualisation
	 */
	COgreScene(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		const Ogre::String& rName,
		COgreVisualisation* pOgreVis);

	/**
	 * \brief Destructor
	 */
	~COgreScene();

	/**
	 * \brief Create a new scene manager and a default 3D scene
	 * \remarks This method should only be called after primary render window
	 * is created and resources are loaded
	 * \return True if new scene was successfully created, false otherwise
	 */
	OpenViBE::boolean init();

	/**
	 * \brief Get the name of this object
	 * \return Name of this object
	 */
	const Ogre::String& getName();

	/**
	 * \brief Get pointer to this scene's SceneManager
	 * \return Pointer to scene manager
	 */
	Ogre::SceneManager* getSceneManager();

	/**
	 * \brief Get axis aligned bounding box of objects contained in this scene
	 * \param rMinX X lower bound
	 * \param rMinY Y lower bound
	 * \param rMinZ Z lower bound
	 * \param rMaxX X upper bound
	 * \param rMaxY Y upper bound
	 * \param rMaxZ Z upper bound
	 */
	void getBoundingBox(
		OpenViBE::float32& rMinX,
		OpenViBE::float32& rMinY,
		OpenViBE::float32& rMinZ,
		OpenViBE::float32& rMaxX,
		OpenViBE::float32& rMaxY,
		OpenViBE::float32& rMaxZ);

	/**
	 * \brief Create a new 3D object
	 * \param rObjectIdentifier Identifier to give to new object
	 * \param rGeometryFileName .scene filename containing object geometry
	 * \param rNameValuePairList Object creation parameters
	 * \return Pointer to newly created object
	 */
	COgreObject* createObject(
		OpenViBE::CIdentifier rObjectIdentifier,
		const OpenViBE::CString& rGeometryFileName,
		const OpenViBE::CNameValuePairList& rNameValuePairList);

	/**
	 * \brief Create a new 3D plane
	 * \param rObjectIdentifier Identifier to give to new object
	 * \param rNameValuePairList Object creation parameters
	 * \return Pointer to newly created object
	 */
	COgreObject* createPlane(
		OpenViBE::CIdentifier rObjectIdentifier,
		const OpenViBE::CNameValuePairList& rNameValuePairList);

	/**
	 * \brief Remove a 3D object
	 * \param rObjectIdentifier Identifier of object to be removed
	 * \return True if object was removed successfully, false otherwise
	 */
	OpenViBE::boolean removeObject(
		OpenViBE::CIdentifier rObjectIdentifier);

	/**
	 * \brief Get an Ogre object
	 * \param rObjectIdentifier Identifier of object to retrieve
	 * \return Pointer to Ogre object if it was found, NULL otherwise
	 */
	COgreObject* getOgreObject(
		OpenViBE::CIdentifier rObjectIdentifier);

private:
	/// OpenViBE kernel context
	const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
	/// Name of this scene
	Ogre::String m_sName;
	/// Pointer to Ogre visualisation
	COgreVisualisation* m_pOgreVis;
	/// Pointer to this scene's SceneManager
	Ogre::SceneManager* m_pSceneManager;
	/// Maps object Ids to object pointers
	std::map<OpenViBE::CIdentifier, COgreObject*> m_mOgreObjectIds;
	/// Maps simulated object IDs to corresponding simulated object names.
	//std::map<OpenViBE::CIdentifier, OpenViBE::CString> m_mSimulatedObjects;
};

#endif // __OpenViBEKernel_Kernel_Player_COgreScene_H__
