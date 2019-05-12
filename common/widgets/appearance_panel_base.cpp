///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "appearance_panel_base.h"

///////////////////////////////////////////////////////////////////////////

APPEARANCE_PANEL_BASE::APPEARANCE_PANEL_BASE( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	this->SetFont( wxFont( 9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	wxBoxSizer* m_outer_sizer;
	m_outer_sizer = new wxBoxSizer( wxVERTICAL );

	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_layers_panel = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	m_layers_window = new wxScrolledWindow( m_layers_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_layers_window->SetScrollRate( 0, 5 );
	m_layers_outer_sizer = new wxBoxSizer( wxVERTICAL );


	m_layers_window->SetSizer( m_layers_outer_sizer );
	m_layers_window->Layout();
	m_layers_outer_sizer->Fit( m_layers_window );
	bSizer11->Add( m_layers_window, 1, wxEXPAND | wxALL, 5 );

	m_staticline2 = new wxStaticLine( m_layers_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer11->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	m_staticText13 = new wxStaticText( m_layers_panel, wxID_ANY, wxT("Show Non-Active Layers As:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	bSizer11->Add( m_staticText13, 0, wxALL, 5 );

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );

	m_radioBtn1 = new wxRadioButton( m_layers_panel, wxID_ANY, wxT("Normal"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtn1->SetToolTip( wxT("Non-active layers will be shown in full color") );

	bSizer19->Add( m_radioBtn1, 0, wxALL, 5 );

	m_radioBtn2 = new wxRadioButton( m_layers_panel, wxID_ANY, wxT("Dimmed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtn2->SetToolTip( wxT("Non-active layers will be grayscale and dimmed") );

	bSizer19->Add( m_radioBtn2, 0, wxALL, 5 );

	m_radioBtn3 = new wxRadioButton( m_layers_panel, wxID_ANY, wxT("Off"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtn3->SetToolTip( wxT("Non-active layers will be hidden") );

	bSizer19->Add( m_radioBtn3, 0, wxALL, 5 );


	bSizer11->Add( bSizer19, 0, wxEXPAND, 5 );

	m_staticline5 = new wxStaticLine( m_layers_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer11->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText9 = new wxStaticText( m_layers_panel, wxID_ANY, wxT("Layer Opacity"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	m_staticText9->SetToolTip( wxT("Set the opacity for all layers") );

	bSizer12->Add( m_staticText9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_slider_all_layers = new wxSlider( m_layers_panel, wxID_ANY, 100, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	bSizer12->Add( m_slider_all_layers, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 10 );


	bSizer11->Add( bSizer12, 0, wxEXPAND|wxRIGHT, 5 );

	m_cb_active_layer_opaque = new wxCheckBox( m_layers_panel, wxID_ANY, wxT("Make Active Layer Opaque"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cb_active_layer_opaque->SetToolTip( wxT("Make the active layer opaque even if layer opacity is not at maximum") );

	bSizer11->Add( m_cb_active_layer_opaque, 0, wxALL, 5 );


	m_layers_panel->SetSizer( bSizer11 );
	m_layers_panel->Layout();
	bSizer11->Fit( m_layers_panel );
	m_notebook->AddPage( m_layers_panel, wxT("Layers"), true );
	m_objects_panel = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );

	m_objects_window = new wxScrolledWindow( m_objects_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_objects_window->SetScrollRate( 5, 5 );
	bSizer15->Add( m_objects_window, 1, wxEXPAND | wxALL, 5 );


	m_objects_panel->SetSizer( bSizer15 );
	m_objects_panel->Layout();
	bSizer15->Fit( m_objects_panel );
	m_notebook->AddPage( m_objects_panel, wxT("Objects"), false );
	m_nets_panel = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );

	m_txt_net_search = new wxTextCtrl( m_nets_panel, wxID_ANY, wxT("Search nets..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_txt_net_search, 0, wxALL|wxEXPAND, 5 );

	m_nets_window = new wxScrolledWindow( m_nets_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_nets_window->SetScrollRate( 5, 5 );
	bSizer16->Add( m_nets_window, 1, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText14 = new wxStaticText( m_nets_panel, wxID_ANY, wxT("Net Classes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	m_staticText14->SetFont( wxFont( 9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	bSizer20->Add( m_staticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	bSizer20->Add( 0, 0, 1, wxEXPAND, 5 );

	m_btn_configure_net_classes = new wxBitmapButton( m_nets_panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxBORDER_NONE );
	m_btn_configure_net_classes->SetToolTip( wxT("Configure Net Classes...") );

	bSizer20->Add( m_btn_configure_net_classes, 0, wxALL, 5 );


	bSizer16->Add( bSizer20, 0, wxEXPAND, 5 );

	m_txt_search_net_classes = new wxTextCtrl( m_nets_panel, wxID_ANY, wxT("Search net classes..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_txt_search_net_classes, 0, wxALL|wxEXPAND, 5 );

	m_net_classes_window = new wxScrolledWindow( m_nets_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_net_classes_window->SetScrollRate( 5, 5 );
	bSizer16->Add( m_net_classes_window, 1, wxEXPAND | wxALL, 5 );

	m_staticline4 = new wxStaticLine( m_nets_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer16->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );

	m_btn_hide_all_nets = new wxButton( m_nets_panel, wxID_ANY, wxT("Hide All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btn_hide_all_nets->SetToolTip( wxT("Hide All Nets") );
	m_btn_hide_all_nets->SetMaxSize( wxSize( 150,-1 ) );

	bSizer17->Add( m_btn_hide_all_nets, 1, wxALL, 5 );

	m_btn_show_all_nets = new wxButton( m_nets_panel, wxID_ANY, wxT("Show All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btn_show_all_nets->SetToolTip( wxT("Show All Nets") );
	m_btn_show_all_nets->SetMaxSize( wxSize( 150,-1 ) );

	bSizer17->Add( m_btn_show_all_nets, 1, wxALL, 5 );


	bSizer16->Add( bSizer17, 0, wxEXPAND, 5 );

	wxString m_radio_net_color_modeChoices[] = { wxT("Ratsnets Only"), wxT("All Net Objects") };
	int m_radio_net_color_modeNChoices = sizeof( m_radio_net_color_modeChoices ) / sizeof( wxString );
	m_radio_net_color_mode = new wxRadioBox( m_nets_panel, wxID_ANY, wxT("Net Colors Apply To:"), wxDefaultPosition, wxDefaultSize, m_radio_net_color_modeNChoices, m_radio_net_color_modeChoices, 1, wxRA_SPECIFY_COLS );
	m_radio_net_color_mode->SetSelection( 0 );
	bSizer16->Add( m_radio_net_color_mode, 0, wxALL|wxEXPAND, 5 );


	m_nets_panel->SetSizer( bSizer16 );
	m_nets_panel->Layout();
	bSizer16->Fit( m_nets_panel );
	m_notebook->AddPage( m_nets_panel, wxT("Nets"), false );

	m_outer_sizer->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText12 = new wxStaticText( this, wxID_ANY, wxT("Stored Settings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer18->Add( m_staticText12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_combo_stored_settings_name = new wxComboBox( this, wxID_ANY, wxT("Default"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer18->Add( m_combo_stored_settings_name, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	m_outer_sizer->Add( bSizer18, 0, wxEXPAND|wxLEFT, 5 );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );

	m_btn_stored_settings_new = new wxButton( this, wxID_ANY, wxT("New"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btn_stored_settings_new->SetToolTip( wxT("Create a New Appearance Setting") );
	m_btn_stored_settings_new->SetMinSize( wxSize( 50,-1 ) );

	bSizer14->Add( m_btn_stored_settings_new, 1, wxALL, 5 );

	m_btn_stored_settings_save = new wxButton( this, wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btn_stored_settings_save->Enable( false );
	m_btn_stored_settings_save->SetToolTip( wxT("Save the Current Appearance Setting") );
	m_btn_stored_settings_save->SetMinSize( wxSize( 50,-1 ) );

	bSizer14->Add( m_btn_stored_settings_save, 1, wxALL, 5 );

	m_btn_stored_settings_delete = new wxButton( this, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btn_stored_settings_delete->Enable( false );
	m_btn_stored_settings_delete->SetToolTip( wxT("Delete the Current Appearance Setting") );
	m_btn_stored_settings_delete->SetMinSize( wxSize( 50,-1 ) );

	bSizer14->Add( m_btn_stored_settings_delete, 1, wxALL, 5 );


	m_outer_sizer->Add( bSizer14, 0, wxEXPAND, 5 );


	this->SetSizer( m_outer_sizer );
	this->Layout();
}

APPEARANCE_PANEL_BASE::~APPEARANCE_PANEL_BASE()
{
}
