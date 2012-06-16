#ifndef __OpenViBEToolkit_Defines_H__
#define __OpenViBEToolkit_Defines_H__

//___________________________________________________________________//
//                                                                   //
// OpenViBE toolkit input and output type identifiers                //
//___________________________________________________________________//
//                                                                   //

#define OVTK_TypeId_Boolean                                            OpenViBE::CIdentifier(0x2CDB2F0B, 0x12F231EA)
#define OVTK_TypeId_Integer                                            OpenViBE::CIdentifier(0x007DEEF9, 0x2F3E95C6)
#define OVTK_TypeId_Float                                              OpenViBE::CIdentifier(0x512A166F, 0x5C3EF83F)
#define OVTK_TypeId_String                                             OpenViBE::CIdentifier(0x79A9EDEB, 0x245D83FC)
#define OVTK_TypeId_Filename                                           OpenViBE::CIdentifier(0x330306DD, 0x74A95F98)
#define OVTK_TypeId_Stimulation                                        OpenViBE::CIdentifier(0x2C132D6E, 0x44AB0D97)
#define OVTK_TypeId_GDF_Stimulation                                    OpenViBE::CIdentifier(0xA538DBF0, 0xBC229750)

#define OVTK_TypeId_ClassificationAlgorithm                            OpenViBE::CIdentifier(0x21CE7F37, 0x28DEF186)

//___________________________________________________________________//
//                                                                   //
// OpenViBE toolkit stream type identifiers                          //
//___________________________________________________________________//
//                                                                   //

#define OVTK_TypeId_EBMLStream                                         OpenViBE::CIdentifier(0x434F6587, 0x2EFD2B7E)
#define   OVTK_TypeId_ChannelLocalisation                              OpenViBE::CIdentifier(0x1E4C0D6E, 0x5204EEB2)
#define   OVTK_TypeId_ExperimentationInformation                       OpenViBE::CIdentifier(0x403488E7, 0x565D70B6)
#define   OVTK_TypeId_Stimulations                                     OpenViBE::CIdentifier(0x6F752DD0, 0x082A321E)
#define   OVTK_TypeId_StreamedMatrix                                   OpenViBE::CIdentifier(0x544A003E, 0x6DCBA5F6)
#define     OVTK_TypeId_FeatureVector                                  OpenViBE::CIdentifier(0x17341935, 0x152FF448)
#define     OVTK_TypeId_Signal                                         OpenViBE::CIdentifier(0x5BA36127, 0x195FEAE1)
#define     OVTK_TypeId_Spectrum                                       OpenViBE::CIdentifier(0x1F261C0A, 0x593BF6BD)

//___________________________________________________________________//
//                                                                   //
// ISO 5218 conformant sex identifiers                               //
//___________________________________________________________________//
//                                                                   //

// deprecated
#define OVTK_Value_Sex_Unknown                                        0
#define OVTK_Value_Sex_Male                                           1
#define OVTK_Value_Sex_Female                                         2
#define OVTK_Value_Sex_NotSpecified                                   9

#define OVTK_Value_Gender_Unknown                                     0
#define OVTK_Value_Gender_Male                                        1
#define OVTK_Value_Gender_Female                                      2
#define OVTK_Value_Gender_NotSpecified                                9

//___________________________________________________________________//
//                                                                   //
// OpenViBE toolkit stimulation identifiers                          //
//___________________________________________________________________//
//                                                                   //

#define OVTK_StimulationId_ExperimentStart                   0x00008001
#define OVTK_StimulationId_ExperimentStop                    0x00008002
#define OVTK_StimulationId_SegmentStart                      0x00008003
#define OVTK_StimulationId_SegmentStop                       0x00008004
#define OVTK_StimulationId_TrialStart                        0x00008005
#define OVTK_StimulationId_TrialStop                         0x00008006
#define OVTK_StimulationId_BaselineStart                     0x00008007
#define OVTK_StimulationId_BaselineStop                      0x00008008
#define OVTK_StimulationId_RestStart                         0x00008009
#define OVTK_StimulationId_RestStop                          0x0000800a
#define OVTK_StimulationId_VisualStimulationStart            0x0000800b
#define OVTK_StimulationId_VisualStimulationStop             0x0000800c
#define OVTK_StimulationId_VisualSteadyStateStimulationStart 0x00008010
#define OVTK_StimulationId_VisualSteadyStateStimulationStop  0x00008011

