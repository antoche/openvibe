#include "Ogre.h"
#include "ovkCTransformAnimator.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using std::string;
using Ogre::SceneNode;
using Ogre::MovableObject;
using Ogre::Entity;

CTransformAnimator::CTransformAnimator(const IKernelContext& rKernelContext, COgreObject& rOgreObject, const CIdentifier& rIdentifier) :
	COgreAnimator(rKernelContext, rOgreObject, rIdentifier)
{
}

CTransformAnimator::~CTransformAnimator()
{
}

boolean CTransformAnimator::init(const std::string& rNodeName)
{
	if(attachToNode(rNodeName) == false)
	{
		return false;
	}
	return true;
}

void CTransformAnimator::setPosition(Ogre::Real positionX, Ogre::Real positionY, Ogre::Real positionZ)
{
	if(m_pSceneNode != NULL)
	{
		m_pSceneNode->setPosition(positionX, positionY, positionZ);
	}
}

void CTransformAnimator::setRotation(Ogre::Real rotationX, Ogre::Real rotationY, Ogre::Real rotationZ)
{
	//TODO
}

void CTransformAnimator::setRotation(Ogre::Real rotationX, Ogre::Real rotationY, Ogre::Real rotationZ, Ogre::Real rotationW)
{
	if(m_pSceneNode != NULL)
	{
		m_pSceneNode->setOrientation(rotationW, rotationX, rotationY, rotationZ);
	}
}

void CTransformAnimator::setScale(Ogre::Real scaleX, Ogre::Real scaleY, Ogre::Real scaleZ)
{
	if(m_pSceneNode != NULL)
	{
		m_pSceneNode->setScale(scaleX, scaleY, scaleZ);
	}
}
