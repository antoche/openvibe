#include "ovp_defines.h"

#include "algorithms/ovpCAlgorithmSphericalSplineInterpolation.h"
#include "algorithms/simple-visualisation/ovpCAlgorithmLevelMeasure.h"

//Presentation
#include "ovpCGrazVisualization.h"
#include "ovpCBoxAlgorithmP300SpellerVisualisation.h"
#include "ovpCBoxAlgorithmP300MagicCardVisualisation.h"
#include "ovpCBoxAlgorithmP300IdentifierCardVisualisation.h"
#include "ovpCDisplayCueImage.h"

//2D plugins
#include "ovpCSignalDisplay.h"
#include "ovpCTimeFrequencyMapDisplay.h"
#include "ovpCPowerSpectrumDisplay.h"
#include "ovpCTopographicMap2DDisplay.h"
#include "box-algorithms/simple-visualisation/ovpCBoxAlgorithmLevelMeasure.h"
#include "box-algorithms/simple-visualisation/ovpCBoxAlgorithmClassifierAccuracyMeasure.h"
#include "ovpCBoxAlgorithmMatrixDisplay.h"
//3D plugins
#include "ovpCSimple3DDisplay.h"
#include "ovpCTopographicMap3DDisplay.h"
#include "ovpCVoxelDisplay.h"

OVP_Declare_Begin()

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_SphericalLinearInterpolationType, "Spherical linear interpolation type");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SphericalLinearInterpolationType, "Spline (potentials)", OVP_TypeId_SphericalLinearInterpolationType_Spline);
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SphericalLinearInterpolationType, "Spline laplacian (currents)",  OVP_TypeId_SphericalLinearInterpolationType_Laplacian);

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_SignalDisplayMode, "Signal display mode");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SignalDisplayMode, "Scroll", OVP_TypeId_SignalDisplayMode_Scroll.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SignalDisplayMode, "Scan",  OVP_TypeId_SignalDisplayMode_Scan.toUInteger());

	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CAlgorithmLevelMeasureDesc)
	OVP_Declare_New(OpenViBEPlugins::Test::CAlgorithmSphericalSplineInterpolationDesc)

	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CGrazVisualizationDesc)
	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CBoxAlgorithmP300SpellerVisualisationDesc)
	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CBoxAlgorithmP300MagicCardVisualisationDesc)
	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CDisplayCueImageDesc)

	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CSignalDisplayDesc)
	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CTimeFrequencyMapDisplayDesc)
	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CPowerSpectrumDisplayDesc)
	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CTopographicMap2DDisplayDesc)
	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CBoxAlgorithmLevelMeasureDesc)
	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CBoxAlgorithmClassifierAccuracyMeasureDesc)
	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CBoxAlgorithmMatrixDisplayDesc)

	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CSimple3DDisplayDesc)
	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CTopographicMap3DDisplayDesc)
	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CVoxelDisplayDesc)
	OVP_Declare_New(OpenViBEPlugins::SimpleVisualisation::CBoxAlgorithmP300IdentifierCardVisualisationDesc)

OVP_Declare_End()
