#ifndef __OpenViBEPlugins_BoxAlgorithm_VotingClassifier_H__
#define __OpenViBEPlugins_BoxAlgorithm_VotingClassifier_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <vector>
#include <map>
#include <cstdio>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_VotingClassifier     OpenViBE::CIdentifier(0xFAF62C2B, 0x0B75D1B3)
#define OVP_ClassId_BoxAlgorithm_VotingClassifierDesc OpenViBE::CIdentifier(0x97E3CCC5, 0xAC353ED2)

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CBoxAlgorithmVotingClassifier : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32Index);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_VotingClassifier);

		protected:

			OpenViBE::uint64 m_ui64NumberOfRepetitions;
			OpenViBE::uint64 m_ui64TargetClassLabel;
			OpenViBE::uint64 m_ui64NonTargetClassLabel;
			OpenViBE::uint64 m_ui64RejectClassLabel;
			OpenViBE::uint64 m_ui64ResultClassLabelBase;
			OpenViBE::boolean m_bChooseOneIfExAequo;

		private:

			typedef struct
			{
				OpenViBE::Kernel::IAlgorithmProxy* m_pDecoder;
				OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer;
				OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_pStimulationSet;
				OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pMatrix;
				std::vector < std::pair < OpenViBE::float64, OpenViBE::uint64 > > m_vScore;
			} SInput;

			std::map < OpenViBE::uint32, CBoxAlgorithmVotingClassifier::SInput > m_vClassificationResults;

			OpenViBE::Kernel::IAlgorithmProxy* m_pClassificationChoiceEncoder;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IStimulationSet*> ip_pClassificationChoiceStimulationSet;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pClassificationChoiceMemoryBuffer;

			OpenViBE::uint64 m_ui64LastTime;
			OpenViBE::boolean m_bMatrixBased;
			OpenViBE::CIdentifier m_oStreamDecoder_OutputTriggerId_ReceivedHeader;
			OpenViBE::CIdentifier m_oStreamDecoder_OutputTriggerId_ReceivedBuffer;
			OpenViBE::CIdentifier m_oStreamDecoder_OutputTriggerId_ReceivedEnd;
		};


		class CBoxAlgorithmVotingClassifierListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			CBoxAlgorithmVotingClassifierListener(void)
				:m_oInputTypeIdentifier(OV_TypeId_Stimulations)
			{
			}

			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oInputTypeIdentifier;
				rBox.getInputType(ui32Index, l_oInputTypeIdentifier);
				if(l_oInputTypeIdentifier==OV_TypeId_Stimulations || l_oInputTypeIdentifier==OV_TypeId_StreamedMatrix)
				{
					m_oInputTypeIdentifier=l_oInputTypeIdentifier;
					for(OpenViBE::uint32 i=0; i<rBox.getInputCount(); i++)
					{
						rBox.setInputType(i, m_oInputTypeIdentifier);
					}
				}
				else
				{
					rBox.setInputType(ui32Index, m_oInputTypeIdentifier);
				}
				return true;
			}

			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				for(OpenViBE::uint32 i=0; i<rBox.getInputCount(); i++)
				{
					char l_sBuffer[1024];
					sprintf(l_sBuffer, "Classification result %i", i);
					rBox.setInputType(i, m_oInputTypeIdentifier);
					rBox.setInputName(i, l_sBuffer);
				}
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);

		protected:

			OpenViBE::CIdentifier m_oInputTypeIdentifier;
		};

		class CBoxAlgorithmVotingClassifierDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Voting Classifier"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Classification"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-apply"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_VotingClassifier; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Classification::CBoxAlgorithmVotingClassifier; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Classification result 1", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput  ("Classification result 2", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Classification choice",   OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Number of repetitions",   OV_TypeId_Integer,     "12");
				rBoxAlgorithmPrototype.addSetting("Target class label",      OV_TypeId_Stimulation, "OVTK_StimulationId_Target");
				rBoxAlgorithmPrototype.addSetting("Non target class label",  OV_TypeId_Stimulation, "OVTK_StimulationId_NonTarget");
				rBoxAlgorithmPrototype.addSetting("Reject class label",      OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				rBoxAlgorithmPrototype.addSetting("Result class label base", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01");
				rBoxAlgorithmPrototype.addSetting("Choose one if ex-aequo",  OV_TypeId_Boolean,     "false");
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyInput);
				return true;
			}

			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const { return new CBoxAlgorithmVotingClassifierListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) { delete pBoxListener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_VotingClassifierDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_VotingClassifier_H__
