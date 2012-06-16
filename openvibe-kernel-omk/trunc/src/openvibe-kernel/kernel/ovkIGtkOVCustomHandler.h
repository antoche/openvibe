#ifndef __OpenViBEKernel_Kernel_Player_IGtkOVCustomHandler_H__
#define __OpenViBEKernel_Kernel_Player_IGtkOVCustomHandler_H__

#include "../ovk_base.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class IGtkOVCustomHandler
		{
		public:

			virtual ~IGtkOVCustomHandler(void)
			{
			}

			virtual OpenViBE::boolean handleRealizeEvent(
				::GtkWidget* pWidget)
			{
				return false;
			}

			virtual OpenViBE::boolean handleUnrealizeEvent(
				::GtkWidget* pWidget)
			{
				return false;
			}

			virtual OpenViBE::boolean handleDestroyEvent(
				::GtkWidget* pWidget)
			{
				return false;
			}

			virtual OpenViBE::boolean handleSizeAllocateEvent(
				::GtkWidget* pWidget,
				unsigned int uiWidth,
				unsigned int uiHeight)
			{
				return false;
			}

			virtual OpenViBE::boolean handleExposeEvent(
				::GtkWidget* pWidget)
			{
				return false;
			}

			virtual OpenViBE::boolean handleMotionEvent(
				::GtkWidget* pWidget,
				int i32X,
				int i32Y)
			{
				return false;
			}

			virtual OpenViBE::boolean handleButtonPressEvent(
				::GtkWidget* pWidget,
				unsigned int uiButton,
				int i32X,
				int i32Y)
			{
				return false;
			}

			virtual OpenViBE::boolean handleButtonReleaseEvent(
				::GtkWidget* pWidget,
				unsigned int uiButton,
				int i32X,
				int i32Y)
			{
				return false;
			}
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_IGtkOVCustomHandler_H__
