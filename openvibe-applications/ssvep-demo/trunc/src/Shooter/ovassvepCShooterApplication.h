#ifndef __OpenViBEApplication_CShooterApplication_H__
#define __OpenViBEApplication_CShooterApplication_H__

#include <iostream>

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include "../ovassvepCApplication.h"
#include "ovassvepCShooterTarget.h"
#include "ovassvepCStarShip.h"


namespace OpenViBESSVEP
{


	class CShooterApplication : public CApplication
	{
		public:
			CShooterApplication();
			~CShooterApplication();
	
			bool setup(OpenViBE::Kernel::IKernelContext* poKernelContext);

			CStarShip* getShip()
			{
				return m_poShip;
			}
			
			void startExperiment();

			void addTarget(OpenViBE::uint32 ui32TargetPosition);

			bool m_bTargetRequest;

		private:
			bool m_bActive;
			
			void processFrame(OpenViBE::uint32 ui32CurrentFrame);
			

			CEGUI::Window* m_poInstructionsReady;
			CStarShip* m_poShip;
			std::vector<CShooterTarget*> m_oTargets;
	};
}

#endif // __OpenViBEApplication_CShooterApplication_H__
