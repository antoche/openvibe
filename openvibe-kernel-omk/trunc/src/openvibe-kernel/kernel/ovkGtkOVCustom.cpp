// Parts of this file are inspired from the GtkOgre Demo available on the Ogre3D website (www.ogre3d.org)
// and distributed with the following restrictions :
//
// Copyright (c) 2006 Christian Lindequist Larsen
//
// This software is provided 'as-is', without any express or implied warranty. In no event will
// the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose, including commercial
// applications, and to alter it and redistribute it freely, subject to the following
// restrictions:
//
//     1. The origin of this software must not be misrepresented; you must not claim that you
//        wrote the original software. If you use this software in a product, an acknowledgment
//        in the product documentation would be appreciated but is not required.
//
//     2. Altered source versions must be plainly marked as such, and must not be
//        misrepresented as being the original software.
//
//     3. This notice may not be removed or altered from any source distribution.
//
#include "ovkIGtkOVCustomHandler.h"
#include "ovkGtkOVCustom.h"

#if defined( __WIN32__ ) || defined( _WIN32 )
#  include <gdk/gdkwin32.h>
#else
#  include <gdk/gdkx.h>
#endif

#include <cstdlib>
#include <cstring>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

#define boolean OpenViBE::boolean

class GtkOVCustomClass
{
public:
	GtkWidgetClass parent_class;
};

static GtkWidgetClass *parent_class = NULL;

GType gtk_ov_custom_get_type(void);

//GtkOVCustom macros
#define GTK_OV_CUSTOM(obj)         GTK_CHECK_CAST(obj, gtk_ov_custom_get_type(), GtkOVCustom)
#define GTK_OV_CUSTOM_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_ov_custom_get_type(), GtkOVCustomClass)
#define GTK_IS_OV_CUSTOM(obj)      GTK_CHECK_TYPE(obj, gtk_ov_custom_get_type())

GtkOVCustom::GtkOVCustom()
{
	//doesn't get called : object is created via Gtk methods
	//members are initialized in init()
}

GtkOVCustom::~GtkOVCustom()
{
	//doesn't get called : object is destroyed via Gtk methods
	//members are destroyed in destroy()
}

//really acts as a constructor
void GtkOVCustom::init()
{
	//GTK_WIDGET_SET_FLAGS(GTK_WIDGET(this), GTK_NO_WINDOW); //unset in realize
	m_pHandler = NULL;
}

//really acts as a destructor
void GtkOVCustom::destroy()
{
}

boolean GtkOVCustom::setHandler(IGtkOVCustomHandler* pHandler)
{
	m_pHandler = pHandler;
	return m_pHandler != NULL;
}

/*
gboolean blah(::GtkWidget* pWidget, ::GdkEventCrossing* pEvent, gpointer pUserData)
{
	gtk_widget_grab_focus(gtk_widget_get_parent(pWidget));
	::SetFocus((HWND)GDK_WINDOW_HWND(gtk_widget_get_parent(pWidget)->window));
	return FALSE;
}
*/

