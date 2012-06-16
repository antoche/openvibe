#include "ovassvepCShooterTarget.h"

using namespace OpenViBESSVEP;

Ogre::SceneNode* CShooterTarget::m_poParentNode = NULL;
CBasicPainter* CShooterTarget::m_poPainter = NULL;

void CShooterTarget::initialize( CBasicPainter* poPainter, Ogre::SceneNode* poParentNode )
{
	m_poPainter = poPainter;
	m_poParentNode = poParentNode;
}

CShooterTarget* CShooterTarget::createTarget( Ogre::Radian rAngle )
{
	if ( m_poPainter == NULL )
	{
		std::cerr << "The CShooterTarget was not initialized" << std::endl;
		return NULL;
	}

	return new CShooterTarget( rAngle );
}

CShooterTarget::CShooterTarget( Ogre::Radian rAngle )
{
	m_poTargetNode = m_poParentNode->createChildSceneNode();


	Ogre::MovableObject* l_poTargetObject = m_poPainter->paintCircle(0.0f, 1.0f - 1.0f * SSVEP_SHOOTER_TARGET_SIZE, SSVEP_SHOOTER_TARGET_SIZE, Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	l_poTargetObject->setVisible( true );
	m_poTargetNode->attachObject( l_poTargetObject );

	m_poTargetNode->roll( rAngle );

}

CShooterTarget::~CShooterTarget()
{
	m_poTargetNode->removeAndDestroyAllChildren();
	m_poTargetNode->getCreator()->destroySceneNode( m_poTargetNode );
}

bool CShooterTarget::collidesWith(std::pair<Ogre::Real, Ogre::Real> pPoint)
{
	Ogre::Radian l_rTheta = (m_poTargetNode->getOrientation()).getRoll();

	Ogre::Real l_rTargetDistance = 1.0f - 2 * SSVEP_SHOOTER_TARGET_SIZE;

	Ogre::Real l_rDistance = sqrt(pow(pPoint.first + Ogre::Math::Sin(l_rTheta) * l_rTargetDistance, 2) + pow(pPoint.second - Ogre::Math::Cos(l_rTheta) * l_rTargetDistance, 2));

	return l_rDistance <= SSVEP_SHOOTER_TARGET_SIZE;
}
