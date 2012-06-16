#include "Ogre.h"
#include "ovkCMaterial.h"
#include "ovkCOgreAnimator.h"

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

COgreAnimator::COgreAnimator(const IKernelContext& rKernelContext, COgreObject& rOgreObject, const CIdentifier& rIdentifier) :
	m_rKernelContext(rKernelContext),
	m_rOgreObject(rOgreObject),
	m_pSceneNode(NULL)
{
}

COgreAnimator::~COgreAnimator()
{
}

OpenViBE::boolean COgreAnimator::attachToNode(const std::string& rNodeName)
{
	m_sNodeName = rNodeName;

	if(rNodeName.empty())
	{
		m_pSceneNode = &m_rOgreObject.getSceneNode();
	}
	else
	{
		m_pSceneNode = dynamic_cast<Ogre::SceneNode*>(findNode(m_rOgreObject.getSceneNode(), m_sNodeName));
		if(m_pSceneNode == NULL)
		{
			m_rKernelContext.getLogManager() << LogLevel_Trace << "Failed to initialize animator : couldn't find node " << rNodeName.c_str()
				<< " in hierarchy of object " << m_rOgreObject.getName().toASCIIString() << "\n";
			return false;
		}
	}
	return true;
}

Ogre::SceneNode& COgreAnimator::getSceneNode() const
{
	return *m_pSceneNode;
}

const std::string& COgreAnimator::getNodeName() const
{
	return m_sNodeName;
}

Ogre::Node* COgreAnimator::findNode(const Ogre::Node& rRootNode, const std::string& rNodeName)
{
	//if current node is the one looked for, return it
	if(getNameWithoutDotSceneDelimiter(rRootNode.getName()) == rNodeName)
	{
		return const_cast<Ogre::Node*>(&rRootNode);
	}

	//look among child nodes
	Ogre::Node* l_pNode = NULL;
	Ogre::Node::ConstChildNodeIterator ite(rRootNode.getChildIterator());
	for(unsigned short i=0; i<rRootNode.numChildren(); i++)
	{
 		l_pNode = findNode(*ite.getNext(), rNodeName);
		if(l_pNode != NULL)
			break;
	}

	return l_pNode;
}

std::string COgreAnimator::getNameWithoutDotSceneDelimiter(const std::string& rName)
{
	//remove delimiter added by the dot scene loader
  static const std::string s_delimiter("_#_");
  size_t delimiterPlace(rName.rfind(s_delimiter));
  std::string s(rName);
  if(delimiterPlace != std::string::npos)
  {
    s.erase(delimiterPlace, rName.size());
  }
  return s;
}