boolean GtkOVCustom::handleRealizeEvent()
{
	//set realized flag
	GTK_WIDGET_SET_FLAGS(this, GTK_REALIZED);

	//create GdkWindow
	GdkWindowAttr l_oWindowAttributes;
	memset(&l_oWindowAttributes, 0, sizeof(l_oWindowAttributes));
	//initial position and size
	l_oWindowAttributes.x = m_oWidget.allocation.x;
	l_oWindowAttributes.y = m_oWidget.allocation.y;
	l_oWindowAttributes.width = m_oWidget.allocation.width;
	l_oWindowAttributes.height = m_oWidget.allocation.height;
	//other attributes
	l_oWindowAttributes.event_mask = GDK_ALL_EVENTS_MASK; //0xFFFFFFFF; //get_events () | Gdk::EXPOSURE_MASK | Gdk::ALL_EVENTS_MASK;
	l_oWindowAttributes.window_type = GDK_WINDOW_CHILD;
	l_oWindowAttributes.wclass = GDK_INPUT_OUTPUT;

	//get parent window
	GdkWindow* l_pParent = gtk_widget_get_parent_window(&m_oWidget);

	//create child window
	m_oWidget.window = GDK_DRAWABLE(gdk_window_new(l_pParent, &l_oWindowAttributes, GDK_WA_X | GDK_WA_Y));

#if defined OVK_OS_Windows
	::gdk_window_ensure_native(m_oWidget.window);
	// ::gtk_widget_reparent(&m_oWidget, ::gtk_widget_get_parent(&m_oWidget));
	::SetWindowPos((HWND)GDK_WINDOW_HWND(m_oWidget.window), 0, l_oWindowAttributes.x, l_oWindowAttributes.y, l_oWindowAttributes.width, l_oWindowAttributes.height, SWP_SHOWWINDOW);
	::gdk_window_set_events(m_oWidget.window, GdkEventMask(GDK_ALL_EVENTS_MASK&~GDK_FOCUS_CHANGE_MASK));
	::gtk_widget_add_events(&m_oWidget, GdkEventMask(GDK_ALL_EVENTS_MASK&~GDK_FOCUS_CHANGE_MASK));
	//::gtk_widget_set_can_focus(&m_oWidget, FALSE);
	//::gdk_window_set_accept_focus(m_oWidget.window, FALSE);
	//::gdk_window_set_focus_on_map(m_oWidget.window, FALSE);
	//printf("%i\n", gdk_window_get_window_type(m_oWidget.window));
	//printf("%p %p\n", ::GetParent((HWND)GDK_WINDOW_HWND(m_oWidget.window)), (HWND)GDK_WINDOW_HWND(l_pParent));
#endif

	//widget has a window now
	GTK_WIDGET_UNSET_FLAGS(&m_oWidget, GTK_NO_WINDOW);

	//let handler know this widget is realized
	if(m_pHandler != NULL)
	{
		m_pHandler->handleRealizeEvent(GTK_WIDGET(this));
	}

	//set event mask as needed here
	//gdk_window_set_events(widget->window, GDK_ALL_EVENTS_MASK);

	//setup event handlers
	g_signal_connect(G_OBJECT(&m_oWidget), "button-press-event", G_CALLBACK(button_press_event_in_frame_cb), this);
	g_signal_connect(G_OBJECT(&m_oWidget), "button-release-event", G_CALLBACK(button_release_event_in_frame_cb), this);
	g_signal_connect(G_OBJECT(&m_oWidget), "motion-notify-event", G_CALLBACK(motion_notify_event_in_frame_cb), this);
	g_signal_connect(G_OBJECT(&m_oWidget), "show", G_CALLBACK(show_cb), this);
	g_signal_connect(G_OBJECT(&m_oWidget), "visibility-notify-event", G_CALLBACK(visibility_notify_event_cb), this);
//	g_signal_connect(G_OBJECT(&m_oWidget), "leave-notify-event", G_CALLBACK(blah), this);

	gtk_widget_set_double_buffered(&m_oWidget, FALSE);
	gdk_window_set_user_data(m_oWidget.window, G_OBJECT(this));
	gdk_window_set_back_pixmap(m_oWidget.window, NULL, FALSE);

	return true;
}

boolean GtkOVCustom::handleSizeRequestEvent(GtkRequisition* pRequisition)
{
	m_oWidget.requisition = *pRequisition;
	return true;
}

boolean GtkOVCustom::handleSizeAllocateEvent(GtkAllocation* pAllocation)
{
	// Chain up - this will adjust the size of the actual window
	if (GTK_WIDGET_CLASS(parent_class)->size_allocate)
	{
		(*GTK_WIDGET_CLASS(parent_class)->size_allocate)(&m_oWidget, pAllocation);
	}

	m_oWidget.allocation = *pAllocation;

	if(m_oWidget.window != NULL)
	{
#if defined OVK_OS_Windows
		::SetWindowPos((HWND)GDK_WINDOW_HWND(m_oWidget.window), 0, pAllocation->x, pAllocation->y, pAllocation->width, pAllocation->height, SWP_SHOWWINDOW);
		// ::gdk_window_show(m_oWidget.window);
#else
		gdk_window_move_resize(m_oWidget.window, pAllocation->x, pAllocation->y, pAllocation->width, pAllocation->height);
#endif
	}

	if(m_pHandler != NULL)
	{
		m_pHandler->handleSizeAllocateEvent(GTK_WIDGET(this), m_oWidget.allocation.width, m_oWidget.allocation.height);
	}
	return true;
}

