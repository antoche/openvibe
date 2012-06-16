#ifndef __OpenViBEPlugins_SignalProcessing_CSimpleDSP_H__
#define __OpenViBEPlugins_SignalProcessing_CSimpleDSP_H__

#include "ovp_defines.h"

#include <system/Time.h>
#include <openvibe-toolkit/ovtk_all.h>

#include "ovpCSimpleDSP/ovpCEquationParser.h"

#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>
#include <ebml/IWriter.h>
#include <ebml/IWriterHelper.h>

#include <ebml/TReaderCallbackProxy.h>
#include <ebml/TWriterCallbackProxy.h>

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CSimpleDSP : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CSimpleDSP(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);
			virtual void evaluate(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_SimpleDSP)

		public:

			std::vector < OpenViBE::IMatrix* > m_vMatrix;
			std::vector < OpenViBE::Kernel::IAlgorithmProxy* > m_vStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;

			CEquationParser * m_pEquationParser;

			OpenViBE::uint64 m_ui64EquationType;
			OpenViBE::float64 m_f64SpecialEquationParameter;
			OpenViBE::float64** m_ppVariable;

			OpenViBE::boolean m_bCheckChunkDates;
		};

		class CSimpleDSPListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				char l_sName[1024];
				::sprintf(l_sName, "Input - %c", 'A' + ui32Index);
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(0, l_oTypeIdentifier);
				rBox.setInputType(ui32Index, l_oTypeIdentifier);
				rBox.setInputName(ui32Index, l_sName);
				return true;
			}

			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::uint32 i;
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				if(this->getTypeManager().isDerivedFromStream(l_oTypeIdentifier, OV_TypeId_StreamedMatrix))
				{
					rBox.setOutputType(0, l_oTypeIdentifier);
					for(i=0; i<rBox.getInputCount(); i++)
					{
						rBox.setInputType(i, l_oTypeIdentifier);
					}
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
				OpenViBE::uint32 i;
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(ui32Index, l_oTypeIdentifier);
				if(this->getTypeManager().isDerivedFromStream(l_oTypeIdentifier, OV_TypeId_StreamedMatrix))
				{
					rBox.setOutputType(0, l_oTypeIdentifier);
					for(i=0; i<rBox.getInputCount(); i++)
					{
						rBox.setInputType(i, l_oTypeIdentifier);
					}
				}
				else
				{
					rBox.getInputType(0, l_oTypeIdentifier);
					rBox.setOutputType(ui32Index, l_oTypeIdentifier);
				}
				return true;
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CSimpleDSPDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Simple DSP"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Bruno Renier / Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA / IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Apply mathematical formulaes to matrices."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_SimpleDSP; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CSimpleDSP(); }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CSimpleDSPListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Input - A", OV_TypeId_Signal);
				rPrototype.addOutput("Output", OV_TypeId_Signal);
				rPrototype.addSetting("Equation", OV_TypeId_String, "x");

				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_SimpleDSPDesc)
		};
	}
}

#endif // __OpenViBEPlugins_SignalProcessing_CSimpleDSP_H__
