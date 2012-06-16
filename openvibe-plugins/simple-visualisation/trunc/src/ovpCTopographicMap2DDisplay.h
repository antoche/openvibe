#ifndef __SamplePlugin_CTopographicMap2DDisplay_H__
#define __SamplePlugin_CTopographicMap2DDisplay_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>

#include "ovpCTopographicMapDatabase.h"
#include "ovpCTopographicMap2DDisplay/ovpCTopographicMap2DView.h"

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CTopographicMap2DDisplay : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >,
		virtual public OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback::ICallback
		{
		public:
			CTopographicMap2DDisplay(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(
				OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean processClock(
				OpenViBE::Kernel::IMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_TopographicMap2DDisplay)

			//IBoxAlgorithmStreamedMatrixInputReaderCallback::ICallback implementation
			virtual void setMatrixDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount);
			virtual void setMatrixDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize);
			virtual void setMatrixDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel);
			virtual void setMatrixBuffer(const OpenViBE::float64* pBuffer);
			/*
			virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
			virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
			virtual void setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer);
			virtual void setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency);
			virtual void setSampleBuffer(const OpenViBE::float64* pBuffer);
			*/
		protected:
			//ebml
			EBML::IReader* m_pStreamedMatrixReader;

			OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback* m_pStreamedMatrixReaderCallBack;

			//Start and end time of the last buffer
			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::uint64 m_ui64EndTime;

			OpenViBE::Kernel::IAlgorithmProxy* m_pSphericalSplineInterpolation;
			CTopographicMapDatabase* m_pTopographicMapDatabase;
			CSignalDisplayDrawable* m_pTopographicMap2DView; //main object used for the display (contains all the GUI code)
		};

		class CTopographicMap2DDisplayDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("2D topographic map"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Vincent Delannoy"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("This box demonstrates how to perform spherical spline interpolation"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Visualisation/Topography"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString(GTK_STOCK_EXECUTE); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_TopographicMap2DDisplay; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SimpleVisualisation::CTopographicMap2DDisplay(); }

			virtual OpenViBE::boolean hasFunctionality(OpenViBE::Kernel::EPluginFunctionality ePF) const
			{
				return ePF == OpenViBE::Kernel::PluginFunctionality_Visualization;
			}

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addSetting("Interpolation type", OVP_TypeId_SphericalLinearInterpolationType, "1");
				rPrototype.addSetting("Delay (in s)", OV_TypeId_Float, "0");
				rPrototype.addInput("Signal", OV_TypeId_StreamedMatrix);
				rPrototype.addInput("Channel localization", OV_TypeId_ChannelLocalisation);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_TopographicMap2DDisplayDesc);
		};
	};
};

#endif // __SamplePlugin_CTopographicMap2DDisplay_H__
