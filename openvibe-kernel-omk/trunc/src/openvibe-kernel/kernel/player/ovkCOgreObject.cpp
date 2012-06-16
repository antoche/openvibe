#include <sstream>
#include <locale>
#if defined OVP_OS_Windows
#include <cerrno>
#endif

#include <cstring>
#include <cstdlib>

#include "Ogre.h"
#include "OgreSceneNode.h"
#include "OgreMovableObject.h"

#include "ovkCMaterial.h"
#include "ovkCTransformAnimator.h"
#include "ovkCColorAnimator.h"
#include "ovkCVertexBufferAnimator.h"
#include "ovkCOgreObject.h"
#include "ovkCOgreVisualisation.h"

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

typedef Ogre::Real ogre_float;
typedef Ogre::uint32 ogre_uint32;
typedef Ogre::uint64 ogre_uint64;
typedef Ogre::uint16 ogre_uint16;
typedef Ogre::uint8  ogre_uint8;
#define uint8  OpenViBE::uint8
#define uint16 OpenViBE::uint16
#define uint32 OpenViBE::uint32
#define uint64 OpenViBE::uint64
#define boolean OpenViBE::boolean

COgreObject::COgreObject(const IKernelContext& rKernelContext, CIdentifier oIdentifier, COgreVisualisation* pOgreVis, Ogre::SceneManager* pSceneManager, const CString& rGeometryFileName) :
	m_rKernelContext(rKernelContext),
	m_oIdentifier(oIdentifier),
	m_sName(oIdentifier.toString()),
	m_pOgreVis(pOgreVis),
	m_pSceneNode(NULL),
	m_pSceneManager(pSceneManager),
	m_pNodeMaterial(NULL),
	m_sResourceGroupName("General"),
	m_eGeometryFileType(GeometryFileType_Invalid),
	m_sGeometryFileName(rGeometryFileName),
	m_pTransformAnimator(NULL),
	m_pColorAnimator(NULL),
	m_pVertexBufferAnimator(NULL)
{
	if(strstr(m_sGeometryFileName.toASCIIString(), ".scene") != NULL)
	{
		m_eGeometryFileType = GeometryFileType_Scene;
	}
	else if(strstr(m_sGeometryFileName.toASCIIString(), ".mesh") != NULL)
	{
		m_eGeometryFileType = GeometryFileType_Mesh;
	}
	else
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Couldn't determine type of geometry file for object \"" << m_sName << "\"\n";
	}

	m_pSceneNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode(m_sName.toASCIIString());

	if(m_pSceneNode == NULL)
	{
		m_rKernelContext.getLogManager() << LogLevel_Error << "Failed to create SceneNode \"" << m_sName.toASCIIString() << "\" for object \"" << m_sName << "\"\n";
	}

	//don't create node material before geometry is loaded!
	//m_pNodeMaterial = new NodeMaterialOrig(m_pSceneNode);

	//don't create animators before geometry is loaded!
	//m_pColorAnimator = new CColorAnimator(*this, OV_UndefinedIdentifier, "DCS_position");
}

COgreObject::COgreObject(const IKernelContext& rKernelContext, OpenViBE::CIdentifier oIdentifier, COgreVisualisation* pOgreVis, Ogre::SceneManager* pSceneManager) :
	m_rKernelContext(rKernelContext),
	m_oIdentifier(oIdentifier),
	m_sName(oIdentifier.toString()),
	m_pOgreVis(pOgreVis),
	m_pSceneNode(NULL),
	m_pSceneManager(pSceneManager),
	m_pNodeMaterial(NULL),
	m_sResourceGroupName("General"),
	m_eGeometryFileType(GeometryFileType_Invalid),
	m_pTransformAnimator(NULL),
	m_pColorAnimator(NULL),
	m_pVertexBufferAnimator(NULL)
{
	m_pSceneNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode(m_sName.toASCIIString());

	if(m_pSceneNode == NULL)
	{
		m_rKernelContext.getLogManager() << LogLevel_Error << "Failed to create SceneNode \"" << m_sName.toASCIIString() << "\" for object \"" << m_sName << "\"\n";
	}
}

