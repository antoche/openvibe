#include "ovCKernelLoader.h"

#if defined OV_OS_Linux
 #include <dlfcn.h>
#elif defined OV_OS_Windows
 #include <windows.h>
#else
#endif

#include <iostream>
#include <string>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

//___________________________________________________________________//
//                                                                   //

namespace OpenViBE
{
	class CKernelLoaderBase : public IKernelLoader
	{
	public:

		CKernelLoaderBase(void);

		virtual boolean initialize(void);
		virtual boolean getKernelDesc(IKernelDesc*& rpKernelDesc);
		virtual boolean uninitialize(void);
		virtual void release(void);

		_IsDerivedFromClass_Final_(IKernelLoader, OV_UndefinedIdentifier)

		virtual boolean isOpen(void)=0;

	protected:

		CString m_sFileName;
		boolean (*onInitializeCB)(void);
		boolean (*onGetKernelDescCB)(IKernelDesc*&);
		boolean (*onUninitializeCB)(void);
	};
};

//___________________________________________________________________//
//                                                                   //

#define boolean OpenViBE::boolean

//___________________________________________________________________//
//                                                                   //

CKernelLoaderBase::CKernelLoaderBase(void)
	:onInitializeCB(NULL)
	,onGetKernelDescCB(NULL)
	,onUninitializeCB(NULL)
{
}

boolean CKernelLoaderBase::initialize(void)
{
	if(!isOpen())
	{
		return false;
	}
	if(!onInitializeCB)
	{
		return true;
	}
	return onInitializeCB();
}

boolean CKernelLoaderBase::getKernelDesc(
	IKernelDesc*& rpKernelDesc)
{
	if(!isOpen())
	{
		return false;
	}
	if(!onGetKernelDescCB)
	{
		return false;
	}
	return onGetKernelDescCB(rpKernelDesc);
}

boolean CKernelLoaderBase::uninitialize(void)
{
	if(!isOpen())
	{
		return false;
	}
	if(!onUninitializeCB)
	{
		return true;
	}
	return onUninitializeCB();
}

void CKernelLoaderBase::release(void)
{
	delete this;
}

//___________________________________________________________________//
//                                                                   //

#if defined OV_OS_Linux

namespace OpenViBE
{
	class CKernelLoaderLinux : public CKernelLoaderBase
	{
	public:

		CKernelLoaderLinux(void);

		virtual boolean load(const CString& sFileName, CString* pError);
		virtual boolean unload(CString* pError);

	protected:

		virtual boolean isOpen(void);

		void* m_pFileHandle;
	};
};

#elif defined OV_OS_Windows

namespace OpenViBE
{
	class CKernelLoaderWindows : public CKernelLoaderBase
	{
	public:
		CKernelLoaderWindows(void);

		virtual boolean load(const CString& sFileName, CString* pError);
		virtual boolean unload(CString* pError);

	protected:

		virtual boolean isOpen(void);

		HMODULE m_pFileHandle;
	};
};

#else

#endif

//___________________________________________________________________//
//                                                                   //

#if defined OV_OS_Linux

CKernelLoaderLinux::CKernelLoaderLinux(void)
	:m_pFileHandle(NULL)
{
}

boolean CKernelLoaderLinux::load(
	const CString& sFileName,
	CString* pError)
{
	if(m_pFileHandle)
	{
		if(pError) *pError="kernel already loaded";
		return false;
	}

	// m_pFileHandle=dlopen(sFileName, RTLD_NOW|RTLD_LOCAL);
	m_pFileHandle=dlopen(sFileName, RTLD_LAZY|RTLD_GLOBAL);
	if(!m_pFileHandle)
	{
		if(pError) *pError=dlerror();
		return false;
	}

	onInitializeCB=(boolean (*)(void))dlsym(m_pFileHandle, "onInitialize");
	onUninitializeCB=(boolean (*)(void))dlsym(m_pFileHandle, "onUninitialize");
	onGetKernelDescCB=(boolean (*)(IKernelDesc*&))dlsym(m_pFileHandle, "onGetKernelDesc");
	if(!onGetKernelDescCB)
	{
		if(pError) *pError=dlerror();

		dlclose(m_pFileHandle);
		m_pFileHandle=NULL;
		onInitializeCB=NULL;
		onGetKernelDescCB=NULL;
		onUninitializeCB=NULL;
		return false;
	}
	return true;
}