#define OVTK_StimulationId_Label(i)                          0x00008100|((i)&0xff)
#define OVTK_StimulationId_LabelStart                        0x00008100
#define OVTK_StimulationId_Label_00                          0x00008100
#define OVTK_StimulationId_Label_01                          0x00008101
#define OVTK_StimulationId_Label_02                          0x00008102
#define OVTK_StimulationId_Label_03                          0x00008103
#define OVTK_StimulationId_Label_04                          0x00008104
#define OVTK_StimulationId_Label_05                          0x00008105
#define OVTK_StimulationId_Label_06                          0x00008106
#define OVTK_StimulationId_Label_07                          0x00008107
#define OVTK_StimulationId_Label_08                          0x00008108
#define OVTK_StimulationId_Label_09                          0x00008109
#define OVTK_StimulationId_Label_0A                          0x0000810a
#define OVTK_StimulationId_Label_0B                          0x0000810b
#define OVTK_StimulationId_Label_0C                          0x0000810c
#define OVTK_StimulationId_Label_0D                          0x0000810d
#define OVTK_StimulationId_Label_0E                          0x0000810e
#define OVTK_StimulationId_Label_0F                          0x0000810f
#define OVTK_StimulationId_Label_10                          0x00008110
#define OVTK_StimulationId_Label_11                          0x00008111
#define OVTK_StimulationId_Label_12                          0x00008112
#define OVTK_StimulationId_Label_13                          0x00008113
#define OVTK_StimulationId_Label_14                          0x00008114
#define OVTK_StimulationId_Label_15                          0x00008115
#define OVTK_StimulationId_Label_16                          0x00008116
#define OVTK_StimulationId_Label_17                          0x00008117
#define OVTK_StimulationId_Label_18                          0x00008118
#define OVTK_StimulationId_Label_19                          0x00008119
#define OVTK_StimulationId_Label_1A                          0x0000811a
#define OVTK_StimulationId_Label_1B                          0x0000811b
#define OVTK_StimulationId_Label_1C                          0x0000811c
#define OVTK_StimulationId_Label_1D                          0x0000811d
#define OVTK_StimulationId_Label_1E                          0x0000811e
#define OVTK_StimulationId_Label_1F                          0x0000811f
#define OVTK_StimulationId_LabelEnd                          0x000081ff

#define OVTK_StimulationId_Train                             0x00008201
#define OVTK_StimulationId_Beep                              0x00008202
#define OVTK_StimulationId_DoubleBeep                        0x00008203
#define OVTK_StimulationId_EndOfFile                         0x00008204
#define OVTK_StimulationId_Target                            0x00008205
#define OVTK_StimulationId_NonTarget                         0x00008206
#define OVTK_StimulationId_TrainCompleted                    0x00008207
#define OVTK_StimulationId_Reset                             0x00008208

//___________________________________________________________________//
//                                                                   //
// GDF file format stimulation identifiers                           //
//___________________________________________________________________//
//                                                                   //

#define OVTK_GDF_Artifact_EOG_Large                               0x101
#define OVTK_GDF_Artifact_ECG                                     0x102
#define OVTK_GDF_Artifact_EMG                                     0x103
#define OVTK_GDF_Artifact_Movement                                0x104
#define OVTK_GDF_Artifact_Failing_Electrode                       0x105
#define OVTK_GDF_Artifact_Sweat                                   0x106
#define OVTK_GDF_Artifact_50_60_Hz_Interference                   0x107
#define OVTK_GDF_Artifact_Breathing                               0x108
#define OVTK_GDF_Artifact_Pulse                                   0x109
#define OVTK_GDF_Artifact_EOG_Small                               0x10A

#define OVTK_GDF_Calibration                                      0x10F

#define OVTK_GDF_EEG_Sleep_Splindles                              0x111
#define OVTK_GDF_EEG_K_Complexes                                  0x112
#define OVTK_GDF_EEG_Saw_Tooth_Waves                              0x113
#define OVTK_GDF_EEG_Idling_EEG_Eyes_Open                         0x114
#define OVTK_GDF_EEG_Idling_EEG_Eyes_Closed                       0x115
#define OVTK_GDF_EEG_Spike                                        0x116
#define OVTK_GDF_EEG_Seizure                                      0x117

#define OVTK_GDF_VEP                                              0x121
#define OVTK_GDF_AEP                                              0x122
#define OVTK_GDF_SEP                                              0x123
#define OVTK_GDF_TMS                                              0x12F

#define OVTK_GDF_SSVEP                                            0x131
#define OVTK_GDF_SSAEP                                            0x132
#define OVTK_GDF_SSSEP                                            0x133

#define OVTK_GDF_Start_Of_Trial                                   0x300
#define OVTK_GDF_Left                                             0x301
#define OVTK_GDF_Right                                            0x302
#define OVTK_GDF_Foot                                             0x303
#define OVTK_GDF_Tongue                                           0x304
#define OVTK_GDF_class5                                           0x305
#define OVTK_GDF_Down                                             0x306
#define OVTK_GDF_class7                                           0x307
#define OVTK_GDF_class8                                           0x308
#define OVTK_GDF_class9                                           0x309
#define OVTK_GDF_class10                                          0x30A
#define OVTK_GDF_class11                                          0x30B
#define OVTK_GDF_Up                                               0x30C
#define OVTK_GDF_Feedback_Continuous                              0x30D
#define OVTK_GDF_Feedback_Discrete                                0x30E
#define OVTK_GDF_Cue_Unknown_Undefined                            0x30F
#define OVTK_GDF_Beep                                             0x311
#define OVTK_GDF_Cross_On_Screen                                  0x312
#define OVTK_GDF_Flashing_Light                                   0x313
// SPECIALY ADDED BY YR
#define OVTK_GDF_End_Of_Trial                                     0x320

