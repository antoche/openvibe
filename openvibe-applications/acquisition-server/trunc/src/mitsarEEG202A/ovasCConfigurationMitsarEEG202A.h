#ifndef __OpenViBE_AcquisitionServer_CConfigurationMitsarEEG202A_H__
#define __OpenViBE_AcquisitionServer_CConfigurationMitsarEEG202A_H__

#if defined TARGET_OS_Windows

#include "../ovasCConfigurationBuilder.h"

#include <gtk/gtk.h>

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CDriverMitsarEEG202A
	 * \author Gelu Ionescu (GIPSA-lab)
	 * \date 26 April 2012
	 * \erief The CConfigurationMitsarEEG202A class handles configuration specific to the Mitsar EEG 202A amplifier.
	 *
	 * submitted by Anton Andreev (GIPSA-lab)
	 */
	class CConfigurationMitsarEEG202A : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:
		CConfigurationMitsarEEG202A(const char* sGTKbuilderXMLFileName, OpenViBE::uint32& rRefIndex, OpenViBE::boolean& rHardwareTaggingState);

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);
		OpenViBE::boolean& m_rEventAndBioChannelsState;

	protected:
		OpenViBE::uint32& m_rRefIndex;
		
	};
};

#endif

#endif // __OpenViBE_AcquisitionServer_CConfigurationMitsarEEG202A_H__