boolean CKernelLoaderLinux::unload(
	CString* pError)
{
	if(!m_pFileHandle)
	{
		if(pError) *pError="no kernel currently loaded";
		return false;
	}
	dlclose(m_pFileHandle);
	m_pFileHandle=NULL;
	onInitializeCB=NULL;
	onGetKernelDescCB=NULL;
	onUninitializeCB=NULL;
	return true;
}

boolean CKernelLoaderLinux::isOpen(void)
{
	return m_pFileHandle!=NULL;
}

#elif defined OV_OS_Windows

CKernelLoaderWindows::CKernelLoaderWindows(void)
	:m_pFileHandle(NULL)
{
}

boolean CKernelLoaderWindows::load(
	const CString& sFileName,
	CString* pError)
{
	if(m_pFileHandle)
	{
		if(pError) *pError="kernel already loaded";
		return false;
	}

	m_pFileHandle=LoadLibrary(sFileName);
	if(!m_pFileHandle)
	{
		if(pError)
		{
			LPVOID l_pMessageBuffer=NULL;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER|
				FORMAT_MESSAGE_FROM_SYSTEM|
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				0, // Default language
				(LPTSTR)&l_pMessageBuffer,
				0,
				NULL);
			*pError=(char*)l_pMessageBuffer;
			LocalFree(l_pMessageBuffer);
		}
		return false;
	}

	onInitializeCB=(boolean (*)(void))GetProcAddress(m_pFileHandle, "onInitialize");
	onUninitializeCB=(boolean (*)(void))GetProcAddress(m_pFileHandle, "onUninitialize");
	onGetKernelDescCB=(boolean (*)(IKernelDesc*&))GetProcAddress(m_pFileHandle, "onGetKernelDesc");
	if(!onGetKernelDescCB)
	{
		if(pError)
		{
			LPVOID l_pMessageBuffer=NULL;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER|
				FORMAT_MESSAGE_FROM_SYSTEM|
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				0, // Default language
				(LPTSTR)&l_pMessageBuffer,
				0,
				NULL);
			*pError=(char*)l_pMessageBuffer;
			LocalFree(l_pMessageBuffer);
		}

		FreeLibrary(m_pFileHandle);
		m_pFileHandle=NULL;
		onInitializeCB=NULL;
		onGetKernelDescCB=NULL;
		onUninitializeCB=NULL;
		return false;
	}
	return true;
}

boolean CKernelLoaderWindows::unload(
	CString* pError)
{
	if(!m_pFileHandle)
	{
		if(pError) *pError="no kernel currently loaded";
		return false;
	}
	FreeLibrary(m_pFileHandle);
	m_pFileHandle=NULL;
	onInitializeCB=NULL;
	onGetKernelDescCB=NULL;
	onUninitializeCB=NULL;

	return true;
}

boolean CKernelLoaderWindows::isOpen(void)
{
	return m_pFileHandle!=NULL;
}

#else

#endif

//___________________________________________________________________//
//                                                                   //

CKernelLoader::CKernelLoader(void)
	:m_pKernelLoaderImpl(NULL)
{
#if defined OV_OS_Linux
	m_pKernelLoaderImpl=new CKernelLoaderLinux();
#elif defined OV_OS_Windows
	m_pKernelLoaderImpl=new CKernelLoaderWindows();
#else
#endif
}

CKernelLoader::~CKernelLoader(void)
{
	delete m_pKernelLoaderImpl;
}

boolean CKernelLoader::load(const CString& sFileName, CString* pError)
{
	return m_pKernelLoaderImpl?m_pKernelLoaderImpl->load(sFileName, pError):false;
}

boolean CKernelLoader::unload(CString* pError)
{
	return m_pKernelLoaderImpl?m_pKernelLoaderImpl->unload(pError):false;
}

boolean CKernelLoader::initialize(void)
{
	return m_pKernelLoaderImpl?m_pKernelLoaderImpl->initialize():false;
}

boolean CKernelLoader::getKernelDesc(IKernelDesc*& rpKernelDesc)
{
	return m_pKernelLoaderImpl?m_pKernelLoaderImpl->getKernelDesc(rpKernelDesc):false;
}

boolean CKernelLoader::uninitialize(void)
{
	return m_pKernelLoaderImpl?m_pKernelLoaderImpl->uninitialize():false;
}
