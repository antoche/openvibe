#ifndef __OpenViBE_CKernelLoader_H__
#define __OpenViBE_CKernelLoader_H__

#include "ovIKernelLoader.h"

namespace OpenViBE
{
	class OV_API CKernelLoader : public OpenViBE::IKernelLoader
	{
	public:

		CKernelLoader(void);
		virtual ~CKernelLoader(void);

		virtual OpenViBE::boolean load(const OpenViBE::CString& sFileName, OpenViBE::CString* pError=NULL);
		virtual OpenViBE::boolean unload(OpenViBE::CString* pError=NULL);

		virtual OpenViBE::boolean initialize(void);
		virtual OpenViBE::boolean getKernelDesc(OpenViBE::Kernel::IKernelDesc*& rpKernelDesc);
		virtual OpenViBE::boolean uninitialize(void);

		_IsDerivedFromClass_Final_(OpenViBE::IKernelLoader, OV_ClassId_KernelLoaderBridge)

	private:

		OpenViBE::IKernelLoader* m_pKernelLoaderImpl; //!< Internal implementation
	};
};

#endif // __OpenViBE_CKernelLoader_H__