boolean GtkOVCustom::handleExposeEvent()
{
	if(m_pHandler != NULL)
	{
		m_pHandler->handleExposeEvent(GTK_WIDGET(this));
	}
	return true;
}

boolean GtkOVCustom::handleMotionEvent(GdkEventButton *bevent)
{
	if(m_pHandler != NULL)
	{
		m_pHandler->handleMotionEvent(GTK_WIDGET(this), (int)bevent->x, (int)bevent->y);
	}
	return true;
}

boolean GtkOVCustom::handleButtonPressEvent(GdkEventButton *bevent)
{
	if(m_pHandler != NULL)
	{
		m_pHandler->handleButtonPressEvent(GTK_WIDGET(this), bevent->button, (int)bevent->x, (int)bevent->y);
	}
	return true;
}

boolean GtkOVCustom::handleButtonReleaseEvent(GdkEventButton *bevent)
{
	if(m_pHandler != NULL)
	{
		m_pHandler->handleButtonReleaseEvent(GTK_WIDGET(this), bevent->button, (int)bevent->x, (int)bevent->y);
	}
	return true;
}

boolean GtkOVCustom::handleUnrealizeEvent()
{
	if (GTK_WIDGET_MAPPED(&m_oWidget))
	{
		gtk_widget_unmap(&m_oWidget);
	}

	GTK_WIDGET_UNSET_FLAGS(&m_oWidget, GTK_MAPPED);

	if(m_pHandler != NULL)
	{
		m_pHandler->handleUnrealizeEvent(GTK_WIDGET(this));
	}
#if 0
	//FIXME : this triggers a Gtk critical warning (assert(widget->style->attach_count > 0) is false)
	//This destroys widget->window and unsets the realized flag
	if(GTK_WIDGET_CLASS(parent_class)->unrealize != NULL)
	{
		(*GTK_WIDGET_CLASS(parent_class)->unrealize)(&m_oWidget);
	}
#elif 0
	gdk_window_destroy(m_oWidget.window);
#endif
	GTK_WIDGET_SET_FLAGS(&m_oWidget, GTK_NO_WINDOW);
	GTK_WIDGET_UNSET_FLAGS(&m_oWidget, GTK_REALIZED);

	return true;
}

boolean GtkOVCustom::handleDestroyEvent()
{
	if(m_pHandler != NULL)
	{
		m_pHandler->handleDestroyEvent(GTK_WIDGET(this));
	}

	destroy();

	// Chain up
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
	{
		(*GTK_OBJECT_CLASS(parent_class)->destroy)(GTK_OBJECT(this));
	}

	return true;
}

static void gtk_ov_custom_class_init(GtkOVCustomClass *klass)
{
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;

	object_class = (GtkObjectClass*)klass;
	widget_class = (GtkWidgetClass*)klass;

	parent_class = (GtkWidgetClass*)gtk_type_class(gtk_widget_get_type());

	object_class->destroy = GtkOVCustom::gtk_ov_custom_destroy;

	widget_class->realize = GtkOVCustom::gtk_ov_custom_realize;
	widget_class->unrealize = GtkOVCustom::gtk_ov_custom_unrealize;

	widget_class->size_request = GtkOVCustom::gtk_ov_custom_size_request;
	widget_class->size_allocate = GtkOVCustom::gtk_ov_custom_size_allocate;

	widget_class->expose_event = GtkOVCustom::gtk_ov_custom_expose;
}

static void gtk_ov_custom_init(GtkOVCustom *pOVCustom)
{
	pOVCustom->init();
}

