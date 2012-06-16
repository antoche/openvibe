#ifndef __OpenViBEKernel_Kernel_CParameter_H__
#define __OpenViBEKernel_Kernel_CParameter_H__

#include "ovkTKernelObject.h"
#include "ovkTParameter.h"

#include <iostream>

namespace OpenViBE
{
	namespace Kernel
	{
		#define _parameter_template_instance_1_(_CName_, CType, IType, oClassId) \
			typedef OpenViBE::Kernel::TBaseParameter < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IParameter >, IType > _Base_##_CName_; \
			class _CName_ : public _Base_##_CName_ \
			{ \
			public: \
				_CName_(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::EParameterType eParameterType) \
					:OpenViBE::Kernel::TBaseParameter < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IParameter >, IType >(rKernelContext, eParameterType) \
				{ \
					m_oDefaultValue=0; \
					memcpy(&m_Value, &m_oDefaultValue, sizeof(IType)); \
				} \
				_IsDerivedFromClass_Final_(_Base_##_CName_, oClassId); \
			protected: \
				CType m_oDefaultValue; \
			};

		#define _parameter_template_instance_2_(_CName_, CType, IType, oClassId) \
			typedef OpenViBE::Kernel::TBaseParameter < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IParameter >, IType > _Base_##_CName_; \
			class _CName_ : public _Base_##_CName_ \
			{ \
			public: \
				_CName_(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::EParameterType eParameterType) \
					:OpenViBE::Kernel::TBaseParameter < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IParameter >, IType >(rKernelContext, eParameterType) \
				{ \
					IType l_pDefaultValue=&m_oDefaultValue; \
					memcpy(&m_Value, &l_pDefaultValue, sizeof(IType)); \
				} \
				_IsDerivedFromClass_Final_(_Base_##_CName_, oClassId); \
			protected: \
				CType m_oDefaultValue; \
			};

		_parameter_template_instance_1_(CIntegerParameter,        OpenViBE::int64,           OpenViBE::int64,            OVK_ClassId_Kernel_IntegerParameter);
		_parameter_template_instance_1_(CUIntegerParameter,       OpenViBE::uint64,          OpenViBE::uint64,           OVK_ClassId_Kernel_UIntegerParameter);
		_parameter_template_instance_1_(CBooleanParameter,        OpenViBE::boolean,         OpenViBE::boolean,          OVK_ClassId_Kernel_BooleanParameter);
		_parameter_template_instance_1_(CFloatParameter,          OpenViBE::float64,         OpenViBE::float64,          OVK_ClassId_Kernel_FloatParameter);
		_parameter_template_instance_2_(CStringParameter,         OpenViBE::CString,         OpenViBE::CString*,         OVK_ClassId_Kernel_StringParameter);
		_parameter_template_instance_2_(CIdentifierParameter,     OpenViBE::CIdentifier,     OpenViBE::CIdentifier*,     OVK_ClassId_Kernel_IdentifierParameter);
		_parameter_template_instance_2_(CMatrixParameter,         OpenViBE::CMatrix,         OpenViBE::IMatrix*,         OVK_ClassId_Kernel_MatrixParameter);
		_parameter_template_instance_2_(CStimulationSetParameter, OpenViBE::CStimulationSet, OpenViBE::IStimulationSet*, OVK_ClassId_Kernel_StimulationSetParameter);
		_parameter_template_instance_2_(CMemoryBufferParameter,   OpenViBE::CMemoryBuffer,   OpenViBE::IMemoryBuffer*,   OVK_ClassId_Kernel_MemoryBufferParameter);
		_parameter_template_instance_2_(CObjectParameter,         OpenViBE::CNullObject,     OpenViBE::IObject*,         OVK_ClassId_Kernel_ObjectParameter);
		_parameter_template_instance_2_(CPointerParameter,        void*,                     void*,                      OVK_ClassId_Kernel_PointerParameter);

		#undef _instance_
	};
};

#endif // __OpenViBEKernel_Kernel_CParameter_H__
