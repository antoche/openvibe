#ifndef __OpenViBEPlugins_Global_Defines_H__
#define __OpenViBEPlugins_Global_Defines_H__

// -----------------------------------------------------
// Confusion Matrix Algorithm
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_ConfusionMatrixAlgorithm                                                                       OpenViBE::CIdentifier(0x699f416b, 0x3bae4324)
#define OVP_GD_ClassId_Algorithm_ConfusionMatrixAlgorithmDesc                                                                   OpenViBE::CIdentifier(0x4cdd225d, 0x6c9a59db)
#define OVP_GD_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_ClassificationResults                                        OpenViBE::CIdentifier(0x45220b61, 0x13fd7491)
#define OVP_GD_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_ClassCodes                                                   OpenViBE::CIdentifier(0x67780c91, 0x2a556c51)
#define OVP_GD_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_Sums                                                         OpenViBE::CIdentifier(0x75502e8e, 0x05d838ee)
#define OVP_GD_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_Percentage                                                   OpenViBE::CIdentifier(0x7e504e8e, 0x058858ee)
#define OVP_GD_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_Targets                                                      OpenViBE::CIdentifier(0x7e504e8f, 0x058858ef)
#define OVP_GD_Algorithm_ConfusionMatrixAlgorithm_OutputParameterId_ConfusionMatrix                                             OpenViBE::CIdentifier(0x67780c91, 0x2a556c51)
#define OVP_GD_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_ResetClassifier                                                OpenViBE::CIdentifier(0x3c132c38, 0x557d2503)
#define OVP_GD_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_FeedClassifier                                                 OpenViBE::CIdentifier(0x3efc64b8, 0x5acc3125)
#define OVP_GD_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_ResetTarget                                                    OpenViBE::CIdentifier(0x4d390bda, 0x6a180667)
#define OVP_GD_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_FeedTarget                                                     OpenViBE::CIdentifier(0x6b1e76b3, 0x06741b21)
#define OVP_GD_Algorithm_ConfusionMatrixAlgorithm_OutputTriggerId_ConfusionComputingPerformed                                   OpenViBE::CIdentifier(0x790c2277, 0x3d041a63)

// -----------------------------------------------------
// SVM classifier
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_SVMClassifier                                                                                  OpenViBE::CIdentifier(0x50486ec2, 0x6f2417fc)
#define OVP_GD_ClassId_Algorithm_SVMClassifierDesc                                                                              OpenViBE::CIdentifier(0x272b056e, 0x0c6502ac)
#define OVP_GD_Algorithm_SVMClassifier_InputParameterId_FeatureVectorSet                                                        OpenViBE::CIdentifier(0x27c05927, 0x5de9103a)
#define OVP_GD_Algorithm_SVMClassifier_InputParameterId_FeatureVector                                                           OpenViBE::CIdentifier(0x6d69bf98, 0x1eb9ee66)
#define OVP_GD_Algorithm_SVMClassifier_InputParameterId_Configuration                                                           OpenViBE::CIdentifier(0xa705428e, 0x5bb1cadd)
#define OVP_GD_Algorithm_SVMClassifier_OutputParameterId_Configuration                                                          OpenViBE::CIdentifier(0x30590936, 0x61ce5971)
#define OVP_GD_Algorithm_SVMClassifier_OutputParameterId_Class                                                                  OpenViBE::CIdentifier(0x8a39a7ea, 0xf2ee45c4)
#define OVP_GD_Algorithm_SVMClassifier_OutputParameterId_ClassificationValues                                                   OpenViBE::CIdentifier(0xda77d7e4, 0x766b48ea)
#define OVP_GD_Algorithm_SVMClassifier_InputTriggerId_Train                                                                     OpenViBE::CIdentifier(0x34684752, 0x78a46de2)
#define OVP_GD_Algorithm_SVMClassifier_InputTriggerId_SaveConfiguration                                                         OpenViBE::CIdentifier(0x79750528, 0x6cc85fc1)
#define OVP_GD_Algorithm_SVMClassifier_InputTriggerId_Classify                                                                  OpenViBE::CIdentifier(0x843a87d8, 0x566e85a1)
#define OVP_GD_Algorithm_SVMClassifier_InputTriggerId_LoadConfiguration                                                         OpenViBE::CIdentifier(0xf346bbe0, 0xadafc735)
#define OVP_GD_Algorithm_SVMClassifier_OutputTriggerId_Success                                                                  OpenViBE::CIdentifier(0x24fab755, 0x78868782)
#define OVP_GD_Algorithm_SVMClassifier_OutputTriggerId_Failed                                                                   OpenViBE::CIdentifier(0x6e72b255, 0x317faa04)

// -----------------------------------------------------
// LDA classifier - GPL
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_LDAClassifierGPL                                                                               OpenViBE::CIdentifier(0xd7183fc7, 0xbd74f298)
#define OVP_GD_ClassId_Algorithm_LDAClassifierGPLDesc                                                                           OpenViBE::CIdentifier(0xd42d544a, 0x7a28ddb1)
#define OVP_GD_Algorithm_LDAClassifierGPL_InputParameterId_FeatureVectorSet                                                     OpenViBE::CIdentifier(0x27c05927, 0x5de9103a)
#define OVP_GD_Algorithm_LDAClassifierGPL_InputParameterId_FeatureVector                                                        OpenViBE::CIdentifier(0x6d69bf98, 0x1eb9ee66)
#define OVP_GD_Algorithm_LDAClassifierGPL_InputParameterId_Configuration                                                        OpenViBE::CIdentifier(0xa705428e, 0x5bb1cadd)
#define OVP_GD_Algorithm_LDAClassifierGPL_OutputParameterId_Configuration                                                       OpenViBE::CIdentifier(0x30590936, 0x61ce5971)
#define OVP_GD_Algorithm_LDAClassifierGPL_OutputParameterId_Class                                                               OpenViBE::CIdentifier(0x8a39a7ea, 0xf2ee45c4)
#define OVP_GD_Algorithm_LDAClassifierGPL_OutputParameterId_ClassificationValues                                                OpenViBE::CIdentifier(0xda77d7e4, 0x766b48ea)
#define OVP_GD_Algorithm_LDAClassifierGPL_InputTriggerId_Train                                                                  OpenViBE::CIdentifier(0x34684752, 0x78a46de2)
#define OVP_GD_Algorithm_LDAClassifierGPL_InputTriggerId_SaveConfiguration                                                      OpenViBE::CIdentifier(0x79750528, 0x6cc85fc1)
#define OVP_GD_Algorithm_LDAClassifierGPL_InputTriggerId_Classify                                                               OpenViBE::CIdentifier(0x843a87d8, 0x566e85a1)
#define OVP_GD_Algorithm_LDAClassifierGPL_InputTriggerId_LoadConfiguration                                                      OpenViBE::CIdentifier(0xf346bbe0, 0xadafc735)
#define OVP_GD_Algorithm_LDAClassifierGPL_OutputTriggerId_Success                                                               OpenViBE::CIdentifier(0x24fab755, 0x78868782)
#define OVP_GD_Algorithm_LDAClassifierGPL_OutputTriggerId_Failed                                                                OpenViBE::CIdentifier(0x6e72b255, 0x317faa04)

