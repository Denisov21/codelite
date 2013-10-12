//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "UI.h"


// Declare the bitmap loading function
extern void wxC32BEInitBitmapResources();

static bool bBitmapLoaded = false;


SFTPStatusPageBase::SFTPStatusPageBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC32BEInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer2);
    
    m_dvListCtrl = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(300,200), wxDV_ROW_LINES|wxDV_SINGLE);
    
    boxSizer2->Add(m_dvListCtrl, 1, wxALL|wxEXPAND, 2);
    
    m_dvListCtrl->AppendTextColumn(_("Time"), wxDATAVIEW_CELL_INERT, 100, wxALIGN_LEFT);
    m_dvListCtrl->AppendBitmapColumn(_("Status"), m_dvListCtrl->GetColumnCount(), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT);
    m_dvListCtrl->AppendTextColumn(_("Account"), wxDATAVIEW_CELL_INERT, 150, wxALIGN_LEFT);
    m_dvListCtrl->AppendTextColumn(_("Message"), wxDATAVIEW_CELL_INERT, 600, wxALIGN_LEFT);
    
    SetSizeHints(-1,-1);
    if ( GetSizer() ) {
         GetSizer()->Fit(this);
    }
    Centre(wxBOTH);
    // Connect events
    m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler(SFTPStatusPageBase::OnContentMenu), NULL, this);
    
}

SFTPStatusPageBase::~SFTPStatusPageBase()
{
    m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler(SFTPStatusPageBase::OnContentMenu), NULL, this);
    
}

SFTPImages::SFTPImages()
    : wxImageList(16, 16, true)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC32BEInitBitmapResources();
        bBitmapLoaded = true;
    }
    wxBitmap bmp;
    
    bmp = wxXmlResource::Get()->LoadBitmap(wxT("sftp_ok"));
    this->Add( bmp );
    m_bitmaps.insert( std::make_pair(wxT("sftp_ok"), bmp ) );
    
    bmp = wxXmlResource::Get()->LoadBitmap(wxT("sftp_error"));
    this->Add( bmp );
    m_bitmaps.insert( std::make_pair(wxT("sftp_error"), bmp ) );
    
    bmp = wxXmlResource::Get()->LoadBitmap(wxT("sftp_info"));
    this->Add( bmp );
    m_bitmaps.insert( std::make_pair(wxT("sftp_info"), bmp ) );
    
    bmp = wxXmlResource::Get()->LoadBitmap(wxT("sftp_tab"));
    this->Add( bmp );
    m_bitmaps.insert( std::make_pair(wxT("sftp_tab"), bmp ) );
    
}

SFTPImages::~SFTPImages()
{
}

