#ifndef __OpenViBEDesigner_CScenarioStateStack_H__
#define __OpenViBEDesigner_CScenarioStateStack_H__

#include "ovd_base.h"

#include <list>

namespace OpenViBEDesigner
{
	class CApplication;

	class CScenarioStateStack
	{
	public:

		CScenarioStateStack(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBEDesigner::CApplication& rApplication, OpenViBE::Kernel::IScenario& rScenario);
		virtual ~CScenarioStateStack(void);

		virtual OpenViBE::boolean undo(void);
		virtual OpenViBE::boolean redo(void);

		virtual OpenViBE::boolean snapshot(void);

	private:

		virtual OpenViBE::boolean restoreState(const OpenViBE::IMemoryBuffer& rState);
		virtual OpenViBE::boolean dumpState(OpenViBE::IMemoryBuffer& rState);

	protected:

		const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
		OpenViBEDesigner::CApplication& m_rApplication;
		OpenViBE::Kernel::IScenario& m_rScenario;

		std::list < OpenViBE::CMemoryBuffer* > m_vStates;
		std::list < OpenViBE::CMemoryBuffer* >::iterator m_itCurrentState;

		OpenViBE::uint32 m_ui32MaximumStateCount;
	};
};

#endif // __OpenViBEDesigner_CScenarioStateStack_H__
