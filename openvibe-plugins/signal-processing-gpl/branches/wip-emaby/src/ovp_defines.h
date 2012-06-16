
#ifndef __OpenViBEPlugins_Defines_H__
#define __OpenViBEPlugins_Defines_H__

// %s/OVP_\([a-zA-Z0-9]*\)_\([a-zA-Z0-9]*\)_\(In\|Out\)ParameterId/OVP_Algorithm_\1_\3putParameterId_\2/g
// %s/OVP_\([a-zA-Z0-9]*\)_\([a-zA-Z0-9]*\)_\(In\|Out\)TriggerId/OVP_Algorithm_\1_\3putTriggerId_\2/g

//___________________________________________________________________//
//                                                                   //
// Plugin Object Descriptor Class Identifiers                        //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_Algorithm_AcquisitionDecoderDesc                                    OpenViBE::CIdentifier(0xA01599B0, 0x7F51631A)
#define OVP_ClassId_Algorithm_EBMLBaseDecoderDesc                                       OpenViBE::CIdentifier(0x4F701AC9, 0xDFBE912E)
#define OVP_ClassId_Algorithm_ExperimentInformationDecoderDesc                          OpenViBE::CIdentifier(0x0F37CA61, 0x8A77F44E)
#define OVP_ClassId_Algorithm_FeatureVectorDecoderDesc                                  OpenViBE::CIdentifier(0xAB0AE561, 0xF181E34F)
#define OVP_ClassId_Algorithm_SignalDecoderDesc                                         OpenViBE::CIdentifier(0xF1547D89, 0x49FFD0C2)
#define OVP_ClassId_Algorithm_SpectrumDecoderDesc                                       OpenViBE::CIdentifier(0x54D18EE8, 0x5DBD913A)
#define OVP_ClassId_Algorithm_StimulationDecoderDesc                                    OpenViBE::CIdentifier(0x391A615B, 0x71CD888A)
#define OVP_ClassId_Algorithm_StreamedMatrixDecoderDesc                                 OpenViBE::CIdentifier(0x384529D5, 0xD8E0A728)

#define OVP_ClassId_Algorithm_EBMLBaseEncoderDesc                                       OpenViBE::CIdentifier(0x47A9E701, 0x7C57BF3C)
#define OVP_ClassId_Algorithm_ExperimentInformationEncoderDesc                          OpenViBE::CIdentifier(0x8CC2C754, 0x61665FDA)
#define OVP_ClassId_Algorithm_FeatureVectorEncoderDesc                                  OpenViBE::CIdentifier(0xC249527B, 0x89EE1996)
#define OVP_ClassId_Algorithm_SignalEncoderDesc                                         OpenViBE::CIdentifier(0x90AC1E0F, 0x01518200)
#define OVP_ClassId_Algorithm_SpectrumEncoderDesc                                       OpenViBE::CIdentifier(0xD6182973, 0x122CE114)
#define OVP_ClassId_Algorithm_StimulationEncoderDesc                                    OpenViBE::CIdentifier(0x9B994B50, 0x52C3F06A)
#define OVP_ClassId_Algorithm_StreamedMatrixEncoderDesc                                 OpenViBE::CIdentifier(0xEEEFE060, 0x646EE8AB)

#define OVP_ClassId_Algorithm_Desc                                                      OpenViBE::CIdentifier(0x8F323B37, 0xAD9C560F)

#define OVP_ClassId_Box_DecoderAlgorithmTestDesc                                        OpenViBE::CIdentifier(0xE5176EB9, 0xD6E47D7F)
#define OVP_ClassId_Box_EncoderAlgorithmTestDesc                                        OpenViBE::CIdentifier(0x95E27325, 0x6893A519)

#define OVP_ClassId_Box_Desc                                                            OpenViBE::CIdentifier(0xD318C958, 0xBCA9A176)