// -----------------------------------------------------
// Brainamp file reader
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_BrainampFileReader                                                                             OpenViBE::CIdentifier(0x2533718f, 0x07ab3af5)
#define OVP_GD_ClassId_Algorithm_BrainampFileReaderDesc                                                                         OpenViBE::CIdentifier(0x2c5b2d6c, 0x242253cf)
#define OVP_GD_Algorithm_BrainampFileReader_InputParameterId_Filename                                                           OpenViBE::CIdentifier(0x000c486c, 0x477c80b9)
#define OVP_GD_Algorithm_BrainampFileReader_InputParameterId_EpochDuration                                                      OpenViBE::CIdentifier(0x002aee72, 0x288d489e)
#define OVP_GD_Algorithm_BrainampFileReader_InputParameterId_SeekTime                                                           OpenViBE::CIdentifier(0x00627156, 0x55790cae)
#define OVP_GD_Algorithm_BrainampFileReader_OutputParameterId_CurrentStartTime                                                  OpenViBE::CIdentifier(0x003ce019, 0x3dade050)
#define OVP_GD_Algorithm_BrainampFileReader_OutputParameterId_SignalSamples                                                     OpenViBE::CIdentifier(0x0073a91a, 0x6d1d3d26)
#define OVP_GD_Algorithm_BrainampFileReader_OutputParameterId_Stimulations                                                      OpenViBE::CIdentifier(0x008f7c49, 0x6ed710a9)
#define OVP_GD_Algorithm_BrainampFileReader_OutputParameterId_SamplingRate                                                      OpenViBE::CIdentifier(0x00d3cabb, 0x339326c2)
#define OVP_GD_Algorithm_BrainampFileReader_OutputParameterId_CurrentEndTime                                                    OpenViBE::CIdentifier(0x3b87de16, 0xb8efb2fb)
#define OVP_GD_Algorithm_BrainampFileReader_InputTriggerId_Close                                                                OpenViBE::CIdentifier(0x003462ea, 0x031fb8fa)
#define OVP_GD_Algorithm_BrainampFileReader_InputTriggerId_Next                                                                 OpenViBE::CIdentifier(0x003cd062, 0x739f973e)
#define OVP_GD_Algorithm_BrainampFileReader_InputTriggerId_Open                                                                 OpenViBE::CIdentifier(0x00bcf286, 0x6f5fa2f4)
#define OVP_GD_Algorithm_BrainampFileReader_InputTriggerId_Seek                                                                 OpenViBE::CIdentifier(0x00c3acab, 0x4e3de014)
#define OVP_GD_Algorithm_BrainampFileReader_OutputTriggerId_Error                                                               OpenViBE::CIdentifier(0x00b1e3df, 0x3abc6c5a)
#define OVP_GD_Algorithm_BrainampFileReader_OutputTriggerId_DataProduced                                                        OpenViBE::CIdentifier(0x00e7d5f9, 0x1471aff2)

// -----------------------------------------------------
// OpenViBE Matrix file reader
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_OpenViBEMatrixFileReader                                                                       OpenViBE::CIdentifier(0x10661a33, 0x0b0f44a7)
#define OVP_GD_ClassId_Algorithm_OpenViBEMatrixFileReaderDesc                                                                   OpenViBE::CIdentifier(0x0e873b5e, 0x0a287fcb)
#define OVP_GD_Algorithm_OpenViBEMatrixFileReader_InputParameterId_Filename                                                     OpenViBE::CIdentifier(0x28f87b29, 0x0b09737e)
#define OVP_GD_Algorithm_OpenViBEMatrixFileReader_OutputParameterId_Matrix                                                      OpenViBE::CIdentifier(0x2f9521e0, 0x027d789f)

// -----------------------------------------------------
// OpenViBE Matrix file writer
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_OpenViBEMatrixFileWriter                                                                       OpenViBE::CIdentifier(0x739158fc, 0x1e8240cc)
#define OVP_GD_ClassId_Algorithm_OpenViBEMatrixFileWriterDesc                                                                   OpenViBE::CIdentifier(0x44cf6dd0, 0x329d47f9)
#define OVP_GD_Algorithm_OpenViBEMatrixFileWriter_InputParameterId_Filename                                                     OpenViBE::CIdentifier(0x330d2d0b, 0x175271e6)
#define OVP_GD_Algorithm_OpenViBEMatrixFileWriter_InputParameterId_Matrix                                                       OpenViBE::CIdentifier(0x6f6402ee, 0x493044f3)

// -----------------------------------------------------
// XML Scenario exporter
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_XMLScenarioExporter                                                                            OpenViBE::CIdentifier(0x53693531, 0xb136cf3f)
#define OVP_GD_ClassId_Algorithm_XMLScenarioExporterDesc                                                                        OpenViBE::CIdentifier(0x9709c9fa, 0xf126f74e)
#define OVP_GD_Algorithm_XMLScenarioExporter_InputParameterId_Scenario                                                          OpenViBE::CIdentifier(0x5b9c0d54, 0x04ba2957)
#define OVP_GD_Algorithm_XMLScenarioExporter_OutputParameterId_MemoryBuffer                                                     OpenViBE::CIdentifier(0x64030633, 0x419e3a33)

// -----------------------------------------------------
// XML Scenario importer
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_XMLScenarioImporter                                                                            OpenViBE::CIdentifier(0xe80c3ea2, 0x149c4a05)
#define OVP_GD_ClassId_Algorithm_XMLScenarioImporterDesc                                                                        OpenViBE::CIdentifier(0xff25d456, 0x721fcc57)
#define OVP_GD_Algorithm_XMLScenarioImporter_InputParameterId_MemoryBuffer                                                      OpenViBE::CIdentifier(0x600463a3, 0x474b7f66)
#define OVP_GD_Algorithm_XMLScenarioImporter_OutputParameterId_Scenario                                                         OpenViBE::CIdentifier(0x29574c87, 0x7ba77780)

// -----------------------------------------------------
// Stimulation based epoching
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_StimulationBasedEpoching                                                                       OpenViBE::CIdentifier(0x421e3f46, 0x12003e6c)
#define OVP_GD_ClassId_Algorithm_StimulationBasedEpochingDesc                                                                   OpenViBE::CIdentifier(0x2eaf37fc, 0x63195eb8)
#define OVP_GD_Algorithm_StimulationBasedEpoching_InputParameterId_InputSignal                                                  OpenViBE::CIdentifier(0x0ed5c92b, 0xe16bef25)
#define OVP_GD_Algorithm_StimulationBasedEpoching_InputParameterId_OffsetSampleCount                                            OpenViBE::CIdentifier(0x7646ce65, 0xe128fc4e)
#define OVP_GD_Algorithm_StimulationBasedEpoching_OutputParameterId_OutputSignal                                                OpenViBE::CIdentifier(0x00d331a2, 0xc13df043)
#define OVP_GD_Algorithm_StimulationBasedEpoching_InputTriggerId_Reset                                                          OpenViBE::CIdentifier(0x6ba44128, 0x418cf901)
#define OVP_GD_Algorithm_StimulationBasedEpoching_InputTriggerId_PerformEpoching                                                OpenViBE::CIdentifier(0xd05579b5, 0x2649a4b2)
#define OVP_GD_Algorithm_StimulationBasedEpoching_OutputTriggerId_EpochingDone                                                  OpenViBE::CIdentifier(0x755bc3fe, 0x24f7b50f)

