#ifndef __OpenViBEPlugins_BoxAlgorithm_QuadraticForm_H__
#define __OpenViBEPlugins_BoxAlgorithm_QuadraticForm_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmQuadraticForm : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processEvent(OpenViBE::CMessageEvent& rMessageEvent);
			virtual OpenViBE::boolean processSignal(OpenViBE::CMessageSignal& rMessageSignal);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_QuadraticForm);

		protected:

			//algorithms for encoding and decoding EBML stream
			OpenViBE::Kernel::IAlgorithmProxy* m_pEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pDecoder;

			//input and output buffers
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > m_oEBMLMemoryBufferHandleInput;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > m_oEBMLMemoryBufferHandleOutput;

			//the signal matrices (input and output)
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_oMatrixInputHandle;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_oMatrixOutputHandle;

			//start and end times
			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::uint64 m_ui64EndTime;

			//The matrix used in the quadratic form: the quadratic operator
			OpenViBE::CMatrix m_oQuadraticOperator;

			//dimensions (number of input channels and number of samples) of the input buffer
			OpenViBE::uint32 m_ui32NbChannels;
			OpenViBE::uint32 m_ui32NbSamplesPerBuffer;
		};

		class CBoxAlgorithmQuadraticFormDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Quadratic Form"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Fabien Lotte"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("IRISA-INSA Rennes"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Perform a quadratic matrix operation on the input signals m (result = m^T * A * m)"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("a square matrix A (which can be seen as a spatial filter) is applied to the input signals m (a vector). Then the transpose m^T of the input signals is multiplied to the resulting vector. In other words the output o is such as: o = m^T * A * m."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_QuadraticForm; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmQuadraticForm; }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("input signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput ("output signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Matrix values", OV_TypeId_String, "1 0 0 1");
				rBoxAlgorithmPrototype.addSetting("Number of rows/columns (square matrix)", OV_TypeId_Integer, "2");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_QuadraticFormDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_QuadraticForm_H__
