#ifndef __OpenViBE_AcquisitionServer_CConfigurationGenericRawReader_H__
#define __OpenViBE_AcquisitionServer_CConfigurationGenericRawReader_H__

#include "../ovasCConfigurationNetworkBuilder.h"

#include <gtk/gtk.h>

namespace OpenViBEAcquisitionServer
{
	class CConfigurationGenericRawReader : public OpenViBEAcquisitionServer::CConfigurationNetworkBuilder
	{
	public:

		CConfigurationGenericRawReader(const char* sGtkBuilderFileName,
			OpenViBE::boolean& rLimitSpeed,
			OpenViBE::uint32& rSampleFormat,
			OpenViBE::uint32& rSampleEndian,
			OpenViBE::uint32& rHeaderSkip,
			OpenViBE::uint32& rFrameSkip,
			OpenViBE::CString& rFilename);

	protected:

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	public:

		OpenViBE::boolean& m_rLimitSpeed;
		OpenViBE::uint32& m_rSampleFormat;
		OpenViBE::uint32& m_rSampleEndian;
		OpenViBE::uint32& m_rHeaderSkip;
		OpenViBE::uint32& m_rFrameSkip;
		OpenViBE::CString& m_rFilename;
	};
};

#endif // __OpenViBE_AcquisitionServer_CConfigurationGenericRawReader_H__