// -----------------------------------------------------
// Matrix average
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_MatrixAverage                                                                                  OpenViBE::CIdentifier(0x5e5a6c1c, 0x6f6beb03)
#define OVP_GD_ClassId_Algorithm_MatrixAverageDesc                                                                              OpenViBE::CIdentifier(0x1992881f, 0xc938c0f2)
#define OVP_GD_Algorithm_MatrixAverage_InputParameterId_MatrixCount                                                             OpenViBE::CIdentifier(0x08563191, 0xe78bb265)
#define OVP_GD_Algorithm_MatrixAverage_InputParameterId_Matrix                                                                  OpenViBE::CIdentifier(0x913e9c3b, 0x8a62f5e3)
#define OVP_GD_Algorithm_MatrixAverage_InputParameterId_MovingAverage                                                           OpenViBE::CIdentifier(0xe63cd759, 0xb6ecf6b7)
#define OVP_GD_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix                                                         OpenViBE::CIdentifier(0x03ce5ae5, 0xbd9031e0)
#define OVP_GD_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix                                                                OpenViBE::CIdentifier(0x50b6ee87, 0xdc42e660)
#define OVP_GD_Algorithm_MatrixAverage_InputTriggerId_Reset                                                                     OpenViBE::CIdentifier(0x670ec053, 0xadfe3f5c)
#define OVP_GD_Algorithm_MatrixAverage_InputTriggerId_ForceAverage                                                              OpenViBE::CIdentifier(0xbf597839, 0xcd6039f0)
#define OVP_GD_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed                                                         OpenViBE::CIdentifier(0x2bff029b, 0xd932a613)

// -----------------------------------------------------
// Signal Statistic
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_SignalStatistic                                                                                OpenViBE::CIdentifier(0x07a71212, 0x53d93d1c)
#define OVP_GD_ClassId_Algorithm_SignalStatisticDesc                                                                            OpenViBE::CIdentifier(0x408157f7, 0x4f1209f7)
#define OVP_GD_Algorithm_SignalStatistic_InputParameterId_PercentileValue                                                       OpenViBE::CIdentifier(0x0cb41979, 0x1cff5a9c)
#define OVP_GD_Algorithm_SignalStatistic_InputParameterId_MatrixInput                                                           OpenViBE::CIdentifier(0x1769269c, 0x41910db9)
#define OVP_GD_Algorithm_SignalStatistic_InputParameterId_ActiveVariance                                                        OpenViBE::CIdentifier(0x304b052d, 0x04f51601)
#define OVP_GD_Algorithm_SignalStatistic_InputParameterId_ActivePercentile                                                      OpenViBE::CIdentifier(0x3ca94023, 0x44e450c6)
#define OVP_GD_Algorithm_SignalStatistic_InputParameterId_ActiveRange                                                           OpenViBE::CIdentifier(0x4ea54a91, 0x69b90629)
#define OVP_GD_Algorithm_SignalStatistic_InputParameterId_ActiveIQR                                                             OpenViBE::CIdentifier(0x4f99672c, 0x7dff3192)
#define OVP_GD_Algorithm_SignalStatistic_InputParameterId_ActiveMedian                                                          OpenViBE::CIdentifier(0x6b0f55f1, 0x30015b5b)
#define OVP_GD_Algorithm_SignalStatistic_InputParameterId_ActiveMean                                                            OpenViBE::CIdentifier(0x6ce22614, 0x3bfd4a7a)
#define OVP_GD_Algorithm_SignalStatistic_OutputParameterId_CompressionRatio                                                     OpenViBE::CIdentifier(0x2a9c502c, 0x582959da)
#define OVP_GD_Algorithm_SignalStatistic_OutputParameterId_MedianOutput                                                         OpenViBE::CIdentifier(0x2b236d6c, 0x4a37734f)
#define OVP_GD_Algorithm_SignalStatistic_OutputParameterId_MeanOutput                                                           OpenViBE::CIdentifier(0x2e1e6a87, 0x17f37568)
#define OVP_GD_Algorithm_SignalStatistic_OutputParameterId_RangeOutput                                                          OpenViBE::CIdentifier(0x3cbc7d63, 0x5bf90946)
#define OVP_GD_Algorithm_SignalStatistic_OutputParameterId_VarianceOutput                                                       OpenViBE::CIdentifier(0x479e18c9, 0x34a561ac)
#define OVP_GD_Algorithm_SignalStatistic_OutputParameterId_PercentileOutput                                                     OpenViBE::CIdentifier(0x77443bef, 0x687b139f)
#define OVP_GD_Algorithm_SignalStatistic_OutputParameterId_InterQuantileRangeOutput                                             OpenViBE::CIdentifier(0x7a4e5c6e, 0x16ea324e)
#define OVP_GD_Algorithm_SignalStatistic_InputTriggerId_Initialize                                                              OpenViBE::CIdentifier(0x42cc2481, 0x70300f6d)
#define OVP_GD_Algorithm_SignalStatistic_InputTriggerId_Process                                                                 OpenViBE::CIdentifier(0x6ccd1d92, 0x02043c21)
#define OVP_GD_Algorithm_SignalStatistic_OutputTriggerId_Done                                                                   OpenViBE::CIdentifier(0x34630103, 0x3f5f0a43)

// -----------------------------------------------------
// Compute Filter Coefficients
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_ComputeFilterCoefficients                                                                      OpenViBE::CIdentifier(0x55bad77b, 0x5d8563a7)
#define OVP_GD_ClassId_Algorithm_ComputeFilterCoefficientsDesc                                                                  OpenViBE::CIdentifier(0xd871bd98, 0x705ed068)
#define OVP_GD_Algorithm_ComputeFilterCoefficients_InputParameterId_FilterType                                                  OpenViBE::CIdentifier(0x1b7bcb2c, 0xe235a6e7)
#define OVP_GD_Algorithm_ComputeFilterCoefficients_InputParameterId_SamplingFrequency                                           OpenViBE::CIdentifier(0x25a9a0ff, 0x168f1b50)
#define OVP_GD_Algorithm_ComputeFilterCoefficients_InputParameterId_LowCutFrequency                                             OpenViBE::CIdentifier(0x3175b774, 0xa15aeeb2)
#define OVP_GD_Algorithm_ComputeFilterCoefficients_InputParameterId_FilterOrder                                                 OpenViBE::CIdentifier(0x8da1e555, 0x17e17828)
#define OVP_GD_Algorithm_ComputeFilterCoefficients_InputParameterId_BandPassRipple                                              OpenViBE::CIdentifier(0xb1500ed4, 0x0e558759)
#define OVP_GD_Algorithm_ComputeFilterCoefficients_InputParameterId_FilterMethod                                                OpenViBE::CIdentifier(0xcfb7cdc9, 0x3eff788e)
#define OVP_GD_Algorithm_ComputeFilterCoefficients_InputParameterId_HighCutFrequency                                            OpenViBE::CIdentifier(0xe36387b7, 0xfb766612)
#define OVP_GD_Algorithm_ComputeFilterCoefficients_OutputParameterId_Matrix                                                     OpenViBE::CIdentifier(0xe5b2a753, 0x150500b4)
#define OVP_GD_Algorithm_ComputeFilterCoefficients_InputTriggerId_ComputeCoefficients                                           OpenViBE::CIdentifier(0x053a2c6e, 0x3a878825)
#define OVP_GD_Algorithm_ComputeFilterCoefficients_InputTriggerId_Initialize                                                    OpenViBE::CIdentifier(0x3d2cba61, 0x3fcf0dac)

