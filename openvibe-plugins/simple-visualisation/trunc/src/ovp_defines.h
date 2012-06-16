#ifndef __OpenViBEPlugins_Defines_H__
#define __OpenViBEPlugins_Defines_H__

#define OVP_TypeId_SphericalLinearInterpolationType                   OpenViBE::CIdentifier(0x44B76D9E, 0x618229BC)
#define OVP_TypeId_SphericalLinearInterpolationType_Spline            1
#define OVP_TypeId_SphericalLinearInterpolationType_Laplacian         2

#define OVP_TypeId_SignalDisplayMode                                  OpenViBE::CIdentifier(0x5DE046A6, 0x086340AA)
#define OVP_TypeId_SignalDisplayMode_Scroll                           OpenViBE::CIdentifier(0x0A4B5537, 0x3F791851)
#define OVP_TypeId_SignalDisplayMode_Scan                             OpenViBE::CIdentifier(0x1CE12013, 0x69464506)

#define OVP_ClassId_BoxAlgorithm_LevelMeasure                         OpenViBE::CIdentifier(0x657138E4, 0x46D6586F)
#define OVP_ClassId_BoxAlgorithm_LevelMeasureDesc                     OpenViBE::CIdentifier(0x4D061428, 0x11B02233)
#define OVP_ClassId_Algorithm_LevelMeasure                            OpenViBE::CIdentifier(0x63C71764, 0x34A9717F)
#define OVP_ClassId_Algorithm_LevelMeasureDesc                        OpenViBE::CIdentifier(0x3EB6754F, 0x22FB1722)

#define OVP_Algorithm_LevelMeasure_InputParameterId_Matrix            OpenViBE::CIdentifier(0x59430053, 0x67C23A83)
#define OVP_Algorithm_LevelMeasure_OutputParameterId_MainWidget       OpenViBE::CIdentifier(0x101C4641, 0x466C71E3)
#define OVP_Algorithm_LevelMeasure_OutputParameterId_ToolbarWidget    OpenViBE::CIdentifier(0x14905FFC, 0x6FE425B2)
#define OVP_Algorithm_LevelMeasure_InputTriggerId_Reset               OpenViBE::CIdentifier(0x3EAF36C5, 0x74490C56)
#define OVP_Algorithm_LevelMeasure_InputTriggerId_Refresh             OpenViBE::CIdentifier(0x71356FE4, 0x3E8F62DC)
#define OVP_Algorithm_LevelMeasure_OutputTriggerId_Refreshed          OpenViBE::CIdentifier(0x3C3C1B06, 0x360305D9)

#define OVP_ClassId_Algorithm_SphericalSplineInterpolation            OpenViBE::CIdentifier(0x4F112803, 0x661D4029)
#define OVP_ClassId_Algorithm_SphericalSplineInterpolationDesc        OpenViBE::CIdentifier(0x00D67A20, 0x3D3D4729)

#define OVP_Algorithm_SphericalSplineInterpolation_InputParameterId_SplineOrder              OpenViBE::CIdentifier(0x3B8200F6, 0x205162C7)
#define OVP_Algorithm_SphericalSplineInterpolation_InputParameterId_ControlPointsCount       OpenViBE::CIdentifier(0x2ABF11FC, 0x174A2CFE)
#define OVP_Algorithm_SphericalSplineInterpolation_InputParameterId_ControlPointsCoordinates OpenViBE::CIdentifier(0x36F743FE, 0x37897AB9)
#define OVP_Algorithm_SphericalSplineInterpolation_InputParameterId_ControlPointsValues      OpenViBE::CIdentifier(0x4EA55599, 0x670274A7)
#define OVP_Algorithm_SphericalSplineInterpolation_InputParameterId_SamplePointsCoordinates  OpenViBE::CIdentifier(0x280A531D, 0x339C18AA)
#define OVP_Algorithm_SphericalSplineInterpolation_OutputParameterId_SamplePointsValues      OpenViBE::CIdentifier(0x12D0319C, 0x51ED4D8B)
#define OVP_Algorithm_SphericalSplineInterpolation_OutputParameterId_MinSamplePointValue     OpenViBE::CIdentifier(0x0CEE2041, 0x79455EED)
#define OVP_Algorithm_SphericalSplineInterpolation_OutputParameterId_MaxSamplePointValue     OpenViBE::CIdentifier(0x1ECB03E3, 0x40EF757F)
#define OVP_Algorithm_SphericalSplineInterpolation_InputTriggerId_PrecomputeTables           OpenViBE::CIdentifier(0x42A650DA, 0x62B35F76)
#define OVP_Algorithm_SphericalSplineInterpolation_InputTriggerId_ComputeSplineCoefs         OpenViBE::CIdentifier(0x5B353712, 0x069F3D3B)
#define OVP_Algorithm_SphericalSplineInterpolation_InputTriggerId_ComputeLaplacianCoefs      OpenViBE::CIdentifier(0x7D8C545E, 0x7C086660)
#define OVP_Algorithm_SphericalSplineInterpolation_InputTriggerId_InterpolateSpline          OpenViBE::CIdentifier(0x1241610E, 0x03CB1AD9)
#define OVP_Algorithm_SphericalSplineInterpolation_InputTriggerId_InterpolateLaplacian       OpenViBE::CIdentifier(0x11CE0AC3, 0x0FD85469)
#define OVP_Algorithm_SphericalSplineInterpolation_OutputTriggerId_Error                     OpenViBE::CIdentifier(0x08CB0679, 0x3A6F3C3A)

