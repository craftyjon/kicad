///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/scrolwin.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/textctrl.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/radiobox.h>
#include <wx/notebook.h>
#include <wx/combobox.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class APPEARANCE_PANEL_BASE
///////////////////////////////////////////////////////////////////////////////
class APPEARANCE_PANEL_BASE : public wxPanel
{
	private:

	protected:
		wxNotebook* m_notebook;
		wxPanel* m_layers_panel;
		wxScrolledWindow* m_layers_window;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText13;
		wxRadioButton* m_radioBtn1;
		wxRadioButton* m_radioBtn2;
		wxRadioButton* m_radioBtn3;
		wxStaticLine* m_staticline5;
		wxStaticText* m_staticText9;
		wxSlider* m_slider_all_layers;
		wxCheckBox* m_cb_active_layer_opaque;
		wxPanel* m_objects_panel;
		wxScrolledWindow* m_objects_window;
		wxPanel* m_nets_panel;
		wxTextCtrl* m_txt_net_search;
		wxScrolledWindow* m_nets_window;
		wxStaticText* m_staticText14;
		wxBitmapButton* m_btn_configure_net_classes;
		wxTextCtrl* m_txt_search_net_classes;
		wxScrolledWindow* m_net_classes_window;
		wxStaticLine* m_staticline4;
		wxButton* m_btn_hide_all_nets;
		wxButton* m_btn_show_all_nets;
		wxRadioBox* m_radio_net_color_mode;
		wxStaticText* m_staticText12;
		wxComboBox* m_combo_stored_settings_name;
		wxButton* m_btn_stored_settings_new;
		wxButton* m_btn_stored_settings_save;
		wxButton* m_btn_stored_settings_delete;

	public:

		APPEARANCE_PANEL_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 240,600 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );
		~APPEARANCE_PANEL_BASE();

};