// -----------------------------------------------------
// Apply Temporal Filter
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_ApplyTemporalFilter                                                                            OpenViBE::CIdentifier(0x916b558f, 0xe501a2f4)
#define OVP_GD_ClassId_Algorithm_ApplyTemporalFilterDesc                                                                        OpenViBE::CIdentifier(0xccbd104b, 0xac1c7d3e)
#define OVP_GD_Algorithm_ApplyTemporalFilter_InputParameterId_FilterCoefficientsMatrix                                          OpenViBE::CIdentifier(0xd316c4e7, 0xe4e89fd3)
#define OVP_GD_Algorithm_ApplyTemporalFilter_InputParameterId_SignalMatrix                                                      OpenViBE::CIdentifier(0xd5339105, 0x1d1293f0)
#define OVP_GD_Algorithm_ApplyTemporalFilter_OutputParameterId_FilteredSignalMatrix                                             OpenViBE::CIdentifier(0x463276d1, 0xeaee8aad)
#define OVP_GD_Algorithm_ApplyTemporalFilter_InputTriggerId_Initialize                                                          OpenViBE::CIdentifier(0x3dae69c7, 0x7cfcbe2c)
#define OVP_GD_Algorithm_ApplyTemporalFilter_InputTriggerId_ApplyFilterWithHistoric                                             OpenViBE::CIdentifier(0xb7b7d546, 0x6000ff51)
#define OVP_GD_Algorithm_ApplyTemporalFilter_InputTriggerId_ApplyFilter                                                         OpenViBE::CIdentifier(0xbc1f5655, 0x9807b400)

// -----------------------------------------------------
// Downsampling
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_Downsampling                                                                                   OpenViBE::CIdentifier(0xbbbb4e18, 0x17695604)
#define OVP_GD_ClassId_Algorithm_DownsamplingDesc                                                                               OpenViBE::CIdentifier(0xc08ba8c1, 0x3a3b6e26)
#define OVP_GD_Algorithm_Downsampling_InputParameterId_SamplingFrequency                                                        OpenViBE::CIdentifier(0x7c510afb, 0x4f2b9fb7)
#define OVP_GD_Algorithm_Downsampling_InputParameterId_NewSamplingFrequency                                                     OpenViBE::CIdentifier(0x8617e5fa, 0xc39cdbe7)
#define OVP_GD_Algorithm_Downsampling_InputParameterId_SignalMatrix                                                             OpenViBE::CIdentifier(0xbb09054a, 0xef13b2c6)
#define OVP_GD_Algorithm_Downsampling_OutputParameterId_SignalMatrix                                                            OpenViBE::CIdentifier(0x4b9be135, 0x14c10757)
#define OVP_GD_Algorithm_Downsampling_InputTriggerId_Resample                                                                   OpenViBE::CIdentifier(0x2a88aff5, 0x79ecaeb3)
#define OVP_GD_Algorithm_Downsampling_InputTriggerId_Initialize                                                                 OpenViBE::CIdentifier(0x82d96f84, 0x9479a701)
#define OVP_GD_Algorithm_Downsampling_InputTriggerId_ResampleWithHistoric                                                       OpenViBE::CIdentifier(0xd5740b33, 0x3785c886)

// -----------------------------------------------------
// Compute Fisher Lda function
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_ComputeFisherLdaFunction                                                                       OpenViBE::CIdentifier(0x124ac7d5, 0x39b975e2)
#define OVP_GD_ClassId_Algorithm_ComputeFisherLdaFunctionDesc                                                                   OpenViBE::CIdentifier(0x0ece9ff0, 0xba40f306)
#define OVP_GD_Algorithm_ComputeFisherLdaFunction_InputParameterId_MatrixFirstClass                                             OpenViBE::CIdentifier(0x639846f9, 0x6637445e)
#define OVP_GD_Algorithm_ComputeFisherLdaFunction_InputParameterId_MatrixSecondClass                                            OpenViBE::CIdentifier(0xe438158e, 0xb31603f2)
#define OVP_GD_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalCovariance                                      OpenViBE::CIdentifier(0x6027e2d7, 0x878b5931)
#define OVP_GD_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalMean                                            OpenViBE::CIdentifier(0x8278cf64, 0x25804b86)
#define OVP_GD_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalProbability                                     OpenViBE::CIdentifier(0xb568892e, 0x81020300)
#define OVP_GD_Algorithm_ComputeFisherLdaFunction_InputTriggerId_Initialize                                                     OpenViBE::CIdentifier(0x0c320928, 0x6b2c5285)
#define OVP_GD_Algorithm_ComputeFisherLdaFunction_InputTriggerId_ComputeFunction                                                OpenViBE::CIdentifier(0xccd22c15, 0xb49f6aad)

// -----------------------------------------------------
// Apply Fisher Lda function
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_ApplyFisherLdaFunction                                                                         OpenViBE::CIdentifier(0xfca53c1a, 0xc4024d00)
#define OVP_GD_ClassId_Algorithm_ApplyFisherLdaFunctionDesc                                                                     OpenViBE::CIdentifier(0xf4d7a3fe, 0x52d8f1eb)
#define OVP_GD_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixSignal                                                   OpenViBE::CIdentifier(0x505bb8bc, 0xdffb05d8)
#define OVP_GD_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalProbability                                        OpenViBE::CIdentifier(0xe791f510, 0x6bd004e7)
#define OVP_GD_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalCovariance                                         OpenViBE::CIdentifier(0xf07b6450, 0x8c00b24b)
#define OVP_GD_Algorithm_ApplyFisherLdaFunction_InputParameterId_MatrixGlobalMean                                               OpenViBE::CIdentifier(0xfd147af9, 0xc8febf3c)
#define OVP_GD_Algorithm_ApplyFisherLdaFunction_OutputParameterId_FisherLdaDecision                                             OpenViBE::CIdentifier(0xe347ee5a, 0xa74645f1)
#define OVP_GD_Algorithm_ApplyFisherLdaFunction_InputTriggerId_ApplyFunction                                                    OpenViBE::CIdentifier(0xa07276db, 0x9e775318)
#define OVP_GD_Algorithm_ApplyFisherLdaFunction_InputTriggerId_Initialize                                                       OpenViBE::CIdentifier(0xc84d2ff2, 0x66228a70)

// -----------------------------------------------------
// Features Extractor for Lda training
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_FeaturesExtractorForLdaTraining                                                                OpenViBE::CIdentifier(0xd1aa6c72, 0x0432e1c5)
#define OVP_GD_ClassId_Algorithm_FeaturesExtractorForLdaTrainingDesc                                                            OpenViBE::CIdentifier(0x2109d46a, 0x9a45e1dc)
#define OVP_GD_Algorithm_FeaturesExtractorForLdaTraining_InputParameterId_Class1SamplesNumber                                   OpenViBE::CIdentifier(0x7f99fbc6, 0x748b183b)
#define OVP_GD_Algorithm_FeaturesExtractorForLdaTraining_InputParameterId_EpochTable                                            OpenViBE::CIdentifier(0xa7be9ae6, 0xd09fad58)
#define OVP_GD_Algorithm_FeaturesExtractorForLdaTraining_OutputParameterId_MatrixSecondClass                                    OpenViBE::CIdentifier(0x0af9008a, 0xe7a7b2b7)
#define OVP_GD_Algorithm_FeaturesExtractorForLdaTraining_OutputParameterId_MatrixFirstClass                                     OpenViBE::CIdentifier(0x76f84603, 0x1f5d7a10)
#define OVP_GD_Algorithm_FeaturesExtractorForLdaTraining_InputTriggerId_Initialize                                              OpenViBE::CIdentifier(0x06176684, 0x5fa2c7c7)
#define OVP_GD_Algorithm_FeaturesExtractorForLdaTraining_InputTriggerId_ExtractFeatures                                         OpenViBE::CIdentifier(0x3b505f2f, 0x024b2c52)