#define OVTK_GDF_Correct                                          0x381
#define OVTK_GDF_Incorrect                                        0x382
// SPECIALY ADDED BY YR
#define OVTK_GDF_End_Of_Session                                   0x3F2
#define OVTK_GDF_Rejection                                        0x3FF

#define OVTK_GDF_OAHE                                             0x401
#define OVTK_GDF_RERA                                             0x402
#define OVTK_GDF_CAHE                                             0x403
#define OVTK_GDF_CSB                                              0x404
#define OVTK_GDF_Sleep_Hypoventilation                            0x405
#define OVTK_GDF_Maximum_Inspiration                              0x40E
#define OVTK_GDF_Start_Of_Inspiration                             0x40F

#define OVTK_GDF_Wake                                             0x410
#define OVTK_GDF_Stage_1                                          0x411
#define OVTK_GDF_Stage_2                                          0x412
#define OVTK_GDF_Stage_3                                          0x413
#define OVTK_GDF_Stage_4                                          0x414
#define OVTK_GDF_REM                                              0x415

#define OVTK_GDF_Lights_On                                        0x420
#define OVTK_GDF_Lights_Off                                       0x8420

#define OVTK_GDF_Eyes_Left                                        0x431
#define OVTK_GDF_Eyes_Right                                       0x432
#define OVTK_GDF_Eyes_Up                                          0x433
#define OVTK_GDF_Eyes_Down                                        0x434
#define OVTK_GDF_Horizontal_Eye_Movement                          0x435
#define OVTK_GDF_Vertical_Eye_Movement                            0x436
#define OVTK_GDF_Rotation_Clockwise                               0x437
#define OVTK_GDF_Rotation_Counterclockwise                        0x438
#define OVTK_GDF_Eye_Blink                                        0x439

#define OVTK_GDF_Left_Hand_Movement                               0x441
#define OVTK_GDF_Right_Hand_Movement                              0x442
#define OVTK_GDF_Head_Movement                                    0x443
#define OVTK_GDF_Tongue_Movement                                  0x444
#define OVTK_GDF_Swallowing                                       0x445
#define OVTK_GDF_Biting                                           0x446
#define OVTK_GDF_Foot_Movement                                    0x447
#define OVTK_GDF_Foot_Right_Movement                              0x448
#define OVTK_GDF_Arm_Movement                                     0x449
#define OVTK_GDF_Arm_Right_Movement                               0x44A

#define OVTK_GDF_ECG_Fiducial_Point_QRS_Complex                   0x501
#define OVTK_GDF_ECG_P_Wave                                       0x502
#define OVTK_GDF_ECG_QRS_Complex                                  0x503
#define OVTK_GDF_ECG_R_Point                                      0x504
#define OVTK_GDF_ECG_T_Wave                                       0x506
#define OVTK_GDF_ECG_U_Wave                                       0x507

#define OVTK_GDF_Start                                            0x580
#define OVTK_GDF_25_Watt                                          0x581
#define OVTK_GDF_50_Watt                                          0x582
#define OVTK_GDF_75_Watt                                          0x583
#define OVTK_GDF_100_Watt                                         0x584
#define OVTK_GDF_125_Watt                                         0x585
#define OVTK_GDF_150_Watt                                         0x586
#define OVTK_GDF_175_Watt                                         0x587
#define OVTK_GDF_200_Watt                                         0x588
#define OVTK_GDF_225_Watt                                         0x589
#define OVTK_GDF_250_Watt                                         0x58A
#define OVTK_GDF_275_Watt                                         0x58B
#define OVTK_GDF_300_Watt                                         0x58C
#define OVTK_GDF_325_Watt                                         0x58D
#define OVTK_GDF_350_Watt                                         0x58E

#define OVTK_GDF_Condition(i)                                       (i)

#define OVTK_GDF_Start_Of_New_Segment                            0x7FFE
#define OVTK_GDF_Non_Equidistant_Sampling_Value                  0x7FFF

//___________________________________________________________________//
//                                                                   //
// Acquisition stream node identifiers                               //
//___________________________________________________________________//
//                                                                   //

