#ifndef __OpenViBEPlugins_Defines_H__
#define __OpenViBEPlugins_Defines_H__

//___________________________________________________________________//
//                                                                   //
// Box algorithms                                                    //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTest                                                OpenViBE::CIdentifier(0x3C2EF355, 0xFE495C3D)
#define OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTestDesc                                            OpenViBE::CIdentifier(0xE5176EB9, 0xD6E47D7F)

#define OVP_ClassId_BoxAlgorithm_StreamEncoderAlgorithmTest                                                OpenViBE::CIdentifier(0x87D18C62, 0xF2DAF779)
#define OVP_ClassId_BoxAlgorithm_StreamEncoderAlgorithmTestDesc                                            OpenViBE::CIdentifier(0x95E27325, 0x6893A519)

//___________________________________________________________________//
//                                                                   //
// EBML base decoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_EBMLBaseStreamDecoder                                                        OpenViBE::CIdentifier(0xFD30C96D, 0x8245A8F8)
#define OVP_ClassId_Algorithm_EBMLBaseStreamDecoderDesc                                                    OpenViBE::CIdentifier(0x4F701AC9, 0xDFBE912E)
#define OVP_Algorithm_EBMLStreamDecoder_InputParameterId_MemoryBufferToDecode                              OpenViBE::CIdentifier(0x2F98EA3C, 0xFB0BE096)
#define OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedHeader                                     OpenViBE::CIdentifier(0x815234BF, 0xAABAE5F2)
#define OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedBuffer                                     OpenViBE::CIdentifier(0xAA2738BF, 0xF7FE9FC3)
#define OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedEnd                                        OpenViBE::CIdentifier(0xC4AA114C, 0x628C2D77)

//___________________________________________________________________//
//                                                                   //
// Acquisition decoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_AcquisitionStreamDecoder                                                     OpenViBE::CIdentifier(0x1E0812B7, 0x3F686DD4)
#define OVP_ClassId_Algorithm_AcquisitionStreamDecoderDesc                                                 OpenViBE::CIdentifier(0xA01599B0, 0x7F51631A)
#define OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_BufferDuration                            OpenViBE::CIdentifier(0x7527D6E5, 0xB7A70339)
#define OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ExperimentInformationStream               OpenViBE::CIdentifier(0xA7F1D539, 0xEC708539)
#define OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_SignalStream                              OpenViBE::CIdentifier(0x42C0D7BD, 0xBBCEA3F3)
#define OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_StimulationStream                         OpenViBE::CIdentifier(0x08FC3C12, 0x86A07BF7)
#define OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelLocalisationStream                 OpenViBE::CIdentifier(0x4EB92F81, 0x6ECDA6B9)

//___________________________________________________________________//
//                                                                   //
// Experiment information decoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_ExperimentInformationStreamDecoder                                           OpenViBE::CIdentifier(0x6FA7D52B, 0x80E2ABD6)
#define OVP_ClassId_Algorithm_ExperimentInformationStreamDecoderDesc                                       OpenViBE::CIdentifier(0x0F37CA61, 0x8A77F44E)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentIdentifier            OpenViBE::CIdentifier(0x40259641, 0x478C73DE)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentDate                  OpenViBE::CIdentifier(0xBC0266A2, 0x9C2935F1)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectIdentifier               OpenViBE::CIdentifier(0x97C5D20D, 0x203E65B3)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectName                     OpenViBE::CIdentifier(0x3D3826EA, 0xE8883815)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectAge                      OpenViBE::CIdentifier(0xC36C6B08, 0x5227380A)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectGender                   OpenViBE::CIdentifier(0x7D5059E8, 0xE4D8B38D)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryIdentifier            OpenViBE::CIdentifier(0xE761D3D4, 0x44BA1EBF)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryName                  OpenViBE::CIdentifier(0x5CA80FA5, 0x774F01CB)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianIdentifier            OpenViBE::CIdentifier(0xC8ECFBBC, 0x0DCDA310)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianName                  OpenViBE::CIdentifier(0xB8A94B68, 0x389393D9)

//___________________________________________________________________//
//
// Channel localisation decoder
//___________________________________________________________________//
//
#define OVP_ClassId_Algorithm_ChannelLocalisationStreamDecoder                                             OpenViBE::CIdentifier(0x8222F065, 0xB05D35CF)
#define OVP_ClassId_Algorithm_ChannelLocalisationStreamDecoderDesc                                         OpenViBE::CIdentifier(0x713A29FD, 0xA5A95E2C)
#define OVP_Algorithm_ChannelLocalisationStreamDecoder_OutputParameterId_Dynamic                           OpenViBE::CIdentifier(0xD20991FD, 0xA3153651)

