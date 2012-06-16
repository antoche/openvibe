#ifndef __OpenViBEKernel_Kernel_Player_COgreAnimator_H__
#define __OpenViBEKernel_Kernel_Player_COgreAnimator_H__

#include <openvibe/ov_all.h>

#include "ovkCOgreObject.h"

namespace Ogre
{
	class Node;
};

class COgreAnimator
{
public:
	/**
	  * \brief Destructor
		*/
	virtual ~COgreAnimator();

	/**
	  * \brief Get the name of the node to animate
		*/
	const std::string& getNodeName() const;

protected:
	/**
	  * \brief Constructor
		*/
	COgreAnimator(
		const OpenViBE::Kernel::IKernelContext& rKernelContext,
		COgreObject& rOgreObject,
		const OpenViBE::CIdentifier& rIdentifier);

	/**
	 * \brief Attach this animator to a scene node
	 */
	virtual OpenViBE::boolean attachToNode(const std::string& rNodeName);

	/**
	  * \brief Get a reference to the ogre node to animate
		*/
	Ogre::SceneNode& getSceneNode() const;

private:
	/**
	  * \brief Find the ogre node named nodeName in the subtree of the associated VisualObject
		*/
	Ogre::Node* findNode(
		const Ogre::Node& rRootNode,
		const std::string& rName);

	/**
	  * \brief Get node name without the delimiter added by the dotscene loader
		*/
	std::string getNameWithoutDotSceneDelimiter(
		const std::string& rName);

protected:
	/** \brief OpenViBE kernel context */
	const OpenViBE::Kernel::IKernelContext& m_rKernelContext;

	/** \brief The id (name) */
	OpenViBE::CIdentifier m_identifier;

	/** \brief Associated ogre object */
	COgreObject& m_rOgreObject;

	/** \brief Name of the node to animate */
	std::string m_sNodeName;

	/** \brief Pointer to the node to animate */
	Ogre::SceneNode* m_pSceneNode;
};

#endif // __OpenViBEKernel_Kernel_Player_COgreAnimator_H__
