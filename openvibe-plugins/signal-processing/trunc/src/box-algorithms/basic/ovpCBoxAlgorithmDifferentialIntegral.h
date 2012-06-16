#ifndef __OpenViBEPlugins_BoxAlgorithm_DifferentialIntegral_H__
#define __OpenViBEPlugins_BoxAlgorithm_DifferentialIntegral_H__

//You may have to change this path to match your folder organisation
#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <vector>

// The unique identifiers for the box and its descriptor.
// Identifier are randomly chosen by the skeleton-generator.

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		/**
		 * \class CBoxAlgorithmDifferentialIntegral
		 * \author Jozef Legény (INRIA)
		 * \date Thu Oct 27 15:24:05 2011
		 * \brief The class CBoxAlgorithmDifferentialIntegral describes the box DifferentialIntegral.
		 *
		 */
		class CBoxAlgorithmDifferentialIntegral : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:
			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
				

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			
			
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_DifferentialIntegral);

		protected:
			OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmDifferentialIntegral > m_oSignalDecoder;
			OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmDifferentialIntegral > m_oSignalEncoder;

		private:
			OpenViBE::float64 operation(OpenViBE::float64 a, OpenViBE::float64 b);
			OpenViBE::uint64 m_ui64Operation;
			OpenViBE::uint64 m_ui64FilterOrder;

			/// Holds the differentials/integrals of all orders from the previous step
			OpenViBE::float64** m_pPastData;
			OpenViBE::float64** m_pTmpData;

			/// Is true when the filter is stabilized
			OpenViBE::boolean* m_pStabilized;
			/// Counts the samples up to the filter order, used to stabilize the filter
			OpenViBE::uint32* m_pStep;


		};


		/**
		 * \class CBoxAlgorithmDifferentialIntegralDesc
		 * \author Jozef Legény (INRIA)
		 * \date Thu Oct 27 15:24:05 2011
		 * \brief Descriptor of the box DifferentialIntegral.
		 *
		 */
		class CBoxAlgorithmDifferentialIntegralDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Signal Differential/Integral"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Jozef Legény"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Calculates a differential or an integral of a signal"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Calculates a differential or an integral of a signal."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_DifferentialIntegral; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmDifferentialIntegral; }
			
			/*
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmDifferentialIntegralListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }
			*/
			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input Signal",OV_TypeId_Signal);

				rBoxAlgorithmPrototype.addOutput("Output Signal",OV_TypeId_Signal);

				rBoxAlgorithmPrototype.addSetting("Operation", OVP_TypeId_DifferentialIntegralOperation, OVP_TypeId_DifferentialIntegralOperation_Differential.toString());
				rBoxAlgorithmPrototype.addSetting("Order", OV_TypeId_Integer,
												  "1");

				
				return true;
			}
			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_DifferentialIntegralDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_DifferentialIntegral_H__
