#ifndef __OpenViBE_AcquisitionServer_CDriverGenericRawFileReader_H__
#define __OpenViBE_AcquisitionServer_CDriverGenericRawFileReader_H__

#include "ovasCDriverGenericRawReader.h"

#include <cstdio>

namespace OpenViBEAcquisitionServer
{
	class CDriverGenericRawFileReader : public OpenViBEAcquisitionServer::CDriverGenericRawReader
	{
	public:

		CDriverGenericRawFileReader(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);

		const char* getName(void) { return "Generic Raw File Reader"; }
		virtual OpenViBE::boolean isConfigurable(void) { return true; }
		virtual OpenViBE::boolean configure(void);

	protected:

		virtual OpenViBE::boolean open(void);
		virtual OpenViBE::boolean close(void);
		virtual OpenViBE::boolean read(void);

	protected:

		FILE* m_pFile;
		OpenViBE::CString m_sFilename;
	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverGenericRawFileReader_H__