COgreObject::~COgreObject()
{
	try
	{
		m_pSceneManager->destroySceneNode(m_pSceneNode->getName());
	}
	catch(Ogre::Exception& e)
	{
		m_rKernelContext.getLogManager()
			<< LogLevel_Warning << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Failed to destroy scene node : " << e.what() << "\n";
	}

	if(m_pNodeMaterial != NULL)
	{
		delete m_pNodeMaterial;
	}

	if(m_pTransformAnimator != NULL)
	{
		delete m_pTransformAnimator;
	}

	if(m_pColorAnimator != NULL)
	{
		delete m_pColorAnimator;
	}

	if(m_pVertexBufferAnimator != NULL)
	{
		delete m_pVertexBufferAnimator;
	}
}

boolean COgreObject::cloneMeshes()
{
	Ogre::Entity* l_pFirstEntity = NULL;
	getFirstEntityInHierarchy(m_pSceneNode, l_pFirstEntity);

	if(l_pFirstEntity == NULL)
	{
		return false;
	}

	try
	{
		//clone mesh
		const Ogre::MeshPtr& l_pMesh = l_pFirstEntity->getMesh();
		std::string l_sNewMeshName = l_pMesh->getName() + m_sName.toASCIIString();
		Ogre::MeshPtr l_pNewMesh = l_pMesh->clone(l_sNewMeshName);

		//create new entity based on new mesh
		std::string l_sEntityName = l_pFirstEntity->getName();
		Ogre::SceneNode* l_pSceneNode = l_pFirstEntity->getParentSceneNode();
		l_pSceneNode->detachObject(l_pFirstEntity);
		m_pSceneManager->destroyEntity(l_pFirstEntity);
		Ogre::Entity* l_pNewEntity = m_pSceneManager->createEntity(l_sEntityName, l_sNewMeshName);

		//attach new entity
		l_pSceneNode->attachObject(l_pNewEntity);
	}
	catch(Ogre::Exception& e)
	{
		m_rKernelContext.getLogManager()
			<< LogLevel_Warning << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Failed to clone mesh : " << e.what() << "\n";
	}

	return true;
}

boolean COgreObject::cloneMaterials()
{
	try
	{
	Ogre::Entity* l_pFirstEntity = NULL;
	getFirstEntityInHierarchy(m_pSceneNode, l_pFirstEntity);

	if(l_pFirstEntity == NULL)
	{
		return false;
	}

#if 0
	std::map<std::string, std::string> m_mClonedMaterials;

	//clone materials of all subentities
	for(uint32 i=0;  i<l_pFirstEntity->getNumSubEntities(); i++)
	{
		Ogre::SubEntity* l_pSubEntity = l_pFirstEntity->getSubEntity(i);
		std::string l_sMaterialName(l_pSubEntity->getMaterial()->getName());
		//if material of current subentity hasn't been cloned yet
		if(m_mClonedMaterials.find(l_sMaterialName) == m_mClonedMaterials.end())
		{
			//generate new name
			m_mClonedMaterials[l_sMaterialName] = l_sMaterialName + m_sName.toASCIIString();
			//clone material
			l_pSubEntity->getMaterial()->clone(m_mClonedMaterials[l_sMaterialName]);
		}

		//set cloned material on subentity
		l_pSubEntity->setMaterialName(m_mClonedMaterials[l_sMaterialName]);
	}
#else
	std::string l_sMaterialName = l_pFirstEntity->getSubEntity(0)->getMaterialName();
	std::string l_sNewMaterialName = l_sMaterialName + m_sName.toASCIIString();
	Ogre::MaterialPtr newMat = l_pFirstEntity->getSubEntity(0)->getMaterial()->clone(l_sNewMaterialName);
	l_pFirstEntity->setMaterialName(l_sNewMaterialName);
#endif
	}
	catch(Ogre::Exception& e)
	{
		m_rKernelContext.getLogManager()
			<< LogLevel_Warning << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Failed to clone materials : " << e.what() << "\n";
	}

	return true;
}

