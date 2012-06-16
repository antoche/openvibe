#ifndef __OpenViBEPlugins_BoxAlgorithm_StreamedMatrixMerger_H__
#define __OpenViBEPlugins_BoxAlgorithm_StreamedMatrixMerger_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <vector>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_StreamedMatrixMerger     OpenViBE::CIdentifier(0x37CAA263, 0x3C054845)
#define OVP_ClassId_BoxAlgorithm_StreamedMatrixMergerDesc OpenViBE::CIdentifier(0x2710C342, 0xDB68BE34)



namespace OpenViBEPlugins
{
	namespace Streaming
	{
		class CBoxAlgorithmStreamedMatrixMerger : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{     
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StreamedMatrixMerger);

		protected:

			std::vector < OpenViBE::Kernel::IAlgorithmProxy* > m_vStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::uint64 m_ui64LastStartTime;
			OpenViBE::uint64 m_ui64LastEndTime;

			OpenViBE::boolean m_bIncoherentChunkDating;
			OpenViBE::boolean m_bHeaderSent;
		};

		class CBoxAlgorithmStreamedMatrixMergerListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				OpenViBE::uint32 i;

				for(i=0; i<rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Input stream %u", i+1);
					rBox.setInputName(i, l_sName);
				}

				return true;
			}

			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(0, l_oTypeIdentifier);
				rBox.setInputType(ui32Index, l_oTypeIdentifier);
				return this->check(rBox);
			};

			virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(0, l_oTypeIdentifier);
				while(rBox.getInputCount() < 2)
				{
					rBox.addInput("", l_oTypeIdentifier);
				}
				return this->check(rBox);
			}

			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				if(this->getTypeManager().isDerivedFromStream(l_oTypeIdentifier, OV_TypeId_StreamedMatrix))
				{
					for(OpenViBE::uint32 i=0; i<rBox.getInputCount(); i++)
					{
						rBox.setInputType(i, l_oTypeIdentifier);
					}
					rBox.setOutputType(0, l_oTypeIdentifier);
				}
				else
				{
					rBox.getOutputType(0, l_oTypeIdentifier);
					rBox.setInputType(ui32Index, l_oTypeIdentifier);
				}
				return true;
			}

			virtual OpenViBE::boolean onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(0, l_oTypeIdentifier);
				if(this->getTypeManager().isDerivedFromStream(l_oTypeIdentifier, OV_TypeId_StreamedMatrix))
				{
					for(OpenViBE::uint32 i=0; i<rBox.getInputCount(); i++)
					{
						rBox.setInputType(i, l_oTypeIdentifier);
					}
				}
				else
				{
					rBox.getInputType(0, l_oTypeIdentifier);
					rBox.setOutputType(0, l_oTypeIdentifier);
				}
				return true;
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmStreamedMatrixMergerDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Streamed matrix merger"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Emmanuel Maby"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Inserm 1028"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Merges streamed matrix buffers in a new stream"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Streaming"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-sort-ascending"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_StreamedMatrixMerger; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Streaming::CBoxAlgorithmStreamedMatrixMerger; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmStreamedMatrixMergerListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input stream 1", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addInput  ("Input stream 2", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addOutput ("Merged streamed matrix", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsDeprecated);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamedMatrixMergerDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_StreamedMatrixMerger_H__
