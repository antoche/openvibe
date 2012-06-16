#ifndef __OpenViBEDesigner_CInputDialog_H__
#define __OpenViBEDesigner_CInputDialog_H__

#include "ovd_base.h"

namespace OpenViBEDesigner
{
	typedef void (*fpButtonCB)(::GtkWidget* pWidget, gpointer pUserData);

	class CInputDialog
	{
	public:

		CInputDialog(
			const char* sGtkBuilder,
			fpButtonCB fpOKButtonCB,
			void* pUserData=NULL,
			const char* sTitle=NULL,
			const char* sLabel=NULL,
			const char* sEntry=NULL);

		~CInputDialog(void);

		void run(void);

		void* getUserData(void);

		const char* getEntry(void);

	private:

		static gboolean key_press_event_cb(
			::GtkWidget* pWidget,
			GdkEventKey* pEventKey,
			gpointer pUserData);
		static void button_clicked_cb(
			::GtkButton* pButton,
			gpointer pUserData);
		void buttonClickedCB(
			::GtkButton* pButton);

	private:

		void* m_pUserData;
		fpButtonCB m_fpOKButtonCB;
		::GtkDialog* m_pInputDialog;
		::GtkLabel* m_pInputDialogLabel;
		::GtkEntry* m_pInputDialogEntry;
		::GtkButton* m_pInputDialogOKButton;
		::GtkButton* m_pInputDialogCancelButton;
	};
};

#endif // __OpenViBEDesigner_CInputDialog_H__
