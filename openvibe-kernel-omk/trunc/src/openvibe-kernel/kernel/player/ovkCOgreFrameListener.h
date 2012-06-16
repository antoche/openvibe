#ifndef __OpenViBEKernel_Kernel_Player_COgreFrameListener_H__
#define __OpenViBEKernel_Kernel_Player_COgreFrameListener_H__

#include "Ogre.h"
#include "OgreStringConverter.h"
#include "OgreException.h"

class COgreFrameListener: public Ogre::FrameListener//, public Ogre::WindowEventListener
{
public:
	/**
	 * \brief Constructor
	 * \param pRenderWindow pointer to window to which to add a frame listener
	 */
	COgreFrameListener(
		Ogre::RenderWindow* pRenderWindow);

	/**
	 * \brief Destructor
	 */
	virtual ~COgreFrameListener();

	/**
	 * \brief Specifies whether overlay should be displayed or not
	 * \param bShow Flag stating whether overlay should be displayed
	 */
	void showDebugOverlay(
		bool bShow);

	/**
	 * \brief callback called once a frame is finished rendering
	 * \return True (so as to continue with the next frame)
	 */
	virtual bool frameEnded(
		const Ogre::FrameEvent& rFrameEvent);

protected:
	/**
	 * \brief Updates overlay with latest rendering statistics
	 */
	void updateStats(void);

protected:
	//! RenderWindow to which a debug overlay is attached
	Ogre::RenderWindow* m_pRenderWindow;
	//! Debug overlay attached to render window
	Ogre::Overlay* m_pDebugOverlay;
};

#endif //__OpenViBEKernel_Kernel_Player_COgreFrameListener_H__
