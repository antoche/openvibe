#ifndef __OpenViBEPlugins_SimpleVisualisation_CTimeFrequencyMapDisplay_H__
#define __OpenViBEPlugins_SimpleVisualisation_CTimeFrequencyMapDisplay_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include "ovpCSpectrumDatabase.h"
#include "ovpCTimeFrequencyMapDisplay/ovpCTimeFrequencyMapDisplayView.h"

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CTimeFrequencyMapDisplay : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			/**
			 * \brief Constructor
			 */
			CTimeFrequencyMapDisplay(void);

			/**
			 * \brief Delete this plugin
			 */
			virtual void release(void) { delete this; }

			/** \name IBoxAlgorithm implementation */
			//@{
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);
			//@}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_TimeFrequencyMapDisplay)

		private:
			//Spectrum database
			CSpectrumDatabase* m_pSpectrumDatabase;
			//GUI management
			CTimeFrequencyMapDisplayView* m_pTimeFrequencyMapDisplayView;
		};

		class CTimeFrequencyMapDisplayDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Time-frequency map display"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Vincent Delannoy"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("2D time-frequency map"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Visualisation/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }
			virtual void release(void)                                   { }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_TimeFrequencyMapDisplay; }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-zoom-fit"); }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SimpleVisualisation::CTimeFrequencyMapDisplay(); }

			virtual OpenViBE::boolean hasFunctionality(OpenViBE::Kernel::EPluginFunctionality ePF) const
			{
				return ePF == OpenViBE::Kernel::PluginFunctionality_Visualization;
			}

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addSetting("Time Scale", OV_TypeId_Float, "10");
				rPrototype.addSetting("Minimum frequency to display", OV_TypeId_Float, "0");
				rPrototype.addSetting("Maximum frequency to display", OV_TypeId_Float, "40");
				rPrototype.addInput("Spectrum", OV_TypeId_Spectrum);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_TimeFrequencyMapDisplayDesc)
		};
	};
};

#endif
