#ifndef __OpenViBEKernel_Kernel_Player_CVertexBufferAnimator_H__
#define __OpenViBEKernel_Kernel_Player_CVertexBufferAnimator_H__

#include "ovkCOgreAnimator.h"
#include "ovkCVertexData.h"

class NodeMaterialOrig;

class CVertexBufferAnimator : public COgreAnimator
{
public:
	CVertexBufferAnimator(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		COgreObject& rOgreObject,
		const OpenViBE::CIdentifier& rIdentifier);

	virtual ~CVertexBufferAnimator();

	virtual OpenViBE::boolean init(const std::string& rNodeName);

	void setVertexColorArray(/*const CVertexData& rVertexData*/Ogre::uint32 ui32VertexColorCount, Ogre::Real* pVertexColorArray);

protected:
	NodeMaterialOrig* m_pNodeMaterial;
	CVertexData m_oVertexData;
};

#endif // __OpenViBEKernel_Kernel_Player_CVertexBufferAnimator_H__
