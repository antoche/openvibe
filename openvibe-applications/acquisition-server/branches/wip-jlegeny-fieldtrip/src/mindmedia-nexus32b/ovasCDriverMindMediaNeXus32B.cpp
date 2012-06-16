#include "ovasCDriverMindMediaNeXus32B.h"
#include "../ovasCConfigurationBuilder.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <system/Time.h>
#include <system/Memory.h>

#if defined OVAS_OS_Windows

#include <cmath>
#include <windows.h>
#define boolean OpenViBE::boolean

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

namespace
{
	const uint32 gst_ui32InternaleBufferCount=32;
}

//___________________________________________________________________//
//                                                                   //

CDriverMindMediaNeXus32B::CDriverMindMediaNeXus32B(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_pSample(NULL)
	,m_ui32SampleIndex(0)
{
	m_oHeader.setSamplingFrequency(512);
	m_oHeader.setChannelCount(4);
}

void CDriverMindMediaNeXus32B::release(void)
{
	delete this;
}

const char* CDriverMindMediaNeXus32B::getName(void)
{
	return "MindMedia NeXus32B";
}

//___________________________________________________________________//
//                                                                   //

#define _MindMedia_NeXus32B_DLLFileName_ "NeXusDLL.dll"

typedef void (*NeXusDLL_ProcessData)(int iSampleCount, int iChannel, float* pSample);
typedef ::DWORD (*NeXusDLL_Init)(::NeXusDLL_ProcessData fpProcessData);
typedef ::DWORD (*NeXusDLL_Start)(::DWORD* dwSamplingRate);
typedef ::DWORD (*NeXusDLL_Stop)(void);

static HANDLE g_pMutex=NULL;
static HINSTANCE g_hNeXusDLLInstance=NULL;
static NeXusDLL_Init g_fpNeXusDLLInit=NULL;
static NeXusDLL_Start g_fpNeXusDLLStart=NULL;
static NeXusDLL_Stop g_fpNeXusDLLStop=NULL;

static OpenViBEAcquisitionServer::CDriverMindMediaNeXus32B* g_pDriver=NULL;

//___________________________________________________________________//
//                                                                   //

static void processData(int iSampleCount, int iChannel, float* pSample)
{
	if(g_pDriver)
	{
		g_pDriver->processData((uint32)iSampleCount, (uint32) iChannel, (float32*) pSample);
	}
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverMindMediaNeXus32B::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(m_rDriverContext.isConnected()) { return false; }

	g_hNeXusDLLInstance=::LoadLibrary(_MindMedia_NeXus32B_DLLFileName_);
	if(!g_hNeXusDLLInstance)
	{
		return false;
	}

	g_fpNeXusDLLInit=(NeXusDLL_Init)GetProcAddress(g_hNeXusDLLInstance, "InitNeXusDevice");
	g_fpNeXusDLLStart=(NeXusDLL_Start)GetProcAddress(g_hNeXusDLLInstance,"StartNeXusDevice");
	g_fpNeXusDLLStop=(NeXusDLL_Stop)GetProcAddress(g_hNeXusDLLInstance, "StopNeXusDevice");
	m_pSample=new float32[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock*gst_ui32InternaleBufferCount];

	if(!g_fpNeXusDLLInit || !g_fpNeXusDLLStart || !g_fpNeXusDLLStop || !m_pSample)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Error finding NeXus API functions / allocating sample buffer\n";

		::FreeLibrary(g_hNeXusDLLInstance);
		delete [] m_pSample;
		g_hNeXusDLLInstance=NULL;
		g_fpNeXusDLLInit=NULL;
		g_fpNeXusDLLStart=NULL;
		g_fpNeXusDLLStop=NULL;
		m_pSample=NULL;
		g_pMutex=NULL;
		return false;
	}

	g_pMutex=CreateMutex(
		NULL,  // default security attributes
		FALSE, // not initially owned
		NULL); // no name

	if(!g_pMutex)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not create synchronisation mutex\n";
		::FreeLibrary(g_hNeXusDLLInstance);
		delete [] m_pSample;
		g_hNeXusDLLInstance=NULL;
		g_fpNeXusDLLInit=NULL;
		g_fpNeXusDLLStart=NULL;
		g_fpNeXusDLLStop=NULL;
		m_pSample=NULL;
		g_pMutex=NULL;
		return false;
	}

	::DWORD l_dwError=g_fpNeXusDLLInit(::processData);
	if(l_dwError)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not initialize device with NeXus API\n";
		::FreeLibrary(g_hNeXusDLLInstance);
		delete [] m_pSample;
		CloseHandle(g_pMutex);
		g_hNeXusDLLInstance=NULL;
		g_fpNeXusDLLInit=NULL;
		g_fpNeXusDLLStart=NULL;
		g_fpNeXusDLLStop=NULL;
		m_pSample=NULL;
		g_pMutex=NULL;
		return false;
	}

	m_pCallback=&rCallback;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;
	m_ui32SampleIndex=0;
	g_pDriver=this;

	return true;
}

