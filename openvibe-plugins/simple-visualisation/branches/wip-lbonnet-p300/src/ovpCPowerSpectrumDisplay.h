#ifndef __OpenViBEPlugins_SimpleVisualisation_CPowerSpectrumDisplay_H__
#define __OpenViBEPlugins_SimpleVisualisation_CPowerSpectrumDisplay_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include "ovpCPowerSpectrumDisplay/ovpCPowerSpectrumDatabase.h"
#include "ovpCPowerSpectrumDisplay/ovpCPowerSpectrumDisplayView.h"

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{

		class CPowerSpectrumDisplay : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>,
		virtual public OpenViBEToolkit::IBoxAlgorithmSpectrumInputReaderCallback::ICallback
		{
			public:
				CPowerSpectrumDisplay();

				virtual void release(void) { delete this; }
				virtual OpenViBE::boolean initialize();
				virtual OpenViBE::boolean uninitialize();
				virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
				virtual OpenViBE::boolean process();

				_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_PowerSpectrumDisplay)

				//IBoxAlgorithmSpectrumInputReaderCallback::ICallback implementation
				virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
				virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
				virtual void setFrequencyBandCount(const OpenViBE::uint32 ui32FrequencyBandCount);
				virtual void setFrequencyBandName(const OpenViBE::uint32 ui32FrequencyBandIndex, const char* sFrequencyBandName);
				virtual void setFrequencyBandStart(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStart);
				virtual void setFrequencyBandStop(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStop);
				virtual void setBuffer(const OpenViBE::float64* pBuffer);

			public:

				//ebml
				EBML::IReader* m_pReader;
				OpenViBEToolkit::IBoxAlgorithmSpectrumInputReaderCallback* m_pSpectrumReaderCallBack;

				//main object used for the display (contains all the GUI code)
				CSignalDisplayDrawable* m_pPowerSpectrumDisplayView;

				//Contains all the data about the incoming signal
				CPowerSpectrumDatabase* m_pPowerSpectrumDisplayDatabase;

				//Start and end time of the last buffer
				OpenViBE::uint64 m_ui64StartTime;
				OpenViBE::uint64 m_ui64EndTime;
		};

		class CPowerSpectrumDisplayDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
			public:
				virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Power spectrum display"); }
				virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Vincent Delannoy"); }
				virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
				virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Power spectrum in frequency bands"); }
				virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("TODO"); }
				virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Visualisation/Basic"); }
				virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
				virtual void release(void)                                   { }
				virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_PowerSpectrumDisplay; }
				virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-zoom-fit"); }
				virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SimpleVisualisation::CPowerSpectrumDisplay(); }

				virtual OpenViBE::boolean hasFunctionality(OpenViBE::Kernel::EPluginFunctionality ePF) const
				{
					return ePF == OpenViBE::Kernel::PluginFunctionality_Visualization;
				}

				virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
				{
					rPrototype.addSetting("Minimum frequency to display", OV_TypeId_Float, "0");
					rPrototype.addSetting("Maximum frequency to display", OV_TypeId_Float, "40");
					rPrototype.addInput("Spectrum", OV_TypeId_Spectrum);
					return true;
				}

				_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_PowerSpectrumDisplayDesc)
		};
	};
};

#endif // __OpenViBEPlugins_SimpleVisualisation_CPowerSpectrumDisplay_H__