boolean COgreObject::createPlane(const CNameValuePairList& rNameValuePairList)
{
	try
	{
		float64 l_f64PlaneWidth = 1.f;
		rNameValuePairList.getValue("PlaneWidth", l_f64PlaneWidth);
		float64 l_f64PlaneHeight = 1.f;
		rNameValuePairList.getValue("PlaneHeight", l_f64PlaneWidth);

		Ogre::MeshManager::getSingleton().createPlane(
			m_sName.toASCIIString(),//"myPlane", //mesh name
			m_sResourceGroupName.toASCIIString(),
			Ogre::Plane(Ogre::Vector3::UNIT_Z, 0.0), //plane orientation and distance from origin
			l_f64PlaneWidth, l_f64PlaneHeight, //width/height
			1, 1, //x and y segment count
			true, //generate normals?
			1, //texcoord set count
			1, 1,//u/v tiling
			Ogre::Vector3::UNIT_Y, //plane up vector
			Ogre::HardwareBuffer::HBU_WRITE_ONLY, //vertex buffer usage
			Ogre::HardwareBuffer::HBU_STATIC); //index buffer usage

		Ogre::Entity* l_pEntity = m_pSceneManager->createEntity(Ogre::String(m_sName.toASCIIString()) + "Entity", m_sName.toASCIIString());//"myPlane"); //FIXME!!
		//l_pEntity->setMaterialName("Examples/RustySteel");
		m_pSceneNode->attachObject(l_pEntity);

		finishLoading(rNameValuePairList);
	}
	catch(Ogre::Exception& e)
	{
		m_rKernelContext.getLogManager()
			<< LogLevel_Warning << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Failed to create plane : " << e.what() << "\n";
		return false;
	}

	return true;
}

boolean COgreObject::loadGeometry(const CNameValuePairList& rNameValuePairList)
{
	if(m_eGeometryFileType == GeometryFileType_Invalid)
	{
		return false;
	}

	try
	{
		if(m_eGeometryFileType == GeometryFileType_Scene)
		{
			m_rKernelContext.getLogManager()
				<< LogLevel_Warning << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
				<< "Failed to load object : .scene files are not supported!\n";
			/*Singleton<Ogre::dsi::dotSceneLoader>::getInstance().
				load(m_sGeometryFileName.toASCIIString(),
					m_sResourceGroupName.toASCIIString(),
					m_pSceneManager,
					NULL,
					m_pSceneNode,
					false,
					false,
					false);*/
		}
		else if(m_eGeometryFileType == GeometryFileType_Mesh)
		{
			//retrieve mesh loading parameters
			Ogre::HardwareBuffer::Usage l_oVertexBufferUsage = Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;
			Ogre::HardwareBuffer::Usage l_oIndexBufferUsage = Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;

			CString l_sVertexBufferUsage;
			if(rNameValuePairList.getValue("VertexBufferUsage", l_sVertexBufferUsage) == true)
			{
				if(l_sVertexBufferUsage == CString("Dynamic"))
				{
					l_oVertexBufferUsage = Ogre::HardwareBuffer::HBU_DYNAMIC;
				}
			}
			CString l_sIndexBufferUsage;
			if(rNameValuePairList.getValue("IndexBufferUsage", l_sIndexBufferUsage) == true)
			{
				if(l_sIndexBufferUsage == CString("Dynamic"))
				{
					l_oIndexBufferUsage = Ogre::HardwareBuffer::HBU_DYNAMIC;
				}
			}

			//load mesh
			Ogre::String l_oMeshName(m_sGeometryFileName);
			Ogre::String l_oResourceGroupName(m_sResourceGroupName);

			Ogre::MeshManager::getSingletonPtr()->load(l_oMeshName,
				l_oResourceGroupName,
				l_oVertexBufferUsage,
				l_oIndexBufferUsage,
				true, //VB shadowed
				true); //IB shadowed

			//create entity
			Ogre::String l_sEntityName = Ogre::String(m_sName.toASCIIString()) + "_Entity";
			Ogre::Entity* l_pEntity = m_pSceneManager->createEntity(l_sEntityName, l_oMeshName);
			m_pSceneNode->attachObject(l_pEntity);

			finishLoading(rNameValuePairList);
		}
	}
	catch(Ogre::Exception& e)
	{
		m_rKernelContext.getLogManager()
			<< LogLevel_Warning << "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Ogre3D" << LogColor_PopStateBit << "::Exception> "
			<< "Failed to load file [" << m_sGeometryFileName << "] : " << e.what() << "\n";
		return false;
	}

	return true;
}

boolean COgreObject::finishLoading(const CNameValuePairList& rNameValuePairList)
{
	//should mesh be cloned?
	boolean l_bCloneMesh = false;
	rNameValuePairList.getValue("CloneMesh", l_bCloneMesh);
	if(l_bCloneMesh == true)
	{
		cloneMeshes();
	}

	//should materials be cloned?
	boolean l_bCloneMaterials = false;
	rNameValuePairList.getValue("CloneMaterials", l_bCloneMaterials);
	if(l_bCloneMaterials)
	{
		cloneMaterials();
	}

	createAnimators();

	loadWorldMatrix();

	return true;
}

