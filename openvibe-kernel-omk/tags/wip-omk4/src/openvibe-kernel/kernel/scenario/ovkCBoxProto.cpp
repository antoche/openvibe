#include "ovkCBoxProto.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

CBoxProto::CBoxProto(const IKernelContext& rKernelContext, IBox& rBox)
	:TKernelObject<IBoxProto>(rKernelContext)
	,m_rBox(rBox)
{
}

uint32 CBoxProto::addInput(
	const CString& sName,
	const CIdentifier& rTypeIdentifier)
{
	return m_rBox.addInput(sName, rTypeIdentifier);
}

uint32 CBoxProto::addOutput(
	const CString& sName,
	const CIdentifier& rTypeIdentifier)
{
	return m_rBox.addOutput(sName, rTypeIdentifier);
}

uint32 CBoxProto::addSetting(
	const CString& sName,
	const CIdentifier& rTypeIdentifier,
	const CString& sDefaultValue)
{
	return m_rBox.addSetting(sName, rTypeIdentifier, sDefaultValue);
}
