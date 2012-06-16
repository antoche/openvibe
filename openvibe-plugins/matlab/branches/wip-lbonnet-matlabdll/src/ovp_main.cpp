#include <openvibe/ov_all.h>

#include "ovp_defines.h"

#if defined TARGET_HAS_ThirdPartyMatlab
#include "box-algorithms/tools/ovpCBoxAlgorithmMatlabFilter.h"
#include "box-algorithms/tools/ovpCBoxAlgorithmMatlabDll.h"
#endif // TARGET_HAS_ThirdPartyMatlab

#include <vector>

OVP_Declare_Begin()

#if defined TARGET_HAS_ThirdPartyMatlab
	OVP_Declare_New(OpenViBEPlugins::Tools::CBoxAlgorithmMatlabFilterDesc);
	OVP_Declare_New(OpenViBEPlugins::Tools::CBoxAlgorithmMatlabDllDesc);
#endif // TARGET_HAS_ThirdPartyMatlab

OVP_Declare_End()
