#ifndef __OpenViBEPluginInspector_CPluginObjectDescEnumBoxAlgorithmSnapshotGenerator_H__
#define __OpenViBEPluginInspector_CPluginObjectDescEnumBoxAlgorithmSnapshotGenerator_H__

#include "ovpiCPluginObjectDescEnum.h"

#include <map>
#include <vector>
#include <string>

#include <gtk/gtk.h>

// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------

class CPluginObjectDescEnumBoxAlgorithmSnapshotGenerator : public CPluginObjectDescEnum
{
public:

	CPluginObjectDescEnumBoxAlgorithmSnapshotGenerator(const OpenViBE::Kernel::IKernelContext& rKernelContext, const OpenViBE::CString& sSnapshotDirectory, const OpenViBE::CString& sDocTemplateDirectory);
	virtual ~CPluginObjectDescEnumBoxAlgorithmSnapshotGenerator(void);
	virtual OpenViBE::boolean callback(const OpenViBE::Plugins::IPluginObjectDesc& rPluginObjectDesc);

protected:

	std::string m_sSnapshotDirectory;
	std::string m_sDocTemplateDirectory;
	std::vector < std::pair < std::string, std::string > > m_vCategories;
	std::map<OpenViBE::uint32, ::GdkColor> m_vColors;
	::GtkWidget* m_pWindow;
	::GtkWidget* m_pWidget;
	OpenViBE::CIdentifier m_oScenarioIdentifier;
	OpenViBE::Kernel::IScenario* m_pScenario;
};

#endif // __OpenViBEPluginInspector_CPluginObjectDescEnumBoxAlgorithmSnapshotGenerator_H__