//___________________________________________________________________//
//                                                                   //
// Feature vector decoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_FeatureVectorStreamDecoder                                                   OpenViBE::CIdentifier(0xC2689ECC, 0x43B335C1)
#define OVP_ClassId_Algorithm_FeatureVectorStreamDecoderDesc                                               OpenViBE::CIdentifier(0xAB0AE561, 0xF181E34F)

//___________________________________________________________________//
//                                                                   //
// Signal decoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_SignalStreamDecoder                                                          OpenViBE::CIdentifier(0x7237C149, 0x0CA66DA7)
#define OVP_ClassId_Algorithm_SignalStreamDecoderDesc                                                      OpenViBE::CIdentifier(0xF1547D89, 0x49FFD0C2)
#define OVP_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate                                   OpenViBE::CIdentifier(0x363D8D79, 0xEEFB912C)

//___________________________________________________________________//
//                                                                   //
// Spectrum decoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_SpectrumStreamDecoder                                                        OpenViBE::CIdentifier(0x128202DB, 0x449FC7A6)
#define OVP_ClassId_Algorithm_SpectrumStreamDecoderDesc                                                    OpenViBE::CIdentifier(0x54D18EE8, 0x5DBD913A)
#define OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands                         OpenViBE::CIdentifier(0x375E55E9, 0x9B3666F6)

//___________________________________________________________________//
//                                                                   //
// Stimulation decoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_StimulationStreamDecoder                                                     OpenViBE::CIdentifier(0xC8807F2B, 0x0813C5B1)
#define OVP_ClassId_Algorithm_StimulationStreamDecoderDesc                                                 OpenViBE::CIdentifier(0x391A615B, 0x71CD888A)
#define OVP_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet                            OpenViBE::CIdentifier(0xF46D0C19, 0x47306BEA)

//___________________________________________________________________//
//                                                                   //
// Streamed matrix decoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_StreamedMatrixStreamDecoder                                                  OpenViBE::CIdentifier(0x7359D0DB, 0x91784B21)
#define OVP_ClassId_Algorithm_StreamedMatrixStreamDecoderDesc                                              OpenViBE::CIdentifier(0x384529D5, 0xD8E0A728)
#define OVP_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix                                 OpenViBE::CIdentifier(0x79EF3123, 0x35E3EA4D)

//___________________________________________________________________//
//                                                                   //
// EBML base encoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_EBMLBaseStreamEncoder                                                        OpenViBE::CIdentifier(0x4272C178, 0x3FE84927)
#define OVP_ClassId_Algorithm_EBMLBaseStreamEncoderDesc                                                    OpenViBE::CIdentifier(0x47A9E701, 0x7C57BF3C)
#define OVP_Algorithm_EBMLStreamEncoder_OutputParameterId_EncodedMemoryBuffer                              OpenViBE::CIdentifier(0xA3D8B171, 0xF8734734)
#define OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeHeader                                        OpenViBE::CIdentifier(0x878EAF60, 0xF9D5303F)
#define OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeBuffer                                        OpenViBE::CIdentifier(0x1B7076FD, 0x449BC70A)
#define OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeEnd                                           OpenViBE::CIdentifier(0x3FC23508, 0x806753D8)
#define OVP_Algorithm_EBMLStreamEncoder_OutputTriggerId_MemoryBufferUpdated                                OpenViBE::CIdentifier(0xD46C7462, 0xD3407E5F)

//___________________________________________________________________//
//                                                                   //
// Acquisition encoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_AcquisitionStreamEncoder                                                     OpenViBE::CIdentifier(0xF9FD2FB5, 0xDF0B3B2C)
#define OVP_ClassId_Algorithm_AcquisitionStreamEncoderDesc                                                 OpenViBE::CIdentifier(0xE3E0D9EB, 0x4D4EBA00)
#define OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_BufferDuration                             OpenViBE::CIdentifier(0xAFA07097, 0x1145B59B)
#define OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ExperimentInformationStream                OpenViBE::CIdentifier(0x38755128, 0xCB0C908A)
#define OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_SignalStream                               OpenViBE::CIdentifier(0x4ED9D929, 0x6DF5B2B6)
#define OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_StimulationStream                          OpenViBE::CIdentifier(0xCDE202AD, 0xF4864EC9)
#define OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelLocalisationStream                  OpenViBE::CIdentifier(0x2CF786E5, 0x520714A1)

