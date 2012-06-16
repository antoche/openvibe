#ifndef __OpenViBE_Kernel_Visualisation_IVisualisationManager_H__
#define __OpenViBE_Kernel_Visualisation_IVisualisationManager_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IBox;

		class OV_API IWindowManager : public OpenViBE::Kernel::IKernelObject
		{
		public:

			virtual void* getWidget(OpenViBE::Kernel::IBox* pBox)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Visualisation_WindowManager)

		};
	};
};

#endif // __OpenViBE_Kernel_Visualisation_IVisualisationManager_H__
