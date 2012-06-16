#ifndef __OpenViBEApplication_ICommand_H__
#define __OpenViBEApplication_ICommand_H__

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBESSVEP
{
	class CApplication;

	class ICommand
	{
		public:
			ICommand(CApplication* poApplication)
				: m_poApplication( poApplication )
			{};

			virtual ~ICommand() {};

			virtual void processFrame() = 0;

		protected:
			CApplication* m_poApplication;


	};


}


#endif // __OpenViBEApplication_ICommand_H__
