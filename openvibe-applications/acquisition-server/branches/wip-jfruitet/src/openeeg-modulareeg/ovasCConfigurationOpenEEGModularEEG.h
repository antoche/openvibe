#ifndef __OpenViBE_AcquisitionServer_CConfigurationOpenEEGModularEEG_H__
#define __OpenViBE_AcquisitionServer_CConfigurationOpenEEGModularEEG_H__

#include "../ovasCConfigurationGlade.h"

#include <gtk/gtk.h>

namespace OpenViBEAcquisitionServer
{
	class CConfigurationOpenEEGModularEEG : public OpenViBEAcquisitionServer::CConfigurationGlade
	{
	public:
		CConfigurationOpenEEGModularEEG(const char* sGladeXMLFileName, OpenViBE::uint32& rUSBIndex);

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	protected:
		OpenViBE::uint32& m_rUSBIndex;
	};
};

#endif // __OpenViBE_AcquisitionServer_CConfigurationOpenEEGModularEEG_H__
