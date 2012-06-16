#ifndef __OpenViBE_AcquisitionServer_CConfigurationOpenEEGModularEEG_H__
#define __OpenViBE_AcquisitionServer_CConfigurationOpenEEGModularEEG_H__

#include "../ovasCConfigurationBuilder.h"

#include <gtk/gtk.h>

namespace OpenViBEAcquisitionServer
{
	class CConfigurationOpenEEGModularEEG : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:
		CConfigurationOpenEEGModularEEG(const char* sGtkBuilderFileName, OpenViBE::uint32& rUSBIndex);
		virtual ~CConfigurationOpenEEGModularEEG(void);

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	protected:
		OpenViBE::uint32& m_rUSBIndex;
		::GtkListStore* m_pListStore;
	};
};

#endif // __OpenViBE_AcquisitionServer_CConfigurationOpenEEGModularEEG_H__
