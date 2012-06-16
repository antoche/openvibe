#include "openvibe-toolkit/ovtk_all.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEToolkit;

// ********************************************************************************************************************
//                                                                                                                    *
// VIM Replace string to easily add enumeration values :                                                              *
//                                                                                                                    *
// :%s/#define \([A-Za-z0-9_]\+\).*/	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "\1", \1);/g   *
//                                                                                                                    *
// ********************************************************************************************************************

boolean OpenViBEToolkit::initialize(const IKernelContext& rKernelContext)
{
	ITypeManager& l_rTypeManager=rKernelContext.getTypeManager();

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_ExperimentStart", OVTK_StimulationId_ExperimentStart);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_ExperimentStop", OVTK_StimulationId_ExperimentStop);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_SegmentStart", OVTK_StimulationId_SegmentStart);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_SegmentStop", OVTK_StimulationId_SegmentStop);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_TrialStart", OVTK_StimulationId_TrialStart);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_TrialStop", OVTK_StimulationId_TrialStop);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_BaselineStart", OVTK_StimulationId_BaselineStart);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_BaselineStop", OVTK_StimulationId_BaselineStop);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_RestStart", OVTK_StimulationId_RestStart);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_RestStop", OVTK_StimulationId_RestStop);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_VisualStimulationStart", OVTK_StimulationId_VisualStimulationStart);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_VisualStimulationStop", OVTK_StimulationId_VisualStimulationStop);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00", OVTK_StimulationId_Label_00);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01", OVTK_StimulationId_Label_01);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_02", OVTK_StimulationId_Label_02);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_03", OVTK_StimulationId_Label_03);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_04", OVTK_StimulationId_Label_04);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_05", OVTK_StimulationId_Label_05);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_06", OVTK_StimulationId_Label_06);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_07", OVTK_StimulationId_Label_07);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_08", OVTK_StimulationId_Label_08);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_09", OVTK_StimulationId_Label_09);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_0A", OVTK_StimulationId_Label_0A);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_0B", OVTK_StimulationId_Label_0B);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_0C", OVTK_StimulationId_Label_0C);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_0D", OVTK_StimulationId_Label_0D);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_0E", OVTK_StimulationId_Label_0E);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_0F", OVTK_StimulationId_Label_0F);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_10", OVTK_StimulationId_Label_10);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_11", OVTK_StimulationId_Label_11);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_12", OVTK_StimulationId_Label_12);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_13", OVTK_StimulationId_Label_13);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_14", OVTK_StimulationId_Label_14);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_15", OVTK_StimulationId_Label_15);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_16", OVTK_StimulationId_Label_16);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_17", OVTK_StimulationId_Label_17);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_18", OVTK_StimulationId_Label_18);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_19", OVTK_StimulationId_Label_19);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_1A", OVTK_StimulationId_Label_1A);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_1B", OVTK_StimulationId_Label_1B);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_1C", OVTK_StimulationId_Label_1C);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_1D", OVTK_StimulationId_Label_1D);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_1E", OVTK_StimulationId_Label_1E);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Label_1F", OVTK_StimulationId_Label_1F);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Train", OVTK_StimulationId_Train);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Beep", OVTK_StimulationId_Beep);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_DoubleBeep", OVTK_StimulationId_DoubleBeep);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_EndOfFile", OVTK_StimulationId_EndOfFile);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_Target", OVTK_StimulationId_Target);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_NonTarget", OVTK_StimulationId_NonTarget);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_StimulationId_TrainCompleted", OVTK_StimulationId_TrainCompleted);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Artifact_EOG_Large", OVTK_GDF_Artifact_EOG_Large);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Artifact_ECG", OVTK_GDF_Artifact_ECG);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Artifact_EMG", OVTK_GDF_Artifact_EMG);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Artifact_Movement", OVTK_GDF_Artifact_Movement);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Artifact_Failing_Electrode", OVTK_GDF_Artifact_Failing_Electrode);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Artifact_Sweat", OVTK_GDF_Artifact_Sweat);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Artifact_50_60_Hz_Interference", OVTK_GDF_Artifact_50_60_Hz_Interference);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Artifact_Breathing", OVTK_GDF_Artifact_Breathing);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Artifact_Pulse", OVTK_GDF_Artifact_Pulse);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Artifact_EOG_Small", OVTK_GDF_Artifact_EOG_Small);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Calibration", OVTK_GDF_Calibration);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_EEG_Sleep_Splindles", OVTK_GDF_EEG_Sleep_Splindles);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_EEG_K_Complexes", OVTK_GDF_EEG_K_Complexes);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_EEG_Saw_Tooth_Waves", OVTK_GDF_EEG_Saw_Tooth_Waves);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_EEG_Idling_EEG_Eyes_Open", OVTK_GDF_EEG_Idling_EEG_Eyes_Open);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_EEG_Idling_EEG_Eyes_Closed", OVTK_GDF_EEG_Idling_EEG_Eyes_Closed);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_EEG_Spike", OVTK_GDF_EEG_Spike);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_EEG_Seizure", OVTK_GDF_EEG_Seizure);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_VEP", OVTK_GDF_VEP);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_AEP", OVTK_GDF_AEP);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_SEP", OVTK_GDF_SEP);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_TMS", OVTK_GDF_TMS);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_SSVEP", OVTK_GDF_SSVEP);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_SSAEP", OVTK_GDF_SSAEP);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_SSSEP", OVTK_GDF_SSSEP);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Start_Of_Trial", OVTK_GDF_Start_Of_Trial);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Left", OVTK_GDF_Left);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Right", OVTK_GDF_Right);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Foot", OVTK_GDF_Foot);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Tongue", OVTK_GDF_Tongue);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_class5", OVTK_GDF_class5);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Down", OVTK_GDF_Down);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_class7", OVTK_GDF_class7);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_class8", OVTK_GDF_class8);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_class9", OVTK_GDF_class9);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_class10", OVTK_GDF_class10);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_class11", OVTK_GDF_class11);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Up", OVTK_GDF_Up);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Feedback_Continuous", OVTK_GDF_Feedback_Continuous);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Feedback_Discrete", OVTK_GDF_Feedback_Discrete);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Cue_Unknown_Undefined", OVTK_GDF_Cue_Unknown_Undefined);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Beep", OVTK_GDF_Beep);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Cross_On_Screen", OVTK_GDF_Cross_On_Screen);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Flashing_Light", OVTK_GDF_Flashing_Light);
	// SPECIALY ADDED BY YR
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_End_Of_Trial", OVTK_GDF_End_Of_Trial);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Correct", OVTK_GDF_Correct);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Incorrect", OVTK_GDF_Incorrect);
	// SPECIALY ADDED BY YR
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_End_Of_Session", OVTK_GDF_End_Of_Session);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Rejection", OVTK_GDF_Rejection);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_OAHE", OVTK_GDF_OAHE);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_RERA", OVTK_GDF_RERA);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_CAHE", OVTK_GDF_CAHE);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_CSB", OVTK_GDF_CSB);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Sleep_Hypoventilation", OVTK_GDF_Sleep_Hypoventilation);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Maximum_Inspiration", OVTK_GDF_Maximum_Inspiration);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Start_Of_Inspiration", OVTK_GDF_Start_Of_Inspiration);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Wake", OVTK_GDF_Wake);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Stage_1", OVTK_GDF_Stage_1);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Stage_2", OVTK_GDF_Stage_2);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Stage_3", OVTK_GDF_Stage_3);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Stage_4", OVTK_GDF_Stage_4);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_REM", OVTK_GDF_REM);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Lights_On", OVTK_GDF_Lights_On);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Lights_Off", OVTK_GDF_Lights_Off);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Eyes_Left", OVTK_GDF_Eyes_Left);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Eyes_Right", OVTK_GDF_Eyes_Right);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Eyes_Up", OVTK_GDF_Eyes_Up);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Eyes_Down", OVTK_GDF_Eyes_Down);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Horizontal_Eye_Movement", OVTK_GDF_Horizontal_Eye_Movement);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Vertical_Eye_Movement", OVTK_GDF_Vertical_Eye_Movement);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Rotation_Clockwise", OVTK_GDF_Rotation_Clockwise);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Rotation_Counterclockwise", OVTK_GDF_Rotation_Counterclockwise);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Eye_Blink", OVTK_GDF_Eye_Blink);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Left_Hand_Movement", OVTK_GDF_Left_Hand_Movement);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Right_Hand_Movement", OVTK_GDF_Right_Hand_Movement);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Head_Movement", OVTK_GDF_Head_Movement);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Tongue_Movement", OVTK_GDF_Tongue_Movement);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Swallowing", OVTK_GDF_Swallowing);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Biting", OVTK_GDF_Biting);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Foot_Movement", OVTK_GDF_Foot_Movement);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Foot_Right_Movement", OVTK_GDF_Foot_Right_Movement);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Arm_Movement", OVTK_GDF_Arm_Movement);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Arm_Right_Movement", OVTK_GDF_Arm_Right_Movement);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_ECG_Fiducial_Point_QRS_Complex", OVTK_GDF_ECG_Fiducial_Point_QRS_Complex);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_ECG_P_Wave", OVTK_GDF_ECG_P_Wave);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_ECG_QRS_Complex", OVTK_GDF_ECG_QRS_Complex);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_ECG_R_Point", OVTK_GDF_ECG_R_Point);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_ECG_T_Wave", OVTK_GDF_ECG_T_Wave);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_ECG_U_Wave", OVTK_GDF_ECG_U_Wave);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Start", OVTK_GDF_Start);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_25_Watt", OVTK_GDF_25_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_50_Watt", OVTK_GDF_50_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_75_Watt", OVTK_GDF_75_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_100_Watt", OVTK_GDF_100_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_125_Watt", OVTK_GDF_125_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_150_Watt", OVTK_GDF_150_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_175_Watt", OVTK_GDF_175_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_200_Watt", OVTK_GDF_200_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_225_Watt", OVTK_GDF_225_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_250_Watt", OVTK_GDF_250_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_275_Watt", OVTK_GDF_275_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_300_Watt", OVTK_GDF_300_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_325_Watt", OVTK_GDF_325_Watt);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_350_Watt", OVTK_GDF_350_Watt);

	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Start_Of_New_Segment", OVTK_GDF_Start_Of_New_Segment);
	l_rTypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "OVTK_GDF_Non_Equidistant_Sampling_Value", OVTK_GDF_Non_Equidistant_Sampling_Value);

	l_rTypeManager.registerEnumerationType(OVTK_TypeId_ClassificationAlgorithm, "Classification algorithm");

	return true;
}

boolean OpenViBEToolkit::uninitialize(const IKernelContext& rKernelContext)
{
	return true;
}
