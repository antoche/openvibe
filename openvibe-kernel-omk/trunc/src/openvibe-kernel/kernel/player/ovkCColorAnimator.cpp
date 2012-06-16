#include "ovkCColorAnimator.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using std::string;
using Ogre::SceneNode;
using Ogre::MovableObject;
using Ogre::Entity;

CColorAnimator::CColorAnimator(const IKernelContext& rKernelContext, COgreObject& rOgreObject, const CIdentifier& rIdentifier) :
	COgreAnimator(rKernelContext, rOgreObject, rIdentifier),
	m_pNodeMaterial(NULL),
	m_oMaterialSetDiffuse(rKernelContext, Ogre::ColourValue(), false),
	m_oColor (1.f,1.f,1.f,1.f)
{
}

CColorAnimator::~CColorAnimator()
{
	/*if(m_pNodeMaterial != NULL)
	{
		delete m_pNodeMaterial;
	}*/
}

boolean CColorAnimator::init(const std::string& rNodeName)
{
	if(attachToNode(rNodeName) == false)
	{
		return false;
	}
	m_pNodeMaterial = m_rOgreObject.getNodeMaterialOrig()->find(&getSceneNode());
	if(m_pNodeMaterial == NULL)
	{
		//TODO couldn't find node!
		return false;
	}

#if 0 //shaders are not used for now
	//determine whether object is programmable
	IsProgrammable l_oIsProgrammable(m_rKernelContext);
	m_pNodeMaterial->doAction(l_oIsProgrammable);
	m_oMaterialSetDiffuse.setUseShader(l_oIsProgrammable);*/

	//initialize diffuse color with white
	if(l_oIsProgrammable == true)
	{
		m_oMaterialSetDiffuse.setDiffuseColor(Ogre::ColourValue(1,1,1,1));
		m_pNodeMaterial->doAction(m_oMaterialSetDiffuse);
	}
#else
	m_oMaterialSetDiffuse.setUseShader(false);
#endif

	return true;
}

void CColorAnimator::setDiffuseColor(const Ogre::ColourValue& diffuse)
{
	if(m_pNodeMaterial != NULL)
	{
#if 0
		m_pNodeMaterial->doAction(MaterialSetDiffuse(m_rKernelContext, diffuse));
#else
		m_oMaterialSetDiffuse.setDiffuseColor(diffuse);
		m_pNodeMaterial->doAction(m_oMaterialSetDiffuse);
#endif
	}
}

void CColorAnimator::setDiffuseColor(Ogre::Real r, Ogre::Real g, Ogre::Real b)
{
	if(m_pNodeMaterial != NULL)
	{
		m_oMaterialSetDiffuse.setDiffuseColor(r, g, b);
		m_pNodeMaterial->doAction(m_oMaterialSetDiffuse);
	}
}

void CColorAnimator::setTransparency(Ogre::Real t)
{
	if(m_pNodeMaterial != NULL)
	{
#if 0
		m_pNodeMaterial->doAction(MaterialSetTransparency(m_rKernelContext, transparency));
#else
		m_oMaterialSetDiffuse.setDiffuseAlpha(1-t);
		m_pNodeMaterial->doAction(m_oMaterialSetDiffuse);
#endif
	}
}
