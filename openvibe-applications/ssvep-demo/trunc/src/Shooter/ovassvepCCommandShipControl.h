#ifndef __OpenViBEApplication_CCommandShipControl_H__
#define __OpenViBEApplication_CCommandShipControl_H__

#include "../ovassvepICommandVRPNButton.h"

namespace OpenViBESSVEP
{
	class CShooterApplication;

	class CCommandShipControl : public ICommandVRPNButton
	{
		public:
			CCommandShipControl(CShooterApplication* poApplication);
			~CCommandShipControl() {};

			void execute(int iButton, int iState);

	};
}


#endif // __OpenViBEApplication_CCommandShipControl_H__
