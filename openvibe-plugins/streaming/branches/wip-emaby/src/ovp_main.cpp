#include "ovp_defines.h"

#include "box-algorithms/streaming/ovpCBoxAlgorithmStreamedMatrixMultiplexer.h"
#include "box-algorithms/streaming/ovpCBoxAlgorithmSignalMerger.h"
#include "box-algorithms/streaming/ovpCBoxAlgorithmStreamedMatrixSwitch.h"
#include "box-algorithms/streaming/ovpCBoxAlgorithmStreamedMatrixMerger.h"


#include <vector>

OVP_Declare_Begin()
	OVP_Declare_New(OpenViBEPlugins::Streaming::CBoxAlgorithmStreamedMatrixMultiplexerDesc);
	OVP_Declare_New(OpenViBEPlugins::Streaming::CBoxAlgorithmSignalMergerDesc);
	OVP_Declare_New(OpenViBEPlugins::Streaming::CBoxAlgorithmStreamedMatrixSwitchDesc);
	OVP_Declare_New(OpenViBEPlugins::Streaming::CBoxAlgorithmStreamedMatrixMergerDesc);

OVP_Declare_End()
