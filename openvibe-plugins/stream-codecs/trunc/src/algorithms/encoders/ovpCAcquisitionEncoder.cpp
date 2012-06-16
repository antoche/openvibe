#include "ovpCAcquisitionEncoder.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

boolean CAcquisitionEncoder::initialize(void)
{
	CEBMLBaseEncoder::initialize();

	ip_ui64BufferDuration.initialize(getInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_BufferDuration));
	ip_pExperimentInformationStream.initialize(getInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ExperimentInformationStream));
	ip_pSignalStream.initialize(getInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_SignalStream));
	ip_pStimulationStream.initialize(getInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_StimulationStream));
	ip_pChannelLocalisationStream.initialize(getInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelLocalisationStream));

	return true;
}

boolean CAcquisitionEncoder::uninitialize(void)
{
	ip_pChannelLocalisationStream.uninitialize();
	ip_pStimulationStream.uninitialize();
	ip_pSignalStream.uninitialize();
	ip_pExperimentInformationStream.uninitialize();
	ip_ui64BufferDuration.uninitialize();

	CEBMLBaseEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CAcquisitionEncoder::processHeader(void)
{
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Acquisition_Header_BufferDuration);
	 m_pEBMLWriterHelper->setUIntegerAsChildData(ip_ui64BufferDuration);
	m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Acquisition_Header_ExperimentInformation);
	 m_pEBMLWriterHelper->setBinaryAsChildData(ip_pExperimentInformationStream->getDirectPointer(), ip_pExperimentInformationStream->getSize());
	m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Acquisition_Header_Signal);
	 m_pEBMLWriterHelper->setBinaryAsChildData(ip_pSignalStream->getDirectPointer(), ip_pSignalStream->getSize());
	m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Acquisition_Header_Stimulation);
	 m_pEBMLWriterHelper->setBinaryAsChildData(ip_pStimulationStream->getDirectPointer(), ip_pStimulationStream->getSize());
	m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Acquisition_Header_ChannelLocalisation);
	 m_pEBMLWriterHelper->setBinaryAsChildData(ip_pChannelLocalisationStream->getDirectPointer(), ip_pChannelLocalisationStream->getSize());
	m_pEBMLWriterHelper->closeChild();

	return true;
}

boolean CAcquisitionEncoder::processBuffer(void)
{
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Acquisition_Buffer_ExperimentInformation);
	 m_pEBMLWriterHelper->setBinaryAsChildData(ip_pExperimentInformationStream->getDirectPointer(), ip_pExperimentInformationStream->getSize());
	m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Acquisition_Buffer_Signal);
	 m_pEBMLWriterHelper->setBinaryAsChildData(ip_pSignalStream->getDirectPointer(), ip_pSignalStream->getSize());
	m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Acquisition_Buffer_Stimulation);
	 m_pEBMLWriterHelper->setBinaryAsChildData(ip_pStimulationStream->getDirectPointer(), ip_pStimulationStream->getSize());
	m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation);
	 m_pEBMLWriterHelper->setBinaryAsChildData(ip_pChannelLocalisationStream->getDirectPointer(), ip_pChannelLocalisationStream->getSize());
	m_pEBMLWriterHelper->closeChild();

	return true;
}