//___________________________________________________________________//
//                                                                   //
// Plugin Object Class Identifiers                                   //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_Algorithm_AcquisitionDecoder                                        OpenViBE::CIdentifier(0x1E0812B7, 0x3F686DD4)
#define OVP_ClassId_Algorithm_EBMLBaseDecoder                                           OpenViBE::CIdentifier(0xFD30C96D, 0x8245A8F8)
#define OVP_ClassId_Algorithm_ExperimentInformationDecoder                              OpenViBE::CIdentifier(0x6FA7D52B, 0x80E2ABD6)
#define OVP_ClassId_Algorithm_FeatureVectorDecoder                                      OpenViBE::CIdentifier(0xC2689ECC, 0x43B335C1)
#define OVP_ClassId_Algorithm_SignalDecoder                                             OpenViBE::CIdentifier(0x7237C149, 0x0CA66DA7)
#define OVP_ClassId_Algorithm_SpectrumDecoder                                           OpenViBE::CIdentifier(0x128202DB, 0x449FC7A6)
#define OVP_ClassId_Algorithm_StimulationDecoder                                        OpenViBE::CIdentifier(0xC8807F2B, 0x0813C5B1)
#define OVP_ClassId_Algorithm_StreamedMatrixDecoder                                     OpenViBE::CIdentifier(0x7359D0DB, 0x91784B21)

#define OVP_ClassId_Algorithm_EBMLBaseEncoder                                           OpenViBE::CIdentifier(0x4272C178, 0x3FE84927)
#define OVP_ClassId_Algorithm_ExperimentInformationEncoder                              OpenViBE::CIdentifier(0x56B354FE, 0xBF175468)
#define OVP_ClassId_Algorithm_FeatureVectorEncoder                                      OpenViBE::CIdentifier(0x7EBE049D, 0xF777A602)
#define OVP_ClassId_Algorithm_SignalEncoder                                             OpenViBE::CIdentifier(0xC488AD3C, 0xEB2E36BF)
#define OVP_ClassId_Algorithm_SpectrumEncoder                                           OpenViBE::CIdentifier(0xB3E252DB, 0xC3214498)
#define OVP_ClassId_Algorithm_StimulationEncoder                                        OpenViBE::CIdentifier(0x6E86F7D5, 0xA4668108)
#define OVP_ClassId_Algorithm_StreamedMatrixEncoder                                     OpenViBE::CIdentifier(0x5CB32C71, 0x576F00A6)

#define OVP_ClassId_                                                                    OpenViBE::CIdentifier(0xFAB0E905, 0x48AFBD19)

#define OVP_ClassId_Box_DecoderAlgorithmTest                                            OpenViBE::CIdentifier(0x3C2EF355, 0xFE495C3D)
#define OVP_ClassId_Box_EncoderAlgorithmTest                                            OpenViBE::CIdentifier(0x87D18C62, 0xF2DAF779)

#define OVP_ClassId_Box_                                                                OpenViBE::CIdentifier(0xFDB56F8A, 0xEE2596C6)

//___________________________________________________________________//
//                                                                   //
// Plugin Object Descriptor Class Identifiers                        //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_WindowingFunctionsDesc                                              OpenViBE::CIdentifier(0x602CF89F, 0x65BA6DA0)

#define OVP_ClassId_SpectralAnalysisCospectraDesc                                       OpenViBE::CIdentifier(0xBE7311CE, 0x99BB5123)

#define OVP_ClassId_FastICADesc                                                         OpenViBE::CIdentifier(0x00E9436C, 0x41C904CA)

#define OVP_ClassId_SpectralAnalysisDesc                                                OpenViBE::CIdentifier(0x0051E63C, 0x68E83AD1)

#define OVP_TypeId_FilterMethod                                                         OpenViBE::CIdentifier(0x2F2C606C, 0x8512ED68)
#define OVP_TypeId_FilterMethod_Butterworth                                             OpenViBE::CIdentifier(0x00E4EADE, 0x9586A50E)
#define OVP_TypeId_FilterMethod_Chebychev                                               OpenViBE::CIdentifier(0x0C3F5301, 0x7397A7FB)
#define OVP_TypeId_FilterMethod_YuleWalker                                              OpenViBE::CIdentifier(0x7BF59C86, 0xB6D459BA)

#define OVP_TypeId_FilterType                                                           OpenViBE::CIdentifier(0xFA20178E, 0x4CBA62E9)
#define OVP_TypeId_FilterType_LowPass                                                   OpenViBE::CIdentifier(0x0C33A568, 0xE2BD46D2)
#define OVP_TypeId_FilterType_BandPass                                                  OpenViBE::CIdentifier(0x19E89DDB, 0xD1FF7FF5)
#define OVP_TypeId_FilterType_HighPass                                                  OpenViBE::CIdentifier(0x2F677195, 0x9F9F15B9)
#define OVP_TypeId_FilterType_BandStop                                                  OpenViBE::CIdentifier(0x41C34DD7, 0xF70E4043)

