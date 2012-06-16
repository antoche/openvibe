#include "OgreRoot.h"
#include "ovkCOgreFrameListener.h"
#include "OgreRenderWindow.h"
#include "OgreRenderSystem.h"
#include "OgreStringConverter.h"

using namespace std;
using namespace Ogre;

COgreFrameListener::COgreFrameListener(RenderWindow* win) :
m_pRenderWindow(win),
m_pDebugOverlay(0)
{
	//FIXME : the overlay should be cloned for each window, as opposed to shared betwen windows!!
	m_pDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");

	//Register as a Window listener
	//WindowEventUtilities::addWindowEventListener(m_pRenderWindow, this);
}

COgreFrameListener::~COgreFrameListener()
{
	//Remove ourself as a Window listener
	//WindowEventUtilities::removeWindowEventListener(m_pRenderWindow, this);
}

void COgreFrameListener::showDebugOverlay(bool bShow)
{
	if (m_pDebugOverlay)
	{
		if(bShow == true)
		{
			m_pDebugOverlay->show();
		}
		else
		{
			m_pDebugOverlay->hide();
		}
	}
}

bool COgreFrameListener::frameEnded(const FrameEvent& evt)
{
	updateStats();
	return true;
}

void COgreFrameListener::updateStats(void)
{
	static String currFps = "Current FPS: ";
	static String avgFps = "Average FPS: ";
	static String bestFps = "Best FPS: ";
	static String worstFps = "Worst FPS: ";
	static String tris = "Triangle Count: ";
	static String batches = "Batch Count: ";

	// update stats when necessary
	try {
		OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
		OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
		OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
		OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

		const RenderTarget::FrameStats& stats = m_pRenderWindow->getStatistics();
		guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
		guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
		guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
			+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
		guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
			+" "+StringConverter::toString(stats.worstFrameTime)+" ms");

		OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

		OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
		guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));
	}
	catch(...) { /* ignore */ }
}
