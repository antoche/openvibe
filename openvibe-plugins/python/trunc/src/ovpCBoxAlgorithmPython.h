#ifndef __OpenViBEPlugins_BoxAlgorithm_Python_H__
#define __OpenViBEPlugins_BoxAlgorithm_Python_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#if defined TARGET_HAS_ThirdPartyPython

#include <string.h>
#include <string>
#include <vector>
//#include <map>
#include <Python.h>

namespace OpenViBEPlugins
{
	namespace Python
	{

		class CBoxAlgorithmPython : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:
			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_Python);

		protected:

			OpenViBE::uint64 m_ui64ClockFrequency;
			OpenViBE::CString m_sScriptFilename;
			
			std::vector < OpenViBEToolkit::TDecoder < CBoxAlgorithmPython >* > m_vDecoders;
			std::vector < OpenViBEToolkit::TEncoder < CBoxAlgorithmPython >* > m_vEncoders;

			//std::map<char,PyObject *> m_PyObjectMap;
			
			PyObject *m_pBox, *m_pBoxInput, *m_pBoxOutput, *m_pBoxSetting, *m_pBoxCurrentTime;
			PyObject *m_pBoxInitialize, *m_pBoxProcess, *m_pBoxUninitialize;
            OpenViBE::boolean m_bInitializeSucceeded ;
            
			
			OpenViBE::boolean logSysStdout(void);
			OpenViBE::boolean logSysStderr(void);
			void buildPythonSettings(void);

			OpenViBE::boolean initializePythonSafely();
			OpenViBE::boolean clearPyObjectMap();
			OpenViBE::boolean transferStreamedMatrixInputChunksToPython(OpenViBE::uint32 input_index);
			OpenViBE::boolean transferStreamedMatrixOutputChunksFromPython(OpenViBE::uint32 output_index);
			OpenViBE::boolean transferSignalInputChunksToPython(OpenViBE::uint32 input_index);
			OpenViBE::boolean transferSignalOutputChunksFromPython(OpenViBE::uint32 output_index);
			OpenViBE::boolean transferStimulationInputChunksToPython(OpenViBE::uint32 input_index);
			OpenViBE::boolean transferStimulationOutputChunksFromPython(OpenViBE::uint32 output_index);

			static OpenViBE::boolean m_bPythonInitialized;
			static OpenViBE::uint32 m_ui32PythonBoxInstanceCount;
			static PyObject *m_pMainModule, *m_pMainDictionnary; 
			static PyObject *m_pOVStreamedMatrixHeader, *m_pOVStreamedMatrixBuffer, *m_pOVStreamedMatrixEnd;
			static PyObject *m_pOVSignalHeader, *m_pOVSignalBuffer, *m_pOVSignalEnd;
			static PyObject *m_pOVStimulationHeader, *m_pOVStimulation, *m_pOVStimulationSet, *m_pOVStimulationEnd;
			static PyObject *m_pOVBuffer;
			static PyObject *m_pExecFileFunction ;
			static PyObject *m_pSysStdout, *m_pSysStderr;
			
		};
		
		class CBoxAlgorithmPythonListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			//virtual OpenViBE::boolean onInitialized(OpenViBE::Kernel::IBox& rBox) { return true; };
			//virtual OpenViBE::boolean onNameChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
			//virtual OpenViBE::boolean onInputConnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//virtual OpenViBE::boolean onInputDisconnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) 
			{
				rBox.setInputType(ui32Index, OV_TypeId_StreamedMatrix);
				return true;
			};
			//virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) 
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
				{
					return true;
				}
				else if (l_oTypeIdentifier == OV_TypeId_Stimulations)
				{
					return true;
				}
				else if (l_oTypeIdentifier == OV_TypeId_Signal)
				{
					return true;
				}
				else
				{
					if (l_oTypeIdentifier == OV_TypeId_FeatureVector)
					{
						this->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "Feature Vector input is not implemented.\n";
					}
					else if (l_oTypeIdentifier == OV_TypeId_Spectrum)
					{
						this->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "Spectrum input is not implemented.\n";
					}
					else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation)
					{
						this->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "Channel Localisation input is not implemented.\n";
					}
					else if (l_oTypeIdentifier == OV_TypeId_ExperimentationInformation)
					{
						this->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "Experimentation Information input is not implemented.\n";
					}
					else
					{
						this->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "Input type chosen is not implemented.\n";
					}
					this->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "Input " << ui32Index << " was set to the default type Streamed Matrix.\n";
					rBox.setInputType(ui32Index, OV_TypeId_StreamedMatrix);
					return false;
				} 
			};
			//virtual OpenViBE::boolean onInputNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//virtual OpenViBE::boolean onOutputConnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//virtual OpenViBE::boolean onOutputDisconnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onOutputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) 
			{ 
				rBox.setOutputType(ui32Index, OV_TypeId_StreamedMatrix);
				return true; 
			};
			//virtual OpenViBE::boolean onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual OpenViBE::boolean onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) 
			{ 
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(ui32Index, l_oTypeIdentifier);
				if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
				{
					return true;
				}
				else if (l_oTypeIdentifier == OV_TypeId_Stimulations)
				{
					return true;
				}
				else if (l_oTypeIdentifier == OV_TypeId_Signal)
				{
					return true;
				}
				else
				{
					if (l_oTypeIdentifier == OV_TypeId_FeatureVector)
					{
						this->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "Feature Vector output is not implemented.\n";
					}
					else if (l_oTypeIdentifier == OV_TypeId_Spectrum)
					{
						this->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "Spectrum output is not implemented.\n";
					}
					else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation)
					{
						this->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "Channel Localisation output is not implemented.\n";
					}
					else if (l_oTypeIdentifier == OV_TypeId_ExperimentationInformation)
					{
						this->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "Experimentation Information output is not implemented.\n";
					}
					else
					{
						this->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "Output type chosen is not implemented.\n";
					}
					this->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "Output " << ui32Index << " was set to the default type Streamed Matrix.\n";
					rBox.setOutputType(ui32Index, OV_TypeId_StreamedMatrix);
					return false;
				}  
			};
			//virtual OpenViBE::boolean onOutputNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//virtual OpenViBE::boolean onSettingAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//virtual OpenViBE::boolean onSettingRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//virtual OpenViBE::boolean onSettingTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//virtual OpenViBE::boolean onSettingNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//virtual OpenViBE::boolean onSettingDefaultValueChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//virtual OpenViBE::boolean onSettingValueChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};
		
		class CBoxAlgorithmPythonDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Python scripting"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Aurélien Van Langhenhove and Laurent George"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("CICIT Garches, Inria"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("This box executes a python script."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Scripting"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_Python; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Python::CBoxAlgorithmPython; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmPythonListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				//rBoxAlgorithmPrototype.addInput  ("Input",  OV_TypeId_StreamedMatrix);
				//rBoxAlgorithmPrototype.addInput  ("Input stimulations", OV_TypeId_Stimulations);
				//rBoxAlgorithmPrototype.addOutput ("Output", OV_TypeId_StreamedMatrix);
				//rBoxAlgorithmPrototype.addOutput ("Output stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Clock frequency (Hz)", OV_TypeId_Integer, "64");
				rBoxAlgorithmPrototype.addSetting("Script", OV_TypeId_Script, "");
				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);
				
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddSetting);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifySetting);
				
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_PythonDesc);
		};
	};
};

#endif // TARGET_HAS_ThirdPartyPython

#endif // __OpenViBEPlugins_BoxAlgorithm_Python_H__