#define OVP_TypeId_WindowMethod                                                         OpenViBE::CIdentifier(0x0A430FE4, 0x4F318280)
#define OVP_TypeId_WindowMethod_Hamming                                                 OpenViBE::CIdentifier(0x3A9FF7F1, 0x54E79D67)
#define OVP_TypeId_WindowMethod_Hanning                                                 OpenViBE::CIdentifier(0x660DA3E7, 0x7BD87719)
#define OVP_TypeId_WindowMethod_Hann                                                    OpenViBE::CIdentifier(0x9BBBDC65, 0xFDFEF7A4)
#define OVP_TypeId_WindowMethod_Blackman                                                OpenViBE::CIdentifier(0x9D5937A4, 0xE43A9E3D)
#define OVP_TypeId_WindowMethod_Triangular                                              OpenViBE::CIdentifier(0xE652A852, 0xFE3CBC46)
#define OVP_TypeId_WindowMethod_SquareRoot                                              OpenViBE::CIdentifier(0xFE80BE5C, 0xAE59A27A)

#define OVP_TypeId_FrequencyCutOffRatio                                                 OpenViBE::CIdentifier(0x709FC9DF, 0x30A2CB2A)
#define OVP_TypeId_FrequencyCutOffRatio_14                                              OpenViBE::CIdentifier(0x1647FA20, 0xF8312286)
#define OVP_TypeId_FrequencyCutOffRatio_13                                              OpenViBE::CIdentifier(0x228279C0, 0x434B9E77)
#define OVP_TypeId_FrequencyCutOffRatio_12                                              OpenViBE::CIdentifier(0x4D216BC9, 0xCA817C88)

#define OVP_TypeId_MinMax                                                               OpenViBE::CIdentifier(0x4263AC45, 0x0AF5E07E)
#define OVP_TypeId_MinMax_Min                                                           OpenViBE::CIdentifier(0x0DB9C7B9, 0x44C8C59D)
#define OVP_TypeId_MinMax_Max                                                           OpenViBE::CIdentifier(0x14711334, 0xE8F32071)

//___________________________________________________________________//
//                                                                   //
// Plugin Object Class Identifiers                                   //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_WindowingFunctions                                                  OpenViBE::CIdentifier(0x002034AE, 0x6509FD8F)

#define OVP_ClassId_SpectralAnalysisCospectra                                           OpenViBE::CIdentifier(0x0002F6AA, 0x418B260F)

#define OVP_ClassId_FastICA                                                             OpenViBE::CIdentifier(0x00649B6E, 0x6C88CD17)

#define OVP_ClassId_SpectralAnalysis                                                    OpenViBE::CIdentifier(0x84218FF8, 0xA87E7995)
#define OVP_TypeId_SpectralComponent                                                    OpenViBE::CIdentifier(0x764E148A, 0xC704D4F5)
#define OVP_TypeId_SpectralComponent_Amplitude                                          OpenViBE::CIdentifier(0x00000000, 0x00000001)
#define OVP_TypeId_SpectralComponent_Phase                                              OpenViBE::CIdentifier(0x00000000, 0x00000002)
#define OVP_TypeId_SpectralComponent_RealPart                                           OpenViBE::CIdentifier(0x00000000, 0x00000004)
#define OVP_TypeId_SpectralComponent_ImaginaryPart                                      OpenViBE::CIdentifier(0x00000000, 0x00000008)

#define OVP_ClassId_Box_TemporalFilterBoxAlgorithm                                      OpenViBE::CIdentifier(0xB4F9D042, 0x9D79F2E5)
#define OVP_ClassId_Box_TemporalFilterBoxAlgorithmDesc                                  OpenViBE::CIdentifier(0x7BF6BA62, 0xAF829A37)

#define OVP_ClassId_Algorithm_ComputeTemporalFilterCoefficients                         OpenViBE::CIdentifier(0x55BAD77B, 0x5D8563A7)
#define OVP_ClassId_Algorithm_ComputeTemporalFilterCoefficientsDesc                     OpenViBE::CIdentifier(0xD871BD98, 0x705ED068)
#define OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_SamplingFrequency  OpenViBE::CIdentifier(0x25A9A0FF, 0x168F1B50)
#define OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterMethod       OpenViBE::CIdentifier(0xCFB7CDC9, 0x3EFF788E)
#define OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterType         OpenViBE::CIdentifier(0x1B7BCB2C, 0xE235A6E7)
#define OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterOrder        OpenViBE::CIdentifier(0x8DA1E555, 0x17E17828)
#define OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_LowCutFrequency    OpenViBE::CIdentifier(0x3175B774, 0xA15AEEB2)
#define OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_HighCutFrequency   OpenViBE::CIdentifier(0xE36387B7, 0xFB766612)
#define OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_BandPassRipple     OpenViBE::CIdentifier(0xB1500ED4, 0x0E558759)
#define OVP_Algorithm_ComputeTemporalFilterCoefficients_OutputParameterId_Matrix            OpenViBE::CIdentifier(0xE5B2A753, 0x150500B4)
#define OVP_Algorithm_ComputeTemporalFilterCoefficients_InputTriggerId_Initialize           OpenViBE::CIdentifier(0x3D2CBA61, 0x3FCF0DAC)
#define OVP_Algorithm_ComputeTemporalFilterCoefficients_InputTriggerId_ComputeCoefficients  OpenViBE::CIdentifier(0x053A2C6E, 0x3A878825)