boolean COgreObject::loadWorldMatrix()
{
	try
	{
	//build world matrix file name
	std::string l_oFilename = m_sGeometryFileName.toASCIIString();
	std::string::size_type l_oDotIndex = l_oFilename.rfind('.');
	if(l_oDotIndex == std::string::npos)
	{
		return true;
	}
	l_oFilename.erase(l_oDotIndex);
	l_oFilename += ".txt";

	//try and open matrix file
#if 0
	std::ifstream l_oFile(l_oFilename.c_str(), std::ios_base::in);
	if(!l_oFile.good())
	{
		return true;
	}
#else
	//FIXME
	//Ogre::DataStreamPtr l_pStream = Ogre::ResourceGroupManager::getSingleton().openResource(l_oFilename, (const Ogre::String&)m_sResourceGroupName);
	//const Ogre::String l_oResourceGroup("TopographicMap3DResources");
	const Ogre::String& l_oResourceGroup = Ogre::ResourceGroupManager::getSingleton().findGroupContainingResource(l_oFilename);

	Ogre::DataStreamListPtr l_pStreamList =
		Ogre::ResourceGroupManager::getSingleton().openResources(l_oFilename, l_oResourceGroup/*(const Ogre::String&)m_sResourceGroupName*/);
	if(l_pStreamList->empty() == true)
	{
		return true;
	}
	Ogre::DataStreamPtr l_pStream = *l_pStreamList->begin();
#endif

	m_rKernelContext.getLogManager() << LogLevel_Trace << "Opening " << CString(l_oFilename.c_str()) << " succeeded\n";

	enum EStatus
	{
		Status_Nothing,
		Status_ParsingLabel,
		Status_ParsingVector,
		Status_ParsingPosition,
		Status_ParsingOrientation,
		Status_ParsingScale
	};

	std::locale l_oLocale("C");
	//current string to parse
	std::string l_sWhat;
	//current parsing status
	uint32 l_ui32Status=Status_Nothing;
	//current value index (must reach 3 when parsing position & orientation)
	uint32 l_ui32CurValueIndex = 0;
	//current quote-delimited string
	std::string l_sCurString;
	//position array (x y z)
	float32 l_pPosition[3];
	//orientation array (roll pitch yaw)
	float32 l_pOrientation[3];
	//scale array (sx sy sz)
	float32 l_pScale[3];

	//read current line
	//std::getline(l_oFile, l_sWhat, '\n');

	while(l_pStream->eof() == false)
	{
		l_sWhat = (std::string)l_pStream->getLine();

		//is line empty?
		if(l_sWhat.length()==0)
		{
			//skip it
			continue;
		}

		//reset token index
		l_ui32CurValueIndex = 0;

		//reset current string
		l_sCurString.erase();

		//output line to be parsed in debug level
		//m_rKernelContext.getLogManager() << LogLevel_Debug << CString(l_sWhat.c_str()) << "\n";

		//remove ending carriage return (if any) for windows / linux compatibility
		if(l_sWhat[l_sWhat.length()-1]=='\r')
		{
			l_sWhat.erase(l_sWhat.length()-1, 1);
		}

		//start parsing current line
		std::string::iterator l_oIt = l_sWhat.begin();

		//parse current line
		while(l_oIt != l_sWhat.end())
		{
			switch(l_ui32Status)
			{
				case Status_Nothing:

					//comments starting
					if(*l_oIt == '#')
					{
						//ignore rest of line by skipping to last character
						l_oIt = l_sWhat.end()-1;
					}
					//label starting
					else if(std::isspace(*l_oIt, l_oLocale) == false)
					{
						l_sCurString.append(1, *l_oIt);
						l_ui32Status = Status_ParsingLabel;
					}
					break;

				case Status_ParsingLabel:

					//keep parsing till space char found
					if(std::isspace(*l_oIt, l_oLocale) == false)
					{
						l_sCurString.append(1, *l_oIt);
					}
					else
					{
						l_ui32Status = Status_ParsingVector;
					}
					break;

				case Status_ParsingVector:

					if(*l_oIt == '[')
					{
						if(l_sCurString == "position")
						{
							l_ui32Status = Status_ParsingPosition;
						}
						else if(l_sCurString == "orientation")
						{
							l_ui32Status = Status_ParsingOrientation;
						}
						else if(l_sCurString == "scale")
						{
							l_ui32Status = Status_ParsingScale;
						}

						l_sCurString.erase();
					}
					break;

				case Status_ParsingPosition:

					//stop at first whitespace char or when vector end is found
					if(std::isspace(*l_oIt, l_oLocale) == true || *l_oIt == ']')
					{
						float64 l_f64Value = atof(l_sCurString.c_str());
#if defined OVP_OS_Windows
						if(errno == ERANGE)
						{
							//string couldn't be converted to a double
							m_rKernelContext.getLogManager() << LogLevel_Trace << "Couldn't convert token \"" << CString(l_sCurString.c_str()) << "\" to floating point value, parsing aborted\n";
							return false;
						}
#endif
						//store value and update current value index
						l_pPosition[l_ui32CurValueIndex++] = l_f64Value;

						//reset current string
						l_sCurString.erase();

						if(l_ui32CurValueIndex == 3)
						{
							//position object
							setPosition(l_pPosition[0], l_pPosition[1], l_pPosition[2]);
						}

						if(l_ui32CurValueIndex == 3 || *l_oIt == ']')
						{
							l_oIt = l_sWhat.end() - 1;
							l_ui32Status = Status_Nothing;
							break;
						}
					}
					//otherwise, append current character to current string
					else
					{
						l_sCurString.append(1, *l_oIt);
					}
					break;

				case Status_ParsingOrientation:

					//stop at first whitespace char or when vector end is found
					if(std::isspace(*l_oIt, l_oLocale) == true || *l_oIt == ']')
					{
						float64 l_f64Value = atof(l_sCurString.c_str());
#if defined OVP_OS_Windows
						if(errno == ERANGE)
						{
							//string couldn't be converted to a double
							m_rKernelContext.getLogManager() << LogLevel_Trace << "Couldn't convert token \"" << CString(l_sCurString.c_str()) << "\" to floating point value, parsing aborted\n";
							return false;
						}
#endif
						//store value and update current value index
						l_pOrientation[l_ui32CurValueIndex++] = l_f64Value;

						//reset current string
						l_sCurString.erase();

						if(l_ui32CurValueIndex == 3)
						{
							//rotate object
							setRotation(l_pOrientation[0], l_pOrientation[1], l_pOrientation[2]);
						}

						if(l_ui32CurValueIndex == 3 || *l_oIt == ']')
						{
							l_oIt = l_sWhat.end() - 1;
							l_ui32Status = Status_Nothing;
							break;
						}
					}
					//otherwise, append current character to current string
					else
					{
						l_sCurString.append(1, *l_oIt);
					}
					break;

				case Status_ParsingScale:

					//stop at first whitespace char or when vector end is found
					if(std::isspace(*l_oIt, l_oLocale) == true || *l_oIt == ']')
					{
						float64 l_f64Value = atof(l_sCurString.c_str());
#if defined OVP_OS_Windows
						if(errno == ERANGE)
						{
							//string couldn't be converted to a double
							m_rKernelContext.getLogManager() << LogLevel_Trace << "Couldn't convert token \"" << CString(l_sCurString.c_str()) << "\" to floating point value, parsing aborted\n";
							return false;
						}
#endif
						//store value and update current value index
						l_pScale[l_ui32CurValueIndex++] = l_f64Value;

						//reset current string
						l_sCurString.erase();

						if(l_ui32CurValueIndex == 3)
						{
							//scale object
							setScale(l_pScale[0], l_pScale[1], l_pScale[2]);
						}

						if(l_ui32CurValueIndex == 3 || *l_oIt == ']')
						{
							l_oIt = l_sWhat.end() - 1;
							l_ui32Status = Status_Nothing;
							break;
						}
					}
					//otherwise, append current character to current string
					else
					{
						l_sCurString.append(1, *l_oIt);
					}
					break;

				default:
					break;
			} // switch(l_ui32Status)

			//increment iterator
			l_oIt++;

		} // while(l_oIt != l_sWhat.end()) (read each character of current line)
	}

	l_pStream->close();
	}
	catch(Ogre::Exception&)
	{
	}

	return true;
}

