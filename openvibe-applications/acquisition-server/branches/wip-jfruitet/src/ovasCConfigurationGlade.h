#ifndef __OpenViBE_AcquisitionServer_CConfigurationGlade_H__
#define __OpenViBE_AcquisitionServer_CConfigurationGlade_H__

#include "ovas_base.h"

#include <glade/glade.h>
#include <gtk/gtk.h>

#include <string>
#include <map>

namespace OpenViBEAcquisitionServer
{
	class IHeader;

	class CConfigurationGlade
	{
	public:

		CConfigurationGlade(const char* sGladeXMLFileName);
		virtual ~CConfigurationGlade(void);

		virtual OpenViBE::boolean configure(
			OpenViBEAcquisitionServer::IHeader& rHeader);

		virtual void buttonChangeChannelNamesCB(void);
		virtual void buttonApplyChannelNameCB(void);
		virtual void buttonRemoveChannelNameCB(void);
		virtual void treeviewApplyChannelNameCB(void);

	protected:

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean doConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	private:

		CConfigurationGlade(void);

	protected:

		OpenViBE::boolean m_bApplyConfiguration;

		::GladeXML* m_pGladeConfigureInterface;
		::GladeXML* m_pGladeConfigureChannelInterface;

		::GtkWidget* m_pDialog;

		::GtkWidget* m_pIdentifier;
		::GtkWidget* m_pAge;
		::GtkWidget* m_pNumberOfChannels;
		::GtkWidget* m_pSamplingFrequency;
		::GtkWidget* m_pGender;

		::GtkListStore* m_pElectrodeNameListStore;
		::GtkListStore* m_pChannelNameListStore;

		::GtkWidget* m_pElectrodeNameTreeView;
		::GtkWidget* m_pChannelNameTreeView;

		std::map < OpenViBE::uint32, std::string > m_vChannelName;
		std::string m_sGladeXMLFileName;
		std::string m_sElectrodeFileName;
		std::string m_sGladeXMLChannelsFileName;
		IHeader* m_pHeader;
	};
};

#endif // __OpenViBE_AcquisitionServer_CConfigurationGlade_H__
