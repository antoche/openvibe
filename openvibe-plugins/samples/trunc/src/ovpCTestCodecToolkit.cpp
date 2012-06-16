#include "ovpCTestCodecToolkit.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;

using namespace OpenViBEToolkit;

boolean CTestCodecToolkit::initialize(void)
{
	// You can also manipulate pointers to Codec object. Creation and destruction must be done like that :	
	TStreamedMatrixDecoder < CTestCodecToolkit > * l_oStreamedMatrixDecoder = new TStreamedMatrixDecoder < CTestCodecToolkit >(*this);
	delete l_oStreamedMatrixDecoder;
	TStreamedMatrixEncoder < CTestCodecToolkit > * l_oStreamedMatrixEncoder = new TStreamedMatrixEncoder < CTestCodecToolkit >(*this);
	delete l_oStreamedMatrixEncoder;

	TChannelLocalisationDecoder < CTestCodecToolkit > * l_oChannelLocalisationDecoder = new TChannelLocalisationDecoder < CTestCodecToolkit >(*this);
	delete l_oChannelLocalisationDecoder;
	TChannelLocalisationEncoder < CTestCodecToolkit > * l_oChannelLocalisationEncoder = new TChannelLocalisationEncoder < CTestCodecToolkit >(*this);
	delete l_oChannelLocalisationEncoder;

	TFeatureVectorDecoder < CTestCodecToolkit > * l_oTFeatureVectorDecoder = new TFeatureVectorDecoder < CTestCodecToolkit >(*this);
	delete l_oTFeatureVectorDecoder;
	TFeatureVectorEncoder < CTestCodecToolkit > * l_oTFeatureVectorEncoder = new TFeatureVectorEncoder < CTestCodecToolkit >(*this);
	delete l_oTFeatureVectorEncoder;

	TSpectrumDecoder < CTestCodecToolkit > * l_oTSpectrumDecoder = new TSpectrumDecoder < CTestCodecToolkit >(*this);
	delete l_oTSpectrumDecoder;
	TSpectrumEncoder < CTestCodecToolkit > * l_oTSpectrumEncoder = new TSpectrumEncoder < CTestCodecToolkit >(*this);
	delete l_oTSpectrumEncoder;

	TSignalDecoder < CTestCodecToolkit > * l_oTSignalDecoder = new TSignalDecoder < CTestCodecToolkit >(*this);
	delete l_oTSignalDecoder;
	TSignalEncoder < CTestCodecToolkit > * l_oTSignalEncoder = new TSignalEncoder < CTestCodecToolkit >(*this);
	delete l_oTSignalEncoder;

	TStimulationDecoder < CTestCodecToolkit > * l_oTStimulationDecoder = new TStimulationDecoder < CTestCodecToolkit >(*this);
	delete l_oTStimulationDecoder;
	TStimulationEncoder < CTestCodecToolkit > * l_oTStimulationEncoder = new TStimulationEncoder < CTestCodecToolkit >(*this);
	delete l_oTStimulationEncoder;

	TExperimentInformationDecoder < CTestCodecToolkit > * l_oTExperimentInformationDecoder = new TExperimentInformationDecoder < CTestCodecToolkit >(*this);
	delete l_oTExperimentInformationDecoder;
	TExperimentInformationEncoder < CTestCodecToolkit > * l_oTExperimentInformationEncoder = new TExperimentInformationEncoder < CTestCodecToolkit >(*this);
	delete l_oTExperimentInformationEncoder;

	//-----------------------------------------------------------------------------------------

	m_oStreamedMatrixDecoder.initialize(*this);
	m_oStreamedMatrixEncoder.initialize(*this);
	m_oStreamedMatrixEncoder.getInputMatrix().setReferenceTarget(m_oStreamedMatrixDecoder.getOutputMatrix());
	m_vDecoders.push_back(&m_oStreamedMatrixDecoder);
	m_vEncoders.push_back(&m_oStreamedMatrixEncoder);

	m_oChannelLocalisationDecoder.initialize(*this);
	m_oChannelLocalisationEncoder.initialize(*this);
	m_oChannelLocalisationEncoder.getInputMatrix().setReferenceTarget(m_oChannelLocalisationDecoder.getOutputMatrix());
	m_oChannelLocalisationEncoder.getInputDynamic().setReferenceTarget(m_oChannelLocalisationDecoder.getOutputDynamic());
	m_vDecoders.push_back(&m_oChannelLocalisationDecoder);
	m_vEncoders.push_back(&m_oChannelLocalisationEncoder);

	m_oFeatureVectorDecoder.initialize(*this);
	m_oFeatureVectorEncoder.initialize(*this);
	m_oFeatureVectorEncoder.getInputMatrix().setReferenceTarget(m_oFeatureVectorDecoder.getOutputMatrix());
	m_vDecoders.push_back(&m_oFeatureVectorDecoder);
	m_vEncoders.push_back(&m_oFeatureVectorEncoder);

	m_oSpectrumDecoder.initialize(*this);
	m_oSpectrumEncoder.initialize(*this);
	m_oSpectrumEncoder.getInputMatrix().setReferenceTarget(m_oSpectrumDecoder.getOutputMatrix());
	m_oSpectrumEncoder.getInputMinMaxFrequencyBands().setReferenceTarget(m_oSpectrumDecoder.getOutputMinMaxFrequencyBands());
	m_vDecoders.push_back(&m_oSpectrumDecoder);
	m_vEncoders.push_back(&m_oSpectrumEncoder);

	m_oSignalDecoder.initialize(*this);
	m_oSignalEncoder.initialize(*this);
	m_oSignalEncoder.getInputMatrix().setReferenceTarget(m_oSignalDecoder.getOutputMatrix());
	m_oSignalEncoder.getInputSamplingRate().setReferenceTarget(m_oSignalDecoder.getOutputSamplingRate());
	m_vDecoders.push_back(&m_oSignalDecoder);
	m_vEncoders.push_back(&m_oSignalEncoder);

	m_oStimDecoder.initialize(*this);
	m_oStimEncoder.initialize(*this);
	m_oStimEncoder.getInputStimulationSet().setReferenceTarget(m_oStimDecoder.getOutputStimulationSet());
	m_vDecoders.push_back(&m_oStimDecoder);
	m_vEncoders.push_back(&m_oStimEncoder);

	m_oExperimentInformationDecoder.initialize(*this);
	m_oExperimentInformationEncoder.initialize(*this);
	m_oExperimentInformationEncoder.getInputExperimentIdentifier().setReferenceTarget(m_oExperimentInformationDecoder.getOutputExperimentIdentifier());
	m_oExperimentInformationEncoder.getInputExperimentDate().setReferenceTarget(m_oExperimentInformationDecoder.getOutputExperimentDate());
	m_oExperimentInformationEncoder.getInputSubjectIdentifier().setReferenceTarget(m_oExperimentInformationDecoder.getOutputSubjectIdentifier());
	m_oExperimentInformationEncoder.getInputSubjectName().setReferenceTarget(m_oExperimentInformationDecoder.getOutputSubjectName());
	m_oExperimentInformationEncoder.getInputSubjectAge().setReferenceTarget(m_oExperimentInformationDecoder.getOutputSubjectAge());
	m_oExperimentInformationEncoder.getInputSubjectGender().setReferenceTarget(m_oExperimentInformationDecoder.getOutputSubjectGender());
	m_oExperimentInformationEncoder.getInputLaboratoryIdentifier().setReferenceTarget(m_oExperimentInformationDecoder.getOutputLaboratoryIdentifier());
	m_oExperimentInformationEncoder.getInputLaboratoryName().setReferenceTarget(m_oExperimentInformationDecoder.getOutputLaboratoryName());
	m_oExperimentInformationEncoder.getInputTechnicianIdentifier().setReferenceTarget(m_oExperimentInformationDecoder.getOutputTechnicianIdentifier());
	m_oExperimentInformationEncoder.getInputTechnicianName().setReferenceTarget(m_oExperimentInformationDecoder.getOutputTechnicianName());
	m_vDecoders.push_back(&m_oExperimentInformationDecoder);
	m_vEncoders.push_back(&m_oExperimentInformationEncoder);

	return true;
}

