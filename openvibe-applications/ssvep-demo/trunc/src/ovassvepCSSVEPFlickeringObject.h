#ifndef __OpenViBEApplication_CSSVEPFlickeringObject_H__
#define __OpenViBEApplication_CSSVEPFlickeringObject_H__

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <Ogre.h>

namespace OpenViBESSVEP
{
	class CSSVEPFlickeringObject
	{
		public:
			CSSVEPFlickeringObject( Ogre::SceneNode* poObjectNode, OpenViBE::uint32 ui32LitFrames, OpenViBE::uint32 ui32DarkFrames );
			~CSSVEPFlickeringObject() {};

			virtual void setVisible( OpenViBE::boolean bVisibility );
			virtual void processFrame(  );

		protected:
			Ogre::SceneNode* m_poObjectNode;
			OpenViBE::uint32 m_ui32CurrentFrame;
			OpenViBE::uint32 m_ui32LitFrames;
			OpenViBE::uint32 m_ui32DarkFrames;

		private:

			OpenViBE::boolean m_bVisible;
	};
}


#endif // __OpenViBEApplication_CSSVEPFlickeringObject_H__
