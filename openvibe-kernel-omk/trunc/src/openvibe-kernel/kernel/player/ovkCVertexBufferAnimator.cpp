#include "ovkMaterialAction.h"
#include "ovkCVertexBufferAnimator.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using std::string;
using Ogre::SceneNode;
using Ogre::MovableObject;
using Ogre::Entity;

CVertexBufferAnimator::CVertexBufferAnimator(const IKernelContext& rKernelContext, COgreObject& rOgreObject, const CIdentifier& rIdentifier) :
	COgreAnimator(rKernelContext, rOgreObject, rIdentifier),
	m_pNodeMaterial(NULL)
{
}

CVertexBufferAnimator::~CVertexBufferAnimator()
{
	/*
  if(m_pNodeMaterial != NULL)
	{
		delete m_pNodeMaterial;
	}*/
}

boolean CVertexBufferAnimator::init(const std::string& rNodeName)
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
	return true;
}

void CVertexBufferAnimator::setVertexColorArray(Ogre::uint32 ui32VertexColorCount, Ogre::Real* pVertexColorArray)
{
	/*const VertexData& l_rVertexData( rVertexDataPrm.getValue() );

	uint32 posCount, colCount;
	const float32* posArray;
	const float32* colArray;
	l_rVertexData.getVertexPositionArray(posCount, posArray);
	l_rVertexData.getVertexColorArray(colCount, colArray);
	m_pNodeMaterial->doAction(MaterialVertexBuffer(colCount, colArray));
	*/
	m_pNodeMaterial->doAction(MaterialVertexBuffer(m_rKernelContext, ui32VertexColorCount, pVertexColorArray));
}