/*
 * Acquisition stream description (fixed on july 2006)
 *
 * version 1 :
 * ----------------------------------------------------------------- *
 * OVTK_NodeId_Acquisition_Header
 *   OVTK_NodeId_Acquisition_AcquisitionInformation
 *     OVTK_NodeId_Acquisition_ExperimentId (integer)
 *     OVTK_NodeId_Acquisition_SubjectAge (integer)
 *     OVTK_NodeId_Acquisition_SubjectGender (integer)
 *   OVTK_NodeId_Acquisition_ChannelCount (integer)
 *   OVTK_NodeId_Acquisition_SamplingFrequency (integer)
 *   OVTK_NodeId_Acquisition_GainFactors
 *     OVTK_NodeId_Acquisition_GainFactor (float)
 *     OVTK_NodeId_Acquisition_GainFactor (float)
 *     ...
 *   OVTK_NodeId_Acquisition_ChannelNames
 *     OVTK_NodeId_Acquisition_ChannelName (string)
 *     OVTK_NodeId_Acquisition_ChannelName (string)
 *     ...
 *   OVTK_NodeId_Acquisition_ChannelLocations
 *     OVTK_NodeId_Acquisition_ChannelLocation (array of 3 float64)
 *     OVTK_NodeId_Acquisition_ChannelLocation (array of 3 float64)
 *     ...
 * OVTK_NodeId_Acquisition_Buffer
 *   OVTK_NodeId_Acquisition_Samples
 *     OVTK_NodeId_Acquisition_SamplesPerChannelCount (integer)
 *     OVTK_NodeId_Acquisition_SampleBlock (array of float32)
 *     OVTK_NodeId_Acquisition_SampleBlock (array of float32)
 *     ...
 *   OVTK_NodeId_Acquisition_Stimulations
 *     OVTK_NodeId_Acquisition_StimulationsCount (integer)
 *     OVTK_NodeId_Acquisition_Stimulation
 *       OVTK_NodeId_Acquisition_StimulationSampleIndex (integer)
 *       OVTK_NodeId_Acquisition_StimulationIdentifier (integer)
 *     OVTK_NodeId_Acquisition_Stimulation
 *     ...
 * OVTK_NodeId_Acquisition_Buffer
 * ...
 */

/*
 * Acquisition stream description (fixed on march 2008)
 * This is a multiplexed stream
 *
 * version 2 :
 * ----------------------------------------------------------------- *
 * OVTK_NodeId_Header
 *   OVTK_NodeId_Acquisition_Header_ExperimentInformation
 *     ... some experiment information stream header
 *   OVTK_NodeId_Acquisition_Header_Signal
 *     ... some signal stream header
 *   OVTK_NodeId_Acquisition_Header_Stimulation
 *     ... some stimulation stream header
 *   OVTK_NodeId_Acquisition_Header_ChannelLocalisation
 *     ... some channel localisation stream header
 * OVTK_NodeId_Buffer
 *   OVTK_NodeId_Acquisition_Buffer_ExperimentInformation
 *     ... some experiment information stream buffer
 *   OVTK_NodeId_Acquisition_Buffer_Signal
 *     ... some signal stream buffer
 *   OVTK_NodeId_Acquisition_Buffer_Stimulation
 *     ... some stimulation stream buffer
 *   OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation
 *     ... some channel localisation stream buffer
 * OVTK_NodeId_Buffer
 * OVTK_NodeId_Buffer
 * ...
 */

#define OVTK_NodeId_Acquisition_Header_BufferDuration                          EBML::CIdentifier(0x00000000, 0x00000080)
#define OVTK_NodeId_Acquisition_Header_ExperimentInformation                   EBML::CIdentifier(0x00000000, 0x00000081)
#define OVTK_NodeId_Acquisition_Header_Signal                                  EBML::CIdentifier(0x00000000, 0x00000082)
#define OVTK_NodeId_Acquisition_Header_Stimulation                             EBML::CIdentifier(0x00000000, 0x00000083)
#define OVTK_NodeId_Acquisition_Header_ChannelLocalisation                     EBML::CIdentifier(0x00000000, 0x00000084)
#define OVTK_NodeId_Acquisition_Buffer_ExperimentInformation                   EBML::CIdentifier(0x00000000, 0x00000041)
#define OVTK_NodeId_Acquisition_Buffer_Signal                                  EBML::CIdentifier(0x00000000, 0x00000042)
#define OVTK_NodeId_Acquisition_Buffer_Stimulation                             EBML::CIdentifier(0x00000000, 0x00000043)
#define OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation                     EBML::CIdentifier(0x00000000, 0x00000044)

