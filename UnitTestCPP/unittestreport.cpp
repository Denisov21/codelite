//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : unittestreport.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "unittestreport.h"


// Declare the bitmap loading function
extern void wxC52E5InitBitmapResources();

static bool bBitmapLoaded = false;


UnitTestsBasePage::UnitTestsBasePage(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC52E5InitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* bSizer8 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer8);
    
    wxFlexGridSizer* fgSizer3 = new wxFlexGridSizer(  2, 2, 0, 0);
    fgSizer3->SetFlexibleDirection( wxBOTH );
    fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    fgSizer3->AddGrowableCol(1);
    
    bSizer8->Add(fgSizer3, 0, wxEXPAND, 5);
    
    m_staticText7 = new wxStaticText(this, wxID_ANY, _("Passed:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    fgSizer3->Add(m_staticText7, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_progressPassed = new ProgressCtrl(this);
    fgSizer3->Add(m_progressPassed, 0, wxALL|wxEXPAND, 5);
    
    m_staticText8 = new wxStaticText(this, wxID_ANY, _("Failed:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    fgSizer3->Add(m_staticText8, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_progressFailed = new ProgressCtrl(this);
    fgSizer3->Add(m_progressFailed, 0, wxALL|wxEXPAND, 5);
    
    wxFlexGridSizer* fgSizer4 = new wxFlexGridSizer(  0, 6, 0, 0);
    fgSizer4->SetFlexibleDirection( wxBOTH );
    fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    fgSizer4->AddGrowableCol(1);
    fgSizer4->AddGrowableCol(3);
    fgSizer4->AddGrowableCol(5);
    
    bSizer8->Add(fgSizer4, 0, 0, 5);
    
    m_staticText10 = new wxStaticText(this, wxID_ANY, _("Total tests:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    fgSizer4->Add(m_staticText10, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_staticTextTotalTests = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(50,-1), 0);
    wxFont m_staticTextTotalTestsFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_staticTextTotalTestsFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_staticTextTotalTests->SetFont(m_staticTextTotalTestsFont);
    
    fgSizer4->Add(m_staticTextTotalTests, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_staticText12 = new wxStaticText(this, wxID_ANY, _("Tests failed:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    fgSizer4->Add(m_staticText12, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_staticTextFailTestsNum = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(50,-1), 0);
    wxFont m_staticTextFailTestsNumFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_staticTextFailTestsNumFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_staticTextFailTestsNum->SetFont(m_staticTextFailTestsNumFont);
    
    fgSizer4->Add(m_staticTextFailTestsNum, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_staticText14 = new wxStaticText(this, wxID_ANY, _("Tests passed:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    fgSizer4->Add(m_staticText14, 0, wxALL, 5);
    
    m_staticTextSuccessTestsNum = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(50,-1), 0);
    wxFont m_staticTextSuccessTestsNumFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_staticTextSuccessTestsNumFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_staticTextSuccessTestsNum->SetFont(m_staticTextSuccessTestsNumFont);
    
    fgSizer4->Add(m_staticTextSuccessTestsNum, 0, wxALL|wxALIGN_LEFT, 5);
    
    m_listCtrlErrors = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxLC_SINGLE_SEL|wxLC_REPORT);
    
    bSizer8->Add(m_listCtrlErrors, 1, wxALL|wxEXPAND, 5);
    
    
    SetSizeHints(-1,-1);
    if ( GetSizer() ) {
         GetSizer()->Fit(this);
    }
    Centre(wxBOTH);
    // Connect events
    m_listCtrlErrors->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(UnitTestsBasePage::OnItemActivated), NULL, this);
    
}

UnitTestsBasePage::~UnitTestsBasePage()
{
    m_listCtrlErrors->Disconnect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(UnitTestsBasePage::OnItemActivated), NULL, this);
    
}
