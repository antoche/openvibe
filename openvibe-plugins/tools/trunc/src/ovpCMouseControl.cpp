#include "ovpCMouseControl.h"

#include <iostream>

#if defined OVP_OS_Linux
	#include <unistd.h>
#endif

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Tools;
using namespace OpenViBEToolkit;
using namespace std;

CMouseControl::CMouseControl(void)
	:m_pReader(NULL)
	,m_pStreamedMatrixReaderCallBack(NULL)
	,m_oStreamedMatrixReaderCallBackProxy(
		*this,
		&CMouseControl::setMatrixDimmensionCount,
		&CMouseControl::setMatrixDimmensionSize,
		&CMouseControl::setMatrixDimmensionLabel,
		&CMouseControl::setMatrixBuffer)
	,m_ui64StartTime(0)
	,m_ui64EndTime(0)
	,m_bError(false)
#if defined OVP_OS_Linux
	,m_pMainDisplay(NULL)
#endif
{
}

void CMouseControl::release(void)
{
	delete this;
}

boolean CMouseControl::initialize(void)
{
	m_pStreamedMatrixReaderCallBack = createBoxAlgorithmStreamedMatrixInputReaderCallback(m_oStreamedMatrixReaderCallBackProxy);
	m_pReader = EBML::createReader(*m_pStreamedMatrixReaderCallBack);

	m_pInputBuffer = NULL;

#if defined OVP_OS_Linux
#else
	getLogManager() << LogLevel_ImportantWarning << "This box algorithm is not yet implemented for your platform\n";
#endif

	return true;
}

boolean CMouseControl::uninitialize(void)
{
	//release the ebml reader
	releaseBoxAlgorithmStreamedMatrixInputReaderCallback(m_pStreamedMatrixReaderCallBack);
	m_pReader->release();
	m_pStreamedMatrixReaderCallBack = NULL;
	m_pReader=NULL;

	return true;
}

boolean CMouseControl::processInput(uint32 ui32InputIndex)
{
	if(m_bError)
	{
		return false;
	}

	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

void CMouseControl::setMatrixDimmensionCount(const uint32 ui32DimmensionCount)
{
}

void CMouseControl::setMatrixDimmensionSize(const uint32 ui32DimmensionIndex, const uint32 ui32DimmensionSize)
{
	if(ui32DimmensionSize != 1)
	{
		getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning <<"Error, dimension size isn't 1 for Amplitude input !\n";
		m_bError = true;
	}
}

void CMouseControl::setMatrixDimmensionLabel(const uint32 ui32DimmensionIndex, const uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel)
{
	/* nothing to do */
}

void CMouseControl::setMatrixBuffer(const float64* pBuffer)
{
	if(m_bError)
	{
		return;
	}

	m_pInputBuffer = pBuffer;
}

boolean CMouseControl::process()
{
	IBoxIO * l_pDynamicContext = getBoxAlgorithmContext()->getDynamicBoxContext();
	uint32 l_ui32InputChunkCount = l_pDynamicContext->getInputChunkCount(0);

	for(uint32 i = 0; i < l_ui32InputChunkCount; i++)
	{
		uint64 l_ui64ChunkSize = 0;
		const uint8 * l_pChunkBuffer = NULL;

		if(l_pDynamicContext->getInputChunk(0, i, m_ui64StartTime, m_ui64EndTime, l_ui64ChunkSize, l_pChunkBuffer))
		{
			m_pReader->processData(l_pChunkBuffer, l_ui64ChunkSize);
			if(m_pInputBuffer == NULL) //dealing with the signal header
			{
			}
			else
			{
#if defined OVP_OS_Linux
				m_pMainDisplay=::XOpenDisplay(NULL);
				if (!m_pMainDisplay)
				{
					getLogManager() << LogLevel_Error << "Impossible to open Display.\n";
					return false;
				}
				m_oRootWindow=DefaultRootWindow(m_pMainDisplay);  //all X11 screens
				::XSelectInput(m_pMainDisplay, m_oRootWindow, ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|OwnerGrabButtonMask);

				int32 l_i32yoffset = 0;
				int32 l_i32xoffset = (int32)(m_pInputBuffer[0]*100.0);

				getLogManager() << LogLevel_Debug << "l_ui32xoffset = " << l_i32xoffset << "\n";

				::XWarpPointer(m_pMainDisplay, m_oRootWindow, 0, 0, 0, 0, 0, l_i32xoffset, l_i32yoffset);
				::XCloseDisplay(m_pMainDisplay);
#endif
				// TODO
				// For windows use:
				// SetCursorPos(int x, int y)
			}
			l_pDynamicContext->markInputAsDeprecated(0, i);
		}
	}
	return true;
}