#define OVTK_NodeId_Acquisition_Header                                         EBML::CIdentifier(0x00000000, 0x00004239) // deprecated
#define OVTK_NodeId_Acquisition_AcquisitionInformation                         EBML::CIdentifier(0x00000000, 0x00004240) // deprecated
#define OVTK_NodeId_Acquisition_ExperimentId                                   EBML::CIdentifier(0x00000000, 0x00004241) // deprecated
#define OVTK_NodeId_Acquisition_SubjectAge                                     EBML::CIdentifier(0x00000000, 0x00004242) // deprecated
#define OVTK_NodeId_Acquisition_SubjectGender                                  EBML::CIdentifier(0x00000000, 0x00004243) // deprecated
#define OVTK_NodeId_Acquisition_ChannelCount                                   EBML::CIdentifier(0x00000000, 0x00004244) // deprecated
#define OVTK_NodeId_Acquisition_SamplingFrequency                              EBML::CIdentifier(0x00000000, 0x00004245) // deprecated
#define OVTK_NodeId_Acquisition_ChannelNames                                   EBML::CIdentifier(0x00000000, 0x00004246) // deprecated
#define OVTK_NodeId_Acquisition_ChannelName                                    EBML::CIdentifier(0x00000000, 0x00004247) // deprecated
#define OVTK_NodeId_Acquisition_GainFactors                                    EBML::CIdentifier(0x00000000, 0x00004248) // deprecated
#define OVTK_NodeId_Acquisition_GainFactor                                     EBML::CIdentifier(0x00000000, 0x00004249) // deprecated
#define OVTK_NodeId_Acquisition_ChannelLocations                               EBML::CIdentifier(0x00000000, 0x00004250) // deprecated
#define OVTK_NodeId_Acquisition_ChannelLocation                                EBML::CIdentifier(0x00000000, 0x00004251) // deprecated
#define OVTK_NodeId_Acquisition_Buffer                                         EBML::CIdentifier(0x00000000, 0x0000005A) // deprecated
#define OVTK_NodeId_Acquisition_Samples                                        EBML::CIdentifier(0x00000000, 0x0000005B) // deprecated
#define OVTK_NodeId_Acquisition_SamplesPerChannelCount                         EBML::CIdentifier(0x00000000, 0x0000005C) // deprecated
#define OVTK_NodeId_Acquisition_SampleBlock                                    EBML::CIdentifier(0x00000000, 0x0000005D) // deprecated
#define OVTK_NodeId_Acquisition_Stimulations                                   EBML::CIdentifier(0x00000000, 0x00000060) // deprecated
#define OVTK_NodeId_Acquisition_StimulationsCount                              EBML::CIdentifier(0x00000000, 0x00000061) // deprecated
#define OVTK_NodeId_Acquisition_Stimulation                                    EBML::CIdentifier(0x00000000, 0x00000062) // deprecated
#define OVTK_NodeId_Acquisition_StimulationSampleIndex                         EBML::CIdentifier(0x00000000, 0x00000063) // deprecated
#define OVTK_NodeId_Acquisition_StimulationIdentifier                          EBML::CIdentifier(0x00000000, 0x00000064) // deprecated
#define OVTK_NodeId_Acquisition_SubjectSex                                     EBML::CIdentifier(0x00000000, 0x00004243) // deprecated

//___________________________________________________________________//
//                                                                   //
// Protocol hierarchy                                                //
//___________________________________________________________________//
//                                                                   //

/*
 * - Standard EBML stream
 *   - Experiment information
 *   - Stimulation
 *   - Streamed Matrix
 *     - Signal
 *     - Spectrum
 *     - Feature vector
 */

//___________________________________________________________________//
//                                                                   //
// Standard EBML stream node identifiers                             //
//___________________________________________________________________//
//                                                                   //

#define OVTK_NodeId_Header                                                     EBML::CIdentifier(0x002B395F, 0x108ADFAE)
#define OVTK_NodeId_Header_StreamType                                          EBML::CIdentifier(0x00CDD0F7, 0x46B0278D)
#define OVTK_NodeId_Header_StreamVersion                                       EBML::CIdentifier(0x006F5A08, 0x7796EBC5)
#define OVTK_NodeId_Buffer                                                     EBML::CIdentifier(0x00CF2101, 0x02375310)
#define OVTK_NodeId_End                                                        EBML::CIdentifier(0x00D9DDC3, 0x0B12873A)

//___________________________________________________________________//
//                                                                   //
// Streamed matrix stream node identifiers                           //
//___________________________________________________________________//
//                                                                   //

#define OVTK_NodeId_Header_StreamedMatrix                                      EBML::CIdentifier(0x0072F560, 0x7ED2CBED)
#define OVTK_NodeId_Header_StreamedMatrix_DimensionCount                       EBML::CIdentifier(0x003FEBD4, 0x2725D428)
#define OVTK_NodeId_Header_StreamedMatrix_Dimension                            EBML::CIdentifier(0x0000E3C0, 0x3A7D5141)
#define OVTK_NodeId_Header_StreamedMatrix_Dimension_Size                       EBML::CIdentifier(0x001302F7, 0x36D8438A)
#define OVTK_NodeId_Header_StreamedMatrix_Dimension_Label                      EBML::CIdentifier(0x00153E40, 0x190227E0)
#define OVTK_NodeId_Buffer_StreamedMatrix                                      EBML::CIdentifier(0x00120663, 0x08FBC165)
#define OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer                            EBML::CIdentifier(0x00B18C10, 0x427D098C)

//___________________________________________________________________//
//                                                                   //
// Signal stream node identifiers                                    //
//___________________________________________________________________//
//                                                                   //

/*
 * Signal stream description (based on v1 november 6th 2006, modified may 24th 2007)
 *
 * version 2 :
 * ----------------------------------------------------------------- *
 * OVTK_NodeId_Header
 *   OVTK_NodeId_Header_StreamType (integer:)
 *   OVTK_NodeId_Header_StreamVersion (integer:2)
 *   OVTK_NodeId_Header_Signal
 *     OVTK_NodeId_Header_Signal_SamplingRate (integer)
 *   OVTK_NodeId_Header_StreamedMatrix
 *     OVTK_NodeId_Header_StreamedMatrix_DimensionCount (integer:2)
 *     OVTK_NodeId_Header_StreamedMatrix_Dimension
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Size (integer:channel count)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:channel 1 name)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:channel 2 name)
 *       ...
 *     OVTK_NodeId_Header_StreamedMatrix_Dimension
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Size (integer:number of samples per buffer)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string)
 *       ...
 * OVTK_NodeId_Buffer
 *   OVTK_NodeId_Buffer_StreamedMatrix
 *     OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer (array of float64)
 * OVTK_NodeId_Buffer
 *   OVTK_NodeId_Buffer_StreamedMatrix
 *     OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer (array of float64)
 * ...
 * OVTK_NodeId_End
 * ----------------------------------------------------------------- *
 */

