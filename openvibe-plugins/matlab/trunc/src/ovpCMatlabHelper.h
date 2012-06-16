#ifndef __OpenViBEPlugins_MatlabHelper_H__
#define __OpenViBEPlugins_MatlabHelper_H__

#if defined TARGET_HAS_ThirdPartyMatlab

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <engine.h>

namespace OpenViBEPlugins
{
	namespace Matlab
	{
		class CMatlabHelper
		{
		public:
			OpenViBE::boolean setStreamedMatrixInputHeader(OpenViBE::uint32 ui32InputIndex, OpenViBE::IMatrix * pMatrix);
			OpenViBE::boolean setFeatureVectorInputHeader(OpenViBE::uint32 ui32InputIndex, OpenViBE::IMatrix * pMatrix);
			OpenViBE::boolean setSignalInputHeader(OpenViBE::uint32 ui32InputIndex, OpenViBE::IMatrix * pMatrix, OpenViBE::uint64 ui64SamplingRate);
			OpenViBE::boolean setChannelLocalisationInputHeader(OpenViBE::uint32 ui32InputIndex, OpenViBE::IMatrix * pMatrix, OpenViBE::boolean bDynamic);
			OpenViBE::boolean setSpectrumInputHeader(OpenViBE::uint32 ui32InputIndex, OpenViBE::IMatrix * pMatrix, OpenViBE::IMatrix * pFrequencyBands);
			OpenViBE::boolean setStimulationsInputHeader(OpenViBE::uint32 ui32InputIndex);
			
			// The input buffers for streamed matrix and its children streams are the same.
			OpenViBE::boolean addStreamedMatrixInputBuffer(OpenViBE::uint32 ui32InputIndex, OpenViBE::IMatrix * pMatrix, OpenViBE::uint64 ui64OpenvibeStartTime,OpenViBE::uint64 ui64OpenvibeEndTime);
			OpenViBE::boolean addStimulationsInputBuffer(OpenViBE::uint32 ui32InputIndex,OpenViBE::IStimulationSet * pStimulationSet, OpenViBE::uint64 ui64OpenvibeStartTime,OpenViBE::uint64 ui64OpenvibeEndTime);
			
			OpenViBE::boolean setStreamedMatrixOutputHeader(OpenViBE::uint32 ui32OutputIndex, OpenViBE::IMatrix * pMatrix);
			OpenViBE::boolean setFeatureVectorOutputHeader(OpenViBE::uint32 ui32OutputIndex, OpenViBE::IMatrix * pMatrix);
			OpenViBE::boolean setSignalOutputHeader(OpenViBE::uint32 ui32OutputIndex, OpenViBE::IMatrix * pMatrix, OpenViBE::uint64 & rSamplingRate);
			OpenViBE::boolean setChannelLocalisationOutputHeader(OpenViBE::uint32 ui32OutputIndex, OpenViBE::IMatrix * pMatrix, OpenViBE::boolean &bDynamic);
			OpenViBE::boolean setSpectrumOutputHeader(OpenViBE::uint32 ui32OutputIndex, OpenViBE::IMatrix * pMatrix, OpenViBE::IMatrix * pFrequencyBands);
			OpenViBE::boolean setStimulationsOutputHeader(OpenViBE::uint32 ui32OutputIndex, OpenViBE::IStimulationSet * pStimulationSet);
			
			// The output buffers for streamed matrix and its children streams are the same.
			OpenViBE::boolean popStreamedMatrixOutputBuffer(OpenViBE::uint32 ui32OutputIndex, OpenViBE::IMatrix * pMatrix, OpenViBE::uint64& rStartTime, OpenViBE::uint64& rEndTime);
			OpenViBE::boolean popStimulationsOutputBuffer(OpenViBE::uint32 ui32OutputIndex,OpenViBE::IStimulationSet * pStimulationSet, OpenViBE::uint64& rStartTime, OpenViBE::uint64& rEndTime);
		
			void setMatlabEngine(Engine * pEngine) { m_pMatlabEngine = pEngine; }
			void setBoxInstanceVariableName(OpenViBE::CString sName) { m_sBoxInstanceVariableName = sName; }

		private:
			Engine * m_pMatlabEngine;
			OpenViBE::CString m_sBoxInstanceVariableName; //must be unique

			OpenViBE::CString escapeMatlabString(OpenViBE::CString sStringToEscape);
		};
	};
};

#endif // TARGET_HAS_ThirdPartyMatlab

#endif // __OpenViBEPlugins_MatlabHelper_H__
