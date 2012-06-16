#ifndef __OpenViBESkeletonGenerator_CDriverSkeletonGenerator_H__
#define __OpenViBESkeletonGenerator_CDriverSkeletonGenerator_H__

#include "ovsgCSkeletonGenerator.h"

#include <vector>
#include <map>

namespace OpenViBESkeletonGenerator
{
	class CDriverSkeletonGenerator : public CSkeletonGenerator
	{
	public:

		CDriverSkeletonGenerator(OpenViBE::Kernel::IKernelContext & rKernelContext, ::GtkBuilder * pBuilderInterface);
		virtual ~CDriverSkeletonGenerator(void);

		OpenViBE::boolean initialize(void);
		OpenViBE::boolean save(OpenViBE::CString sFileName);
		OpenViBE::boolean load(OpenViBE::CString sFileName);
		void getCurrentParameters(void);
		
		OpenViBE::CString              m_sDriverName;
		OpenViBE::CString              m_sClassName;
		OpenViBE::CString              m_sSamplingFrequencies;
		std::vector<OpenViBE::CString> m_vSamplingFrequencies;
		OpenViBE::CString              m_sMinChannel;
		OpenViBE::CString              m_sMaxChannel;
		
		void buttonCheckCB(void);
		void buttonOkCB(void);
		void buttonTooltipCB(::GtkButton* pButton);
		void buttonExitCB(void);

	private:

		typedef enum
		{
			WidgetName_DriverName,
			WidgetName_ClassName,
			WidgetName_ChannelCount,
			WidgetName_SamplingFrequencies,
			WidgetName_TargetDirectory,
		} EWidgetName;

		std::map < ::GtkButton*, EWidgetName > m_vWidgetName;
	};
}

#endif //__OpenViBESkeletonGenerator_CDriverSkeletonGenerator_H__
