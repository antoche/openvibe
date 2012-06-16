#ifndef __OpenViBEKernel_Kernel_Player_CTransformAnimator_H__
#define __OpenViBEKernel_Kernel_Player_CTransformAnimator_H__

#include "ovkCOgreAnimator.h"

class CTransformAnimator : public COgreAnimator
{
public:
	CTransformAnimator(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		COgreObject& rOgreObject,
		const OpenViBE::CIdentifier& rIdentifier);

	virtual ~CTransformAnimator();

	virtual OpenViBE::boolean init(const std::string& rNodeName);

	void setPosition(Ogre::Real positionX, Ogre::Real positionY, Ogre::Real positionZ);
	void setRotation(Ogre::Real rotationX, Ogre::Real rotationY, Ogre::Real rotationZ);
	void setRotation(Ogre::Real rotationX, Ogre::Real rotationY, Ogre::Real rotationZ, Ogre::Real rotationW);
	void setScale(Ogre::Real scaleX, Ogre::Real scaleY, Ogre::Real scaleZ);
};

#endif // __OpenViBEKernel_Kernel_Player_CTransformAnimator_H__