CIdentifier COgreObject::getIdentifier()
{
	return m_oIdentifier;
}

const CString& COgreObject::getName()
{
	return m_sName;
}

const CString& COgreObject::getGeometryFileName()
{
	return m_sGeometryFileName;
}

NodeMaterialOrig* COgreObject::getNodeMaterialOrig()
{
	return m_pNodeMaterial;
}

Ogre::SceneNode& COgreObject::getSceneNode() const
{
	return *m_pSceneNode;
}

boolean COgreObject::setVisible(boolean bVisible)
{
	Ogre::Entity* l_pFirstEntity = NULL;
	if(getFirstEntityInHierarchy(m_pSceneNode, l_pFirstEntity) == true)
	{
		l_pFirstEntity->setVisible(bVisible);
		return true;
	}

	return false;
}

boolean COgreObject::setDiffuseColor(const Ogre::ColourValue& oDiffuse)
{
	if(m_pColorAnimator != NULL)
	{
		m_pColorAnimator->setDiffuseColor(oDiffuse);
	}
	return true;
}

boolean COgreObject::setDiffuseColor(ogre_float r, ogre_float g, ogre_float b)
{
	if(m_pColorAnimator != NULL)
	{
		m_pColorAnimator->setDiffuseColor(r, g, b);
	}
	return true;
}

