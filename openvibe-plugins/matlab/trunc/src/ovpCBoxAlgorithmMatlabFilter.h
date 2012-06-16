#ifndef __OpenViBEPlugins_BoxAlgorithm_MatlabFilter_H__
#define __OpenViBEPlugins_BoxAlgorithm_MatlabFilter_H__

#if defined TARGET_HAS_ThirdPartyMatlab

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>



// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_MatlabFilter     OpenViBE::CIdentifier(0x03303E0D, 0x39FE10DE)
#define OVP_ClassId_BoxAlgorithm_MatlabFilterDesc OpenViBE::CIdentifier(0x46130E2E, 0x34F90BA0)

namespace OpenViBEPlugins
{
	namespace Tools
	{
		class CBoxAlgorithmMatlabFilter : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_MatlabFilter);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationEncoder;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamedMatrixDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamedMatrixEncoder;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pStimulationSet;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > op_pStimulationSet;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrix;

			OpenViBE::CStimulationSet m_oPendingStimulationSet;
			OpenViBE::uint64 m_ui64LatestStimulationChunkEndTime;

			void* m_pMatlabEngineHandle;
			void* m_pMatlabStimulationHandle;
			void* m_pMatlabMatrixHandle;
			void* m_pMatlabBCIContextHandle;

			OpenViBE::CString m_sMatlabPath;
			OpenViBE::CString m_sProcessFunction;
			OpenViBE::CString m_sInitializeFunction;

		private:
			OpenViBE::boolean OpenMatlabEngineSafely(void);
			OpenViBE::boolean CloseMatlabEngineSafely(void);
		};

		class CBoxAlgorithmMatlabFilterDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Matlab filter"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Y.Renard / R. Oostenveld"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA / Donders Institute for Brain, Cognition and Behaviour"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("This box communicates with the Matlab Engine to perform matrix and stim processing."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("User must implement the matlab functions:\n[matrix_out]=bci_Initialize(bci_context,matrix_in)\n[matrix_out,stim_out]=bci_Process(bci_context,matrix_in,stim_in)"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Scripting"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("2.1"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_MatlabFilter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Tools::CBoxAlgorithmMatlabFilter; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Streamed matrix", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addInput  ("Stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Filtered streamed matrix", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addOutput ("Stimulations", OV_TypeId_Stimulations);
#if defined OVP_OS_Linux
				rBoxAlgorithmPrototype.addSetting("Matlab launch command", OV_TypeId_String, "[ssh user@host] /path/to/matlab");
#elif defined OVP_OS_Windows
				rBoxAlgorithmPrototype.addSetting("Path to Matlab Executable", OV_TypeId_String, "C:/Program Files/MATLAB/R2011a/bin/win32");
#else
#endif
				rBoxAlgorithmPrototype.addSetting("Matlab working directory", OV_TypeId_String, "[path-to-my-matlab-workspace]");
				rBoxAlgorithmPrototype.addSetting("Initialize function", OV_TypeId_String, "bci_Initialize");
				rBoxAlgorithmPrototype.addSetting("Process function", OV_TypeId_String, "bci_Process");
				
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_MatlabFilterDesc);
		};
	};
};

#endif // TARGET_HAS_ThirdPartyMatlab

#endif // __OpenViBEPlugins_BoxAlgorithm_MatlabFilter_H__
