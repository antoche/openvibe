#include <openvibe/ov_all.h>

#include "ovp_defines.h"

#include "ovpCBoxAlgorithmMatlabFilter.h"
#include "ovpCBoxAlgorithmMatlabScripting.h"

#include <vector>

OVP_Declare_Begin()

#if defined TARGET_HAS_ThirdPartyMatlab
	OVP_Declare_New(OpenViBEPlugins::Tools::CBoxAlgorithmMatlabFilterDesc);
	
	OVP_Declare_New(OpenViBEPlugins::Matlab::CBoxAlgorithmMatlabScriptingDesc);

#endif // TARGET_HAS_ThirdPartyMatlab

OVP_Declare_End()