boolean COgreObject::setTransparency(ogre_float t)
{
	if(m_pColorAnimator!= NULL)
	{
		m_pColorAnimator->setTransparency(t);
	}
	return true;
}

boolean COgreObject::setPosition(ogre_float positionX, ogre_float positionY, ogre_float positionZ)
{
	if(m_pTransformAnimator != NULL)
	{
		m_pTransformAnimator->setPosition(positionX, positionY, positionZ);
	}
	return true;
}

boolean COgreObject::setRotation(ogre_float rotationX, ogre_float rotationY, ogre_float rotationZ)
{
	if(m_pTransformAnimator != NULL)
	{
		m_pTransformAnimator->setRotation(rotationX, rotationY, rotationZ);
	}
	return true;
}

boolean COgreObject::setRotation(ogre_float rotationX, ogre_float rotationY, ogre_float rotationZ, ogre_float rotationW)
{
	if(m_pTransformAnimator != NULL)
	{
		m_pTransformAnimator->setRotation(rotationX, rotationY, rotationZ, rotationW);
	}
	return true;
}

boolean COgreObject::setScale(ogre_float scaleX, ogre_float scaleY, ogre_float scaleZ)
{
	if(m_pTransformAnimator != NULL)
	{
		m_pTransformAnimator->setScale(scaleX, scaleY, scaleZ);
	}
	return true;
}

boolean COgreObject::setVertexColorArray(ogre_uint32 ui32VertexColorCount, ogre_float* pVertexColorArray)
{
	if(m_pVertexBufferAnimator != NULL)
	{
		m_pVertexBufferAnimator->setVertexColorArray(ui32VertexColorCount, pVertexColorArray);
	}
	return true;
}

boolean COgreObject::getVertexCount(ogre_uint32& ui32VertexCount) const
{
	Ogre::Entity* l_pEntity;
	if(getFirstEntityInHierarchy(&getSceneNode(), l_pEntity) == false)
	{
		return false;
	}

	Ogre::SubMesh* l_pSubMesh;
	if(getFirstSubMesh(l_pEntity, l_pSubMesh) == false)
	{
		return false;
	}

	if(l_pSubMesh->useSharedVertices == true)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Failed to retrieve vertex count : submesh uses shared vertices!\n";
		return false;
	}

	ui32VertexCount = (uint32)l_pSubMesh->vertexData->vertexCount;

	return true;
}

