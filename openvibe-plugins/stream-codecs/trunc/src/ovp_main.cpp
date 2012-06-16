#include "algorithms/decoders/ovpCAcquisitionDecoder.h"
#include "algorithms/decoders/ovpCExperimentInformationDecoder.h"
#include "algorithms/decoders/ovpCChannelLocalisationDecoder.h"
#include "algorithms/decoders/ovpCFeatureVectorDecoder.h"
#include "algorithms/decoders/ovpCSignalDecoder.h"
#include "algorithms/decoders/ovpCSpectrumDecoder.h"
#include "algorithms/decoders/ovpCStimulationDecoder.h"
#include "algorithms/decoders/ovpCStreamedMatrixDecoder.h"
// #include "algorithms/decoders/ovpCMasterAcquisitionDecoder.h"

#include "algorithms/encoders/ovpCAcquisitionEncoder.h"
#include "algorithms/encoders/ovpCExperimentInformationEncoder.h"
#include "algorithms/encoders/ovpCChannelLocalisationEncoder.h"
#include "algorithms/encoders/ovpCFeatureVectorEncoder.h"
#include "algorithms/encoders/ovpCSignalEncoder.h"
#include "algorithms/encoders/ovpCSpectrumEncoder.h"
#include "algorithms/encoders/ovpCStimulationEncoder.h"
#include "algorithms/encoders/ovpCStreamedMatrixEncoder.h"
#include "algorithms/encoders/ovpCMasterAcquisitionEncoder.h"

#include "box-algorithms/ovpCDecoderAlgorithmTest.h"
#include "box-algorithms/ovpCEncoderAlgorithmTest.h"

OVP_Declare_Begin()
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CAcquisitionDecoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CExperimentInformationDecoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CChannelLocalisationDecoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CFeatureVectorDecoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CSignalDecoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CSpectrumDecoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CStimulationDecoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc)
	// OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CMasterAcquisitionDecoderDesc);

	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CAcquisitionEncoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CExperimentInformationEncoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CChannelLocalisationEncoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CFeatureVectorEncoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CSignalEncoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CSpectrumEncoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CStimulationEncoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CMasterAcquisitionEncoderDesc);

	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CDecoderAlgorithmTestDesc)
	OVP_Declare_New(OpenViBEPlugins::StreamCodecs::CEncoderAlgorithmTestDesc)
OVP_Declare_End()
