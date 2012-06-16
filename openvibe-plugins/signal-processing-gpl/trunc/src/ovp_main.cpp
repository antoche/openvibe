#include "ovp_defines.h"

#include "ovpCWindowingFunctions.h"

#include "ovpCSpectralAnalysisCospectra.h"

#include "ovpCFastICA.h"

#include "ovpCSpectralAnalysis.h"

#include "ovpCTemporalFilter.h"

#include "algorithms/filter/ovpCComputeTemporalFilterCoefficients.h"
#include "algorithms/filter/ovpCApplyTemporalFilter.h"
#include "box-algorithms/filter/ovpCTemporalFilterBoxAlgorithm.h"
#include "box-algorithms/filter/ovpCBoxAlgorithmXDAWNSpatialFilterTrainer.h"
#include "box-algorithms/filter/ovpCBoxAlgorithmCSPSpatialFilterTrainer.h"

#include "algorithms/basic/ovpCDownsampling.h"
#include "box-algorithms/basic/ovpCDownsamplingBoxAlgorithm.h"

#include "box-algorithms/basic/ovpCBoxAlgorithmIFFTbox.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmEnvelope.h"

#include "algorithms/classif/ovpCComputeFisherLdaFunction.h"
#include "algorithms/classif/ovpCApplyFisherLdaFunction.h"
#include "algorithms/classif/ovpCFeatureExtractionLda.h"
#include "box-algorithms/classif/ovpCLDABoxAlgorithm.h"

#include "algorithms/basic/ovpCDetectingMinMax.h"
#include "box-algorithms/basic/ovpCDetectingMinMaxBoxAlgorithm.h"

#include "algorithms/classif/ovpCNaiveBayesApplyFunction.h"
#include "box-algorithms/classif/ovpCNaiveBayesApplyBoxAlgorithm.h"



OVP_Declare_Begin();
	rPluginModuleContext.getTypeManager().registerBitMaskType (OVP_TypeId_SpectralComponent, "Spectral component");
	rPluginModuleContext.getTypeManager().registerBitMaskEntry(OVP_TypeId_SpectralComponent, "Amplitude",      OVP_TypeId_SpectralComponent_Amplitude.toUInteger());
	rPluginModuleContext.getTypeManager().registerBitMaskEntry(OVP_TypeId_SpectralComponent, "Phase",          OVP_TypeId_SpectralComponent_Phase.toUInteger());
	rPluginModuleContext.getTypeManager().registerBitMaskEntry(OVP_TypeId_SpectralComponent, "Real part",      OVP_TypeId_SpectralComponent_RealPart.toUInteger());
	rPluginModuleContext.getTypeManager().registerBitMaskEntry(OVP_TypeId_SpectralComponent, "Imaginary part", OVP_TypeId_SpectralComponent_ImaginaryPart.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_FilterMethod, "Filter method");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterMethod, "Butterworth", OVP_TypeId_FilterMethod_Butterworth.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterMethod, "Chebychev",   OVP_TypeId_FilterMethod_Chebychev.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterMethod, "Yule-Walker", OVP_TypeId_FilterMethod_YuleWalker.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_FilterType, "Filter type");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "Low pass",  OVP_TypeId_FilterType_LowPass.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "Band pass", OVP_TypeId_FilterType_BandPass.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "High pass", OVP_TypeId_FilterType_HighPass.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "Band stop", OVP_TypeId_FilterType_BandStop.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_WindowMethod, "Window method");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Hamming",     OVP_TypeId_WindowMethod_Hamming.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Hanning",     OVP_TypeId_WindowMethod_Hanning.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Hann",        OVP_TypeId_WindowMethod_Hann.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Blackman",    OVP_TypeId_WindowMethod_Blackman.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Triangular",  OVP_TypeId_WindowMethod_Triangular.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Square root", OVP_TypeId_WindowMethod_SquareRoot.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_FrequencyCutOffRatio, "Frequency cut off ratio");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FrequencyCutOffRatio, "1/4", OVP_TypeId_FrequencyCutOffRatio_14.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FrequencyCutOffRatio, "1/3", OVP_TypeId_FrequencyCutOffRatio_13.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FrequencyCutOffRatio, "1/2", OVP_TypeId_FrequencyCutOffRatio_12.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_MinMax, "Min/Max");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_MinMax, "Min", OVP_TypeId_MinMax_Min.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_MinMax, "Max", OVP_TypeId_MinMax_Max.toUInteger());

	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CWindowingFunctionsDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CSpectralAnalysisCospectraDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CFastICADesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CSpectralAnalysisDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CTemporalFilterDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CBoxAlgorithmXDAWNSpatialFilterTrainerDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CBoxAlgorithmCSPSpatialFilterTrainerDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CTemporalFilterBoxAlgorithmDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CComputeTemporalFilterCoefficientsDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CApplyTemporalFilterDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CDownsamplingDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CDownsamplingBoxAlgorithmDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CComputeFisherLdaFunctionDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CApplyFisherLdaFunctionDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CFeatureExtractionLdaDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CLDABoxAlgorithmDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CDetectingMinMaxDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CDetectingMinMaxBoxAlgorithmDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CNaiveBayesApplyFunctionDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingGpl::CNaiveBayesApplyBoxAlgorithmDesc);
	
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingBasic::CBoxAlgorithmIFFTboxDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessingBasic::CBoxAlgorithmEnvelopeDesc);
	
	OVP_Declare_End();