boolean CTestCodecToolkit::uninitialize(void)
{
	for(uint32 i = 0; i< m_vDecoders.size(); i++)
	{
		m_vDecoders[i]->uninitialize();
	}
	for(uint32 i = 0; i< m_vEncoders.size(); i++)
	{
		m_vEncoders[i]->uninitialize();
	}

	return true;
}

boolean CTestCodecToolkit::processInput(uint32 ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CTestCodecToolkit::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			// we can manipulate decoders and encoders without knowing their types
			m_vDecoders[i]->decode(i,j);

			if(m_vDecoders[i]->isHeaderReceived())
			{
				m_vEncoders[i]->encodeHeader(i);
			}
			if(m_vDecoders[i]->isBufferReceived())
			{
				//let's check what is inside the buffer
				CIdentifier l_oInputType;
				l_rStaticBoxContext.getInputType(i, l_oInputType);
				if(l_oInputType==OV_TypeId_StreamedMatrix)
				{
					IMatrix* l_pMatrix = m_oStreamedMatrixDecoder.getOutputMatrix();
					this->getLogManager() << LogLevel_Info << "Streamed Matrix buffer received ("<<l_pMatrix->getBufferElementCount()<<" elements in buffer).\n";
				}
				else if(l_oInputType==OV_TypeId_ChannelLocalisation)
				{
					IMatrix* l_pMatrix = m_oChannelLocalisationDecoder.getOutputMatrix();
					this->getLogManager() << LogLevel_Info << "Channel localisation buffer received ("<<l_pMatrix->getBufferElementCount()<<" elements in buffer).\n";
				}
				else if(l_oInputType==OV_TypeId_FeatureVector)
				{
					IMatrix* l_pMatrix = m_oFeatureVectorDecoder.getOutputMatrix();
					this->getLogManager() << LogLevel_Info << "Feature Vector buffer received ("<<l_pMatrix->getBufferElementCount()<<" features in vector).\n";
				}
				else if(l_oInputType==OV_TypeId_Spectrum)
				{
					IMatrix* l_pMatrix = m_oSpectrumDecoder.getOutputMinMaxFrequencyBands();
					this->getLogManager() << LogLevel_Info << "Spectrum min/MAX bands received ("<<l_pMatrix->getBufferElementCount()<<" elements in matrix).\n";
				}
				else if(l_oInputType==OV_TypeId_Signal)
				{
					IMatrix* l_pMatrix = m_oSignalDecoder.getOutputMatrix();
					uint64 l_uiSamplingFrequency = m_oSignalDecoder.getOutputSamplingRate();
					this->getLogManager() << LogLevel_Info << "Signal buffer received ("<<l_pMatrix->getBufferElementCount()<<" elements in buffer) with sampling frequency "<<l_uiSamplingFrequency<<"Hz.\n";
				}
				else if(l_oInputType==OV_TypeId_Stimulations)
				{
					IStimulationSet* l_pStimulations =  m_oStimDecoder.getOutputStimulationSet();
					// as we constantly receive stimulations on the stream, we will check if the incoming set is empty or not
					if(l_pStimulations->getStimulationCount() != 0)
					{
						this->getLogManager() << LogLevel_Info << "Stimulation Set buffer received (1st stim is ["<<l_pStimulations->getStimulationIdentifier(0)<<":"<<this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, l_pStimulations->getStimulationIdentifier(0))<<"]).\n";
						m_oStimEncoder.getInputStimulationSet()->clear();
						m_oStimEncoder.getInputStimulationSet()->appendStimulation(l_pStimulations->getStimulationIdentifier(0)+1,l_pStimulations->getStimulationDate(0),0);
					}
				}
				else if(l_oInputType==OV_TypeId_ExperimentationInformation)
				{
					uint64 l_pXPid = m_oExperimentInformationDecoder.getOutputExperimentIdentifier();
					this->getLogManager() << LogLevel_Info << "Experiment information buffer received (xp ID: "<<l_pXPid<<").\n";
				}
				else
				{
					this->getLogManager() << LogLevel_Error << "Undefined input type.\n";
					return true;
				}
				m_vEncoders[i]->encodeBuffer(i);
			}
			if(m_vDecoders[i]->isEndReceived())
			{
				m_vEncoders[i]->encodeEnd(i);
			}
			l_rDynamicBoxContext.markOutputAsReadyToSend(i,l_rDynamicBoxContext.getInputChunkStartTime(i, j), l_rDynamicBoxContext.getInputChunkEndTime(i, j));
		}
	}

	return true;
}
