#include "ovasCDriverMindMediaNeXus32B.h"
#include "../ovasCConfigurationGlade.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <system/Time.h>
#include <system/Memory.h>

#include <cmath>

#if defined OVAS_OS_Windows
 #include <windows.h>
 #define boolean OpenViBE::boolean
#endif

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

#define OVAS_MaxSampleCountJitter 16

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

#if defined OVAS_OS_Windows

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

#endif

//___________________________________________________________________//
//                                                                   //

boolean CDriverMindMediaNeXus32B::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
#if defined OVAS_OS_Windows

	if(m_rDriverContext.isConnected()) { return false; }

	g_hNeXusDLLInstance=::LoadLibrary(_MindMedia_NeXus32B_DLLFileName_);
	if(!g_hNeXusDLLInstance)
	{
		return false;
	}

	g_fpNeXusDLLInit=(NeXusDLL_Init)GetProcAddress(g_hNeXusDLLInstance, "InitNeXusDevice");
	g_fpNeXusDLLStart=(NeXusDLL_Start)GetProcAddress(g_hNeXusDLLInstance,"StartNeXusDevice");
	g_fpNeXusDLLStop=(NeXusDLL_Stop)GetProcAddress(g_hNeXusDLLInstance, "StopNeXusDevice");
	m_pSample=new float32[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock*2];

	if(!g_fpNeXusDLLInit || !g_fpNeXusDLLStart || !g_fpNeXusDLLStop || !m_pSample)
	{
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

#else

	return false;

#endif
}

boolean CDriverMindMediaNeXus32B::start(void)
{
#if defined OVAS_OS_Windows

	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	::DWORD l_dwSamplingFrequency=::DWORD(m_oHeader.getSamplingFrequency());
	::DWORD l_dwError=g_fpNeXusDLLStart(&l_dwSamplingFrequency);
	if(l_dwError)
	{
		return false;
	}

	m_ui32StartTime=System::Time::getTime();
	m_ui64SampleCountTotal=0;
	m_ui64AutoAddedSampleCount=0;
	m_ui64AutoRemovedSampleCount=0;

	return true;

#else

	return false;

#endif
}

boolean CDriverMindMediaNeXus32B::loop(void)
{
#if defined OVAS_OS_Windows

	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return true; }

	uint32 l_ui32ElapsedTime=System::Time::getTime()-m_ui32StartTime;

#if 0
	static uint32 l_ui32LastElapsedTime=0;
	if(l_ui32ElapsedTime-l_ui32LastElapsedTime > 1000)
	{
		std::cout<<"Got more than a second time jump, welcome in the fourth dimension\n";
		std::cout<<"Last elapsed time was : "<<l_ui32LastElapsedTime<<"\n";
		std::cout<<"Current time is : "<<l_ui32ElapsedTime<<"\n";
	}
	l_ui32LastElapsedTime=l_ui32ElapsedTime;