boolean COgreObject::getVertexPositionArray(ogre_uint32 ui32VertexCount, ogre_float* pVertexPositionArray) const
{
	if(pVertexPositionArray == NULL)
	{
		return false;
	}

	Ogre::Entity* l_pEntity;
	if(getFirstEntityInHierarchy(&getSceneNode(), l_pEntity) == false)
	{
		return false;
	}

	Ogre::SubMesh* l_pSubMesh;
	if(getFirstSubMesh(l_pEntity, l_pSubMesh) == false)
	{
		return false;
	}

	if(l_pSubMesh->useSharedVertices == true)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Failed to retrieve vertices : submesh uses shared vertices!\n";
		return false;
	}

	if(l_pSubMesh->vertexData->vertexCount > ui32VertexCount)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Failed to retrieve vertices : array is too small!\n";
		return false;
	}

	//get world matrix
	Ogre::Matrix4 l_oMatrix4;
	l_oMatrix4=l_pEntity->getParentSceneNode()->_getFullTransform();

	//get pointer to POSITION element
	const Ogre::VertexElement* l_pPositionElement = l_pSubMesh->vertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

	//retrieve VB for POSITION element
	Ogre::HardwareVertexBufferSharedPtr l_pVertexBuffer = l_pSubMesh->vertexData->vertexBufferBinding->getBuffer(l_pPositionElement->getSource());

	//lock VB for reading
	unsigned char* l_pVertexBufferPointer = static_cast<unsigned char*>(l_pVertexBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

	//iterate through VB
	float* l_pReal;

	Ogre::Vector3 l_oWorldVector;

	for( size_t j=0; j<l_pSubMesh->vertexData->vertexCount; ++j, l_pVertexBufferPointer += l_pVertexBuffer->getVertexSize())
	{
		//get pointer to POSITION data
		l_pPositionElement->baseVertexPointerToElement(l_pVertexBufferPointer, &l_pReal);
#if 0
		//store it
		pVertexPositionArray[j*3] = l_pReal[0];
		pVertexPositionArray[j*3+1] = l_pReal[1];
		pVertexPositionArray[j*3+2] = l_pReal[2];
#else
		Ogre::Vector3 tmp(l_pReal[0], l_pReal[1], l_pReal[2]);
		l_oWorldVector = l_oMatrix4 * tmp;
		pVertexPositionArray[j*3] = l_oWorldVector[0];
		pVertexPositionArray[j*3+1] = l_oWorldVector[1];
		pVertexPositionArray[j*3+2] = l_oWorldVector[2];
#endif
	}

	//unlock VB
	l_pVertexBuffer->unlock();

	return true;
}

boolean COgreObject::getTriangleCount(ogre_uint32& ui32TriangleCount) const
{
	Ogre::Entity* l_pEntity;
	if(getFirstEntityInHierarchy(&getSceneNode(), l_pEntity) == false)
	{
		return false;
	}

	Ogre::SubMesh* l_pSubMesh;
	if(getFirstSubMesh(l_pEntity, l_pSubMesh) == false)
	{
		return false;
	}

	if(l_pSubMesh->useSharedVertices == true)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Failed to retrieve triangle count : submesh uses shared vertices!\n";
		return false;
	}

	ui32TriangleCount = (uint32)l_pSubMesh->indexData->indexCount / 3;

	return true;
}

boolean COgreObject::getTriangleIndexArray(ogre_uint32 ui32TriangleCount, uint32* pTriangleIndexArray) const
{
	if(pTriangleIndexArray == NULL)
	{
		return false;
	}

	Ogre::Entity* l_pEntity;
	if(getFirstEntityInHierarchy(&getSceneNode(), l_pEntity) == false)
	{
		return false;
	}

	Ogre::SubMesh* l_pSubMesh;
	if(getFirstSubMesh(l_pEntity, l_pSubMesh) == false)
	{
		return false;
	}

	if(l_pSubMesh->useSharedVertices == true)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Failed to retrieve face indices : submesh uses shared vertices!\n";
		return false;
	}

	if(l_pSubMesh->indexData->indexCount != 3 * ui32TriangleCount)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Failed to retrieve indices : array is too small!\n";
		return false;
	}

	//retrieve IB
	Ogre::HardwareIndexBufferSharedPtr l_pIndexBuffer = l_pSubMesh->indexData->indexBuffer;

	//lock IB for reading
	unsigned char* l_pIndexBufferPointer = static_cast<unsigned char*>(l_pIndexBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

	//iterate through IB
	size_t l_ui32StartIndex = l_pSubMesh->indexData->indexStart;

	if(l_pIndexBuffer->getType() == Ogre::HardwareIndexBuffer::IT_16BIT)
	{
		for(size_t j = l_ui32StartIndex; j<l_pSubMesh->indexData->indexCount+l_ui32StartIndex; ++j, l_pIndexBufferPointer += l_pIndexBuffer->getIndexSize())
		{
			pTriangleIndexArray[j] = *((ogre_uint16*)l_pIndexBufferPointer);
		}
	}
	else //IT_32BIT
	{
		for(size_t j = l_ui32StartIndex; j<l_pSubMesh->indexData->indexCount+l_ui32StartIndex; ++j, l_pIndexBufferPointer += l_pIndexBuffer->getIndexSize())
		{
			pTriangleIndexArray[j] = *((ogre_uint32*)l_pIndexBufferPointer);
		}
	}

	//unlock VB
	l_pIndexBuffer->unlock();

	return true;
}

