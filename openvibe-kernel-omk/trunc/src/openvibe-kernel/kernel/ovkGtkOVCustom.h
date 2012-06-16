#ifndef __OpenViBEKernel_Kernel_Player_GtkOVCustom_H__
#define __OpenViBEKernel_Kernel_Player_GtkOVCustom_H__

#include <gtk/gtkwidget.h>

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class IGtkOVCustomHandler;
	};
};

/**
 * \brief Creates a new GtkOVCustom widget
 * \param pHandler pointer to IGtkOVCustomHandler to notify upon event reception
 * \return pointer to newly created GtkOVCustom widget cast as a GtkWidget, or NULL if failed
 */
GtkWidget* gtk_ov_custom_new(OpenViBE::Kernel::IGtkOVCustomHandler* pHandler);

/**
 * \class GtkOVCustom
 * \author Vincent Delannoy (INRIA/IRISA)
 * \date 2007-11
 * \brief Custom Gtk widget whose behaviour may be adapted to the developer's needs
 * This widget is to be instanciated using the gtk_ov_custom_new function only.
 */
class GtkOVCustom
{
private:
	/**
	 * \brief This MUST be the first member declared in this class, so as to allow this class to be used as a GtkWidget
	 */
	GtkWidget m_oWidget;

private:
	/**
	 * \brief Constructor, not to be used. Use the gtk_ov_custom_new() global function instead
	 */
	GtkOVCustom();

	/**
	 * \brief Destructor, not to be used. Send the destroy event to instances instead
	 */
	~GtkOVCustom();

public:
	/**
	 * \brief Initializes the GtkOVCustom widget (acts as a constructor).
	 * Not to be called directly (called automatically upon widget creation)
	 */
	void init();

	/**
	 * \brief Initializes the GtkOVCustom widget (acts as a destructor).
	 * Not to be called directly (called automatically upon widget destruction)
	 */
	void destroy();

	/**
	 * \brief Sets the 'handler' of this widget, which is to be notified upon widget events
	 * \return True if handler was successfully set, false otherwise
	 */
	OpenViBE::boolean setHandler(
		OpenViBE::Kernel::IGtkOVCustomHandler* pHandler);

	/** \name Event handling */
	//@{

	/**
	 * \brief Realize event handler
	 * Only once this event has been received may Ogre resources be parsed
	 * and the associated Ogre::RenderWindow be created
	 * \return True
	 */
	OpenViBE::boolean handleRealizeEvent();

	/**
	 * \brief Size request event handler
	 * \return True
	 */
	OpenViBE::boolean handleSizeRequestEvent(
		GtkRequisition* pRequisition);

	/**
	 * \brief Size allocate event handler
	 * \return True
	 */
	OpenViBE::boolean handleSizeAllocateEvent(
		GtkAllocation* pAllocation);

	/**
	 * \brief Expose event handler
	 * \return True
	 */
	OpenViBE::boolean handleExposeEvent();

	/**
	 * \brief Mouse motion event handler
	 * \param bevent Structure containing new mouse coordinates
	 * \return True
	 */
	OpenViBE::boolean handleMotionEvent(
		::GdkEventButton* bevent);

	/**
	 * \brief Button press event handler
	 * \param bevent Structure containing index of button pressed
	 * \return True
	 */
	OpenViBE::boolean handleButtonPressEvent(
		::GdkEventButton *bevent);

	/**
	 * \brief Button release event handler
	 * \param bevent Structure containing index of button released
	 * \return True
	 */
	OpenViBE::boolean handleButtonReleaseEvent(
		::GdkEventButton *bevent);

	/**
	 * \brief Unrealize event handler
	 * \return True
	 */
	OpenViBE::boolean handleUnrealizeEvent();

	/**
	 * \brief Destroy event handler
	 * \return True
	 */
	OpenViBE::boolean handleDestroyEvent();

	//@}

	/** \name Object event callbacks */
	//@{

	/**
	 * \brief Destroy event callback
	 * \param pObject Structure containing object being destroyed
	 * \return True
	 */
	static void gtk_ov_custom_destroy(
		GtkObject* pObject);

	//@}

	/** \name Widget event callbacks */
	//@{

	/// \brief Realize event callback
	static void gtk_ov_custom_realize(
		GtkWidget* pWidget);
	/// \brief Unrealize event callback
	static void gtk_ov_custom_unrealize(
		GtkWidget* pWidget);
	/// \brief Size request event callback
	static void gtk_ov_custom_size_request(
		GtkWidget *pWidget,
		GtkRequisition* pRequisition);
	/// \brief Size allocate event callback
	static void gtk_ov_custom_size_allocate(
		GtkWidget* pWidget,
		GtkAllocation* pAllocation);
	/// \brief Expose event callback
	static gint gtk_ov_custom_expose(
		GtkWidget *pWidget,
		GdkEventExpose *pEvent);

	//@}

	/// \brief Mouse motion event callback
	static gboolean motion_notify_event_in_frame_cb(
		GtkWidget *widget,
		GdkEventButton *bevent,
		gpointer user_data);
	/// \brief Button press event callback
	static gboolean button_press_event_in_frame_cb(
		GtkWidget *widget,
		GdkEventButton *bevent,
		gpointer user_data);
	/// \brief Button release event callback
	static gboolean button_release_event_in_frame_cb(
		GtkWidget *widget,
		GdkEventButton *bevent,
		gpointer user_data);
	/// \brief Widget show callback
	static void show_cb(
		GtkWidget *widget,
		gpointer user_data);
	/// \brief Widget visibility notification callback
	static void visibility_notify_event_cb(
		GtkWidget *pWidget,
		GdkEventVisibility* pEvent,
		gpointer user_data);

//WARNING : since this is a Gtk widget, the constructor doesn't get called,
//so any member that requires its constructor to be called upon object creation
//won't work properly. Complex members should therefore be allocated dynamically in init()
private:
	/**
	 * \brief Object handling this widget
	 */
	OpenViBE::Kernel::IGtkOVCustomHandler* m_pHandler;
};

#endif // __OpenViBEKernel_Kernel_Player_GtkOVCustom_H__
