#ifndef __OpenViBEPlugins_SimpleVisualisation_CTimeRuler_H__
#define __OpenViBEPlugins_SimpleVisualisation_CTimeRuler_H__

#include <openvibe/ov_all.h>
#include "ovpIStreamDatabase.h"
#include <gtk/gtk.h>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		/**
		 * Displays a time ruler.
		 * Uses information fetched from a stream database object.
		 */
		class CTimeRuler
		{
		public:
			/**
			 * \brief Constructor
			 * \param rStreamDatabase Object from which data is retrieved
			 * \param i32WidthRequest Width to be requested by widget
			 * \param i32HeightRequest Height to be requested by widget
			 */
			CTimeRuler(
				IStreamDatabase& rStreamDatabase,
				OpenViBE::int32 i32WidthRequest,
				OpenViBE::int32 i32HeightRequest);

			/**
			 * \brief Destructor
			 */
			~CTimeRuler();

			/**
			 * \brief Get widget handled by this object
			 * \return Gtk widget
			 */
			GtkWidget* getWidget() const { return m_pWidget; }

			/**
			 * \brief Toggle ruler on/off
			 * \param bActive Activation flag
			 */
			void toggle(OpenViBE::boolean bActive);

			/**
			 * \brief Draw ruler
			 */
			void draw();

			/**
			 * \brief Link ruler width to another widget's
			 * \param pWidget Widget whose width must be matched by this object
			 */
			void linkWidthToWidget(GtkWidget* pWidget);

			/**
			 * \brief Callback notified upon resize events
			 * \param i32Width New window width
			 * \param i32Height New window height
			 */
			void onResizeEventCB(
				gint i32Width,
				gint i32Height);

		private:
			//! Ruler widget
			GtkWidget* m_pWidget;
			//! Database from which stream information is retrieved
			IStreamDatabase& m_rStreamDatabase;
			//! Height request
			OpenViBE::int32 m_i32HeightRequest;
			//! Size available per label along the ruler
			OpenViBE::uint64 m_ui64PixelsPerBottomRulerLabel;
		};
	}
}

#endif