#define OVTK_NodeId_Header_Signal                                              EBML::CIdentifier(0x007855DE, 0x3748D375)
#define OVTK_NodeId_Header_Signal_SamplingRate                                 EBML::CIdentifier(0x00141C43, 0x0C37006B)

//___________________________________________________________________//
//                                                                   //
// Channel localisation stream node identifiers                      //
//___________________________________________________________________//
//                                                                   //

/*
 * Channel localisation description (nov 04th 2008)
 *
 * version 1 :
 * ----------------------------------------------------------------- *
 * OVTK_NodeId_Header
 *   OVTK_NodeId_Header_StreamType (integer:)
 *   OVTK_NodeId_Header_StreamVersion (integer:2)
 *   OVTK_NodeId_Header_ChannelLocalisation
 *     OVTK_NodeId_Header_ChannelLocalisation_Dynamic (boolean)
 *   OVTK_NodeId_Header_StreamedMatrix
 *     OVTK_NodeId_Header_StreamedMatrix_DimensionCount (integer:2)
 *     OVTK_NodeId_Header_StreamedMatrix_Dimension
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Size (integer:channel count)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:channel 1 name)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:channel 2 name)
 *       ...
 *     OVTK_NodeId_Header_StreamedMatrix_Dimension
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Size (integer:3)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:x)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:y)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:z)
 * OVTK_NodeId_Buffer
 *   OVTK_NodeId_Buffer_StreamedMatrix
 *     OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer (array of float64)
 * OVTK_NodeId_Buffer
 *   OVTK_NodeId_Buffer_StreamedMatrix
 *     OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer (array of float64)
 * ...
 * OVTK_NodeId_End
 * ----------------------------------------------------------------- *
 */

#define OVTK_NodeId_Header_ChannelLocalisation                                 EBML::CIdentifier(0xF2CFE60B, 0xEFD63E3B)
#define OVTK_NodeId_Header_ChannelLocalisation_Dynamic                         EBML::CIdentifier(0x5338AF5C, 0x07C469C3)

//___________________________________________________________________//
//                                                                   //
// Stimulation stream node identifiers                               //
//___________________________________________________________________//
//                                                                   //

/*
 * Stimulation stream description (based on v1 november 6th 2006, modified may 24th 2007)
 *
 * version 3 :
 * ----------------------------------------------------------------- *
 * OVTK_NodeId_Header
 *   OVTK_NodeId_Header_StreamType (integer:)
 *   OVTK_NodeId_Header_StreamVersion (integer:2)
 * OVTK_NodeId_Buffer
 *   OVTK_NodeId_Buffer_Stimulation
 *     OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations (integer)
 *     OVTK_NodeId_Buffer_Stimulation_Stimulation
 *       OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier (integer)
 *       OVTK_NodeId_Buffer_Stimulation_Stimulation_Date (integer)
 *       OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration (integer)
 *     OVTK_NodeId_Stimulation_Stimulation
 *     ...
 * OVTK_NodeId_Buffer
 * OVTK_NodeId_Buffer
 * ...
 * OVTK_NodeId_End
 * ----------------------------------------------------------------- *
 */

#define OVTK_NodeId_Buffer_Stimulation                                         EBML::CIdentifier(0x006DEABE, 0x7FC05A20)
#define OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations                    EBML::CIdentifier(0x00BB790B, 0x2B8574D8)
#define OVTK_NodeId_Buffer_Stimulation_Stimulation                             EBML::CIdentifier(0x0016EAC6, 0x29FBCAA1)
#define OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier                  EBML::CIdentifier(0x006FA5DB, 0x4BAC31E9)
#define OVTK_NodeId_Buffer_Stimulation_Stimulation_Date                        EBML::CIdentifier(0x00B866D8, 0x14DA5374)
#define OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration                    EBML::CIdentifier(0x14EE055F, 0x87FBCC9C)

//___________________________________________________________________//
//                                                                   //
// Experimentation information node identifiers                      //
//___________________________________________________________________//
//                                                                   //

/*
 * Experimentation information stream description (fixed on november 6th 2006)
 *
 * version 1 :
 * ----------------------------------------------------------------- *
 * OVTK_NodeId_Header
 *   OVTK_NodeId_Header_StreamType (integer:)
 *   OVTK_NodeId_Header_StreamVersion (integer:1)
 *   OVTK_NodeId_Header_ExperimentInformation
 *     OVTK_NodeId_Header_ExperimentInformation_Experiment
 *       OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier (integer)
 *       OVTK_NodeId_Header_ExperimentInformation_Experiment_Date (date)
 *     OVTK_NodeId_Header_ExperimentInformation_Subject
 *       OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier (integer)
 *       OVTK_NodeId_Header_ExperimentInformation_Subject_Name (string)
 *       OVTK_NodeId_Header_ExperimentInformation_Subject_Age (integer)
 *       OVTK_NodeId_Header_ExperimentInformation_Subject_Gender (integer)
 *     OVTK_NodeId_Header_ExperimentInformation_Context
 *       OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier (integer)
 *       OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName (string)
 *       OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier (integer)
 *       OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName (string)
 * OVTK_NodeId_End
 * ----------------------------------------------------------------- *
 */

