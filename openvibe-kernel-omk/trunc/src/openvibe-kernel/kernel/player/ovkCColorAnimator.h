#ifndef __OpenViBEKernel_Kernel_Player_CColorAnimator_H__
#define __OpenViBEKernel_Kernel_Player_CColorAnimator_H__

#include "ovkCOgreAnimator.h"
#include "ovkMaterialAction.h"

class NodeMaterialOrig;

class CColorAnimator : public COgreAnimator
{
public:
	CColorAnimator(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		COgreObject& rOgreObject,
		const OpenViBE::CIdentifier& rIdentifier);

	virtual ~CColorAnimator();

	virtual OpenViBE::boolean init(const std::string& rNodeName);

	void setDiffuseColor(const Ogre::ColourValue& diffuse);
	void setDiffuseColor(Ogre::Real r, Ogre::Real g, Ogre::Real b);
	void setTransparency(Ogre::Real transparency);

protected:
	NodeMaterialOrig* m_pNodeMaterial;
	MaterialSetDiffuse m_oMaterialSetDiffuse;
	Ogre::Vector4 m_oColor;
};

#endif // __OpenViBEKernel_Kernel_Player_CColorAnimator_H__