GType gtk_ov_custom_get_type(void)
{
	static GType ov_custom_type = 0;
	if (!ov_custom_type)
	{
#if 0
		static const GtkTypeInfo ov_custom_info =
		{
			(gchar*)"GtkOVCustom",
			sizeof(GtkOVCustom),
			sizeof(GtkOVCustomClass),
			(GtkClassInitFunc)gtk_ov_custom_class_init,
			(GtkObjectInitFunc)gtk_ov_custom_init,
			NULL,
			NULL,
			(GtkClassInitFunc)NULL,
		};

		ov_custom_type = gtk_type_unique(gtk_widget_get_type(), &ov_custom_info);
#else
		static const GTypeInfo ov_custom_info =
		{
			sizeof(GtkOVCustomClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			(GClassInitFunc)gtk_ov_custom_class_init,
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof(GtkOVCustom),
			0,
			(GInstanceInitFunc)gtk_ov_custom_init,
		};

		ov_custom_type = g_type_register_static(GTK_TYPE_WIDGET, "GtkOVCustom", &ov_custom_info, GTypeFlags(0));
	}
	return ov_custom_type;
#endif
}

GtkWidget* gtk_ov_custom_new(IGtkOVCustomHandler* pHandler)
{
#if 0
	GtkOVCustom* l_pOVCustom = GTK_OV_CUSTOM(gtk_type_new(gtk_ov_custom_get_type()));	//deprecated
#else
	GtkOVCustom* l_pOVCustom = GTK_OV_CUSTOM(g_object_new(gtk_ov_custom_get_type(), NULL));
#endif

	l_pOVCustom->setHandler(pHandler);

	return GTK_WIDGET(l_pOVCustom);
}

// GtkObject functions
void GtkOVCustom::gtk_ov_custom_destroy(GtkObject *pObject)
{
	//FIXME : called twice upon widget destruction -why??
	g_return_if_fail(pObject != NULL);
	g_return_if_fail(GTK_IS_OV_CUSTOM(pObject));

	GTK_OV_CUSTOM(pObject)->handleDestroyEvent();
}

// GtkWidget functions
void GtkOVCustom::gtk_ov_custom_realize(GtkWidget *pWidget)
{
	g_return_if_fail(pWidget != NULL);
	g_return_if_fail(GTK_IS_OV_CUSTOM(pWidget));

	GTK_OV_CUSTOM(pWidget)->handleRealizeEvent();
}

void GtkOVCustom::gtk_ov_custom_unrealize(GtkWidget *pWidget)
{
	g_return_if_fail(pWidget != NULL);
	g_return_if_fail(GTK_IS_OV_CUSTOM(pWidget));

	GTK_OV_CUSTOM(pWidget)->handleUnrealizeEvent();
}

void GtkOVCustom::gtk_ov_custom_size_request(GtkWidget *pWidget, GtkRequisition* pRequisition)
{
	g_return_if_fail(pWidget != NULL);
	g_return_if_fail(GTK_IS_OV_CUSTOM(pWidget));

	GTK_OV_CUSTOM(pWidget)->handleSizeRequestEvent(pRequisition);
}

void GtkOVCustom::gtk_ov_custom_size_allocate(GtkWidget *pWidget, GtkAllocation *pAllocation)
{
	g_return_if_fail(pWidget != NULL);
	g_return_if_fail(GTK_IS_OV_CUSTOM(pWidget));

	GTK_OV_CUSTOM(pWidget)->handleSizeAllocateEvent(pAllocation);
}

// Update the render window and swap buffers
gint GtkOVCustom::gtk_ov_custom_expose(GtkWidget *pWidget, GdkEventExpose *pEvent)
{
	GTK_OV_CUSTOM(pWidget)->handleExposeEvent();
	return FALSE;
}

gboolean GtkOVCustom::button_press_event_in_frame_cb(GtkWidget *pWidget, GdkEventButton *pEvent, gpointer user_data)
{
	GTK_OV_CUSTOM(pWidget)->handleButtonPressEvent(pEvent);
	return FALSE;
}

gboolean GtkOVCustom::motion_notify_event_in_frame_cb(GtkWidget *pWidget, GdkEventButton *pEvent, gpointer user_data)
{
	GTK_OV_CUSTOM(pWidget)->handleMotionEvent(pEvent);
	return FALSE;
}

gboolean GtkOVCustom::button_release_event_in_frame_cb(GtkWidget *pWidget, GdkEventButton *pEvent, gpointer user_data)
{
	GTK_OV_CUSTOM(pWidget)->handleButtonReleaseEvent(pEvent);
	return FALSE;
}

void GtkOVCustom::show_cb(GtkWidget *pWidget, gpointer user_data)
{
	::gdk_window_show(GTK_WIDGET(user_data)->window);
}

void GtkOVCustom::visibility_notify_event_cb(GtkWidget *pWidget, GdkEventVisibility* pEvent, gpointer user_data)
{
}