//___________________________________________________________________//
//                                                                   //
// Plugin Object Class Identifiers                        //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_SignalDisplay                                        OpenViBE::CIdentifier(0x0055BE5F, 0x087BDD12)
#define OVP_ClassId_GrazVisualization                                    OpenViBE::CIdentifier(0x00DD290D, 0x5F142820)
#define OVP_ClassId_PowerSpectrumDisplay                                 OpenViBE::CIdentifier(0x004C0EA4, 0x713EC6D9)
#define OVP_ClassId_TopographicMap2DDisplay                              OpenViBE::CIdentifier(0x0B104632, 0x451C265F)
#define OVP_ClassId_Simple3DDisplay                                      OpenViBE::CIdentifier(0x31A00483, 0x35924E6B)
#define OVP_ClassId_TopographicMap3DDisplay                              OpenViBE::CIdentifier(0x36F95BE4, 0x0EF06290)
#define OVP_ClassId_VoxelDisplay                                         OpenViBE::CIdentifier(0x76E42EA2, 0x66FB5265)
#define OVP_ClassId_TimeFrequencyMapDisplay                              OpenViBE::CIdentifier(0x3AE63330, 0x76532117)
#define OVP_ClassId_BoxAlgorithm_P300IdentifierCardVisualisation         OpenViBE::CIdentifier(0x3AF7FF20, 0xA68745DB)

//___________________________________________________________________//
//                                                                   //
// Plugin Object Descriptor Class Identifiers                                   //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_SignalDisplayDesc                                    OpenViBE::CIdentifier(0x00C4F2D5, 0x58810276)
#define OVP_ClassId_GrazVisualizationDesc                                OpenViBE::CIdentifier(0x00F1955D, 0x38813A6A)
#define OVP_ClassId_PowerSpectrumDisplayDesc                             OpenViBE::CIdentifier(0x00116B40, 0x69E1B00D)
#define OVP_ClassId_TopographicMap2DDisplayDesc                          OpenViBE::CIdentifier(0x7154037A, 0x4BC52A9F)
#define OVP_ClassId_Simple3DDisplayDesc                                  OpenViBE::CIdentifier(0x443E145F, 0x77205DA0)
#define OVP_ClassId_TopographicMap3DDisplayDesc                          OpenViBE::CIdentifier(0x6AD52C48, 0x6E1C1746)
#define OVP_ClassId_VoxelDisplayDesc                                     OpenViBE::CIdentifier(0x79321659, 0x642D3D0C)
#define OVP_ClassId_TimeFrequencyMapDisplayDesc                          OpenViBE::CIdentifier(0x1BAE74F3, 0x20FB7C89)
#define OVP_ClassId_BoxAlgorithm_P300IdentifierCardVisualisationDesc     OpenViBE::CIdentifier(0x84F146EF, 0x4AA712A4)
//___________________________________________________________________//
//                                                                   //
// Gloabal defines                                                   //
//___________________________________________________________________//
//                                                                   //

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
 #include "ovp_global_defines.h"
#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

//___________________________________________________________________//
//                                                                   //
// Some enumerations                                                 //
//___________________________________________________________________//
//                                                                   //

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		enum EDisplayMode
		{
			//DisplayMode_Default,
			DisplayMode_ZoomIn,
			DisplayMode_ZoomOut,
			//DisplayMode_BestFit,
			DisplayMode_GlobalBestFit,
			//DisplayMode_Normal,
		};
	};
};

//___________________________________________________________________//
//                                                                   //
// Operating System identification                                   //
//___________________________________________________________________//
//                                                                   //

// #define OVP_OS_Linux
// #define OVP_OS_Windows
// #define OVP_OS_MacOS
// #define OVP_OS_

#if defined TARGET_OS_Windows
 #define OVP_OS_Windows
#elif defined TARGET_OS_Linux
 #define OVP_OS_Linux
#elif defined TARGET_OS_MacOS
 #define OVP_OS_MacOS
#else
 #warning "No target operating system defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Hardware Architecture identification                              //
//___________________________________________________________________//
//                                                                   //

// #define OVP_ARCHITECTURE_i386
// #define OVP_ARCHITECTURE_

#if defined TARGET_ARCHITECTURE_i386
 #define OVP_ARCHITECTURE_i386
#else
 #warning "No target architecture defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Compilator software identification                                //
//___________________________________________________________________//
//                                                                   //

// #define OVP_COMPILATOR_GCC
// #define OVP_COMPILATOR_VisualStudio
// #define OVP_COMPILATOR_

#if defined TARGET_COMPILATOR_GCC
 #define OVP_COMPILATOR_GCC
#elif defined TARGET_COMPILATOR_VisualStudio
 #define OVP_COMPILATOR_VisualStudio
#else
 #warning "No target compilator defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// API Definition                                                    //
//___________________________________________________________________//
//                                                                   //

// Taken from
// - http://people.redhat.com/drepper/dsohowto.pdf
// - http://www.nedprod.com/programs/gccvisibility.html
#if defined OVP_Shared
 #if defined OVP_OS_Windows
  #define OVP_API_Export __declspec(dllexport)
  #define OVP_API_Import __declspec(dllimport)
 #elif defined OVP_OS_Linux
  #define OVP_API_Export __attribute__((visibility("default")))
  #define OVP_API_Import __attribute__((visibility("default")))
 #else
  #define OVP_API_Export
  #define OVP_API_Import
 #endif
#else
 #define OVP_API_Export
 #define OVP_API_Import
#endif

#if defined OVP_Exports
 #define OVP_API OVP_API_Export
#else
 #define OVP_API OVP_API_Import
#endif

//___________________________________________________________________//
//                                                                   //
// NULL Definition                                                   //
//___________________________________________________________________//
//                                                                   //

#ifndef NULL
#define NULL 0
#endif

#endif // __OpenViBEPlugins_Defines_H__