//___________________________________________________________________//
//                                                                   //
// Experiment information encoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_ExperimentInformationStreamEncoder                                           OpenViBE::CIdentifier(0x56B354FE, 0xBF175468)
#define OVP_ClassId_Algorithm_ExperimentInformationStreamEncoderDesc                                       OpenViBE::CIdentifier(0x8CC2C754, 0x61665FDA)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentIdentifier             OpenViBE::CIdentifier(0x40259641, 0x478C73DE)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentDate                   OpenViBE::CIdentifier(0xBC0266A2, 0x9C2935F1)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectIdentifier                OpenViBE::CIdentifier(0x97C5D20D, 0x203E65B3)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectName                      OpenViBE::CIdentifier(0x3D3826EA, 0xE8883815)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectAge                       OpenViBE::CIdentifier(0xC36C6B08, 0x5227380A)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectGender                    OpenViBE::CIdentifier(0x7D5059E8, 0xE4D8B38D)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryIdentifier             OpenViBE::CIdentifier(0xE761D3D4, 0x44BA1EBF)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryName                   OpenViBE::CIdentifier(0x5CA80FA5, 0x774F01CB)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianIdentifier             OpenViBE::CIdentifier(0xC8ECFBBC, 0x0DCDA310)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianName                   OpenViBE::CIdentifier(0xB8A94B68, 0x389393D9)

//___________________________________________________________________//
//
// Channel localisation encoder
//___________________________________________________________________//
//
#define OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoder                                             OpenViBE::CIdentifier(0xC4AA738A, 0x2368C0EA)
#define OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoderDesc                                         OpenViBE::CIdentifier(0x3F7B49A3, 0x2B8F861A)
#define OVP_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Dynamic                            OpenViBE::CIdentifier(0xCF5DD4F8, 0xC2FF2878)

//___________________________________________________________________//
//                                                                   //
// Feature vector encoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_FeatureVectorStreamEncoder                                                   OpenViBE::CIdentifier(0x7EBE049D, 0xF777A602)
#define OVP_ClassId_Algorithm_FeatureVectorStreamEncoderDesc                                               OpenViBE::CIdentifier(0xC249527B, 0x89EE1996)

//___________________________________________________________________//
//                                                                   //
// Signal encoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_SignalStreamEncoder                                                          OpenViBE::CIdentifier(0xC488AD3C, 0xEB2E36BF)
#define OVP_ClassId_Algorithm_SignalStreamEncoderDesc                                                      OpenViBE::CIdentifier(0x90AC1E0F, 0x01518200)
#define OVP_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate                                    OpenViBE::CIdentifier(0x998710FF, 0x2C5CCA82)

//___________________________________________________________________//
//                                                                   //
// Spectrum encoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_SpectrumStreamEncoder                                                        OpenViBE::CIdentifier(0xB3E252DB, 0xC3214498)
#define OVP_ClassId_Algorithm_SpectrumStreamEncoderDesc                                                    OpenViBE::CIdentifier(0xD6182973, 0x122CE114)
#define OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands                          OpenViBE::CIdentifier(0xF5251300, 0x72520E28)

//___________________________________________________________________//
//                                                                   //
// Stimulation encoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_StimulationStreamEncoder                                                     OpenViBE::CIdentifier(0x6E86F7D5, 0xA4668108)
#define OVP_ClassId_Algorithm_StimulationStreamEncoderDesc                                                 OpenViBE::CIdentifier(0x9B994B50, 0x52C3F06A)
#define OVP_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet                             OpenViBE::CIdentifier(0x8565254C, 0x3A49268E)

//___________________________________________________________________//
//                                                                   //
// Streamed matrix encoder
//___________________________________________________________________//
//                                                                   //
#define OVP_ClassId_Algorithm_StreamedMatrixStreamEncoder                                                  OpenViBE::CIdentifier(0x5CB32C71, 0x576F00A6)
#define OVP_ClassId_Algorithm_StreamedMatrixStreamEncoderDesc                                              OpenViBE::CIdentifier(0xEEEFE060, 0x646EE8AB)
#define OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix                                  OpenViBE::CIdentifier(0xA3E9E5B0, 0xAE756303)

//___________________________________________________________________//
//                                                                   //
// Master Acquisition encoder
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_Algorithm_MasterAcquisitionStreamEncoder                                               OpenViBE::CIdentifier(0x2D15E00B, 0x51414EB6)
#define OVP_ClassId_Algorithm_MasterAcquisitionStreamEncoderDesc                                           OpenViBE::CIdentifier(0xE6EC841D, 0x9E75A8FB)
#define OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SubjectIdentifier                    OpenViBE::CIdentifier(0xD5BB5231, 0x59389B72)
#define OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SubjectAge                           OpenViBE::CIdentifier(0x9EF355E4, 0xC8531112)
#define OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SubjectGender                        OpenViBE::CIdentifier(0xA9056AE3, 0x57FE6AF0)
#define OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SignalMatrix                         OpenViBE::CIdentifier(0xE9AC8077, 0xE369A51D)
#define OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SignalSamplingRate                   OpenViBE::CIdentifier(0xB84AD0CA, 0x4F316DD3)
#define OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_StimulationSet                       OpenViBE::CIdentifier(0x5B728D37, 0xFD088887)
#define OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_BufferDuration                       OpenViBE::CIdentifier(0xE1FC7385, 0x586A4F3F)

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