// -----------------------------------------------------
// Detects Min or Max of input buffer
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_DetectsMinOrMaxOfInputBuffer                                                                   OpenViBE::CIdentifier(0x46c14a64, 0xe00541dd)
#define OVP_GD_ClassId_Algorithm_DetectsMinOrMaxOfInputBufferDesc                                                               OpenViBE::CIdentifier(0x5b194cda, 0x54e6dec7)
#define OVP_GD_Algorithm_DetectsMinOrMaxOfInputBuffer_InputParameterId_SamplingFrequency                                        OpenViBE::CIdentifier(0x8519915d, 0xb6be506d)
#define OVP_GD_Algorithm_DetectsMinOrMaxOfInputBuffer_InputParameterId_SignalInputMatrix                                        OpenViBE::CIdentifier(0x9ca3b6bb, 0x6e24a3e3)
#define OVP_GD_Algorithm_DetectsMinOrMaxOfInputBuffer_InputParameterId_TimeWindowEnd                                            OpenViBE::CIdentifier(0x9f55a091, 0xa042e9c0)
#define OVP_GD_Algorithm_DetectsMinOrMaxOfInputBuffer_InputParameterId_TimeWindowStart                                          OpenViBE::CIdentifier(0xb3ded659, 0xd8a85cfa)
#define OVP_GD_Algorithm_DetectsMinOrMaxOfInputBuffer_OutputParameterId_SignalOutputMatrix                                      OpenViBE::CIdentifier(0x853f2de5, 0x628237ce)
#define OVP_GD_Algorithm_DetectsMinOrMaxOfInputBuffer_InputTriggerId_DetectsMax                                                 OpenViBE::CIdentifier(0x24926194, 0x086e6c2a)
#define OVP_GD_Algorithm_DetectsMinOrMaxOfInputBuffer_InputTriggerId_Initialize                                                 OpenViBE::CIdentifier(0x6b43b69d, 0xda1eae30)
#define OVP_GD_Algorithm_DetectsMinOrMaxOfInputBuffer_InputTriggerId_DetectsMin                                                 OpenViBE::CIdentifier(0xfcb3cfc2, 0x980e3085)

// -----------------------------------------------------
// Apply Linear Naive Bayes function
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_ApplyLinearNaiveBayesFunction                                                                  OpenViBE::CIdentifier(0xf9c21d2c, 0x9e6abe8e)
#define OVP_GD_ClassId_Algorithm_ApplyLinearNaiveBayesFunctionDesc                                                              OpenViBE::CIdentifier(0xcde5efeb, 0x0689c756)
#define OVP_GD_Algorithm_ApplyLinearNaiveBayesFunction_InputParameterId_MatrixLogTerm                                           OpenViBE::CIdentifier(0xb10d8b3c, 0x2a1d3705)
#define OVP_GD_Algorithm_ApplyLinearNaiveBayesFunction_InputParameterId_MatrixMean                                              OpenViBE::CIdentifier(0xdd1479ac, 0xb2e13471)
#define OVP_GD_Algorithm_ApplyLinearNaiveBayesFunction_InputParameterId_MatrixVariance                                          OpenViBE::CIdentifier(0xeb3cd401, 0x54e4692d)
#define OVP_GD_Algorithm_ApplyLinearNaiveBayesFunction_InputParameterId_MatrixSignal                                            OpenViBE::CIdentifier(0xf1d46621, 0x9cae8931)
#define OVP_GD_Algorithm_ApplyLinearNaiveBayesFunction_OutputParameterId_MatrixClassFunctional                                  OpenViBE::CIdentifier(0x57a78d33, 0x724ce7f9)
#define OVP_GD_Algorithm_ApplyLinearNaiveBayesFunction_InputTriggerId_Initialize                                                OpenViBE::CIdentifier(0x0b3f7d98, 0x70d9d6e6)
#define OVP_GD_Algorithm_ApplyLinearNaiveBayesFunction_InputTriggerId_ApplyFunction                                             OpenViBE::CIdentifier(0x622d05b7, 0x002f601f)

// -----------------------------------------------------
// Level measure
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_LevelMeasure                                                                                   OpenViBE::CIdentifier(0x63c71764, 0x34a9717f)
#define OVP_GD_ClassId_Algorithm_LevelMeasureDesc                                                                               OpenViBE::CIdentifier(0x3eb6754f, 0x22fb1722)
#define OVP_GD_Algorithm_LevelMeasure_InputParameterId_Matrix                                                                   OpenViBE::CIdentifier(0x59430053, 0x67c23a83)
#define OVP_GD_Algorithm_LevelMeasure_OutputParameterId_MainWidget                                                              OpenViBE::CIdentifier(0x101c4641, 0x466c71e3)
#define OVP_GD_Algorithm_LevelMeasure_OutputParameterId_ToolbarWidget                                                           OpenViBE::CIdentifier(0x14905ffc, 0x6fe425b2)
#define OVP_GD_Algorithm_LevelMeasure_InputTriggerId_Reset                                                                      OpenViBE::CIdentifier(0x3eaf36c5, 0x74490c56)
#define OVP_GD_Algorithm_LevelMeasure_InputTriggerId_Refresh                                                                    OpenViBE::CIdentifier(0x71356fe4, 0x3e8f62dc)
#define OVP_GD_Algorithm_LevelMeasure_OutputTriggerId_Refreshed                                                                 OpenViBE::CIdentifier(0x3c3c1b06, 0x360305d9)

// -----------------------------------------------------
// Spherical spline interpolation
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_SphericalSplineInterpolation                                                                   OpenViBE::CIdentifier(0x4f112803, 0x661d4029)
#define OVP_GD_ClassId_Algorithm_SphericalSplineInterpolationDesc                                                               OpenViBE::CIdentifier(0x00d67a20, 0x3d3d4729)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_InputParameterId_CoordinatesWhereToInterpolateValues                      OpenViBE::CIdentifier(0x280a531d, 0x339c18aa)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_InputParameterId_NumberOfValues                                           OpenViBE::CIdentifier(0x2abf11fc, 0x174a2cfe)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_InputParameterId_ValuesCoordinates                                        OpenViBE::CIdentifier(0x36f743fe, 0x37897ab9)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_InputParameterId_SplineOrder                                              OpenViBE::CIdentifier(0x3b8200f6, 0x205162c7)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_InputParameterId_Values                                                   OpenViBE::CIdentifier(0x4ea55599, 0x670274a7)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_OutputParameterId_MinInterpolatedValue                                    OpenViBE::CIdentifier(0x0cee2041, 0x79455eed)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_OutputParameterId_InterpolatedValues                                      OpenViBE::CIdentifier(0x12d0319c, 0x51ed4d8b)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_OutputParameterId_MaxInterpolatedValue                                    OpenViBE::CIdentifier(0x1ecb03e3, 0x40ef757f)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_InputTriggerId_InterpolationUsingLaplacianCoefficients                    OpenViBE::CIdentifier(0x11ce0ac3, 0x0fd85469)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_InputTriggerId_InterpolationUsingSplineCoefficients                       OpenViBE::CIdentifier(0x1241610e, 0x03cb1ad9)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_InputTriggerId_Precomputation                                             OpenViBE::CIdentifier(0x42a650da, 0x62b35f76)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_InputTriggerId_SplineCoefficientsComputation                              OpenViBE::CIdentifier(0x5b353712, 0x069f3d3b)
#define OVP_GD_Algorithm_SphericalSplineInterpolation_InputTriggerId_LaplacianCoefficientsComputation                           OpenViBE::CIdentifier(0x7d8c545e, 0x7c086660)

