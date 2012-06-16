#ifndef __OpenViBEKernel_Kernel_Player_COgreObject_H__
#define __OpenViBEKernel_Kernel_Player_COgreObject_H__

#include <openvibe/ov_all.h>
#include <OgreColourValue.h>

namespace Ogre
{
	class SceneNode;
	class SceneManager;
};

class COgreVisualisation;
class NodeMaterialOrig;
class CColorAnimator;
class CTransformAnimator;
class CVertexBufferAnimator;

/**
 * \class COgreObject
 * \author Vincent Delannoy (INRIA/IRISA)
 * \date 2008-06
 * \brief An OgreObject manages a 3D object in an Ogre scene
 */
class COgreObject
{
	enum EGeometryFileType
	{
		GeometryFileType_Invalid=0,
		GeometryFileType_Scene,
		GeometryFileType_Mesh
	};

friend class COgreScene;
public:
	/**
	 * \brief Constructor
	 * \param[in] rKernelContext OpenViBE kernel context
	 * \param[in] rIdentifier Object identifier
	 * \param[in] pOgreVis pointer to OgreVisualisation object
	 * \param[in] pSceneManager pointer to scene manager this object is to be added to
	 * \param[in] rGeometryfileName name of file containing this object's geometry
	 */
	COgreObject(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		OpenViBE::CIdentifier oIdentifier,
		COgreVisualisation* pOgreVis,
		Ogre::SceneManager* pSceneManager,
		const OpenViBE::CString& rGeometryFileName);

	COgreObject(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		OpenViBE::CIdentifier oIdentifier,
		COgreVisualisation* pOgreVis,
		Ogre::SceneManager* pSceneManager);

	/// Destructor
	~COgreObject();

	/**
	 * \brief Clone object
	 * \param[in] oIdentifier Identifier of the object
	 * \param[out] pClone Reference of pointer to cloned object
	 * \return True if cloned object could be created, false otherwise
	 */
	/*
	OpenViBE::boolean clone(
		OpenViBE::CIdentifier oIdentifier,
		COgreObject*& pClone);*/

	/// Return object identifier
	OpenViBE::CIdentifier getIdentifier();

	/// Return object name
	const OpenViBE::CString& getName();

	/// Return geometry file name
	const OpenViBE::CString& getGeometryFileName();

	/// Return the node to change the materials
  NodeMaterialOrig* getNodeMaterialOrig() ;

	/// Retrieve scene node corresponding to this object
	Ogre::SceneNode& getSceneNode() const;

	/**
	 * \brief Set visibility
	 * \param bVisible Visibility flag
	 * \return True if visibility could be set, false otherwise
	 */
	OpenViBE::boolean setVisible(
		OpenViBE::boolean bVisible);

	/**
	 * \brief Set the color of an object
	 * \param oDiffuse RGBA diffuse color
	 * \return True if color could be set, false otherwise
	 */
	OpenViBE::boolean setDiffuseColor(
		const Ogre::ColourValue& oDiffuse);

	/**
	 * \brief Set the color of an object
	 * \param f32ColorRed Red component (0 to 1)
	 * \param f32ColorGreen Green component (0 to 1)
	 * \param f32ColorBlue Blue component (0 to 1)
	 * \return True if color could be set, false otherwise
	 */
	OpenViBE::boolean setDiffuseColor(
		Ogre::Real f32ColorRed,
		Ogre::Real f32ColorGreen,
		Ogre::Real f32ColorBlue
		);

	/**
	 * \brief Set the transparency of an object
	 * \param f32Transparency Transparency (0 to 1, 1 being totally transparent)
	 * \return True if transparency could be set, false otherwise
	 */
	OpenViBE::boolean setTransparency(
		Ogre::Real transparency);

	/**
	 * \brief Set the position of an object
	 * \param f32PositionX X coordinate of position
	 * \param f32PositionY Y coordinate of position
	 * \param f32PositionZ Z coordinate of position
	 * \return True if object position could be set, false otherwise
	 */
	OpenViBE::boolean setPosition(
		Ogre::Real positionX,
		Ogre::Real positionY,
		Ogre::Real positionZ);

	/**
	 * \brief Set the orientation of an object
	 * \return True if orientation could be set, false otherwise
	 */
	OpenViBE::boolean setRotation(
		Ogre::Real rotationX,
		Ogre::Real rotationY,
		Ogre::Real rotationZ);

