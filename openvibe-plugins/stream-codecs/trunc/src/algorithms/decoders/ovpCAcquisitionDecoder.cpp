#include "ovpCAcquisitionDecoder.h"

#include <system/Memory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

CAcquisitionDecoder::CAcquisitionDecoder(void)
{
}

// ________________________________________________________________________________________________________________
//

boolean CAcquisitionDecoder::initialize(void)
{
	CEBMLBaseDecoder::initialize();

	op_ui64BufferDuration.initialize(getOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_BufferDuration));
	op_pExperimentInformationStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ExperimentInformationStream));
	op_pSignalStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_SignalStream));
	op_pStimulationStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_StimulationStream));
	op_pChannelLocalisationStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelLocalisationStream));

	return true;
}

boolean CAcquisitionDecoder::uninitialize(void)
{
	op_pChannelLocalisationStream.uninitialize();
	op_pStimulationStream.uninitialize();
	op_pSignalStream.uninitialize();
	op_pExperimentInformationStream.uninitialize();
	op_ui64BufferDuration.uninitialize();

	CEBMLBaseDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

EBML::boolean CAcquisitionDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	     if(rIdentifier==OVTK_NodeId_Acquisition_Header_BufferDuration)        return false;
	else if(rIdentifier==OVTK_NodeId_Acquisition_Header_ExperimentInformation) return false;
	else if(rIdentifier==OVTK_NodeId_Acquisition_Header_Signal)                return false;
	else if(rIdentifier==OVTK_NodeId_Acquisition_Header_Stimulation)           return false;
	else if(rIdentifier==OVTK_NodeId_Acquisition_Header_ChannelLocalisation)   return false;
	else if(rIdentifier==OVTK_NodeId_Acquisition_Buffer_ExperimentInformation) return false;
	else if(rIdentifier==OVTK_NodeId_Acquisition_Buffer_Signal)                return false;
	else if(rIdentifier==OVTK_NodeId_Acquisition_Buffer_Stimulation)           return false;
	else if(rIdentifier==OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation)   return false;
	return CEBMLBaseDecoder::isMasterChild(rIdentifier);
}

void CAcquisitionDecoder::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Acquisition_Header_BufferDuration)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Header_ExperimentInformation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Header_Signal)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Header_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Header_ChannelLocalisation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Buffer_ExperimentInformation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Buffer_Signal)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Buffer_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation))
	{
	}
	else
	{
		CEBMLBaseDecoder::openChild(rIdentifier);
	}
}

void CAcquisitionDecoder::processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Acquisition_Header_BufferDuration)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Header_ExperimentInformation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Header_Signal)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Header_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Header_ChannelLocalisation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Buffer_ExperimentInformation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Buffer_Signal)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Buffer_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation))
	{
		if(l_rTop==OVTK_NodeId_Acquisition_Header_BufferDuration)        op_ui64BufferDuration=m_pEBMLReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize);
		if(l_rTop==OVTK_NodeId_Acquisition_Header_ExperimentInformation) this->appendMemoryBuffer(op_pExperimentInformationStream, pBuffer, ui64BufferSize);
		if(l_rTop==OVTK_NodeId_Acquisition_Header_Signal)                this->appendMemoryBuffer(op_pSignalStream, pBuffer, ui64BufferSize);
		if(l_rTop==OVTK_NodeId_Acquisition_Header_Stimulation)           this->appendMemoryBuffer(op_pStimulationStream, pBuffer, ui64BufferSize);
		if(l_rTop==OVTK_NodeId_Acquisition_Header_ChannelLocalisation)   this->appendMemoryBuffer(op_pChannelLocalisationStream, pBuffer, ui64BufferSize);
		if(l_rTop==OVTK_NodeId_Acquisition_Buffer_ExperimentInformation) this->appendMemoryBuffer(op_pExperimentInformationStream, pBuffer, ui64BufferSize);
		if(l_rTop==OVTK_NodeId_Acquisition_Buffer_Signal)                this->appendMemoryBuffer(op_pSignalStream, pBuffer, ui64BufferSize);
		if(l_rTop==OVTK_NodeId_Acquisition_Buffer_Stimulation)           this->appendMemoryBuffer(op_pStimulationStream, pBuffer, ui64BufferSize);
		if(l_rTop==OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation)   this->appendMemoryBuffer(op_pChannelLocalisationStream, pBuffer, ui64BufferSize);
	}
	else
	{
		CEBMLBaseDecoder::processChildData(pBuffer, ui64BufferSize);
	}
}

void CAcquisitionDecoder::closeChild(void)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Acquisition_Header_BufferDuration)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Header_ExperimentInformation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Header_Signal)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Header_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Header_ChannelLocalisation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Buffer_ExperimentInformation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Buffer_Signal)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Buffer_Stimulation)
	 ||(l_rTop==OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation))
	{
	}
	else
	{
		CEBMLBaseDecoder::closeChild();
	}

	m_vNodes.pop();
}

void CAcquisitionDecoder::appendMemoryBuffer(IMemoryBuffer* pMemoryBuffer, const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	if(pMemoryBuffer)
	{
		uint64 l_ui64CurrentBufferSize=pMemoryBuffer->getSize();
		pMemoryBuffer->setSize(l_ui64CurrentBufferSize+ui64BufferSize, false);
		System::Memory::copy(pMemoryBuffer->getDirectPointer()+l_ui64CurrentBufferSize, pBuffer, ui64BufferSize);
	}
}
