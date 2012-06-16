#ifndef __OpenViBEPlugins_SimpleVisualisation_CSignalDisplayLeftRuler_H__
#define __OpenViBEPlugins_SimpleVisualisation_CSignalDisplayLeftRuler_H__

#include <openvibe/ov_all.h>
#include <gtk/gtk.h>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		/**
		 * Used to display a vertical "amplitude" ruler.
		 * */
		class CSignalDisplayLeftRuler
		{
		public:
			/**
			 * \brief Constructor
			 * \param i32WidthRequest Width to be requested by widget
			 * \param i32HeightRequest Height to be requested by widget
			 */
			CSignalDisplayLeftRuler(
				OpenViBE::int32 i32WidthRequest,
				OpenViBE::int32 i32HeightRequest);

			/**
			 * \brief Destructor
			 */
			~CSignalDisplayLeftRuler();

			/*
			 * \brief Update ruler with latest min/max values and request a redraw
			 * \param f64Min Minimum value to be displayed
			 * \param f64Max Maximum value to be displayed
			 */
			void update(
				OpenViBE::float64 f64Min,
				OpenViBE::float64 f64Max);

			//! returns the widget, so it can be added to the main interface
			::GtkWidget* getWidget() const;

			/**
			 * \brief Draws the ruler by using the information from the database.
			 */
			void draw();

		public:
			GtkWidget * m_pLeftRuler;
			OpenViBE::int32 m_i32WidthRequest;
			OpenViBE::float64 m_f64MaximumDisplayedValue;
			OpenViBE::float64 m_f64MinimumDisplayedValue;
			OpenViBE::uint64 m_ui64PixelsPerLeftRulerLabel;
		};

	};
};

#endif


