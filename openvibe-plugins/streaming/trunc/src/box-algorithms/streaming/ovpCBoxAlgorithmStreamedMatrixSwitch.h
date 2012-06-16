#ifndef __OpenViBEPlugins_BoxAlgorithm_StreamedMatrixSwitch_H__
#define __OpenViBEPlugins_BoxAlgorithm_StreamedMatrixSwitch_H__

//You may have to change this path to match your folder organisation
#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <stdio.h>
#include <map>
#include <vector>

// The unique identifiers for the box and its descriptor.
// Identifier are randomly chosen by the skeleton-generator.
#define OVP_ClassId_BoxAlgorithm_StreamedMatrixSwitch OpenViBE::CIdentifier(0x556A2C32, 0x61DF49FC)
#define OVP_ClassId_BoxAlgorithm_StreamedMatrixSwitchDesc OpenViBE::CIdentifier(0x556A2C32, 0x61DF49FC)

namespace OpenViBEPlugins
{
	namespace Streaming
	{
		/**
		 * \class CBoxAlgorithmStreamedMatrixSwitch
		 * \author Laurent Bonnet (INRIA)
		 * \date Thu May 12 18:02:05 2011
		 * \brief The class CBoxAlgorithmStreamedMatrixSwitch describes the box Streamed Matrix Switch.
		 *
		 */
		class CBoxAlgorithmStreamedMatrixSwitch : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:
			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
				
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StreamedMatrixSwitch);

		protected:
			OpenViBEToolkit::TStimulationDecoder < CBoxAlgorithmStreamedMatrixSwitch > m_oStimulationDecoder;
			OpenViBEToolkit::TDecoder < CBoxAlgorithmStreamedMatrixSwitch > * m_pStreamDecoder;
			//OpenViBEToolkit::TEncoder < CBoxAlgorithmStreamedMatrixSwitch > * m_pStreamEncoder;

			std::map <OpenViBE::uint64, OpenViBE::uint32> m_mStimulationOutputIndexMap;
			OpenViBE::int32 m_i32ActiveOutputIndex;
			OpenViBE::uint64 m_ui64LastStimulationInputChunkEndTime;
		};


		class CBoxAlgorithmStreamedMatrixSwitchListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{ 
				if(ui32Index == 0)
				{
					rBox.setInputType(0,OV_TypeId_Stimulations);
					return true; 
				}
				
				OpenViBE::CIdentifier l_oIdentifier;
				rBox.getInputType(1,l_oIdentifier);
				
				//The input can be only streamed matrix or children streams
				if(!this->getTypeManager().isDerivedFromStream(l_oIdentifier, OV_TypeId_StreamedMatrix))
				{
					OpenViBE::CIdentifier l_oOutputIdentifier;
					rBox.getOutputType(0,l_oOutputIdentifier);
					rBox.setInputType(ui32Index, l_oOutputIdentifier);
				}

				// all output must have the input type
				for(OpenViBE::uint32 i = 0; i < rBox.getOutputCount(); i++)
				{
					rBox.setOutputType(i,l_oIdentifier);
				}
				return true; 
			};
			
			virtual OpenViBE::boolean onOutputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				// the output must have the same type as the input
				OpenViBE::CIdentifier l_oIdentifier;
				rBox.getInputType(1,l_oIdentifier);
				rBox.setOutputType(ui32Index,l_oIdentifier);
				
				char l_sName[1024];
				::sprintf(l_sName, "Switch stim for output %i", ui32Index+1);
				rBox.addSetting(l_sName, OV_TypeId_Stimulation,"OVTK_StimulationId_Label_00");

				return true;
			};

			virtual OpenViBE::boolean onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.removeSetting(ui32Index);
				return true;
			}
			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};
		

		/**
		 * \class CBoxAlgorithmStreamedMatrixSwitchDesc
		 * \author Laurent Bonnet (INRIA)
		 * \date Thu May 12 18:02:05 2011
		 * \brief Descriptor of the box Streamed Matrix Switch.
		 *
		 */
		class CBoxAlgorithmStreamedMatrixSwitchDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Stream Switch"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Laurent Bonnet"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Redirect its input on a particular output"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This box act as a switch between N possible outputs for its Streamed Matrix input. N Stimulation settings trigger the switch."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Streaming"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-sort-ascending"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_StreamedMatrixSwitch; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Streaming::CBoxAlgorithmStreamedMatrixSwitch; }
			
			
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmStreamedMatrixSwitchListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }
			
			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Triggers",OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput("Matrix",OV_TypeId_StreamedMatrix);

				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				
				rBoxAlgorithmPrototype.addOutput("Output",OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addOutput("Output",OV_TypeId_StreamedMatrix);

				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);
				
				rBoxAlgorithmPrototype.addSetting("Switch stim for output 1",OV_TypeId_Stimulation,"OVTK_StimulationId_Label_00");
				rBoxAlgorithmPrototype.addSetting("Switch stim for output 2",OV_TypeId_Stimulation,"OVTK_StimulationId_Label_01");

				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifySetting);
				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddSetting);
				
				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);
				
				return true;
			}
			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamedMatrixSwitchDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_StreamedMatrixSwitch_H__
