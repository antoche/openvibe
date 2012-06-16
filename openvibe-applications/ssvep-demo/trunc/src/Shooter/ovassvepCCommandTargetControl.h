#ifndef __OpenViBEApplication_CCommandTargetControl_
#define __OpenViBEApplication_CCommandTargetControl_

#include "../ovassvepICommandVRPNButton.h"
#include "../ovassvepCVRPNServer.h"

namespace OpenViBESSVEP
{
	class CShooterApplication;

	class CCommandTargetControl : public ICommandVRPNButton
	{
		public:
			CCommandTargetControl(CShooterApplication* poApplication);
			~CCommandTargetControl() {};

			void execute(int iButton, int iState);
			void processFrame();

		private:
			CVRPNServer* m_poVRPNServer;

	};
}


#endif // __OpenViBEApplication_CCommandTargetControl_
