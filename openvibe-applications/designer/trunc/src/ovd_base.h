#ifndef __OpenViBEDesigner_base_H__
#define __OpenViBEDesigner_base_H__

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <gtk/gtk.h>

#include <gdk/gdk.h>

#include "ovd_defines.h"

#define G_CALLBACK2(x) G_CALLBACK_AUTOCAST(G_CALLBACK(x))

class G_CALLBACK_AUTOCAST
{
public:

	typedef union
	{
		GCallback fp;;
		gpointer p;
	} _data;

	_data m_pData;

	G_CALLBACK_AUTOCAST(gpointer p) { m_pData.p=p; }
	G_CALLBACK_AUTOCAST(GCallback fp) { m_pData.fp=fp; }

	operator gpointer (void) { return m_pData.p; }
	operator GCallback (void) { return m_pData.fp; }

private:

	G_CALLBACK_AUTOCAST(void);
};

#endif // __OpenViBEDesigner_base_H__
