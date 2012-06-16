#ifndef __OpenViBE_GetCpuTime_H__
#define __OpenViBE_GetCpuTime_H__

#include "windows.h"
#include "ov_base.h"
#include "ovCString.h"

//___________________________________________________________________//
//                                                                   //
// Get Cpu Time                                                      //
//___________________________________________________________________//
//                                                                   //
namespace OpenViBE
{
    OV_API OpenViBE::boolean GetEnvVar( OpenViBE::CString& sVar, OpenViBE::CString& sValue );
    OV_API OpenViBE::boolean PutEnvVar( OpenViBE::CString& sVar, OpenViBE::CString& sValue );

    OV_API inline double GetCPUTimeInMilliseconds()
    {
        static bool bGotCPUFreq = false;
        static double timerFrequency = 1;
        if ( !bGotCPUFreq )
        {
            unsigned __int64 cpufreq = 1;
            QueryPerformanceFrequency((LARGE_INTEGER*)&cpufreq);
            timerFrequency = (1000.0/cpufreq);
        }
        unsigned __int64 curTime = 0;
        QueryPerformanceCounter((LARGE_INTEGER *)&curTime);
        return timerFrequency * curTime;
    };

}
#endif