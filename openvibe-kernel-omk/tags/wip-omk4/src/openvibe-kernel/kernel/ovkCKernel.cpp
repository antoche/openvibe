#include "ovkCKernel.h"

#include <string>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

#if defined OVK_OS_Linux

	#include <signal.h>

	static void _openvibeKernelSignalHandler_(int iSignal)
	{
		throw;
	}

#endif

CKernel::CKernel(void)
	:m_oContext(*this)
{
#if defined OVK_OS_Linux

	signal(SIGILL,  _openvibeKernelSignalHandler_); // Illegal instruction
	signal(SIGFPE,  _openvibeKernelSignalHandler_); // Floadint point exception
	signal(SIGSEGV, _openvibeKernelSignalHandler_); // Invalid memory reference
	signal(SIGPIPE, _openvibeKernelSignalHandler_); // Broken pipe: write to pipe with no readers

	signal(SIGBUS,  _openvibeKernelSignalHandler_); // Bus error (bad memory access)
	signal(SIGSYS,  _openvibeKernelSignalHandler_); // Bad argument to routine (SVID)
	signal(SIGTRAP, _openvibeKernelSignalHandler_); // Trace/breakpoint trap

#endif
}

void CKernel::release(void)
{
	delete this;
}
