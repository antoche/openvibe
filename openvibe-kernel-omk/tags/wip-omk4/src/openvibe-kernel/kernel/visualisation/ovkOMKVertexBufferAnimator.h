#ifndef __OVKOMKVERTEXBUFFERANIMATOR_H__
#define __OVKOMKVERTEXBUFFERANIMATOR_H__

#include "OMKNamespaceCompatibility.h"
#include "OMKOgreAnimatorT.h"
#if defined UPDATED_OMK
#include "OMKNodeMaterialInstance.h"
#endif
#include "ovkOMKVertexDataType.h"

namespace OMK
{
  namespace Vis
  {
    class OgreVis;

		class OMKVertexBufferAnimator : public OgreAnimatorT< Type::VertexDataType >
		{
			DECLARE_ANIMATOR_FACTORY( OMKVertexBufferAnimator ) ;
		private:
			///\brief Called by the partner to process the visualisation
			///
			/// Called by the public version if the value was changed.
			/// Must be overwritten by sons.
			virtual void selfProcessVis(const Type::VertexDataType& rVertexData) ;
			/// init, load param
			virtual void init(const OMKConfigurationParameterDescriptor& node);

		protected:
			Ogre::SceneNode* _anim;
#if defined UPDATED_OMK
			NodeMaterialInstance* _nmi;
#endif
			Ogre::Vector4 _color;
		};
  };// namespace Vis
};// namespace OMK

#endif // __OVKOMKVERTEXBUFFERANIMATOR_H__
