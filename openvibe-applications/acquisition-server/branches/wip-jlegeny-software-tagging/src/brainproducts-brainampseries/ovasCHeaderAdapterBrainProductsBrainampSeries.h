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

#ifndef __OpenViBE_AcquisitionServer_CHeaderAdapterBrainProductsBrainampSeries_H__
#define __OpenViBE_AcquisitionServer_CHeaderAdapterBrainProductsBrainampSeries_H__

#include "../ovasCHeader.h"

#if defined OVAS_OS_Windows

#include "ovas_defines_brainamp_series.h"

namespace OpenViBEAcquisitionServer
{
	class CHeaderAdapterBrainProductsBrainampSeries : public OpenViBEAcquisitionServer::CHeaderAdapter
	{
	public:

		CHeaderAdapterBrainProductsBrainampSeries(
			OpenViBEAcquisitionServer::IHeader& rAdpatedHeader,
			OpenViBEAcquisitionServer::EParameter* peChannelSelected);

		virtual OpenViBE::boolean setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
		virtual OpenViBE::boolean setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);

		virtual OpenViBE::uint32 getChannelCount(void) const;
		virtual const char* getChannelName(const OpenViBE::uint32 ui32ChannelIndex) const;

		OpenViBEAcquisitionServer::EParameter* m_peChannelSelected;
	};
}

#endif // OVAS_OS_Windows

#endif // __OpenViBE_AcquisitionServer_CHeaderAdapterBrainProductsBrainampSeries_H__