#define OVP_ClassId_Algorithm_ApplyTemporalFilter                                       OpenViBE::CIdentifier(0x916B558F, 0xE501A2F4)
#define OVP_ClassId_Algorithm_ApplyTemporalFilterDesc                                   OpenViBE::CIdentifier(0xCCBD104B, 0xAC1C7D3E)
#define OVP_Algorithm_ApplyTemporalFilter_InputParameterId_FilterCoefficientsMatrix     OpenViBE::CIdentifier(0xD316C4E7, 0xE4E89FD3)
#define OVP_Algorithm_ApplyTemporalFilter_InputParameterId_SignalMatrix                 OpenViBE::CIdentifier(0xD5339105, 0x1D1293F0)
#define OVP_Algorithm_ApplyTemporalFilter_OutputParameterId_FilteredSignalMatrix        OpenViBE::CIdentifier(0x463276D1, 0xEAEE8AAD)
#define OVP_Algorithm_ApplyTemporalFilter_InputTriggerId_Initialize                     OpenViBE::CIdentifier(0x3DAE69C7, 0x7CFCBE2C)
#define OVP_Algorithm_ApplyTemporalFilter_InputTriggerId_ApplyFilter                    OpenViBE::CIdentifier(0xBC1F5655, 0x9807B400)
#define OVP_Algorithm_ApplyTemporalFilter_InputTriggerId_ApplyFilterWithHistoric        OpenViBE::CIdentifier(0xB7B7D546, 0x6000FF51)

#define OVP_ClassId_TemporalFilter                                                      OpenViBE::CIdentifier(0x4DEB9943, 0x7E638DC1)
#define OVP_ClassId_TemporalFilterDesc                                                  OpenViBE::CIdentifier(0x9EB3386D, 0x492C30A2)

#define OVP_ClassId_Algorithm_Downsampling                                              OpenViBE::CIdentifier(0xBBBB4E18, 0x17695604)
#define OVP_ClassId_Algorithm_DownsamplingDesc                                          OpenViBE::CIdentifier(0xC08BA8C1, 0x3A3B6E26)
#define OVP_Algorithm_Downsampling_InputParameterId_SamplingFrequency                   OpenViBE::CIdentifier(0x7C510AFB, 0x4F2B9FB7)
#define OVP_Algorithm_Downsampling_InputParameterId_NewSamplingFrequency                OpenViBE::CIdentifier(0x8617E5FA, 0xC39CDBE7)
#define OVP_Algorithm_Downsampling_InputTriggerId_Initialize                            OpenViBE::CIdentifier(0x82D96F84, 0x9479A701)
#define OVP_Algorithm_Downsampling_InputTriggerId_Resample                              OpenViBE::CIdentifier(0x2A88AFF5, 0x79ECAEB3)
#define OVP_Algorithm_Downsampling_InputTriggerId_ResampleWithHistoric                              OpenViBE::CIdentifier(0xD5740B33, 0x3785C886)
#define OVP_Algorithm_Downsampling_InputParameterId_SignalMatrix                        OpenViBE::CIdentifier(0xBB09054A, 0xEF13B2C6)
#define OVP_Algorithm_Downsampling_OutputParameterId_SignalMatrix                       OpenViBE::CIdentifier(0x4B9BE135, 0x14C10757)

#define OVP_ClassId_Box_DownsamplingBoxAlgorithm                                        OpenViBE::CIdentifier(0x6755FD0F, 0xE4857EA8)
#define OVP_ClassId_Box_DownsamplingBoxAlgorithmDesc                                    OpenViBE::CIdentifier(0xC8A99636, 0x81EF1AAD)