// -----------------------------------------------------
// Acquisition stream decoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_AcquisitionStreamDecoder                                                                       OpenViBE::CIdentifier(0x1e0812b7, 0x3f686dd4)
#define OVP_GD_ClassId_Algorithm_AcquisitionStreamDecoderDesc                                                                   OpenViBE::CIdentifier(0xa01599b0, 0x7f51631a)
#define OVP_GD_Algorithm_AcquisitionStreamDecoder_InputParameterId_MemoryBufferToDecode                                         OpenViBE::CIdentifier(0x2f98ea3c, 0xfb0be096)
#define OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_StimulationStream                                           OpenViBE::CIdentifier(0x08fc3c12, 0x86a07bf7)
#define OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_SignalStream                                                OpenViBE::CIdentifier(0x42c0d7bd, 0xbbcea3f3)
#define OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelLocalisationStream                                   OpenViBE::CIdentifier(0x4eb92f81, 0x6ecda6b9)
#define OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_BufferDuration                                              OpenViBE::CIdentifier(0x7527d6e5, 0xb7a70339)
#define OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ExperimentInformationStream                                 OpenViBE::CIdentifier(0xa7f1d539, 0xec708539)
#define OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedHeader                                                OpenViBE::CIdentifier(0x815234bf, 0xaabae5f2)
#define OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedBuffer                                                OpenViBE::CIdentifier(0xaa2738bf, 0xf7fe9fc3)
#define OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedEnd                                                   OpenViBE::CIdentifier(0xc4aa114c, 0x628c2d77)

// -----------------------------------------------------
// Channel localisation stream decoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_ChannelLocalisationStreamDecoder                                                               OpenViBE::CIdentifier(0x8222f065, 0xb05d35cf)
#define OVP_GD_ClassId_Algorithm_ChannelLocalisationStreamDecoderDesc                                                           OpenViBE::CIdentifier(0x713a29fd, 0xa5a95e2c)
#define OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_InputParameterId_MemoryBufferToDecode                                 OpenViBE::CIdentifier(0x2f98ea3c, 0xfb0be096)
#define OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputParameterId_Matrix                                              OpenViBE::CIdentifier(0x79ef3123, 0x35e3ea4d)
#define OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputParameterId_Dynamic                                             OpenViBE::CIdentifier(0xd20991fd, 0xa3153651)
#define OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputTriggerId_ReceivedHeader                                        OpenViBE::CIdentifier(0x815234bf, 0xaabae5f2)
#define OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputTriggerId_ReceivedBuffer                                        OpenViBE::CIdentifier(0xaa2738bf, 0xf7fe9fc3)
#define OVP_GD_Algorithm_ChannelLocalisationStreamDecoder_OutputTriggerId_ReceivedEnd                                           OpenViBE::CIdentifier(0xc4aa114c, 0x628c2d77)

// -----------------------------------------------------
// Experiment information stream decoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_ExperimentInformationStreamDecoder                                                             OpenViBE::CIdentifier(0x6fa7d52b, 0x80e2abd6)
#define OVP_GD_ClassId_Algorithm_ExperimentInformationStreamDecoderDesc                                                         OpenViBE::CIdentifier(0x0f37ca61, 0x8a77f44e)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_InputParameterId_MemoryBufferToDecode                               OpenViBE::CIdentifier(0x2f98ea3c, 0xfb0be096)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectName                                       OpenViBE::CIdentifier(0x3d3826ea, 0xe8883815)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentIdentifier                              OpenViBE::CIdentifier(0x40259641, 0x478c73de)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryName                                    OpenViBE::CIdentifier(0x5ca80fa5, 0x774f01cb)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectGender                                     OpenViBE::CIdentifier(0x7d5059e8, 0xe4d8b38d)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectIdentifier                                 OpenViBE::CIdentifier(0x97c5d20d, 0x203e65b3)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianName                                    OpenViBE::CIdentifier(0xb8a94b68, 0x389393d9)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentDate                                    OpenViBE::CIdentifier(0xbc0266a2, 0x9c2935f1)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectAge                                        OpenViBE::CIdentifier(0xc36c6b08, 0x5227380a)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianIdentifier                              OpenViBE::CIdentifier(0xc8ecfbbc, 0x0dcda310)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryIdentifier                              OpenViBE::CIdentifier(0xe761d3d4, 0x44ba1ebf)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputTriggerId_ReceivedHeader                                      OpenViBE::CIdentifier(0x815234bf, 0xaabae5f2)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputTriggerId_ReceivedBuffer                                      OpenViBE::CIdentifier(0xaa2738bf, 0xf7fe9fc3)
#define OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputTriggerId_ReceivedEnd                                         OpenViBE::CIdentifier(0xc4aa114c, 0x628c2d77)

// -----------------------------------------------------
// Feature vector stream decoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_FeatureVectorStreamDecoder                                                                     OpenViBE::CIdentifier(0xc2689ecc, 0x43b335c1)
#define OVP_GD_ClassId_Algorithm_FeatureVectorStreamDecoderDesc                                                                 OpenViBE::CIdentifier(0xab0ae561, 0xf181e34f)
#define OVP_GD_Algorithm_FeatureVectorStreamDecoder_InputParameterId_MemoryBufferToDecode                                       OpenViBE::CIdentifier(0x2f98ea3c, 0xfb0be096)
#define OVP_GD_Algorithm_FeatureVectorStreamDecoder_OutputParameterId_Matrix                                                    OpenViBE::CIdentifier(0x79ef3123, 0x35e3ea4d)
#define OVP_GD_Algorithm_FeatureVectorStreamDecoder_OutputTriggerId_ReceivedHeader                                              OpenViBE::CIdentifier(0x815234bf, 0xaabae5f2)
#define OVP_GD_Algorithm_FeatureVectorStreamDecoder_OutputTriggerId_ReceivedBuffer                                              OpenViBE::CIdentifier(0xaa2738bf, 0xf7fe9fc3)
#define OVP_GD_Algorithm_FeatureVectorStreamDecoder_OutputTriggerId_ReceivedEnd                                                 OpenViBE::CIdentifier(0xc4aa114c, 0x628c2d77)

// -----------------------------------------------------
// Signal stream decoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_SignalStreamDecoder                                                                            OpenViBE::CIdentifier(0x7237c149, 0x0ca66da7)
#define OVP_GD_ClassId_Algorithm_SignalStreamDecoderDesc                                                                        OpenViBE::CIdentifier(0xf1547d89, 0x49ffd0c2)
#define OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode                                              OpenViBE::CIdentifier(0x2f98ea3c, 0xfb0be096)
#define OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate                                                     OpenViBE::CIdentifier(0x363d8d79, 0xeefb912c)
#define OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix                                                           OpenViBE::CIdentifier(0x79ef3123, 0x35e3ea4d)
#define OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader                                                     OpenViBE::CIdentifier(0x815234bf, 0xaabae5f2)
#define OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer                                                     OpenViBE::CIdentifier(0xaa2738bf, 0xf7fe9fc3)
#define OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd                                                        OpenViBE::CIdentifier(0xc4aa114c, 0x628c2d77)

