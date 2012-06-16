#if defined TARGET_HAS_ThirdPartyPython
#include "ovpCBoxAlgorithmPython.h"

#ifdef OVP_OS_Windows
#include "windows.h"
#endif

class CPythonInitializer
{
public:
	CPythonInitializer(void);
	~CPythonInitializer(void);
	OpenViBE::boolean IsPythonAvailable(void);
private:
	//		PyThreadState *m_pMainPyThreadState;
	OpenViBE::boolean m_bPythonAvailable;
};

CPythonInitializer::CPythonInitializer(void)
{
	m_bPythonAvailable = false;
	//m_pMainPyThreadState = NULL;

#ifdef OVP_OS_Windows
		__try
		{
			if (!Py_IsInitialized())
			{
				Py_Initialize();
			}
			m_bPythonAvailable = true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
#else
		if (!Py_IsInitialized())
		{
			Py_Initialize();
		}
		m_bPythonAvailable = true;
#endif



}

CPythonInitializer::~CPythonInitializer(void)
{
	if (m_bPythonAvailable)
	{
		Py_Finalize();
	}
}

OpenViBE::boolean CPythonInitializer::IsPythonAvailable(void)
{
	return m_bPythonAvailable;
}


OVP_Declare_Begin();

static CPythonInitializer l_oPythonInitializer;
if (l_oPythonInitializer.IsPythonAvailable())
{
	OVP_Declare_New(OpenViBEPlugins::Python::CBoxAlgorithmPythonDesc);
}
OVP_Declare_End();

#endif // TARGET_HAS_ThirdPartyPython