#define OVP_ClassId_Algorithm_ComputeFisherLdaFunction                                  OpenViBE::CIdentifier(0x124AC7D5, 0x39B975E2)
#define OVP_ClassId_Algorithm_ComputeFisherLdaFunctionDesc                              OpenViBE::CIdentifier(0x0ECE9FF0, 0xBA40F306)
#define OVP_Algorithm_ComputeFisherLdaFunction_InputParameterId_MatrixFirstClass         OpenViBE::CIdentifier(0x639846F9, 0x6637445E)
#define OVP_Algorithm_ComputeFisherLdaFunction_InputParameterId_MatrixSecondClass        OpenViBE::CIdentifier(0xE438158E, 0xB31603F2)
#define OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalMean        OpenViBE::CIdentifier(0x8278CF64, 0x25804B86)
#define OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalCovariance  OpenViBE::CIdentifier(0x6027E2D7, 0x878B5931)
#define OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalProbability OpenViBE::CIdentifier(0xB568892E, 0x81020300)
#define OVP_Algorithm_ComputeFisherLdaFunction_InputTriggerId_Initialize                 OpenViBE::CIdentifier(0x0C320928, 0x6B2C5285)
#define OVP_Algorithm_ComputeFisherLdaFunction_InputTriggerId_ComputeFunction            OpenViBE::CIdentifier(0xCCD22C15, 0xB49F6AAD)

#define OVP_ClassId_Algorithm_ApplyFisherLdaFunction                                    OpenViBE::CIdentifier(0xFCA53C1A, 0xC4024D00)
#define OVP_ClassId_Algorithm_ApplyFisherLdaFunctionDesc                                OpenViBE::CIdentifier(0xF4D7A3FE, 0x52D8F1EB)
#define OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixSignal              OpenViBE::CIdentifier(0x505BB8BC, 0xDFFB05D8)
#define OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalMean          OpenViBE::CIdentifier(0xFD147AF9, 0xC8FEBF3C)
#define OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalCovariance    OpenViBE::CIdentifier(0xF07B6450, 0x8C00B24B)
#define OVP_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalProbability   OpenViBE::CIdentifier(0xE791F510, 0x6BD004E7)
#define OVP_Algorithm_ApplyFisherLdaFunction_OutputParameterId_FisherLdaDecision        OpenViBE::CIdentifier(0xE347EE5A, 0xA74645F1)
#define OVP_Algorithm_ApplyFisherLdaFunction_InputTriggerId_Initialize                  OpenViBE::CIdentifier(0xC84D2FF2, 0x66228A70)
#define OVP_Algorithm_ApplyFisherLdaFunction_InputTriggerId_ApplyFunction               OpenViBE::CIdentifier(0xA07276DB, 0x9E775318)

#define OVP_ClassId_Algorithm_FeatureExtractionLda                                      OpenViBE::CIdentifier(0xD1AA6C72, 0x0432E1C5)
#define OVP_ClassId_Algorithm_FeatureExtractionLdaDesc                                  OpenViBE::CIdentifier(0x2109D46A, 0x9A45E1DC)
#define OVP_Algorithm_FeatureExtractionLda_InputParameterId_EpochTable                  OpenViBE::CIdentifier(0xA7BE9AE6, 0xD09FAD58)
#define OVP_Algorithm_FeatureExtractionLda_InputParameterId_Class1SamplesNumber         OpenViBE::CIdentifier(0x7F99FBC6, 0x748B183B)
#define OVP_Algorithm_FeatureExtractionLda_OutputParameterId_MatrixFirstClass           OpenViBE::CIdentifier(0x76F84603, 0x1F5D7A10)
#define OVP_Algorithm_FeatureExtractionLda_OutputParameterId_MatrixSecondClass          OpenViBE::CIdentifier(0x0AF9008A, 0xE7A7B2B7)
#define OVP_Algorithm_FeatureExtractionLda_OutputParameterId_MatrixEpoch                OpenViBE::CIdentifier(0xD4B473F6, 0xD5626A12)
#define OVP_Algorithm_FeatureExtractionLda_InputTriggerId_Initialize                    OpenViBE::CIdentifier(0x06176684, 0x5FA2C7C7)
#define OVP_Algorithm_FeatureExtractionLda_InputTriggerId_ExtractFeature                OpenViBE::CIdentifier(0x3B505F2F, 0x024B2C52)

