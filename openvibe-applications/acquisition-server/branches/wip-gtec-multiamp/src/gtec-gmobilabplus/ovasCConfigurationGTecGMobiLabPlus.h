/**
 * The gMobilab driver was contributed
 * by Lucie Daubigney from Supelec Metz
 */

#ifndef __OpenViBE_AcquisitionServer_ovasCConfigurationGTecGMobiLabPlus_H__
#define __OpenViBE_AcquisitionServer_ovasCConfigurationGTecGMobiLabPlus_H__

#include "../ovasCConfigurationBuilder.h"

#if defined TARGET_HAS_ThirdPartyGMobiLabPlusAPI

#include <gMOBIlabplus.h>

#include <gtk/gtk.h>

namespace OpenViBEAcquisitionServer
{
	class CConfigurationGTecGMobiLabPlus : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		CConfigurationGTecGMobiLabPlus(const char* sGtkBuilderFileName);

		virtual OpenViBE::boolean postConfigure(void);
		std::string getPortName(void);

	private:

		std::string m_oPortName;
	};
};

#endif // TARGET_HAS_ThirdPartyGMobiLabPlusAPI

#endif