	/**
	 * \brief Set the orientation of an object
	 * \param f32OrientationX X component of a quaternion
	 * \param f32OrientationY Y component of a quaternion
	 * \param f32OrientationZ Z component of a quaternion
	 * \param f32OrientationW W component of a quaternion
	 * \return True if orientation could be set, false otherwise
	 */
	OpenViBE::boolean setRotation(
		Ogre::Real rotationX,
		Ogre::Real rotationY,
		Ogre::Real rotationZ,
		Ogre::Real rotationW);

	/**
	 * \brief Set the scale of an object
	 * \param f32ScaleX X component of scale
	 * \param f32ScaleY Y component of scale
	 * \param f32ScaleZ Z component of scale
	 * \return True if scale could be set, false otherwise
	 */
	OpenViBE::boolean setScale(
		Ogre::Real scaleX,
		Ogre::Real scaleY,
		Ogre::Real scaleZ);

	/**
	 * \brief Set vertex colors of an object
	 * \remarks This method applies vertex colors to the first mesh of the first
	 * Ogre::Entity encountered in the object hierarchy only. For it to work, the
	 * Ogre::Mesh needs to have been flagged as using vertex colors in authoring tools.
	 * \param ui32VertexColorCount Number of colors contained in pVertexColorArray
	 * \param pVertexColorArray Array of color triplets
	 * \return True if colors could be set, false otherwise
	 */
	OpenViBE::boolean setVertexColorArray(
		Ogre::uint32 ui32VertexColorCount,
		Ogre::Real* pVertexColorArray);

	/**
	 * \brief Get object vertex count
	 * \remarks This method returns the number of vertices in the first mesh of the first
	 * Ogre::Entity encountered in the object hierarchy only.
	 * \param ui32VertexCount Number of vertices found in first mesh of first entity
	 * \return True if vertex count could be retrieved, false otherwise
	 */
	OpenViBE::boolean getVertexCount(
		Ogre::uint32& ui32VertexCount) const;

	/**
	 * \brief Get vertex position array
	 * \remarks This method returns vertex positions of the first mesh of the first
	 * Ogre::Entity encountered in the object hierarchy only.
	 * \param ui32VertexCount Number of vertices that can be stored in pVertexPositionArray
	 * \param pVertexPositionArray Preallocated array destined to store vertex position triplets
	 * \return True if vertex positions could be retrieved, false otherwise
	 */
	OpenViBE::boolean getVertexPositionArray(
		Ogre::uint32 ui32VertexCount,
		Ogre::Real* pVertexPositionArray) const;

	/**
	 * \brief Get object triangle count
	 * \remarks This method returns the number of triangles in the first mesh of the first
	 * Ogre::Entity encountered in the object hierarchy only.
	 * \param ui32TriangleCount Number of triangles found in first mesh of first entity
	 * \return True if triangle count could be retrieved, false otherwise
	 */
	OpenViBE::boolean getTriangleCount(
		Ogre::uint32& ui32TriangleCount) const;

	/**
	 * \brief Get array of face indices
	 * \remarks This method returns face indices of the first mesh of the first
	 * Ogre::Entity encountered in the object hierarchy only.
	 * \param ui32TriangleCount Number of triangles that can be stored in pTriangleIndexArray
	 * \param pTriangleIndexArray Preallocated array destined to store face indices triplets
	 * \return True if face indices could be retrieved, false otherwise
	 */
	OpenViBE::boolean getTriangleIndexArray(
		Ogre::uint32 ui32TriangleCount,
		OpenViBE::uint32* pTriangleIndexArray) const;

	/**
	 * \brief Get world bounding box of an object
	 * \param rMinX X lower bound
	 * \param rMinY Y lower bound
	 * \param rMinZ Z lower bound
	 * \param rMaxX X upper bound
	 * \param rMaxY Y upper bound
	 * \param rMaxZ Z upper bound
	 * \return True if bounding box could be retrieved, false otherwise
	 */
	OpenViBE::boolean getWorldBoundingBox(
		Ogre::Real& rMinX,
		Ogre::Real& rMinY,
		Ogre::Real& rMinZ,
		Ogre::Real& rMaxX,
		Ogre::Real& rMaxY,
		Ogre::Real& rMaxZ);

private:
	/**
	 * \brief Clone the geometry from an existing Entity
	 * \param pEntity pointer to Entity whose geometry is to be cloned
	 * \return True if geometry could be cloned, false otherwise
	 */
	OpenViBE::boolean cloneGeometry(
		Ogre::Entity* pEntity);