boolean COgreObject::createAnimators()
{
	//create node material once model hierarchy is loaded
	m_pNodeMaterial = new NodeMaterialOrig(m_pSceneNode);

	//create animators once node material is loaded
	m_pTransformAnimator = new CTransformAnimator(m_rKernelContext, *this, OV_UndefinedIdentifier);
	if(m_pTransformAnimator->init(""/*"DCS_position"*/) == false)
	{
		delete m_pTransformAnimator;
		m_pTransformAnimator = NULL;
	}

	m_pColorAnimator = new CColorAnimator(m_rKernelContext, *this, OV_UndefinedIdentifier);
	if(m_pColorAnimator->init(""/*"DCS_position"*/) == false)
	{
		delete m_pColorAnimator;
		m_pColorAnimator = NULL;
	}

	m_pVertexBufferAnimator = new CVertexBufferAnimator(m_rKernelContext, *this, OV_UndefinedIdentifier);
	if(m_pVertexBufferAnimator->init(""/*"DCS_position"*/) == false)
	{
		delete m_pVertexBufferAnimator;
		m_pVertexBufferAnimator = NULL;
	}

	//REMOVE ME
	/*
	Ogre::Entity* l_pEntity = NULL;
	getFirstEntityInHierarchy(m_pSceneNode, l_pEntity);
	if(l_pEntity != NULL)
	{
		l_pEntity->setNormaliseNormals(true);
	}*/

	return true;
}

boolean COgreObject::getFirstEntityInHierarchy(Ogre::SceneNode* pNode, Ogre::Entity*& pEntity) const
{
	for(uint32 i=0; i<pNode->numAttachedObjects(); i++)
	{
		Ogre::MovableObject* pMovableObject = pNode->getAttachedObject(0);
		if(pMovableObject->getMovableType() == Ogre::EntityFactory::FACTORY_TYPE_NAME)
		{
			pEntity = dynamic_cast<Ogre::Entity*>(pMovableObject);
			return true;
		}
	}

	for(uint32 i=0; i<pNode->numChildren(); i++)
	{
		Ogre::SceneNode* l_pChildNode = dynamic_cast<Ogre::SceneNode*>(pNode->getChild(i));

		if(l_pChildNode != NULL && getFirstEntityInHierarchy(l_pChildNode, pEntity) == true)
		{
			return true;
		}
	}

	m_rKernelContext.getLogManager() << LogLevel_Trace << "Failed to retrieve Entity\n";
	return false;
}

boolean COgreObject::getFirstSubMesh(Ogre::Entity* l_pEntity, Ogre::SubMesh*& l_pSubMesh) const
{
	Ogre::Mesh* l_pMesh = l_pEntity->getMesh().get();
	if(l_pMesh == NULL)
	{
		return false;
	}

	l_pSubMesh = l_pMesh->getSubMesh(0);

	if(l_pSubMesh == NULL)
	{
		m_rKernelContext.getLogManager() << LogLevel_Trace << "Failed to retrieve SubMesh\n";
		return false;
	}

	return true;
}

boolean COgreObject::getWorldBoundingBox(float32& rMinX, float32& rMinY, float32& rMinZ, float32& rMaxX, float32& rMaxY, float32& rMaxZ)
{
	Ogre::Entity* l_pEntity;
	if(getFirstEntityInHierarchy(&getSceneNode(), l_pEntity) == false)
	{
		return false;
	}

	const Ogre::AxisAlignedBox& l_rWorldAABB = l_pEntity->getWorldBoundingBox(true);

	rMinX = l_rWorldAABB.getMinimum().x;
	rMinY = l_rWorldAABB.getMinimum().y;
	rMinZ = l_rWorldAABB.getMinimum().z;

	rMaxX = l_rWorldAABB.getMaximum().x;
	rMaxY = l_rWorldAABB.getMaximum().y;
	rMaxZ = l_rWorldAABB.getMaximum().z;

	return true;
}
