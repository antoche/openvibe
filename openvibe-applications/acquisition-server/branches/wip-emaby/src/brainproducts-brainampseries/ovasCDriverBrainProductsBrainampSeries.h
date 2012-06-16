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

#ifndef __OpenViBE_AcquisitionServer_CDriverBrainProductsBrainampSeries_H__
#define __OpenViBE_AcquisitionServer_CDriverBrainProductsBrainampSeries_H__

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

#if defined OVAS_OS_Windows

#include "ovas_defines_brainamp_series.h"

#include "ovasCConfigurationBrainProductsBrainampSeries.h"
#include "ovasCHeaderAdapterBrainProductsBrainampSeries.h"

#include <vector>
#include <list>

namespace OpenViBEAcquisitionServer
{
	class CDriverBrainProductsBrainampSeries : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		friend class CConfigurationBrainProductsBrainampSeries;

		static char* getErrorMessage(OpenViBE::uint32 ui32Error);

	public:

		CDriverBrainProductsBrainampSeries(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual void release(void);
		virtual const char* getName(void);

		virtual OpenViBE::boolean initialize(
			const OpenViBE::uint32 ui32SampleCountPerSentBlock,
			OpenViBEAcquisitionServer::IDriverCallback& rCallback);
		virtual OpenViBE::boolean start(void);
		virtual OpenViBE::boolean loop(void);
		virtual OpenViBE::boolean stop(void);
		virtual OpenViBE::boolean uninitialize(void);

		virtual OpenViBE::boolean startImpedanceCheck(void);
		virtual OpenViBE::boolean stopImpedanceCheck(void);

		virtual OpenViBE::boolean isConfigurable(void);
		virtual OpenViBE::boolean configure(void);
		virtual const OpenViBEAcquisitionServer::IHeader* getHeader(void) { return &m_oHeaderAdapter; }

	protected:

		OpenViBE::boolean getDeviceDetails(const OpenViBE::uint32 ui32Index, OpenViBE::uint32* pAmplifierCount, OpenViBE::uint32* pAmplifierType);

	protected:

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		OpenViBEAcquisitionServer::CHeader m_oHeader;
		OpenViBEAcquisitionServer::CHeaderAdapterBrainProductsBrainampSeries m_oHeaderAdapter;

		OpenViBE::boolean m_bImpedanceCheck;

		void* m_pDevice;
		CBrainampSetup* m_pDeviceSetup;
		CBrainampCalibrationSettings* m_pDeviceCalibrationSettings;

		OpenViBE::uint32 m_ui32ImpedanceCheckSignalFrequency;
		OpenViBE::uint32 m_ui32DecimationFactor;
		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::float32* m_pSample;
		OpenViBE::int16* m_pBuffer;
		OpenViBE::uint16 m_ui16Marker;

		std::list < std::vector < OpenViBE::float64 > > m_vImpedanceBuffer;
		std::vector < OpenViBE::float64 > m_vImpedance;

		unsigned long m_ui32BytesReturned;

		OpenViBE::uint32 m_ui32USBIndex;
		OpenViBE::float32 m_pf32ResolutionFactor[256];
		OpenViBEAcquisitionServer::EParameter m_peChannelSelected[256];
		OpenViBEAcquisitionServer::EParameter m_peLowPassFilterFull[256];
		OpenViBEAcquisitionServer::EParameter m_peResolutionFull[256];
		OpenViBEAcquisitionServer::EParameter m_peDCCouplingFull[256];
		OpenViBEAcquisitionServer::EParameter m_eLowPass;
		OpenViBEAcquisitionServer::EParameter m_eResolution;
		OpenViBEAcquisitionServer::EParameter m_eDCCoupling;
		OpenViBEAcquisitionServer::EParameter m_eImpedance;
	};
};

#endif // OVAS_OS_Windows

#endif // __OpenViBE_AcquisitionServer_CDriverBrainProductsBrainampSeries_H__
