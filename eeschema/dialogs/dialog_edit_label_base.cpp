///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 30 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "dialog_edit_label_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_LABEL_EDITOR_BASE::DIALOG_LABEL_EDITOR_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_textControlSizer = new wxFlexGridSizer( 2, 2, 3, 3 );
	m_textControlSizer->AddGrowableCol( 1 );
	m_textControlSizer->SetFlexibleDirection( wxBOTH );
	m_textControlSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_Label = new wxStaticText( this, wxID_ANY, _("Label:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Label->Wrap( -1 );
	m_Label->SetToolTip( _("Enter the text to be used within the schematic") );
	
	m_textControlSizer->Add( m_Label, 0, wxALIGN_TOP|wxRIGHT|wxTOP, 3 );
	
	wxBoxSizer* bSizeText;
	bSizeText = new wxBoxSizer( wxVERTICAL );
	
	m_valueSingleLine = new wxTextCtrl( this, wxID_VALUESINGLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_RICH );
	m_valueSingleLine->SetMinSize( wxSize( 360,-1 ) );
	
	m_valueSingleLine->SetValidator( wxTextValidator( wxFILTER_EXCLUDE_CHAR_LIST, &m_labelText ) );
	
	bSizeText->Add( m_valueSingleLine, 0, wxBOTTOM|wxEXPAND|wxLEFT, 3 );
	
	m_valueMultiLine = new wxTextCtrl( this, wxID_VALUEMULTI, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	m_valueMultiLine->SetMinSize( wxSize( 480,72 ) );
	
	bSizeText->Add( m_valueMultiLine, 1, wxBOTTOM|wxEXPAND|wxLEFT, 3 );
	
	m_valueCombo = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER ); 
	m_valueCombo->SetValidator( wxTextValidator( wxFILTER_EXCLUDE_CHAR_LIST, &m_comboText ) );
	
	bSizeText->Add( m_valueCombo, 0, wxEXPAND|wxBOTTOM|wxLEFT, 5 );
	
	
	m_textControlSizer->Add( bSizeText, 1, wxEXPAND, 3 );
	
	m_textSizeLabel = new wxStaticText( this, wxID_ANY, _("Text Size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_textSizeLabel->Wrap( -1 );
	m_textControlSizer->Add( m_textSizeLabel, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT, 3 );
	
	wxBoxSizer* bSizeCtrlSizer;
	bSizeCtrlSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_textSizeCtrl = new wxTextCtrl( this, wxID_SIZE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizeCtrlSizer->Add( m_textSizeCtrl, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT|wxTOP, 3 );
	
	m_textSizeUnits = new wxStaticText( this, wxID_ANY, _("units"), wxDefaultPosition, wxDefaultSize, 0 );
	m_textSizeUnits->Wrap( -1 );
	bSizeCtrlSizer->Add( m_textSizeUnits, 0, wxALIGN_CENTER_VERTICAL|wxTOP, 2 );
	
	
	m_textControlSizer->Add( bSizeCtrlSizer, 1, wxEXPAND, 3 );
	
	
	bMainSizer->Add( m_textControlSizer, 1, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );
	
	wxBoxSizer* m_OptionsSizer;
	m_OptionsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_TextOrientChoices[] = { _("Right"), _("Up"), _("Left"), _("Down") };
	int m_TextOrientNChoices = sizeof( m_TextOrientChoices ) / sizeof( wxString );
	m_TextOrient = new wxRadioBox( this, wxID_ANY, _("Orientation"), wxDefaultPosition, wxDefaultSize, m_TextOrientNChoices, m_TextOrientChoices, 1, wxRA_SPECIFY_COLS );
	m_TextOrient->SetSelection( 0 );
	m_OptionsSizer->Add( m_TextOrient, 1, wxRIGHT|wxTOP|wxEXPAND, 3 );
	
	wxString m_TextStyleChoices[] = { _("Normal"), _("Italic"), _("Bold"), _("Bold and italic") };
	int m_TextStyleNChoices = sizeof( m_TextStyleChoices ) / sizeof( wxString );
	m_TextStyle = new wxRadioBox( this, wxID_ANY, _("Style"), wxDefaultPosition, wxDefaultSize, m_TextStyleNChoices, m_TextStyleChoices, 1, wxRA_SPECIFY_COLS );
	m_TextStyle->SetSelection( 3 );
	m_OptionsSizer->Add( m_TextStyle, 1, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 3 );
	
	wxString m_TextShapeChoices[] = { _("Input"), _("Output"), _("Bidirectional"), _("Tri-state"), _("Passive") };
	int m_TextShapeNChoices = sizeof( m_TextShapeChoices ) / sizeof( wxString );
	m_TextShape = new wxRadioBox( this, wxID_ANY, _("Shape"), wxDefaultPosition, wxDefaultSize, m_TextShapeNChoices, m_TextShapeChoices, 1, wxRA_SPECIFY_COLS );
	m_TextShape->SetSelection( 3 );
	m_OptionsSizer->Add( m_TextShape, 1, wxALL|wxLEFT|wxTOP|wxEXPAND, 3 );
	
	
	bMainSizer->Add( m_OptionsSizer, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	
	bMainSizer->Add( m_sdbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	
	this->SetSizer( bMainSizer );
	this->Layout();
	bMainSizer->Fit( this );
	
	// Connect Events
	m_valueSingleLine->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DIALOG_LABEL_EDITOR_BASE::OnEnterKey ), NULL, this );
	m_valueCombo->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DIALOG_LABEL_EDITOR_BASE::OnEnterKey ), NULL, this );
}

DIALOG_LABEL_EDITOR_BASE::~DIALOG_LABEL_EDITOR_BASE()
{
	// Disconnect Events
	m_valueSingleLine->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DIALOG_LABEL_EDITOR_BASE::OnEnterKey ), NULL, this );
	m_valueCombo->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DIALOG_LABEL_EDITOR_BASE::OnEnterKey ), NULL, this );
	
}
