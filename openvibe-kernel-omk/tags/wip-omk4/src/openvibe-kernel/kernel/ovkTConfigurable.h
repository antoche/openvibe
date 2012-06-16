#ifndef __OpenViBEKernel_Kernel_TConfigurable_H__
#define __OpenViBEKernel_Kernel_TConfigurable_H__

#include "ovkTKernelObject.h"

#include "ovkCParameter.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class IParameter;

		template <class IBase>
		class TBaseConfigurable : public IBase
		{
		public:

			TBaseConfigurable(const OpenViBE::Kernel::IKernelContext& rKernelContext)
				:IBase(rKernelContext)
			{
			}

			virtual ~TBaseConfigurable(void)
			{
				std::map < OpenViBE::CIdentifier, std::pair < OpenViBE::boolean, OpenViBE::Kernel::IParameter* > >::iterator itParameter=m_vParameter.begin();
				while(itParameter!=m_vParameter.end())
				{
					if(itParameter->second.first)
					{
						delete itParameter->second.second;
					}
					++itParameter;
				}
			}

			virtual OpenViBE::CIdentifier getNextParameterIdentifier(const OpenViBE::CIdentifier& rPreviousIdentifier) const
			{
				return getNextIdentifier< std::pair < OpenViBE::boolean, OpenViBE::Kernel::IParameter* > >(m_vParameter, rPreviousIdentifier);
			}

			virtual OpenViBE::Kernel::IParameter* getParameter(const OpenViBE::CIdentifier& rParameterIdentifier)
			{
				std::map < OpenViBE::CIdentifier, std::pair < OpenViBE::boolean, OpenViBE::Kernel::IParameter* > >::iterator itParameter=m_vParameter.find(rParameterIdentifier);
				if(itParameter==m_vParameter.end())
				{
					return NULL;
				}
				return itParameter->second.second;
			}

			virtual OpenViBE::boolean setParameter(const OpenViBE::CIdentifier& rParameterIdentifier, OpenViBE::Kernel::IParameter& rpParameter)
			{
				this->removeParameter(rParameterIdentifier);

				m_vParameter[rParameterIdentifier]=std::pair < OpenViBE::boolean, OpenViBE::Kernel::IParameter* >(false, &rpParameter);

				return true;
			}

			virtual OpenViBE::Kernel::IParameter* createParameter(const OpenViBE::CIdentifier& rParameterIdentifier, const EParameterType eParameterType)
			{
				std::map < OpenViBE::CIdentifier, std::pair < OpenViBE::boolean, OpenViBE::Kernel::IParameter* > >::iterator itParameter=m_vParameter.find(rParameterIdentifier);
				if(itParameter!=m_vParameter.end())
				{
					return NULL;
				}

				OpenViBE::Kernel::IParameter* l_pParameter=NULL;
				switch(eParameterType)
				{
					case OpenViBE::Kernel::ParameterType_UInteger:       l_pParameter=new OpenViBE::Kernel::CUIntegerParameter(this->getKernelContext(), eParameterType);       break;
					case OpenViBE::Kernel::ParameterType_Integer:        l_pParameter=new OpenViBE::Kernel::CIntegerParameter(this->getKernelContext(), eParameterType);        break;
					case OpenViBE::Kernel::ParameterType_Boolean:        l_pParameter=new OpenViBE::Kernel::CBooleanParameter(this->getKernelContext(), eParameterType);        break;
					case OpenViBE::Kernel::ParameterType_Float:          l_pParameter=new OpenViBE::Kernel::CFloatParameter(this->getKernelContext(), eParameterType);          break;
					case OpenViBE::Kernel::ParameterType_String:         l_pParameter=new OpenViBE::Kernel::CStringParameter(this->getKernelContext(), eParameterType);         break;
					case OpenViBE::Kernel::ParameterType_Identifier:     l_pParameter=new OpenViBE::Kernel::CIdentifierParameter(this->getKernelContext(), eParameterType);     break;
					case OpenViBE::Kernel::ParameterType_Matrix:         l_pParameter=new OpenViBE::Kernel::CMatrixParameter(this->getKernelContext(), eParameterType);         break;
					case OpenViBE::Kernel::ParameterType_StimulationSet: l_pParameter=new OpenViBE::Kernel::CStimulationSetParameter(this->getKernelContext(), eParameterType); break;
					case OpenViBE::Kernel::ParameterType_MemoryBuffer:   l_pParameter=new OpenViBE::Kernel::CMemoryBufferParameter(this->getKernelContext(), eParameterType);   break;
					case OpenViBE::Kernel::ParameterType_Object:         l_pParameter=new OpenViBE::Kernel::CObjectParameter(this->getKernelContext(), eParameterType);         break;
					case OpenViBE::Kernel::ParameterType_None:
					case OpenViBE::Kernel::ParameterType_Pointer:        l_pParameter=new OpenViBE::Kernel::CPointerParameter(this->getKernelContext(), eParameterType);        break;
				};

				if(l_pParameter!=NULL)
				{
					m_vParameter[rParameterIdentifier]=std::pair < OpenViBE::boolean, OpenViBE::Kernel::IParameter* >(true, l_pParameter);
				}

				return l_pParameter;
			}

			virtual OpenViBE::boolean removeParameter(const OpenViBE::CIdentifier& rParameterIdentifier)
			{
				std::map < OpenViBE::CIdentifier, std::pair < OpenViBE::boolean, OpenViBE::Kernel::IParameter* > >::iterator itParameter=m_vParameter.find(rParameterIdentifier);
				if(itParameter==m_vParameter.end())
				{
					return false;
				}

				if(itParameter->second.first)
				{
					delete itParameter->second.second;
				}
				m_vParameter.erase(itParameter);

				return true;
			}

			_IsDerivedFromClass_Final_(IBase, OVK_ClassId_Kernel_ConfigurableT);

		private:

			std::map < OpenViBE::CIdentifier, std::pair < OpenViBE::boolean, OpenViBE::Kernel::IParameter* > > m_vParameter;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_TConfigurable_H__
