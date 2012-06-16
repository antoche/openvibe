#ifndef __OpenViBEPlugins_SimpleVisualisation_CBottomTimeRuler_H__
#define __OpenViBEPlugins_SimpleVisualisation_CBottomTimeRuler_H__

#include <openvibe/ov_all.h>
#include "ovpCBufferDatabase.h"
#include <gtk/gtk.h>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CBufferDatabase;

		/**
		 * Used to display an horizontal temporal ruler.
		 * Uses information fetched from a signal database object.
		 * \param pDatabase Object from which to fetch time data
		 * \param i32WidthRequest Width to be requested by widget
		 * \param i32HeightRequest Height to be requested by widget
		 */
		class CBottomTimeRuler
		{
		public:
			CBottomTimeRuler(
				CBufferDatabase& pDatabase,
				OpenViBE::int32 i32WidthRequest,
				OpenViBE::int32 i32HeightRequest);

			~CBottomTimeRuler();

			//! returns the widget, so it can be added to the main interface
			GtkWidget* getWidget() const;

			//! draws the ruler
			void draw();

			/**
			 * \brief Resize this ruler when the widget passed in parameter is resized
			 * \param pWidget Widget whose width is matched by this widget
			 */
			void linkWidthToWidget(
				GtkWidget* pWidget);

			//! in scan mode, leftmost displayed time is not always the one of the oldest buffer
			void setLeftmostDisplayedTime(OpenViBE::uint64 ui64LeftmostDisplayedTime);

			/**
			 * \brief Callback notified upon resize events
			 * \param i32Width New window width
			 * \param i32Height New window height
			 */
			void onResizeEventCB(
				gint i32Width,
				gint i32Height);

		private:
			//! Draw ruler
			void drawRuler(
				OpenViBE::int64 i64BaseX,
				OpenViBE::int32 i32RulerWidth,
				OpenViBE::float64 f64StartTime,
				OpenViBE::float64 f64EndTime,
				OpenViBE::float64 f64Length,
				OpenViBE::float64 f64BaseValue,
				OpenViBE::float64 f64ValueStep,
				OpenViBE::int32 ui32ClipLeft,
				OpenViBE::int32 ui32ClipRight);

		private:
			//! Gtk widget
			GtkWidget* m_pBottomRuler;
			//! Signal database from which time information is retrieved
			CBufferDatabase* m_pDatabase;
			//! Height request
			OpenViBE::int32 m_i32HeightRequest;
			//! Space allocated per label
			OpenViBE::uint64 m_ui64PixelsPerBottomRulerLabel;
			//! When in scan mode, current leftmost displayed time
			OpenViBE::uint64 m_ui64LeftmostDisplayedTime;
		};

	};
};

#endif


