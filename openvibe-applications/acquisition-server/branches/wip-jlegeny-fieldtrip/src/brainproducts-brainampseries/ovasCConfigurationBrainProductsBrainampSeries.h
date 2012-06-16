/*
 * Brain Products Brainamp Series driver for OpenViBE
 * Copyright (C) 2010 INRIA - Author : Yann Renard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#ifndef __OpenViBE_AcquisitionServer_CConfigurationBrainProductsBrainampSeries_H__
#define __OpenViBE_AcquisitionServer_CConfigurationBrainProductsBrainampSeries_H__

#include "../ovasCConfigurationBuilder.h"

#if defined OVAS_OS_Windows

#include "ovas_defines_brainamp_series.h"

#include <gtk/gtk.h>

#include <vector>

namespace OpenViBEAcquisitionServer
{
	class CDriverBrainProductsBrainampSeries;

	class CConfigurationBrainProductsBrainampSeries : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		CConfigurationBrainProductsBrainampSeries(
			CDriverBrainProductsBrainampSeries& rDriver,
			const char* sGtkBuilderFileName,
			OpenViBE::uint32& rUSBIndex,
			OpenViBE::uint32& rui32DecimationFactor,
			EParameter* pChannelSelected,
			EParameter* peLowPassFilterFull,
			EParameter* peResolutionFull,
			EParameter* peDCCouplingFull,
			EParameter& reLowPass,
			EParameter& reResolution,
			EParameter& reDCCoupling,
			EParameter& reImpedance);

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

		void buttonChannelDetailsPressedCB(void);
		void comboBoxDeviceChangedCB(void);

	protected:

		OpenViBEAcquisitionServer::CDriverBrainProductsBrainampSeries& m_rDriver;
		OpenViBE::uint32& m_rUSBIndex;
		OpenViBE::uint32& m_rui32DecimationFactor;
		EParameter* m_peChannelSelected;
		EParameter* m_peLowPassFilterFull;
		EParameter* m_peResolutionFull;
		EParameter* m_peDCCouplingFull;
		EParameter& m_reLowPass;
		EParameter& m_reResolution;
		EParameter& m_reDCCoupling;
		EParameter& m_reImpedance;

	private:

		::GtkWidget* m_pCalibrateDialog;
		OpenViBE::boolean m_bCalibrationDone;
	};
};

#endif // OVAS_OS_Windows

#endif // __OpenViBE_AcquisitionServer_CConfigurationBrainProductsBrainampSeries_H__