// -----------------------------------------------------
// Spectrum stream decoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder                                                                          OpenViBE::CIdentifier(0x128202db, 0x449fc7a6)
#define OVP_GD_ClassId_Algorithm_SpectrumStreamDecoderDesc                                                                      OpenViBE::CIdentifier(0x54d18ee8, 0x5dbd913a)
#define OVP_GD_Algorithm_SpectrumStreamDecoder_InputParameterId_MemoryBufferToDecode                                            OpenViBE::CIdentifier(0x2f98ea3c, 0xfb0be096)
#define OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands                                           OpenViBE::CIdentifier(0x375e55e9, 0x9b3666f6)
#define OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_Matrix                                                         OpenViBE::CIdentifier(0x79ef3123, 0x35e3ea4d)
#define OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedHeader                                                   OpenViBE::CIdentifier(0x815234bf, 0xaabae5f2)
#define OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedBuffer                                                   OpenViBE::CIdentifier(0xaa2738bf, 0xf7fe9fc3)
#define OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedEnd                                                      OpenViBE::CIdentifier(0xc4aa114c, 0x628c2d77)

// -----------------------------------------------------
// Stimulation stream decoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_StimulationStreamDecoder                                                                       OpenViBE::CIdentifier(0xc8807f2b, 0x0813c5b1)
#define OVP_GD_ClassId_Algorithm_StimulationStreamDecoderDesc                                                                   OpenViBE::CIdentifier(0x391a615b, 0x71cd888a)
#define OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode                                         OpenViBE::CIdentifier(0x2f98ea3c, 0xfb0be096)
#define OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet                                              OpenViBE::CIdentifier(0xf46d0c19, 0x47306bea)
#define OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader                                                OpenViBE::CIdentifier(0x815234bf, 0xaabae5f2)
#define OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer                                                OpenViBE::CIdentifier(0xaa2738bf, 0xf7fe9fc3)
#define OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd                                                   OpenViBE::CIdentifier(0xc4aa114c, 0x628c2d77)

// -----------------------------------------------------
// Streamed matrix stream decoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder                                                                    OpenViBE::CIdentifier(0x7359d0db, 0x91784b21)
#define OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoderDesc                                                                OpenViBE::CIdentifier(0x384529d5, 0xd8e0a728)
#define OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode                                      OpenViBE::CIdentifier(0x2f98ea3c, 0xfb0be096)
#define OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix                                                   OpenViBE::CIdentifier(0x79ef3123, 0x35e3ea4d)
#define OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader                                             OpenViBE::CIdentifier(0x815234bf, 0xaabae5f2)
#define OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer                                             OpenViBE::CIdentifier(0xaa2738bf, 0xf7fe9fc3)
#define OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd                                                OpenViBE::CIdentifier(0xc4aa114c, 0x628c2d77)

// -----------------------------------------------------
// Acquisition stream encoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_AcquisitionStreamEncoder                                                                       OpenViBE::CIdentifier(0xf9fd2fb5, 0xdf0b3b2c)
#define OVP_GD_ClassId_Algorithm_AcquisitionStreamEncoderDesc                                                                   OpenViBE::CIdentifier(0xe3e0d9eb, 0x4d4eba00)
#define OVP_GD_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelLocalisationStream                                    OpenViBE::CIdentifier(0x2cf786e5, 0x520714a1)
#define OVP_GD_Algorithm_AcquisitionStreamEncoder_InputParameterId_ExperimentInformationStream                                  OpenViBE::CIdentifier(0x38755128, 0xcb0c908a)
#define OVP_GD_Algorithm_AcquisitionStreamEncoder_InputParameterId_SignalStream                                                 OpenViBE::CIdentifier(0x4ed9d929, 0x6df5b2b6)
#define OVP_GD_Algorithm_AcquisitionStreamEncoder_InputParameterId_BufferDuration                                               OpenViBE::CIdentifier(0xafa07097, 0x1145b59b)
#define OVP_GD_Algorithm_AcquisitionStreamEncoder_InputParameterId_StimulationStream                                            OpenViBE::CIdentifier(0xcde202ad, 0xf4864ec9)
#define OVP_GD_Algorithm_AcquisitionStreamEncoder_OutputParameterId_EncodedMemoryBuffer                                         OpenViBE::CIdentifier(0xa3d8b171, 0xf8734734)
#define OVP_GD_Algorithm_AcquisitionStreamEncoder_InputTriggerId_EncodeBuffer                                                   OpenViBE::CIdentifier(0x1b7076fd, 0x449bc70a)
#define OVP_GD_Algorithm_AcquisitionStreamEncoder_InputTriggerId_EncodeEnd                                                      OpenViBE::CIdentifier(0x3fc23508, 0x806753d8)
#define OVP_GD_Algorithm_AcquisitionStreamEncoder_InputTriggerId_EncodeHeader                                                   OpenViBE::CIdentifier(0x878eaf60, 0xf9d5303f)

// -----------------------------------------------------
// Experiment information stream encoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_ExperimentInformationStreamEncoder                                                             OpenViBE::CIdentifier(0x56b354fe, 0xbf175468)
#define OVP_GD_ClassId_Algorithm_ExperimentInformationStreamEncoderDesc                                                         OpenViBE::CIdentifier(0x8cc2c754, 0x61665fda)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectName                                        OpenViBE::CIdentifier(0x3d3826ea, 0xe8883815)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentIdentifier                               OpenViBE::CIdentifier(0x40259641, 0x478c73de)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryName                                     OpenViBE::CIdentifier(0x5ca80fa5, 0x774f01cb)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectGender                                      OpenViBE::CIdentifier(0x7d5059e8, 0xe4d8b38d)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectIdentifier                                  OpenViBE::CIdentifier(0x97c5d20d, 0x203e65b3)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianName                                     OpenViBE::CIdentifier(0xb8a94b68, 0x389393d9)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentDate                                     OpenViBE::CIdentifier(0xbc0266a2, 0x9c2935f1)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectAge                                         OpenViBE::CIdentifier(0xc36c6b08, 0x5227380a)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianIdentifier                               OpenViBE::CIdentifier(0xc8ecfbbc, 0x0dcda310)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryIdentifier                               OpenViBE::CIdentifier(0xe761d3d4, 0x44ba1ebf)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_OutputParameterId_EncodedMemoryBuffer                               OpenViBE::CIdentifier(0xa3d8b171, 0xf8734734)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeBuffer                                         OpenViBE::CIdentifier(0x1b7076fd, 0x449bc70a)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeEnd                                            OpenViBE::CIdentifier(0x3fc23508, 0x806753d8)
#define OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeHeader                                         OpenViBE::CIdentifier(0x878eaf60, 0xf9d5303f)

// -----------------------------------------------------
// Channel localisation stream encoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_ChannelLocalisationStreamEncoder                                                               OpenViBE::CIdentifier(0xc4aa738a, 0x2368c0ea)
#define OVP_GD_ClassId_Algorithm_ChannelLocalisationStreamEncoderDesc                                                           OpenViBE::CIdentifier(0x3f7b49a3, 0x2b8f861a)
#define OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Matrix                                               OpenViBE::CIdentifier(0xa3e9e5b0, 0xae756303)
#define OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Dynamic                                              OpenViBE::CIdentifier(0xcf5dd4f8, 0xc2ff2878)
#define OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_OutputParameterId_EncodedMemoryBuffer                                 OpenViBE::CIdentifier(0xa3d8b171, 0xf8734734)
#define OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeBuffer                                           OpenViBE::CIdentifier(0x1b7076fd, 0x449bc70a)
#define OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeEnd                                              OpenViBE::CIdentifier(0x3fc23508, 0x806753d8)
#define OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeHeader                                           OpenViBE::CIdentifier(0x878eaf60, 0xf9d5303f)

