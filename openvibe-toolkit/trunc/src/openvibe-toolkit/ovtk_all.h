#ifndef __OpenViBEToolkit_All_H__
#define __OpenViBEToolkit_All_H__


#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
 #include "ovp_global_defines.h"
#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

//___________________________________________________________________//
//                                                                   //
// Basic includes                                                    //
//___________________________________________________________________//
//                                                                   //

#include "ovtk_defines.h"

//___________________________________________________________________//
//                                                                   //
// Tools                                                             //
//___________________________________________________________________//
//                                                                   //

#include "tools/ovtkColorGradient.h"
#include "tools/ovtkMatrix.h"
#include "tools/ovtkStimulationSet.h"
#include "tools/ovtkString.h"

//___________________________________________________________________//
//                                                                   //
// Codecs                                                            //
//___________________________________________________________________//
//                                                                   //

#include "codecs/ovtkTCodec.h"
#include "codecs/encoders/ovtkTEncoder.h"
#include "codecs/decoders/ovtkTDecoder.h"

#include "codecs/decoders/ovtkTAcquisitionDecoder.h"

#include "codecs/encoders/ovtkTStimulationStreamEncoder.h"
#include "codecs/decoders/ovtkTStimulationStreamDecoder.h"

#include "codecs/encoders/ovtkTStreamedMatrixEncoder.h"
#include "codecs/decoders/ovtkTStreamedMatrixDecoder.h"

#include "codecs/encoders/ovtkTSignalEncoder.h"
#include "codecs/decoders/ovtkTSignalDecoder.h"

#include "codecs/encoders/ovtkTChannelLocalisationEncoder.h"
#include "codecs/decoders/ovtkTChannelLocalisationDecoder.h"

#include "codecs/encoders/ovtkTExperimentInformationEncoder.h"
#include "codecs/decoders/ovtkTExperimentInformationDecoder.h"

#include "codecs/encoders/ovtkTFeatureVectorEncoder.h"
#include "codecs/decoders/ovtkTFeatureVectorDecoder.h"

#include "codecs/encoders/ovtkTSpectrumEncoder.h"
#include "codecs/decoders/ovtkTSpectrumDecoder.h"

//___________________________________________________________________//
//                                                                   //
//                                                                   //
//___________________________________________________________________//
//                                                                   //

#include "ovtkIVector.h"
#include "ovtkIFeatureVector.h"
#include "ovtkIFeatureVectorSet.h"

#include "box-algorithms/ovtkTBoxAlgorithm.h"

#if defined TARGET_HAS_EBML
#include "box-algorithms/ovtkTTrainingBoxAlgorithm.h"
#endif

#include "algorithms/ovtkTAlgorithm.h"
#include "algorithms/classification/ovtkCAlgorithmClassifier.h"

#include "algorithms/scenario-io/ovtkCAlgorithmScenarioImporter.h"
#include "algorithms/scenario-io/ovtkCAlgorithmScenarioExporter.h"

//___________________________________________________________________//
//                                                                   //
//                                                                   //
//___________________________________________________________________//
//                                                                   //

#if defined TARGET_HAS_EBML
#include "deprecated/reader/ovtkIBoxAlgorithmEBMLInputReaderCallback.h"
#include "deprecated/reader/ovtkIBoxAlgorithmStreamedMatrixInputReaderCallback.h"
#include "deprecated/reader/ovtkIBoxAlgorithmSignalInputReaderCallback.h"
#include "deprecated/reader/ovtkIBoxAlgorithmSpectrumInputReaderCallback.h"
#include "deprecated/reader/ovtkIBoxAlgorithmFeatureVectorInputReaderCallback.h"
#include "deprecated/reader/ovtkIBoxAlgorithmStimulationInputReaderCallback.h"
#include "deprecated/reader/ovtkIBoxAlgorithmExperimentInformationInputReaderCallback.h"
#endif

//___________________________________________________________________//
//                                                                   //
//                                                                   //
//___________________________________________________________________//
//                                                                   //

#if defined TARGET_HAS_EBML
#include "deprecated/writer/ovtkIBoxAlgorithmEBMLOutputWriter.h"
#include "deprecated/writer/ovtkIBoxAlgorithmStreamedMatrixOutputWriter.h"
#include "deprecated/writer/ovtkIBoxAlgorithmSignalOutputWriter.h"
#include "deprecated/writer/ovtkIBoxAlgorithmSpectrumOutputWriter.h"
#include "deprecated/writer/ovtkIBoxAlgorithmFeatureVectorOutputWriter.h"
#include "deprecated/writer/ovtkIBoxAlgorithmStimulationOutputWriter.h"
#include "deprecated/writer/ovtkIBoxAlgorithmExperimentInformationOutputWriter.h"
#endif

//___________________________________________________________________//
//                                                                   //
//                                                                   //
//___________________________________________________________________//
//                                                                   //

namespace OpenViBEToolkit
{
	OVTK_API OpenViBE::boolean initialize(const OpenViBE::Kernel::IKernelContext& rKernelContext);
	OVTK_API OpenViBE::boolean uninitialize(const OpenViBE::Kernel::IKernelContext& rKernelContext);
};

#endif // __OpenViBEToolkit_All_H__