SFTPTreeViewBase::SFTPTreeViewBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC32BEInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer16 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer16);
    
    m_auibar28 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxAUI_TB_DEFAULT_STYLE);
    m_auibar28->SetToolBitmapSize(wxSize(16,16));
    
    boxSizer16->Add(m_auibar28, 0, wxEXPAND, 5);
    
    m_auibar28->AddTool(ID_OPEN_ACCOUNT_MANAGER, _("Open account manager..."), wxXmlResource::Get()->LoadBitmap(wxT("ssh-16")), wxNullBitmap, wxITEM_NORMAL, _("Open account manager..."), _("Open account manager..."), NULL);
    
    m_auibar28->AddTool(ID_SFTP_CONNECT, _("Connect"), wxXmlResource::Get()->LoadBitmap(wxT("connect")), wxNullBitmap, wxITEM_NORMAL, _("Establish connection to the selected account"), _("Establish connection to the selected account"), NULL);
    
    m_auibar28->AddTool(ID_SFTP_DISCONNECT, _("Disconnect"), wxXmlResource::Get()->LoadBitmap(wxT("disconnect")), wxNullBitmap, wxITEM_NORMAL, _("Close the current connection"), _("Close the current connection"), NULL);
    m_auibar28->Realize();
    
    wxArrayString m_choiceAccountArr;
    m_choiceAccount = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), m_choiceAccountArr, 0);
    
    boxSizer16->Add(m_choiceAccount, 0, wxALL|wxEXPAND, 2);
    
    wxBoxSizer* boxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    
    boxSizer16->Add(boxSizer20, 0, wxALL|wxEXPAND, 2);
    
    m_treeListCtrl = new wxTreeListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(200,200), wxTL_DEFAULT_STYLE|wxTL_MULTIPLE);
    
    boxSizer16->Add(m_treeListCtrl, 1, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 2);
    
    m_treeListCtrl->AppendColumn(_("Name"), 400, wxALIGN_LEFT, wxCOL_RESIZABLE|wxCOL_SORTABLE);
    m_treeListCtrl->AppendColumn(_("Type"), 100, wxALIGN_LEFT, wxCOL_RESIZABLE);
    m_treeListCtrl->AppendColumn(_("Size"), 100, wxALIGN_LEFT, wxCOL_RESIZABLE);
    
    SetSizeHints(-1,-1);
    if ( GetSizer() ) {
         GetSizer()->Fit(this);
    }
    Centre(wxBOTH);
    // Connect events
    this->Connect(ID_OPEN_ACCOUNT_MANAGER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(SFTPTreeViewBase::OnOpenAccountManager), NULL, this);
    this->Connect(ID_SFTP_CONNECT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(SFTPTreeViewBase::OnConnect), NULL, this);
    this->Connect(ID_SFTP_CONNECT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SFTPTreeViewBase::OnConnectUI), NULL, this);
    this->Connect(ID_SFTP_DISCONNECT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(SFTPTreeViewBase::OnDisconnect), NULL, this);
    this->Connect(ID_SFTP_DISCONNECT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SFTPTreeViewBase::OnDisconnectUI), NULL, this);
    m_treeListCtrl->Connect(wxEVT_TREELIST_ITEM_EXPANDING, wxTreeListEventHandler(SFTPTreeViewBase::OnItemExpanding), NULL, this);
    m_treeListCtrl->Connect(wxEVT_TREELIST_ITEM_ACTIVATED, wxTreeListEventHandler(SFTPTreeViewBase::OnItemActivated), NULL, this);
    m_treeListCtrl->Connect(wxEVT_TREELIST_ITEM_CONTEXT_MENU, wxTreeListEventHandler(SFTPTreeViewBase::OnContextMenu), NULL, this);
    
}

SFTPTreeViewBase::~SFTPTreeViewBase()
{
    this->Disconnect(ID_OPEN_ACCOUNT_MANAGER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(SFTPTreeViewBase::OnOpenAccountManager), NULL, this);
    this->Disconnect(ID_SFTP_CONNECT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(SFTPTreeViewBase::OnConnect), NULL, this);
    this->Disconnect(ID_SFTP_CONNECT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SFTPTreeViewBase::OnConnectUI), NULL, this);
    this->Disconnect(ID_SFTP_DISCONNECT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(SFTPTreeViewBase::OnDisconnect), NULL, this);
    this->Disconnect(ID_SFTP_DISCONNECT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SFTPTreeViewBase::OnDisconnectUI), NULL, this);
    m_treeListCtrl->Disconnect(wxEVT_TREELIST_ITEM_EXPANDING, wxTreeListEventHandler(SFTPTreeViewBase::OnItemExpanding), NULL, this);
    m_treeListCtrl->Disconnect(wxEVT_TREELIST_ITEM_ACTIVATED, wxTreeListEventHandler(SFTPTreeViewBase::OnItemActivated), NULL, this);
    m_treeListCtrl->Disconnect(wxEVT_TREELIST_ITEM_CONTEXT_MENU, wxTreeListEventHandler(SFTPTreeViewBase::OnContextMenu), NULL, this);
    
}
