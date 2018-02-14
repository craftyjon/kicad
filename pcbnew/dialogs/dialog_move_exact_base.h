///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug  4 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DIALOG_MOVE_EXACT_BASE_H__
#define __DIALOG_MOVE_EXACT_BASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
class DIALOG_SHIM;
class TEXT_CTRL_EVAL;

#include "dialog_shim.h"
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_MOVE_EXACT_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_MOVE_EXACT_BASE : public DIALOG_SHIM
{
	private:
	
	protected:
		wxBoxSizer* bMainSizer;
		wxCheckBox* m_polarCoords;
		wxStaticText* m_xLabel;
		TEXT_CTRL_EVAL* m_xEntry;
		wxStaticText* m_xUnit;
		wxButton* m_clearX;
		wxStaticText* m_yLabel;
		TEXT_CTRL_EVAL* m_yEntry;
		wxStaticText* m_yUnit;
		wxButton* m_clearY;
		wxStaticText* m_rotLabel;
		TEXT_CTRL_EVAL* m_rotEntry;
		wxStaticText* m_rotUnit;
		wxButton* m_clearRot;
		wxRadioBox* m_originChooser;
		wxBoxSizer* bAnchorSizer;
		wxCheckBox* m_cbOverride;
		wxChoice* m_anchorChoice;
		wxStaticLine* m_staticline1;
		wxStdDialogButtonSizer* m_stdButtons;
		wxButton* m_stdButtonsOK;
		wxButton* m_stdButtonsCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnPolarChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTextFocusLost( wxFocusEvent& event ) { event.Skip(); }
		virtual void OnClear( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOriginChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOverrideChanged( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DIALOG_MOVE_EXACT_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Move Item"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 427,250 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~DIALOG_MOVE_EXACT_BASE();
	
};

#endif //__DIALOG_MOVE_EXACT_BASE_H__