	/**
	 * \brief Load the geometry using the dotSceneInterface
	 * \param rNameValuePairList Geometry parameters list
	 * \return True if geometry could be loaded, false otherwise
	 */
	OpenViBE::boolean loadGeometry(
		const OpenViBE::CNameValuePairList& rNameValuePairList);

	/**
	 * \brief Create plane from scratch
	 * \param rNameValuePairList Plane parameters list
	 * \return True if plane could be created, false otherwise
	 */
	OpenViBE::boolean createPlane(
		const OpenViBE::CNameValuePairList& rNameValuePairList);

	/**
	 * \brief Finish loading geometry
	 * \details This method checks whether materials and/or mesh should be cloned,
	 * and finishes building the object.
	 * \param rNameValuePairList Plane parameters list
	 * \return True
	 */
	OpenViBE::boolean finishLoading(
		const OpenViBE::CNameValuePairList& rNameValuePairList);

	/// Clone meshes used by this object
	OpenViBE::boolean cloneMeshes();

	/// Clone materials used by this object
	OpenViBE::boolean cloneMaterials();

	/**
	 * \brief Look for text file containing object orientation, position
	 * and/or scale parameters
	 * \details File name should be identical to that of the mesh file, and
	 * file extension should be "txt"
	 * \return True
	 */
	OpenViBE::boolean loadWorldMatrix();

	/**
	 * \brief Create animators
	 * This creates all potentially needed animators : a transform animator to rotate, translate and
	 * scale the object, a color animator to modify its color/transparency, and a vertex buffer animator to
	 * access the object geometry (e.g. set vertex colors)
	 * \return True if animators could be created, false otherwise
	 */
	OpenViBE::boolean createAnimators();

	/**
	 * \brief Retrieve first Entity in Ogre hierarchy starting at pNode
	 * \param pNode Pointer to an Ogre node from which to start looking for an Entity
	 * \param pEntity Pointer to first Entity found in an Ogre hierarchy
	 * \return True if an Entity could be found (pEntity then points to it), false otherwise
	 */
	OpenViBE::boolean getFirstEntityInHierarchy(
		Ogre::SceneNode* pNode,
		Ogre::Entity*& pEntity) const;

	/**
	 * \brief Retrieve first SubMesh referenced in an Entity
	 * \param pEntity Pointer to an entity
	 * \param pSubMesh Pointer to first SubMesh referenced in pEntity
	 * \return True if a SubMesh could be found (pSubMesh then points to it), false otherwise
	 */
	OpenViBE::boolean getFirstSubMesh(
		Ogre::Entity* pEntity,
		Ogre::SubMesh*& pSubMesh) const;

private:
	/// OpenViBE kernel context
	const OpenViBE::Kernel::IKernelContext& m_rKernelContext;

	// identifier of the visual object
	OpenViBE::CIdentifier m_oIdentifier;

	// name of the visual object
	OpenViBE::CString m_sName;

	/// reference to the associated visualisation
	COgreVisualisation* m_pOgreVis;

	/// root node of the loaded geometry
	Ogre::SceneNode* m_pSceneNode;

	/// scene manager to attach this object to
	Ogre::SceneManager* const m_pSceneManager;

	/// MaterialAnimation stack
  NodeMaterialOrig* m_pNodeMaterial;

	/// name of the resource group of the geometry file
	OpenViBE::CString m_sResourceGroupName;

	/// type of file containing this object's geometry
	EGeometryFileType m_eGeometryFileType;

	/// name of the geometry file
	OpenViBE::CString m_sGeometryFileName;

	/** \name Animators */
	//@{
	/**
	 * \brief Transform animator
   * This allows for modification of the object position, orientation and scale
	 */
	CTransformAnimator* m_pTransformAnimator;
	/**
	 * \brief Color animator
   * This allows for modification of the color and transparency of the material used by the object
	 */
	CColorAnimator* m_pColorAnimator;
	/**
	 * \brief Vertex buffer animator
   * This allows for updating vertex buffer contents
	 */
	CVertexBufferAnimator* m_pVertexBufferAnimator;
	//@}
};

#endif // __OpenViBEKernel_Kernel_Player_COgreObject_H__
