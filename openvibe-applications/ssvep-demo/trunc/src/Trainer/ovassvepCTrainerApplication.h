#ifndef __OpenViBEApplication_CTrainerApplication_H__
#define __OpenViBEApplication_CTrainerApplication_H__

#include <iostream>
#include <CEGUI.h>

#include "../ovassvepCApplication.h"
#include "ovassvepCTrainerFlickeringObject.h"

namespace OpenViBESSVEP
{

	class CTrainerApplication : public CApplication
	{
		public:
			CTrainerApplication();
			~CTrainerApplication() {};

			bool setup(OpenViBE::Kernel::IKernelContext* poKernelContext);
			void setTarget(OpenViBE::int32 i32Target);

			void startExperiment();
			void startFlickering();
			void stopFlickering();


		private:
			bool m_bActive;
			void processFrame(OpenViBE::uint32 ui32CurrentFrame);
			void addObject(CTrainerFlickeringObject *target);

			std::vector<CTrainerFlickeringObject*> m_oObjects;

			time_t m_ttStartTime;

			CEGUI::Window* m_poInstructionsReady;


	};
}


#endif // __OpenViBEApplication_CTrainerApplication_H__
