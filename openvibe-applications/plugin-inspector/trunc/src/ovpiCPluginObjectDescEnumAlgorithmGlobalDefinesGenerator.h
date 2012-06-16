#ifndef __OpenViBEPluginInspector_CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator_H__
#define __OpenViBEPluginInspector_CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator_H__

#include "ovpiCPluginObjectDescEnum.h"

#include <fstream>

class CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator : public CPluginObjectDescEnum
{
public:

	CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator(const OpenViBE::Kernel::IKernelContext& rKernelContext, const OpenViBE::CString& sFilename);
	virtual ~CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator(void);
	virtual OpenViBE::boolean callback(const OpenViBE::Plugins::IPluginObjectDesc& rPluginObjectDesc);

protected:

	std::ofstream m_oFile;
};

#endif // __OpenViBEPluginInspector_CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator_H__