#define OVP_ClassId_Box_LDABoxAlgorithm                                                 OpenViBE::CIdentifier(0xF37C8DBC, 0x18FEA50E)
#define OVP_ClassId_Box_LDABoxAlgorithmDesc                                             OpenViBE::CIdentifier(0x89E0A991, 0x4E0680D5)
//#define OVP_Algorithm_LDABoxAlgorithm_InputParameterId_SignalMatrix                     OpenViBE::CIdentifier(0xEC68E737, 0x59E1B68B)

#define OVP_ClassId_Box_NaiveBayesApplyBoxAlgorithm                                                 OpenViBE::CIdentifier(0xCCE94653, 0x690907D9)
#define OVP_ClassId_Box_NaiveBayesApplyBoxAlgorithmDesc                                             OpenViBE::CIdentifier(0x7C0C9AFC, 0xC806CC84)
#define OVP_ClassId_Algorithm_NaiveBayesApplyFunction                                	OpenViBE::CIdentifier(0xF9C21D2C, 0x9E6ABE8E)
#define OVP_ClassId_Algorithm_NaiveBayesApplyDesc                                  	OpenViBE::CIdentifier(0xCDE5EFEB, 0x0689C756)
#define OVP_Algorithm_NaiveBayesApplyFunction_InputParameterId_MatrixSignal		OpenViBE::CIdentifier(0xF1D46621, 0x9CAE8931)
#define OVP_Algorithm_NaiveBayesApplyFunction_InputParameterId_MatrixMean		OpenViBE::CIdentifier(0xDD1479AC, 0xB2E13471)
#define OVP_Algorithm_NaiveBayesApplyFunction_InputParameterId_MatrixVariance		OpenViBE::CIdentifier(0xEB3CD401, 0x54E4692D)
#define OVP_Algorithm_NaiveBayesApplyFunction_InputParameterId_LogTerm			OpenViBE::CIdentifier(0xB10D8B3C, 0x2A1D3705)
#define OVP_Algorithm_NaiveBayesApplyFunction_OutputParameterId_MatrixClassFunctional	OpenViBE::CIdentifier(0x57A78D33, 0x724CE7F9)
#define OVP_Algorithm_NaiveBayesApplyFunction_InputTriggerId_Initialize			OpenViBE::CIdentifier(0x0B3F7D98, 0x70D9D6E6)
#define OVP_Algorithm_NaiveBayesApplyFunction_InputTriggerId_Apply			OpenViBE::CIdentifier(0x622D05B7, 0x002F601F)

#define OVP_ClassId_Algorithm_DetectingMinMax                                           OpenViBE::CIdentifier(0x46C14A64, 0xE00541DD)
#define OVP_ClassId_Algorithm_DetectingMinMaxDesc                                       OpenViBE::CIdentifier(0x5B194CDA, 0x54E6DEC7)
#define OVP_Algorithm_DetectingMinMax_InputParameterId_SignalMatrix                     OpenViBE::CIdentifier(0x9CA3B6BB, 0x6E24A3E3)
#define OVP_Algorithm_DetectingMinMax_InputParameterId_TimeWindowStart                  OpenViBE::CIdentifier(0xB3DED659, 0xD8A85CFA)
#define OVP_Algorithm_DetectingMinMax_InputParameterId_TimeWindowEnd                    OpenViBE::CIdentifier(0x9F55A091, 0xA042E9C0)
#define OVP_Algorithm_DetectingMinMax_InputParameterId_SamplingFrequency                OpenViBE::CIdentifier(0x8519915D, 0xB6BE506D)
#define OVP_Algorithm_DetectingMinMax_OutputParameterId_SignalMatrix                    OpenViBE::CIdentifier(0x853F2DE5, 0x628237CE)
#define OVP_Algorithm_DetectingMinMax_InputTriggerId_Initialize                         OpenViBE::CIdentifier(0x6B43B69D, 0xDA1EAE30)
#define OVP_Algorithm_DetectingMinMax_InputTriggerId_DetectsMin                         OpenViBE::CIdentifier(0xFCB3CFC2, 0x980E3085)
#define OVP_Algorithm_DetectingMinMax_InputTriggerId_DetectsMax                         OpenViBE::CIdentifier(0x24926194, 0x086E6C2A)

#define OVP_ClassId_Box_DetectingMinMaxBoxAlgorithm                                     OpenViBE::CIdentifier(0xD647A2C4, 0xD4833160)
#define OVP_ClassId_Box_DetectingMinMaxBoxAlgorithmDesc                                 OpenViBE::CIdentifier(0xEF9E296A, 0x10285AE1)

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