boolean CDriverMindMediaNeXus32B::start(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	::DWORD l_dwSamplingFrequency=::DWORD(m_oHeader.getSamplingFrequency());
	::DWORD l_dwError=g_fpNeXusDLLStart(&l_dwSamplingFrequency);
	if(l_dwError)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not start acquisition with NeXus API\n";
		return false;
	}

	return true;
}

boolean CDriverMindMediaNeXus32B::loop(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return true; }

	WaitForSingleObject(g_pMutex, INFINITE);
	if(m_ui32SampleIndex < m_ui32SampleCountPerSentBlock)
	{
		ReleaseMutex(g_pMutex);
		return true;
	}

	m_pCallback->setSamples(m_pSample);
	m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());
	System::Memory::copy(
		m_pSample,
		m_pSample+m_oHeader.getChannelCount()*m_ui32SampleCountPerSentBlock,
		m_oHeader.getChannelCount()*m_ui32SampleCountPerSentBlock*sizeof(float32));
	m_ui32SampleIndex-=m_ui32SampleCountPerSentBlock;
	ReleaseMutex(g_pMutex);

	return true;
}

boolean CDriverMindMediaNeXus32B::stop(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }

	::DWORD l_dwError=g_fpNeXusDLLStop();
	if(l_dwError)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not stop acquisition with NeXus API\n";
		return false;
	}
	return true;
}

boolean CDriverMindMediaNeXus32B::uninitialize(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	::FreeLibrary(g_hNeXusDLLInstance);
	delete [] m_pSample;
	CloseHandle(g_pMutex);
	m_pSample=NULL;
	m_pCallback=NULL;
	g_hNeXusDLLInstance=NULL;
	g_fpNeXusDLLInit=NULL;
	g_fpNeXusDLLStart=NULL;
	g_fpNeXusDLLStop=NULL;
	g_pDriver=NULL;
	g_pMutex=NULL;

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverMindMediaNeXus32B::isConfigurable(void)
{
	return true;
}

boolean CDriverMindMediaNeXus32B::configure(void)
{
	CConfigurationBuilder m_oConfiguration("../share/openvibe-applications/acquisition-server/interface-MindMedia-NeXus32B.ui");
	return m_oConfiguration.configure(m_oHeader);
}

void CDriverMindMediaNeXus32B::processData(
	uint32 ui32SampleCount,
	uint32 ui32Channel,
	float32* pSample)
{
	WaitForSingleObject(g_pMutex, INFINITE);

	if(m_ui32SampleIndex<m_ui32SampleCountPerSentBlock*gst_ui32InternaleBufferCount)
	{
		uint32 l_ui32BufferIndex=m_ui32SampleIndex/m_ui32SampleCountPerSentBlock;
		uint32 l_ui32SampleIndex=m_ui32SampleIndex%m_ui32SampleCountPerSentBlock;

		for(uint32 i=0; i<m_oHeader.getChannelCount(); i++)
		{
			m_pSample[
				l_ui32BufferIndex*m_ui32SampleCountPerSentBlock*m_oHeader.getChannelCount()+
				i*m_ui32SampleCountPerSentBlock+l_ui32SampleIndex]=pSample[i];
		}

		m_ui32SampleIndex++; // Please don't overflow :o)
	}

	ReleaseMutex(g_pMutex);
}

#endif // defined OVAS_OS_Windows
