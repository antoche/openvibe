#ifndef __OpenViBEKernel_Kernel_Player_CBoxAlgorithmContext_H__
#define __OpenViBEKernel_Kernel_Player_CBoxAlgorithmContext_H__

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CSimulatedBox;
		class CBoxAlgorithmContext : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IBoxAlgorithmContext>
		{
		public:

			CBoxAlgorithmContext(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::CSimulatedBox* pSimulatedBox, const OpenViBE::Kernel::IBox* pBox);
			virtual ~CBoxAlgorithmContext(void);

			virtual OpenViBE::Kernel::IBox* getStaticBoxContext(void);
			virtual OpenViBE::Kernel::IBoxIO* getDynamicBoxContext(void);
			virtual OpenViBE::Kernel::IPlayerContext* getPlayerContext(void);
			virtual OpenViBE::Kernel::IVisualisationContext* getVisualisationContext(void);

			virtual OpenViBE::boolean markAlgorithmAsReadyToProcess(void);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IBoxAlgorithmContext>, OVK_ClassId_Kernel_Player_BoxAlgorithmContext)

			virtual OpenViBE::boolean isAlgorithmReadyToProcess(void);

		protected:

			OpenViBE::Kernel::IBox* m_pStaticBoxContext;
			OpenViBE::Kernel::IBoxIO* m_pDynamicBoxContext;
			OpenViBE::Kernel::IPlayerContext* m_pPlayerContext;
			OpenViBE::Kernel::IVisualisationContext* m_pVisualisationContext;
			OpenViBE::boolean m_bReadyToProcess;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CBoxAlgorithmContext_H__