#define OVTK_NodeId_Header_ExperimentInformation                               EBML::CIdentifier(0x00746BA0, 0x115AE04D)
#define OVTK_NodeId_Header_ExperimentInformation_Experiment                    EBML::CIdentifier(0x0011D6B7, 0x48F1AA39)
#define OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier         EBML::CIdentifier(0x006ACD74, 0x1C960C26)
#define OVTK_NodeId_Header_ExperimentInformation_Experiment_Date               EBML::CIdentifier(0x002F8FB7, 0x6DA7552D)
#define OVTK_NodeId_Header_ExperimentInformation_Subject                       EBML::CIdentifier(0x003EC620, 0x333E0A94)
#define OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier            EBML::CIdentifier(0x00D62974, 0x473D4AA5)
#define OVTK_NodeId_Header_ExperimentInformation_Subject_Name                  EBML::CIdentifier(0x0041FD0A, 0x6BCD9A99)
#define OVTK_NodeId_Header_ExperimentInformation_Subject_Age                   EBML::CIdentifier(0x00DF7DD9, 0x33336C51)
#define OVTK_NodeId_Header_ExperimentInformation_Subject_Sex                   EBML::CIdentifier(0x0069BB84, 0x3FC8E149) /* for retro compat */
#define OVTK_NodeId_Header_ExperimentInformation_Subject_Gender                EBML::CIdentifier(0x0069BB84, 0x3FC8E149)
#define OVTK_NodeId_Header_ExperimentInformation_Context                       EBML::CIdentifier(0x0018C291, 0x7985DFDD)
#define OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier  EBML::CIdentifier(0x003F11B9, 0x26D76D9C)
#define OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName        EBML::CIdentifier(0x00EB1F23, 0x51C23B83)
#define OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier  EBML::CIdentifier(0x00874A7F, 0x60DC34C2)
#define OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName        EBML::CIdentifier(0x00C8C393, 0x31CE5B3E)

//___________________________________________________________________//
//                                                                   //
// Feature vector stream node identifiers                            //
//___________________________________________________________________//
//                                                                   //

/*
 * Feature vector stream description (fixed on may 24th 2007)
 *
 * version 1 :
 * ----------------------------------------------------------------- *
 * OVTK_NodeId_Header
 *   OVTK_NodeId_Header_StreamType (integer:)
 *   OVTK_NodeId_Header_StreamVersion (integer:1)
 *   OVTK_NodeId_Header_StreamedMatrix
 *     OVTK_NodeId_Header_StreamedMatrix_DimensionCount (integer:1)
 *     OVTK_NodeId_Header_StreamedMatrix_Dimension
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Size (integer:feature count)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:feature 1 name)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:feature 2 name)
 *       ...
 * OVTK_NodeId_Buffer
 *   OVTK_NodeId_Buffer_StreamedMatrix
 *     OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer (array of float64)
 * OVTK_NodeId_Buffer
 *   OVTK_NodeId_Buffer_StreamedMatrix
 *     OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer (array of float64)
 * ...
 * OVTK_NodeId_End
 * ----------------------------------------------------------------- *
 */

//___________________________________________________________________//
//                                                                   //
// Spectrum stream node identifiers                                  //
//___________________________________________________________________//
//                                                                   //

/*
 * Spectrum stream description (fixed on june 4th 2007)
 *
 * version 1 :
 * ----------------------------------------------------------------- *
 * OVTK_NodeId_Header
 *   OVTK_NodeId_Header_StreamType (integer:)
 *   OVTK_NodeId_Header_StreamVersion (integer:1)
 *   OVTK_NodeId_Header_StreamedMatrix
 *     OVTK_NodeId_Header_StreamedMatrix_DimensionCount (integer:2)
 *     OVTK_NodeId_Header_StreamedMatrix_Dimension
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Size (integer:channel count)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:channel 1 name)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:channel 2 name)
 *       ...
 *     OVTK_NodeId_Header_StreamedMatrix_Dimension
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Size (integer:number of frequency band)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:frequency band 1 name)
 *       OVTK_NodeId_Header_StreamedMatrix_Dimension_Label (string:frequency band 2 name)
 *       ...
 *   OVTK_NodeId_Header_Spectrum
 *     OVTK_NodeId_Header_Spectrum_FrequencyBand
 *       OVTK_NodeId_Header_Spectrum_FrequencyBand_Start (float64)
 *       OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop (float64)
 *     OVTK_NodeId_Header_Spectrum_FrequencyBand
 *       OVTK_NodeId_Header_Spectrum_FrequencyBand_Start (float64)
 *       OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop (float64)
 *     ...
 * OVTK_NodeId_Buffer
 *   OVTK_NodeId_Buffer_StreamedMatrix
 *     OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer (array of float64)
 * OVTK_NodeId_Buffer
 *   OVTK_NodeId_Buffer_StreamedMatrix
 *     OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer (array of float64)
 * ...
 * OVTK_NodeId_End
 * ----------------------------------------------------------------- *
 */

