#ifndef __OpenViBEKernel_Kernel_TParameter_H__
#define __OpenViBEKernel_Kernel_TParameter_H__

#include "ovkTKernelObject.h"

#include <iostream>

namespace OpenViBE
{
	namespace Kernel
	{
		template <
			class IBase,
			class IType >
		class TBaseParameter : public IBase
		{
		public:

			TBaseParameter(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::EParameterType eParameterType)
				:IBase(rKernelContext)
				,m_pParameterRef(0)
				,m_pValueRef(0)
				,m_Value(0)
				,m_eParameterType(eParameterType)
			{
			}

			virtual OpenViBE::uint64 getParameterSize(void) const
			{
				return sizeof(IType);
			}

			virtual OpenViBE::Kernel::EParameterType getType(void) const
			{
				return m_eParameterType;
			}

			virtual OpenViBE::boolean clearReferenceTarget(void)
			{
				m_pValueRef=NULL;
				m_pParameterRef=NULL;
				return true;
			}

			virtual OpenViBE::boolean getReferenceTarget(OpenViBE::Kernel::IParameter*& pParameterRef) const
			{
				pParameterRef=m_pParameterRef;
				return true;
			}

			virtual OpenViBE::boolean setReferenceTarget(OpenViBE::Kernel::IParameter* pParameterRef)
			{
				if(m_pValueRef)
				{
					m_pValueRef=NULL;
				}
				m_pParameterRef=pParameterRef;
				return true;
			}

			virtual OpenViBE::boolean getReferenceTarget(void* pValue) const
			{
				memcpy(&pValue, &m_pValueRef, sizeof(IType*));
				return true;
			}

			virtual OpenViBE::boolean setReferenceTarget(const void* pValue)
			{
				if(m_pParameterRef)
				{
					m_pParameterRef=NULL;
				}
				memcpy(&m_pValueRef, &pValue, sizeof(IType*));
				return true;
			}

			virtual OpenViBE::boolean getValue(void* pValue) const
			{
				if(m_pParameterRef)
				{
					return m_pParameterRef->getValue(pValue);
				}
				if(m_pValueRef)
				{
					memcpy(pValue, m_pValueRef, sizeof(IType));
				}
				else
				{
					memcpy(pValue, &m_Value, sizeof(IType));
				}
				return true;
			}

			virtual OpenViBE::boolean setValue(const void* pValue)
			{
				if(m_pParameterRef)
				{
					return m_pParameterRef->setValue(pValue);
				}
				if(m_pValueRef)
				{
					memcpy(m_pValueRef, pValue, sizeof(IType));
				}
				else
				{
					memcpy(&m_Value, pValue, sizeof(IType));
				}
				return true;
			}

			_IsDerivedFromClass_(IBase, OVK_ClassId_Kernel_ParameterT);

		protected:

			OpenViBE::Kernel::IParameter* m_pParameterRef;
			IType* m_pValueRef;
			IType m_Value;
			OpenViBE::Kernel::EParameterType m_eParameterType;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_TParameter_H__