#endif

	if(l_ui32ElapsedTime > (1000*m_ui64SampleCountTotal)/m_oHeader.getSamplingFrequency())
	{
		WaitForSingleObject(g_pMutex, INFINITE);

		uint32 l_ui32NextSampleIndex=0;

		if(m_ui32SampleIndex>m_ui32SampleCountPerSentBlock)
		{
			if(m_ui32SampleIndex-m_ui32SampleCountPerSentBlock>OVAS_MaxSampleCountJitter)
			{
				l_ui32NextSampleIndex=OVAS_MaxSampleCountJitter;
				m_ui64AutoRemovedSampleCount+=m_ui32SampleIndex-m_ui32SampleCountPerSentBlock-OVAS_MaxSampleCountJitter;
			}
			else
			{
				l_ui32NextSampleIndex=m_ui32SampleIndex-m_ui32SampleCountPerSentBlock;
			}
		}
		if(m_ui32SampleIndex<m_ui32SampleCountPerSentBlock)
		{
			m_ui64AutoAddedSampleCount+=m_ui32SampleCountPerSentBlock-m_ui32SampleIndex;

			for(uint32 i=0; i<m_oHeader.getChannelCount(); i++)
			{
				for(uint32 j=m_ui32SampleIndex>0?m_ui32SampleIndex:1; j<m_ui32SampleCountPerSentBlock; j++)
				{
					m_pSample[j+i*m_ui32SampleCountPerSentBlock]=m_pSample[j-1+i*m_ui32SampleCountPerSentBlock];
				}
			}
		}

#if 1
		static uint64 l_ui64AutoAddedSampleCount=0;
		static uint64 l_ui64AutoRemovedSampleCount=0;
		if(((l_ui64AutoAddedSampleCount>>5)-(m_ui64AutoAddedSampleCount>>5)!=0) || ((l_ui64AutoRemovedSampleCount>>5)-(m_ui64AutoRemovedSampleCount>>5)!=0))
		{
			m_rDriverContext.getLogManager() << LogLevel_Warning << "time:" << l_ui32ElapsedTime << "ms [" << m_ui64AutoAddedSampleCount << ":" << m_ui64AutoRemovedSampleCount << "] [added:removed] dummy samples so far...\n";

			l_ui64AutoAddedSampleCount=m_ui64AutoAddedSampleCount;
			l_ui64AutoRemovedSampleCount=m_ui64AutoRemovedSampleCount;
		}
#endif

		m_pCallback->setSamples(m_pSample);

		m_ui64SampleCountTotal+=m_ui32SampleCountPerSentBlock;

		System::Memory::copy(
			m_pSample,
			m_pSample+m_oHeader.getChannelCount()*m_ui32SampleCountPerSentBlock,
			m_oHeader.getChannelCount()*m_ui32SampleCountPerSentBlock*sizeof(float32));

		m_ui32SampleIndex=l_ui32NextSampleIndex;

		ReleaseMutex(g_pMutex);
	}

	return true;

#else

	return false;

#endif
}

boolean CDriverMindMediaNeXus32B::stop(void)
{
#if defined OVAS_OS_Windows

	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }

	::DWORD l_dwError=g_fpNeXusDLLStop();
	if(l_dwError)
	{
		return false;
	}
	return true;

#else

	return false;

#endif
}

boolean CDriverMindMediaNeXus32B::uninitialize(void)
{
#if defined OVAS_OS_Windows

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

#else

	return false;

#endif
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverMindMediaNeXus32B::isConfigurable(void)
{
#if defined OVAS_OS_Windows

	return true;

#else

	return false;

#endif
}

boolean CDriverMindMediaNeXus32B::configure(void)
{
#if defined OVAS_OS_Windows

	CConfigurationGlade m_oConfiguration("../share/openvibe-applications/acquisition-server/interface-MindMedia-NeXus32B.glade");
	return m_oConfiguration.configure(m_oHeader);

#else

	return false;

#endif
}

void CDriverMindMediaNeXus32B::processData(
	uint32 ui32SampleCount,
	uint32 ui32Channel,
	float32* pSample)
{
#if defined OVAS_OS_Windows

	WaitForSingleObject(g_pMutex, INFINITE);

	if(m_ui32SampleIndex<m_ui32SampleCountPerSentBlock*2)
	{
		uint32 l_ui32BufferIndex=m_ui32SampleIndex/m_ui32SampleCountPerSentBlock;
		uint32 l_ui32SampleIndex=m_ui32SampleIndex%m_ui32SampleCountPerSentBlock;

		for(uint32 i=0; i<m_oHeader.getChannelCount(); i++)
		{
			m_pSample[
				l_ui32BufferIndex*m_ui32SampleCountPerSentBlock*m_oHeader.getChannelCount()+
				i*m_ui32SampleCountPerSentBlock+l_ui32SampleIndex]=pSample[i];
		}
	}

	m_ui32SampleIndex++; // Please don't overflow :o)

	ReleaseMutex(g_pMutex);
#endif
}
