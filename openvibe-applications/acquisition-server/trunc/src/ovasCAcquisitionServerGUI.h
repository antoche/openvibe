#ifndef __OpenViBE_AcquisitionServer_CAcquisitionServerGUI_H__
#define __OpenViBE_AcquisitionServer_CAcquisitionServerGUI_H__

#include "ovas_base.h"
#include "ovasIDriver.h"
#include "ovasIHeader.h"
#include "ovasCHeader.h"

#include <socket/IConnectionServer.h>

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/version.hpp>

#include <gtk/gtk.h>

namespace OpenViBEAcquisitionServer
{
	class CAcquisitionServer;
	class CAcquisitionServerThread;

	class CAcquisitionServerGUI
	{
	public:

		CAcquisitionServerGUI(const OpenViBE::Kernel::IKernelContext& rKernelContext);
		virtual ~CAcquisitionServerGUI(void);

		virtual OpenViBE::boolean initialize(void);

		OpenViBEAcquisitionServer::IDriver& getDriver(void);
		OpenViBE::uint32 getSampleCountPerBuffer(void);
		OpenViBE::uint32 getTCPPort(void);
		OpenViBEAcquisitionServer::IHeader& getHeaderCopy(void);
		void setClientCount(OpenViBE::uint32 ui32ClientCount);
		void setDrift(OpenViBE::float64 f64Drift);
		void setImpedance(OpenViBE::uint32 ui32ChannelIndex, OpenViBE::float64 f64Impedance);
		void disconnect(void);

		// GTK button callbacks
		virtual void buttonPreferencePressedCB(::GtkButton* pButton);
		virtual void buttonConfigurePressedCB(::GtkButton* pButton);
		virtual void buttonConnectToggledCB(::GtkToggleButton* pButton);
		virtual void buttonStartPressedCB(::GtkButton* pButton);
		virtual void buttonStopPressedCB(::GtkButton* pButton);
		virtual void comboBoxDriverChanged(::GtkComboBox* pComboBox);
		virtual void comboBoxSampleCountPerSentBlockChanged(::GtkComboBox* pComboBox);

	protected :

		const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
		OpenViBEAcquisitionServer::IDriver* m_pDriver;
		OpenViBEAcquisitionServer::IDriverContext* m_pDriverContext;
		OpenViBEAcquisitionServer::CAcquisitionServer* m_pAcquisitionServer;
		OpenViBEAcquisitionServer::CAcquisitionServerThread* m_pAcquisitionServerThread;
		OpenViBEAcquisitionServer::CHeader m_oHeaderCopy;

		OpenViBE::uint32 m_ui32SampleCountPerBuffer;

		std::vector < OpenViBEAcquisitionServer::IDriver* > m_vDriver;

		::GtkBuilder* m_pBuilderInterface;

		::GtkWidget* m_pImpedanceWindow;
		std::vector < ::GtkWidget* > m_vLevelMesure;

		boost::thread* m_pThread;		
	};
};

#endif // __OpenViBE_AcquisitionServer_CAcquisitionServerGUI_H__