#define OVTK_NodeId_Header_Spectrum                                     EBML::CIdentifier(0x00CCFA4B, 0x14F37D4D)
#define OVTK_NodeId_Header_Spectrum_FrequencyBand                       EBML::CIdentifier(0x0010983C, 0x21F8BDE5)
#define OVTK_NodeId_Header_Spectrum_FrequencyBand_Start                 EBML::CIdentifier(0x00AA5654, 0x2403A2CB)
#define OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop                  EBML::CIdentifier(0x00A44C82, 0x05BE50D5)

//___________________________________________________________________//
//                                                                   //
//                                                                   //
//___________________________________________________________________//
//                                                                   //

/*
#define OVTK_NodeId_                                                    EBML::CIdentifier(0x0027615F, 0x2243F7B5)
#define OVTK_NodeId_                                                    EBML::CIdentifier(0x00866CC6, 0x1EFE4BDC)
#define OVTK_NodeId_                                                    EBML::CIdentifier(0x00C91900, 0x55E50FF0)
#define OVTK_NodeId_                                                    EBML::CIdentifier(0x00E0E260, 0x646BF99E)
#define OVTK_NodeId_                                                    EBML::CIdentifier(0x00DCE72C, 0x386A40C5)
#define OVTK_NodeId_                                                    EBML::CIdentifier(0x00C520C6, 0x09AE98B5)
#define OVTK_NodeId_                                                    EBML::CIdentifier(0x00F1CBCB, 0x56BD6821)
*/
#define OVTK_ClassId_                                                   OpenViBE::CIdentifier(0x00C6D56F, 0x30890D27)
#define OVTK_ClassId_Stimulation                                        OpenViBE::CIdentifier(0xE4E131F0, 0xC6550E9E)
#define OVTK_ClassId_BaseAlgorithms_DataImporterT                       OpenViBE::CIdentifier(0xA6BB8664, 0xF1B9DE96)
#define OVTK_ClassId_BaseAlgorithms_DataExporterT                       OpenViBE::CIdentifier(0xA432362A, 0x5CD4F227)

#define OVTK_ClassId_Vector                                             OpenViBE::CIdentifier(0x5D113170, 0x941C8932)
#define OVTK_ClassId_FeatureVector                                      OpenViBE::CIdentifier(0xF44A9CBD, 0x459DBA69)
#define OVTK_ClassId_FeatureVectorSet                                   OpenViBE::CIdentifier(0x6D5FB77B, 0x5BDD0420)

//___________________________________________________________________//
//                                                                   //
// Operating System identification                                   //
//___________________________________________________________________//
//                                                                   //

// #define OVTK_OS_Linux
// #define OVTK_OS_Windows
// #define OVTK_OS_MacOS
// #define OVTK_OS_

#if defined TARGET_OS_Windows
 #define OVTK_OS_Windows
#elif defined TARGET_OS_Linux
 #define OVTK_OS_Linux
#elif defined TARGET_OS_MacOS
 #define OVTK_OS_MacOS
#else
 #warning "No target operating system defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Hardware Architecture identification                              //
//___________________________________________________________________//
//                                                                   //

// #define OVTK_ARCHITECTURE_i386
// #define OVTK_ARCHITECTURE_

#if defined TARGET_ARCHITECTURE_i386
 #define OVTK_ARCHITECTURE_i386
#else
 #warning "No target architecture defined !"
#endif

//___________________________________________________________________//
//                                                                   //
// Compilator software identification                                //
//___________________________________________________________________//
//                                                                   //

// #define OVTK_COMPILATOR_GCC
// #define OVTK_COMPILATOR_VisualStudio
// #define OVTK_COMPILATOR_

#if defined TARGET_COMPILATOR_GCC
 #define OVTK_COMPILATOR_GCC
#elif defined TARGET_COMPILATOR_VisualStudio
 #define OVTK_COMPILATOR_VisualStudio
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
#if defined OVTK_Shared
 #if defined OVTK_OS_Windows
  #define OVTK_API_Export __declspec(dllexport)
  #define OVTK_API_Import __declspec(dllimport)
 #elif defined OVTK_OS_Linux
  #define OVTK_API_Export __attribute__((visibility("default")))
  #define OVTK_API_Import __attribute__((visibility("default")))
 #else
  #define OVTK_API_Export
  #define OVTK_API_Import
 #endif
#else
 #define OVTK_API_Export
 #define OVTK_API_Import
#endif

#if defined OVTK_Exports
 #define OVTK_API OVTK_API_Export
#else
 #define OVTK_API OVTK_API_Import
#endif

//___________________________________________________________________//
//                                                                   //
// NULL Definition                                                   //
//___________________________________________________________________//
//                                                                   //

#ifndef NULL
#define NULL 0
#endif

#endif // __OpenViBEToolkit_Defines_H__