// -----------------------------------------------------
// Feature vector stream encoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder                                                                     OpenViBE::CIdentifier(0x7ebe049d, 0xf777a602)
#define OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoderDesc                                                                 OpenViBE::CIdentifier(0xc249527b, 0x89ee1996)
#define OVP_GD_Algorithm_FeatureVectorStreamEncoder_InputParameterId_Matrix                                                     OpenViBE::CIdentifier(0xa3e9e5b0, 0xae756303)
#define OVP_GD_Algorithm_FeatureVectorStreamEncoder_OutputParameterId_EncodedMemoryBuffer                                       OpenViBE::CIdentifier(0xa3d8b171, 0xf8734734)
#define OVP_GD_Algorithm_FeatureVectorStreamEncoder_InputTriggerId_EncodeBuffer                                                 OpenViBE::CIdentifier(0x1b7076fd, 0x449bc70a)
#define OVP_GD_Algorithm_FeatureVectorStreamEncoder_InputTriggerId_EncodeEnd                                                    OpenViBE::CIdentifier(0x3fc23508, 0x806753d8)
#define OVP_GD_Algorithm_FeatureVectorStreamEncoder_InputTriggerId_EncodeHeader                                                 OpenViBE::CIdentifier(0x878eaf60, 0xf9d5303f)

// -----------------------------------------------------
// Signal stream encoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_SignalStreamEncoder                                                                            OpenViBE::CIdentifier(0xc488ad3c, 0xeb2e36bf)
#define OVP_GD_ClassId_Algorithm_SignalStreamEncoderDesc                                                                        OpenViBE::CIdentifier(0x90ac1e0f, 0x01518200)
#define OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate                                                      OpenViBE::CIdentifier(0x998710ff, 0x2c5cca82)
#define OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix                                                            OpenViBE::CIdentifier(0xa3e9e5b0, 0xae756303)
#define OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer                                              OpenViBE::CIdentifier(0xa3d8b171, 0xf8734734)
#define OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer                                                        OpenViBE::CIdentifier(0x1b7076fd, 0x449bc70a)
#define OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeEnd                                                           OpenViBE::CIdentifier(0x3fc23508, 0x806753d8)
#define OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader                                                        OpenViBE::CIdentifier(0x878eaf60, 0xf9d5303f)

// -----------------------------------------------------
// Spectrum stream encoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder                                                                          OpenViBE::CIdentifier(0xb3e252db, 0xc3214498)
#define OVP_GD_ClassId_Algorithm_SpectrumStreamEncoderDesc                                                                      OpenViBE::CIdentifier(0xd6182973, 0x122ce114)
#define OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_Matrix                                                          OpenViBE::CIdentifier(0xa3e9e5b0, 0xae756303)
#define OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands                                            OpenViBE::CIdentifier(0xf5251300, 0x72520e28)
#define OVP_GD_Algorithm_SpectrumStreamEncoder_OutputParameterId_EncodedMemoryBuffer                                            OpenViBE::CIdentifier(0xa3d8b171, 0xf8734734)
#define OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeBuffer                                                      OpenViBE::CIdentifier(0x1b7076fd, 0x449bc70a)
#define OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeEnd                                                         OpenViBE::CIdentifier(0x3fc23508, 0x806753d8)
#define OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeHeader                                                      OpenViBE::CIdentifier(0x878eaf60, 0xf9d5303f)

// -----------------------------------------------------
// Stimulation stream encoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_StimulationStreamEncoder                                                                       OpenViBE::CIdentifier(0x6e86f7d5, 0xa4668108)
#define OVP_GD_ClassId_Algorithm_StimulationStreamEncoderDesc                                                                   OpenViBE::CIdentifier(0x9b994b50, 0x52c3f06a)
#define OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet                                               OpenViBE::CIdentifier(0x8565254c, 0x3a49268e)
#define OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer                                         OpenViBE::CIdentifier(0xa3d8b171, 0xf8734734)
#define OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer                                                   OpenViBE::CIdentifier(0x1b7076fd, 0x449bc70a)
#define OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeEnd                                                      OpenViBE::CIdentifier(0x3fc23508, 0x806753d8)
#define OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader                                                   OpenViBE::CIdentifier(0x878eaf60, 0xf9d5303f)

// -----------------------------------------------------
// Streamed matrix stream encoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder                                                                    OpenViBE::CIdentifier(0x5cb32c71, 0x576f00a6)
#define OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoderDesc                                                                OpenViBE::CIdentifier(0xeeefe060, 0x646ee8ab)
#define OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix                                                    OpenViBE::CIdentifier(0xa3e9e5b0, 0xae756303)
#define OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer                                      OpenViBE::CIdentifier(0xa3d8b171, 0xf8734734)
#define OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer                                                OpenViBE::CIdentifier(0x1b7076fd, 0x449bc70a)
#define OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd                                                   OpenViBE::CIdentifier(0x3fc23508, 0x806753d8)
#define OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader                                                OpenViBE::CIdentifier(0x878eaf60, 0xf9d5303f)

// -----------------------------------------------------
// Master acquisition stream encoder
// -----------------------------------------------------

#define OVP_GD_ClassId_Algorithm_MasterAcquisitionStreamEncoder                                                                 OpenViBE::CIdentifier(0x2d15e00b, 0x51414eb6)
#define OVP_GD_ClassId_Algorithm_MasterAcquisitionStreamEncoderDesc                                                             OpenViBE::CIdentifier(0xe6ec841d, 0x9e75a8fb)
#define OVP_GD_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_StimulationSet                                         OpenViBE::CIdentifier(0x5b728d37, 0xfd088887)
#define OVP_GD_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SubjectAge                                             OpenViBE::CIdentifier(0x9ef355e4, 0xc8531112)
#define OVP_GD_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SubjectGender                                          OpenViBE::CIdentifier(0xa9056ae3, 0x57fe6af0)
#define OVP_GD_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SignalSamplingRate                                     OpenViBE::CIdentifier(0xb84ad0ca, 0x4f316dd3)
#define OVP_GD_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SubjectIdentifier                                      OpenViBE::CIdentifier(0xd5bb5231, 0x59389b72)
#define OVP_GD_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_BufferDuration                                         OpenViBE::CIdentifier(0xe1fc7385, 0x586a4f3f)
#define OVP_GD_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SignalMatrix                                           OpenViBE::CIdentifier(0xe9ac8077, 0xe369a51d)
#define OVP_GD_Algorithm_MasterAcquisitionStreamEncoder_OutputParameterId_EncodedMemoryBuffer                                   OpenViBE::CIdentifier(0xa3d8b171, 0xf8734734)
#define OVP_GD_Algorithm_MasterAcquisitionStreamEncoder_InputTriggerId_EncodeBuffer                                             OpenViBE::CIdentifier(0x1b7076fd, 0x449bc70a)
#define OVP_GD_Algorithm_MasterAcquisitionStreamEncoder_InputTriggerId_EncodeEnd                                                OpenViBE::CIdentifier(0x3fc23508, 0x806753d8)
#define OVP_GD_Algorithm_MasterAcquisitionStreamEncoder_InputTriggerId_EncodeHeader                                             OpenViBE::CIdentifier(0x878eaf60, 0xf9d5303f)

#endif // __OpenViBEPlugins_Global_Defines_H__
