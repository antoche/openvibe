#include "ovp_defines.h"

#include "box-algorithms/tools/ovpCBoxAlgorithmEBMLStreamSpy.h"
#include "box-algorithms/tools/ovpCBoxAlgorithmStimulationListener.h"

#include "ovpCMouseControl.h"
// #include "ovpCBoxAlgorithmLatencyEvaluation.h"
#include "box-algorithms/tools/ovpCBoxAlgorithmMatrixValidityChecker.h"

OVP_Declare_Begin();
	OVP_Declare_New(OpenViBEPlugins::Tools::CMouseControlDesc);
	OVP_Declare_New(OpenViBEPlugins::Tools::CBoxAlgorithmStimulationListenerDesc);
	OVP_Declare_New(OpenViBEPlugins::Tools::CBoxAlgorithmEBMLStreamSpyDesc);
// 	OVP_Declare_New(OpenViBEPlugins::Tools::CBoxAlgorithmLatencyEvaluationDesc);
	OVP_Declare_New(OpenViBEPlugins::Tools::CBoxAlgorithmMatrixValidityCheckerDesc);
OVP_Declare_End();
