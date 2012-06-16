#include "ovpCChannelLocalisationDecoder.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

// ________________________________________________________________________________________________________________
//

boolean CChannelLocalisationDecoder::initialize(void)
{
	CStreamedMatrixDecoder::initialize();

	op_bDynamic.initialize(getOutputParameter(OVP_Algorithm_ChannelLocalisationStreamDecoder_OutputParameterId_Dynamic));

	return true;
}

boolean CChannelLocalisationDecoder::uninitialize(void)
{
	op_bDynamic.uninitialize();

	CStreamedMatrixDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

EBML::boolean CChannelLocalisationDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	     if(rIdentifier==OVTK_NodeId_Header_ChannelLocalisation)         { return true; }
	else if(rIdentifier==OVTK_NodeId_Header_ChannelLocalisation_Dynamic) { return false; }
	return CStreamedMatrixDecoder::isMasterChild(rIdentifier);
}

void CChannelLocalisationDecoder::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_ChannelLocalisation)
	 ||(l_rTop==OVTK_NodeId_Header_ChannelLocalisation_Dynamic))
	{
	}
	else
	{
		CStreamedMatrixDecoder::openChild(rIdentifier);
	}
}

void CChannelLocalisationDecoder::processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_ChannelLocalisation)
	 ||(l_rTop==OVTK_NodeId_Header_ChannelLocalisation_Dynamic))
	{
		if(l_rTop==OVTK_NodeId_Header_ChannelLocalisation_Dynamic)
		{
			op_bDynamic=(m_pEBMLReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize)?true:false);
		}
	}
	else
	{
		CStreamedMatrixDecoder::processChildData(pBuffer, ui64BufferSize);
	}
}

void CChannelLocalisationDecoder::closeChild(void)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_ChannelLocalisation)
	 ||(l_rTop==OVTK_NodeId_Header_ChannelLocalisation_Dynamic))
	{
	}
	else
	{
		CStreamedMatrixDecoder::closeChild();
	}

	m_vNodes.pop();
}
