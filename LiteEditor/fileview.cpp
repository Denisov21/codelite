//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : fileview.cpp
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
#include <wx/tokenzr.h>
#include <wx/mimetype.h>
#include "build_settings_config.h"
#include "environmentconfig.h"
#include "evnvarlist.h"
#include "pluginmanager.h"
#include "workspacesettingsdlg.h"
#include "progress_dialog.h"
#include "bitmap_loader.h"
#include "fileview.h"
#include "frame.h"
#include "nameanddescdlg.h"
#include "globals.h"
#include "manager.h"
#include "tree.h"
#include <wx/xrc/xmlres.h>
#include "wx/imaglist.h"
#include <wx/textdlg.h>
#include <deque>
#include "new_item_dlg.h"
#include "project_settings_dlg.h"
#include "depends_dlg.h"
#include "buildmanager.h"
#include "macros.h"
#include "pluginmanager.h"
#include "dirtraverser.h"
#include "ctags_manager.h"
#include <wx/progdlg.h>
#include "editor_config.h"
#include "editorsettingslocal.h"
#include "localworkspace.h"
#include "plugin.h"
#include "event_notifier.h"
#include "build_custom_targets_menu_manager.h"
#include "ImportFilesDialogNew.h"
#include "project.h"
#include "macros.h"
#include <wx/treectrl.h>
#include "drawingutils.h"
#include <wx/richmsgdlg.h>
#include "cl_command_event.h"
#include "NewVirtualFolderDlg.h"
#include "workspacetab.h"
#include "file_logger.h"
#include "clFileOrFolderDropTarget.h"
#include "importfilessettings.h"
#include <project.h>
#include "compiler.h"
#include "ICompilerLocator.h"

IMPLEMENT_DYNAMIC_CLASS(FileViewTree, wxTreeCtrl)

static const wxString gsCustomTargetsMenu(wxT("Custom Build Targets"));

BEGIN_EVENT_TABLE(FileViewTree, wxTreeCtrl)
EVT_TREE_BEGIN_DRAG(wxID_ANY, FileViewTree::OnItemBeginDrag)
EVT_TREE_END_DRAG(wxID_ANY, FileViewTree::OnItemEndDrag)
EVT_TREE_ITEM_MENU(wxID_ANY, FileViewTree::OnPopupMenu)
EVT_TREE_SEL_CHANGED(wxID_ANY, FileViewTree::OnSelectionChanged)

EVT_MENU(XRCID("local_workspace_prefs"), FileViewTree::OnLocalPrefs)
EVT_MENU(XRCID("local_workspace_settings"), FileViewTree::OnLocalWorkspaceSettings)
EVT_MENU(XRCID("remove_project"), FileViewTree::OnRemoveProject)
EVT_MENU(XRCID("rename_project"), FileViewTree::OnRenameProject)
EVT_MENU(XRCID("set_as_active"), FileViewTree::OnSetActive)
EVT_MENU(XRCID("new_item"), FileViewTree::OnNewItem)
EVT_MENU(XRCID("add_existing_item"), FileViewTree::OnAddExistingItem)
EVT_MENU(XRCID("new_virtual_folder"), FileViewTree::OnNewVirtualFolder)
EVT_MENU(XRCID("remove_virtual_folder"), FileViewTree::OnRemoveVirtualFolder)
EVT_MENU(XRCID("local_project_prefs"), FileViewTree::OnLocalPrefs)
EVT_MENU(XRCID("project_properties"), FileViewTree::OnProjectProperties)
EVT_MENU(XRCID("sort_item"), FileViewTree::OnSortItem)
EVT_MENU(XRCID("remove_item"), FileViewTree::OnRemoveItem)
EVT_MENU(XRCID("export_makefile"), FileViewTree::OnExportMakefile)
EVT_MENU(XRCID("save_as_template"), FileViewTree::OnSaveAsTemplate)
EVT_MENU(XRCID("build_order"), FileViewTree::OnBuildOrder)
EVT_MENU(XRCID("clean_project"), FileViewTree::OnClean)
EVT_MENU(XRCID("build_project"), FileViewTree::OnBuild)
EVT_MENU(XRCID("rebuild_project"), FileViewTree::OnReBuild)
EVT_MENU(XRCID("generate_makefile"), FileViewTree::OnRunPremakeStep)
EVT_MENU(XRCID("stop_build"), FileViewTree::OnStopBuild)
EVT_MENU(XRCID("retag_project"), FileViewTree::OnRetagProject)
EVT_MENU(XRCID("build_project_only"), FileViewTree::OnBuildProjectOnly)
EVT_MENU(XRCID("clean_project_only"), FileViewTree::OnCleanProjectOnly)
EVT_MENU(XRCID("rebuild_project_only"), FileViewTree::OnRebuildProjectOnly)
EVT_MENU(XRCID("import_directory"), FileViewTree::OnImportDirectory)
EVT_MENU(XRCID("reconcile_project"), FileViewTree::OnReconcileProject)
EVT_MENU(XRCID("open_in_editor"), FileViewTree::OnOpenInEditor)
EVT_MENU(XRCID("compile_item"), FileViewTree::OnCompileItem)
EVT_MENU(XRCID("exclude_from_build"), FileViewTree::OnExcludeFromBuild)
EVT_MENU(XRCID("preprocess_item"), FileViewTree::OnPreprocessItem)
EVT_MENU(XRCID("rename_item"), FileViewTree::OnRenameItem)
EVT_MENU(XRCID("rename_virtual_folder"), FileViewTree::OnRenameVirtualFolder)
EVT_MENU(XRCID("open_with_default_application"), FileViewTree::OnOpenWithDefaultApplication)

EVT_UPDATE_UI(XRCID("remove_project"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("rename_project"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("set_as_active"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("new_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("add_existing_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("new_virtual_folder"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("remove_virtual_folder"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("project_properties"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("sort_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("remove_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("export_makefile"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("save_as_template"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("build_order"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("clean_project"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("build_project"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("rebuild_project"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("retag_project"), FileViewTree::OnRetagInProgressUI)
EVT_UPDATE_UI(XRCID("retag_workspace"), FileViewTree::OnRetagInProgressUI)
EVT_UPDATE_UI(XRCID("build_project_only"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("clean_project_only"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("rebuild_project_only"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("import_directory"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("reconcile_project"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("compile_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("exclude_from_build"), FileViewTree::OnExcludeFromBuildUI)
EVT_UPDATE_UI(XRCID("preprocess_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("rename_item"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("generate_makefile"), FileViewTree::OnBuildInProgress)
EVT_UPDATE_UI(XRCID("local_workspace_settings"), FileViewTree::OnBuildInProgress)
END_EVENT_TABLE()

static int PROJECT_IMG_IDX = wxNOT_FOUND;
static int FOLDER_IMG_IDX = wxNOT_FOUND;
static int WORKSPACE_IMG_IDX = wxNOT_FOUND;
static int ACTIVE_PROJECT_IMG_IDX = wxNOT_FOUND;

FileViewTree::FileViewTree() {}

void FileViewTree::OnBuildInProgress(wxUpdateUIEvent& event) { event.Enable(!ManagerST::Get()->IsBuildInProgress()); }

FileViewTree::FileViewTree(wxWindow* parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Create(parent, id, pos, size, style);
    MSWSetNativeTheme(this);

    // Initialise images map
    BitmapLoader* bmpLoader = PluginManager::Get()->GetStdIcons();

    // Prepare the standard mime-type image list
    wxImageList* images = bmpLoader->MakeStandardMimeImageList();

    FOLDER_IMG_IDX = images->Add(bmpLoader->LoadBitmap(wxT("mime/16/folder")));
    ACTIVE_PROJECT_IMG_IDX = images->Add(bmpLoader->LoadBitmap(wxT("workspace/16/project_active")));
    WORKSPACE_IMG_IDX = bmpLoader->GetMimeImageId(FileExtManager::TypeWorkspace);
    PROJECT_IMG_IDX = bmpLoader->GetMimeImageId(FileExtManager::TypeProject);

    AssignImageList(images);
    Connect(GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler(FileViewTree::OnMouseDblClick));
    Connect(GetId(), wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(FileViewTree::OnItemActivated));
    EventNotifier::Get()->Connect(
        wxEVT_REBUILD_WORKSPACE_TREE, wxCommandEventHandler(FileViewTree::OnBuildTree), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_BUILD_PROJECT_ONLY, wxCommandEventHandler(FileViewTree::OnBuildProjectOnlyInternal), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_CMD_CLEAN_PROJECT_ONLY, wxCommandEventHandler(FileViewTree::OnCleanProjectOnlyInternal), NULL, this);

    Bind(wxEVT_DND_FOLDER_DROPPED, &FileViewTree::OnFolderDropped, this);
}

FileViewTree::~FileViewTree()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_REBUILD_WORKSPACE_TREE, wxCommandEventHandler(FileViewTree::OnBuildTree), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_BUILD_PROJECT_ONLY, wxCommandEventHandler(FileViewTree::OnBuildProjectOnlyInternal), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_CLEAN_PROJECT_ONLY, wxCommandEventHandler(FileViewTree::OnCleanProjectOnlyInternal), NULL, this);
    Unbind(wxEVT_DND_FOLDER_DROPPED, &FileViewTree::OnFolderDropped, this);
}

void FileViewTree::Create(wxWindow* parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    bool multi(true);
    style |= (wxTR_HAS_BUTTONS | wxTR_FULL_ROW_HIGHLIGHT | wxTR_NO_LINES);
    if(multi) style |= wxTR_MULTIPLE;

    wxTreeCtrl::Create(parent, id, pos, size, style);
    SetDropTarget(new clFileOrFolderDropTarget(this));
    BuildTree();
}

void FileViewTree::BuildTree()
{
    wxWindowUpdateLocker locker(this);
    clCommandEvent event(wxEVT_WORKSPACE_VIEW_BUILD_STARTING);
    if(EventNotifier::Get()->ProcessEvent(event)) {
        // User wishes to replace the icons
        wxImageList* imgList = reinterpret_cast<wxImageList*>(event.GetClientData());
        if(imgList) {
            AssignImageList(imgList);
        }
    }

    DeleteAllItems();
    long flags = GetWindowStyle();
    SetWindowStyle(flags | wxTR_MULTIPLE);
    m_itemsToSort.clear();

    if(ManagerST::Get()->IsWorkspaceOpen()) {
        // Add an invisible tree root
        ProjectItem data;
        data.m_displayName = WorkspaceST::Get()->GetName();
        data.m_kind = ProjectItem::TypeWorkspace;

        wxTreeItemId root = AddRoot(data.m_displayName, WORKSPACE_IMG_IDX, -1, new FilewViewTreeItemData(data));
        m_itemsToSort[root.m_pItem] = true;

        wxArrayString list;
        ManagerST::Get()->GetProjectList(list);

        for(size_t n = 0; n < list.GetCount(); n++) {
            BuildProjectNode(list.Item(n));
        }
        SortTree();

        // set selection to first item
        SelectItem(root, HasFlag(wxTR_MULTIPLE) ? false : true);
    }
}

void FileViewTree::SortItem(wxTreeItemId& item)
{
    if(item.IsOk() && ItemHasChildren(item)) {
        SortChildren(item);
    }
}

void FileViewTree::SortTree()
{
    // sort the tree
    std::map<void*, bool>::const_iterator iter = m_itemsToSort.begin();
    for(; iter != m_itemsToSort.end(); iter++) {
        wxTreeItemId item = iter->first;
        SortItem(item);
    }
    m_itemsToSort.clear();
}

wxTreeItemId FileViewTree::GetSingleSelection()
{
#if wxVERSION_NUMBER > 2900
    return GetFocusedItem();
#else
    if(HasFlag(wxTR_MULTIPLE)) {
        wxTreeItemId invalid;
        wxArrayTreeItemIds arr;
        size_t count = GetMultiSelection(arr);
        return (count > 0) ? arr.Item(0) : invalid;

    } else {
        // Single selection tree
        return GetSelection();
    }
#endif
}

int FileViewTree::GetIconIndex(const ProjectItem& item)
{
    BitmapLoader* bmpLoader = PluginManager::Get()->GetStdIcons();
    int icondIndex(bmpLoader->GetMimeImageId(FileExtManager::TypeText));
    switch(item.GetKind()) {
    case ProjectItem::TypeProject:
        icondIndex = PROJECT_IMG_IDX;
        break;
    case ProjectItem::TypeVirtualDirectory:
        icondIndex = FOLDER_IMG_IDX;
        break;
    case ProjectItem::TypeFile: {
        wxFileName filename(item.GetFile());
        icondIndex = bmpLoader->GetMimeImageId(filename.GetFullName());
        if(icondIndex == wxNOT_FOUND) icondIndex = bmpLoader->GetMimeImageId(FileExtManager::TypeText);
    } break;
    default:
        break;
    }
    return icondIndex;
}

void FileViewTree::BuildProjectNode(const wxString& projectName)
{
    wxString err_msg;
    ProjectPtr prj = WorkspaceST::Get()->FindProjectByName(projectName, err_msg);
    ProjectTreePtr tree = prj->AsTree();
    TreeWalker<wxString, ProjectItem> walker(tree->GetRoot());

    std::map<wxString, wxTreeItemId> items;
    for(; !walker.End(); walker++) {

        // Did we get the icon from a plugin?
        bool iconFromPlugin = false;

        // Add the item to the tree
        ProjectTreeNode* node = walker.GetNode();
        wxTreeItemId parentHti;
        if(node->IsRoot()) {
            parentHti = GetRootItem();
        } else {
            wxString parentKey = node->GetParent()->GetKey();
            if(items.find(parentKey) == items.end()) continue;
            parentHti = items.find(parentKey)->second;
        }

        // Allow plugins to customize the project icon/colours
        int projectIconIndex = wxNOT_FOUND;
        if(node->GetData().GetKind() == ProjectItem::TypeProject) {
            DoGetProjectIconIndex(node->GetData().GetDisplayName(), projectIconIndex, iconFromPlugin);

        } else {
            projectIconIndex = GetIconIndex(node->GetData());
        }

        wxTreeItemId hti = AppendItem(parentHti,                        // parent
                                      node->GetData().GetDisplayName(), // display name
                                      projectIconIndex,                 // item image index
                                      projectIconIndex,                 // selected item image
                                      new FilewViewTreeItemData(node->GetData()));

        // FIXME ::
        // Use a more efficient way for checking if a file is 'Excluded' from the build

        // if ( IsFileExcludedFromBuild(hti) ) {
        //    SetItemTextColour(hti, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT) );
        //}
        m_itemsToSort[parentHti.m_pItem] = true;

        // Set active project with bold
        wxString activeProjectName = ManagerST::Get()->GetActiveProjectName();
        wxString displayName = node->GetData().GetDisplayName();

        if(parentHti == GetRootItem() && displayName == activeProjectName) {
            SetItemBold(hti);
            if(!iconFromPlugin) {
                SetItemImage(hti, ACTIVE_PROJECT_IMG_IDX);
                SetItemImage(hti, ACTIVE_PROJECT_IMG_IDX, wxTreeItemIcon_Selected);
                SetItemImage(hti, ACTIVE_PROJECT_IMG_IDX, wxTreeItemIcon_SelectedExpanded);
            }
        }

        items[node->GetKey()] = hti;
    }
}

//-----------------------------------------------
// Event handlers
//-----------------------------------------------

void FileViewTree::ShowFileContextMenu()
{
    wxArrayTreeItemIds items;
    GetSelections(items);
    if(items.IsEmpty()) return;

    wxMenu* menu = wxXmlResource::Get()->LoadMenu(wxT("file_tree_file"));
    if(!ManagerST::Get()->IsBuildInProgress()) {
        // Let the plugins alter it
        clContextMenuEvent event(wxEVT_CONTEXT_MENU_FILE);
        event.SetMenu(menu);

        wxArrayString files;
        for(size_t i = 0; i < items.GetCount(); ++i) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(items.Item(i)));
            if(data->GetData().GetKind() == ProjectItem::TypeFile) {
                files.Add(data->GetData().GetFile());
            }
        }
        event.SetStrings(files);
        EventNotifier::Get()->ProcessEvent(event);

        // Let the plugin hook their content (using the deprecated way)
        PluginManager::Get()->HookPopupMenu(menu, MenuTypeFileView_File);
    }

    PopupMenu(menu);
    wxDELETE(menu);
}

void FileViewTree::ShowVirtualFolderContextMenu(FilewViewTreeItemData* itemData)
{
    wxMenu* menu = wxXmlResource::Get()->LoadMenu("file_tree_folder");
    if(!ManagerST::Get()->IsBuildInProgress()) {
        // Let the plugins alter it
        clContextMenuEvent event(wxEVT_CONTEXT_MENU_VIRTUAL_FOLDER);
        event.SetMenu(menu);
        EventNotifier::Get()->ProcessEvent(event);

        // Let the plugin hook their content (using the deprecated way)
        PluginManager::Get()->HookPopupMenu(menu, MenuTypeFileView_Folder);
    }

    PopupMenu(menu);
    wxDELETE(menu);
}

void FileViewTree::ShowProjectContextMenu(const wxString& projectName)
{
    wxMenu* menu = wxXmlResource::Get()->LoadMenu(wxT("file_tree_project"));
    // set the icon for the default actions (build, clean and settings)
    wxBitmap bmpBuild = PluginManager::Get()->GetStdIcons()->LoadBitmap("toolbars/16/build/build");
    wxBitmap bmpClean = PluginManager::Get()->GetStdIcons()->LoadBitmap("toolbars/16/build/clean");
    wxBitmap bmpSettings = wxXmlResource::Get()->LoadBitmap(wxT("configure"));

    menu->FindItem(XRCID("build_project"))->SetBitmap(bmpBuild);
    menu->FindItem(XRCID("clean_project"))->SetBitmap(bmpClean);
    menu->FindItem(XRCID("project_properties"))->SetBitmap(bmpSettings);

    BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
    if(bldConf && bldConf->IsCustomBuild()) {
        wxMenuItem* item = NULL;
#if 0
        wxString toolName = bldConf->GetToolName();
        if(toolName != wxT("None")) {
            
            // add the custom execution command
            item = new wxMenuItem(menu, wxID_SEPARATOR);
            menu->Prepend(item);
            wxString menu_text(_("Run ") + toolName);

            item = new wxMenuItem(menu, XRCID("generate_makefile"), menu_text, wxEmptyString, wxITEM_NORMAL);
            menu->Prepend(item);
        }
#endif
        // append the custom build targets
        const BuildConfig::StringMap_t& targets = bldConf->GetCustomTargets();
        if(targets.empty() == false) {
            wxMenu* customTargetsMenu = new wxMenu();
            CustomTargetsMgr::Get().SetTargets(projectName, targets);
            const CustomTargetsMgr::Map_t& targetsMap = CustomTargetsMgr::Get().GetTargets();
            // get list of custom targets, and create menu entry for each target
            CustomTargetsMgr::Map_t::const_iterator iter = targetsMap.begin();
            for(; iter != targetsMap.end(); ++iter) {
                item = new wxMenuItem(customTargetsMenu,
                                      iter->first,        // Menu ID
                                      iter->second.first, // Menu Name
                                      wxEmptyString,
                                      wxITEM_NORMAL);
                customTargetsMenu->Append(item);
            }

            // iterator over the menu items and search for 'Project Only' target
            // this is the position that we want to place our custom targets menu
            wxMenuItemList items = menu->GetMenuItems();
            wxMenuItemList::iterator liter = items.begin();
            size_t position(0);
            for(; liter != items.end(); liter++) {
                wxMenuItem* mi = *liter;
                if(mi->GetId() == XRCID("project_only")) {
                    break;
                }
                position++;
            }
            menu->Insert(position, XRCID("custom_targets"), gsCustomTargetsMenu, customTargetsMenu);
        }
    }

    if(!ManagerST::Get()->IsBuildInProgress()) {
        // Let the plugins alter it
        clContextMenuEvent event(wxEVT_CONTEXT_MENU_PROJECT);
        event.SetMenu(menu);
        EventNotifier::Get()->ProcessEvent(event);

        // Use the old system
        PluginManager::Get()->HookPopupMenu(menu, MenuTypeFileView_Project);
    }

    PopupMenu(menu);
    wxDELETE(menu);
}

void FileViewTree::ShowWorkspaceContextMenu()
{
    // Load the basic menu
    wxMenu* menu = wxXmlResource::Get()->LoadMenu(wxT("workspace_popup_menu"));

    if(!ManagerST::Get()->IsBuildInProgress()) {
        // Let the plugins alter it
        clContextMenuEvent event(wxEVT_CONTEXT_MENU_WORKSPACE);
        event.SetMenu(menu);
        EventNotifier::Get()->ProcessEvent(event);

        // Use the old system
        PluginManager::Get()->HookPopupMenu(menu, MenuTypeFileView_Workspace);
    }

    // Show it
    PopupMenu(menu);
    wxDELETE(menu);
}

void FileViewTree::OnPopupMenu(wxTreeEvent& event)
{
    if(event.GetItem().IsOk()) {
        if(IsSelected(event.GetItem()) == false) {
            // Don't call SelectItem() if it's already selected: in <wx2.9 it toggles!
            SelectItem(event.GetItem());
        }
        wxTreeItemId item = event.GetItem();

        if(item.IsOk()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            switch(data->GetData().GetKind()) {
            case ProjectItem::TypeProject:
                ShowProjectContextMenu(data->GetData().GetDisplayName());
                break;
            case ProjectItem::TypeVirtualDirectory:
                ShowVirtualFolderContextMenu(data);
                break;
            case ProjectItem::TypeFile:
                ShowFileContextMenu();
                break;
            case ProjectItem::TypeWorkspace:
                ShowWorkspaceContextMenu();
                break;
            default:
                break;
            }
        }
    } else {
        PopupMenu(wxXmlResource::Get()->LoadMenu(wxT("file_view_empty")));
    }
}

TreeItemInfo FileViewTree::GetSelectedItemInfo()
{
    wxTreeItemId item = GetSingleSelection();
    TreeItemInfo info;
    info.m_item = item;
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));

        // set the text of the item
        info.m_text = GetItemText(item);
        info.m_itemType = data->GetData().GetKind();
        info.m_fileName = data->GetData().GetFile();
        if(info.m_itemType == ProjectItem::TypeVirtualDirectory) {
            // incase of virtual directories, set the file name to be the directory of
            // the project
            wxString path = GetItemPath(item);
            wxString project = path.BeforeFirst(wxT(':'));
            info.m_fileName = wxFileName(ManagerST::Get()->GetProjectCwd(project), wxEmptyString);
        }
    }
    return info;
}

void FileViewTree::OnMouseDblClick(wxMouseEvent& event)
{
    wxArrayTreeItemIds items;
    size_t num = GetMultiSelection(items);
    if(num <= 0) {
        event.Skip();
        return;
    }

    // Make sure the double click was done on an actual item
    int flags = wxTREE_HITTEST_ONITEMLABEL;
    for(size_t i = 0; i < num; i++) {
        if(HitTest(event.GetPosition(), flags) == items.Item(i)) {
            wxTreeItemId item = items.Item(i);
            DoItemActivated(item, event);
            return;
        }
    }
    event.Skip();
}

void FileViewTree::DoItemActivated(wxTreeItemId& item, wxEvent& event)
{
    //-----------------------------------------------------
    // Each tree items, keeps a private user data that
    // holds the key for searching the its corresponding
    // node in the m_tree data structure
    //-----------------------------------------------------
    if(item.IsOk() == false) return;
    FilewViewTreeItemData* itemData = static_cast<FilewViewTreeItemData*>(GetItemData(item));
    if(!itemData) {
        event.Skip();
        return;
    }

    // if file item was hit, open it
    if(itemData->GetData().GetKind() == ProjectItem::TypeFile) {

        wxString filename = itemData->GetData().GetFile();
        wxString project = itemData->GetData().Key().BeforeFirst(wxT(':'));

        // Convert the file name to be in absolute path
        wxFileName fn(filename);
        fn.MakeAbsolute(ManagerST::Get()->GetProjectCwd(project));

        // send event to the plugins to see if they want the file opening in another way
        wxString file_path = fn.GetFullPath();
        clCommandEvent activateEvent(wxEVT_TREE_ITEM_FILE_ACTIVATED);
        activateEvent.SetFileName(file_path);
        if(EventNotifier::Get()->ProcessEvent(activateEvent)) return;

        clMainFrame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), project, -1);

    } else if(itemData->GetData().GetKind() == ProjectItem::TypeProject) {
        // make it active
        DoSetProjectActive(item);
    } else {
        event.Skip();
    }
}

void FileViewTree::OnOpenInEditor(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    size_t num = GetMultiSelection(items);
    for(size_t i = 0; i < num; i++) {
        wxTreeItemId item = items.Item(i);
        FilewViewTreeItemData* itemData = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(itemData && itemData->GetData().GetKind() == ProjectItem::TypeFile) {
            wxString filename = itemData->GetData().GetFile();
            wxString project = itemData->GetData().Key().BeforeFirst(wxT(':'));

            // Convert the file name to an absolute path
            wxFileName fn(filename);
            fn.MakeAbsolute(ManagerST::Get()->GetProjectCwd(project));

            // DON'T ask the plugins if they want the file opening in another way, as happens from a double-click
            // Here we _know_ the user wants to open in CL
            clMainFrame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), project, -1);
        }
    }
}

void FileViewTree::OnExportMakefile(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName, errMsg;
        BuilderPtr builder = BuildManagerST::Get()->GetSelectedBuilder(); // use current builder
        projectName = GetItemText(item);
        if(!builder->Export(projectName, wxEmptyString, false, true, errMsg)) {
            wxMessageBox(errMsg, _("CodeLite"), wxICON_HAND);
            return;
        }
    }
}

void FileViewTree::OnRemoveProject(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeProject) {
            DoRemoveProject(data->GetData().GetDisplayName());
        }
    }
}

void FileViewTree::OnSortItem(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = GetSingleSelection();
    SortItem(item);
}

bool FileViewTree::AddFilesToVirtualFolder(const wxString& vdFullPath, wxArrayString& paths)
{
    wxArrayString actualAdded;
    ManagerST::Get()->AddFilesToProject(paths, vdFullPath, actualAdded);

    // locate the item
    wxTreeItemId item = ItemByFullPath(vdFullPath);
    if(item.IsOk()) {
        for(size_t i = 0; i < actualAdded.Count(); i++) {

            // Add the tree node
            wxFileName fnFileName(actualAdded.Item(i));
            wxString path(vdFullPath);
            path += wxT(":");
            path += fnFileName.GetFullName();
            ProjectItem projItem(path, fnFileName.GetFullName(), fnFileName.GetFullPath(), ProjectItem::TypeFile);

            wxTreeItemId hti = AppendItem(item,                      // parent
                                          projItem.GetDisplayName(), // display name
                                          GetIconIndex(projItem),    // item image index
                                          GetIconIndex(projItem),    // selected item image
                                          new FilewViewTreeItemData(projItem));
            wxUnusedVar(hti);
        }

        SortItem(item);
        Expand(item);
        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
        return true;
    }
    return false;
}

bool FileViewTree::AddFilesToVirtualFolderIntelligently(const wxString& vdFullPath, wxArrayString& paths)
{
    // Try to put .cpp files in a :src and .h files in a :include dir
    // This should only happen if :src and :include are terminal subdirs of vdFullPath itself, not distant cousins
    // Note: This function is only used atm to place a pair of cpp/h files, so I'm not checking 'paths'.
    // If you use it for anything else in the future...

    // Check first for vdFullPath being one of the subdirs:
    wxString basename, srcname, includename;
    if(!vdFullPath.EndsWith(wxT(":src"), &basename)) {
        vdFullPath.EndsWith(wxT(":include"), &basename);
    }
    if(basename.empty()) {
        basename = vdFullPath; // If not, assume that we were passed the parent folder
    }

    // Either way, basename should now contain the putative parent of src and include
    // Check by getting treeitemids for all 3
    wxTreeItemId parentitem = ItemByFullPath(basename);
    if(!parentitem.IsOk()) {
        return false;
    }
    wxTreeItemId srcitem = DoGetItemByText(parentitem, wxT("src"));
    wxTreeItemId includeitem = DoGetItemByText(parentitem, wxT("include"));
    if(!(srcitem.IsOk() && includeitem.IsOk())) {
        return false; // The alleged parent folder doesn't have a relevant matching pair of children
    }

    // We're winning. Now it's just a matter of putting the cpp file into :src, etc
    wxArrayString cppfiles, hfiles;
    for(int c = (int)paths.GetCount() - 1; c >= 0; --c) {
        wxString file = paths.Item(c);
        if(file.Right(4) == wxT(".cpp")) {
            cppfiles.Add(file);
            paths.RemoveAt(c);
        } else if((file.Right(2) == wxT(".h")) || (file.Right(4) == wxT(".hpp"))) {
            hfiles.Add(file);
            paths.RemoveAt(c);
        }
    }
    // Finally do the Adds
    AddFilesToVirtualFolder(basename + wxT(":src"), cppfiles);
    AddFilesToVirtualFolder(basename + wxT(":include"), hfiles);
    // There shouldn't have been any other files passed; but if there were, add them to the selected folder
    if(paths.GetCount()) {
        AddFilesToVirtualFolder(vdFullPath, paths);
    }

    return true;
}

bool FileViewTree::AddFilesToVirtualFolder(wxTreeItemId& item, wxArrayString& paths)
{
    if(item.IsOk() == false) return false;

    FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
    switch(data->GetData().GetKind()) {
    case ProjectItem::TypeVirtualDirectory:
        // OK
        break;
    default:
        return false;
    }

    wxArrayString actualAdded;
    wxString vdPath = GetItemPath(item);
    wxString project;
    project = vdPath.BeforeFirst(wxT(':'));
    ManagerST::Get()->AddFilesToProject(paths, vdPath, actualAdded);
    for(size_t i = 0; i < actualAdded.Count(); i++) {

        // Add the tree node
        wxFileName fnFileName(actualAdded.Item(i));
        wxString path(vdPath);
        path += wxT(":");
        path += fnFileName.GetFullName();
        ProjectItem projItem(path, fnFileName.GetFullName(), fnFileName.GetFullPath(), ProjectItem::TypeFile);

        wxTreeItemId hti = AppendItem(item,                      // parent
                                      projItem.GetDisplayName(), // display name
                                      GetIconIndex(projItem),    // item image index
                                      GetIconIndex(projItem),    // selected item image
                                      new FilewViewTreeItemData(projItem));
        wxUnusedVar(hti);
    }

    SortItem(item);
    Expand(item);
    SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
    return true;
}

void FileViewTree::OnAddExistingItem(wxCommandEvent& WXUNUSED(event))
{
    wxString start_path(wxEmptyString);
    wxTreeItemId item = GetSingleSelection();
    if(!item.IsOk()) {
        return;
    }

    const wxString ALL(
        wxT("All Files (*)|*|") wxT("C/C++ Source Files (*.c;*.cpp;*.cxx;*.cc)|*.c;*.cpp;*.cxx;*.cc|")
            wxT("C/C++ Header Files (*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc)|*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc"));

    wxString vdPath = GetItemPath(item);
    wxString project, vd;
    project = vdPath.BeforeFirst(wxT(':'));
    vd = vdPath.AfterFirst(wxT(':'));

    ProjectPtr proj = ManagerST::Get()->GetProject(project);
    start_path = proj->GetBestPathForVD(vd);

    wxArrayString paths;
    wxFileDialog dlg(
        this, _("Add Existing Item"), start_path, wxEmptyString, ALL, wxFD_MULTIPLE | wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if(dlg.ShowModal() == wxID_OK) {
        dlg.GetPaths(paths);

        if(paths.IsEmpty() == false) {
            // keep the last used path
            wxFileName fn(paths.Item(0));
            start_path = fn.GetPath();
        }
        AddFilesToVirtualFolder(item, paths);
    }
}

void FileViewTree::OnNewItem(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = GetSingleSelection();
    if(!item.IsOk()) {
        return;
    }

    wxString path = GetItemPath(item);

    // Project name
    wxString projName = path.BeforeFirst(wxT(':'));
    wxString vd = path.AfterFirst(wxT(':'));
    wxString projCwd;
    ProjectPtr project = ManagerST::Get()->GetProject(projName);
    if(project) {
        projCwd = project->GetBestPathForVD(vd);
    }

    NewItemDlg dlg(clMainFrame::Get(), projCwd);
    dlg.SetTitle(_("New Item"));
    if(dlg.ShowModal() == wxID_OK) {
        DoAddNewItem(item, dlg.GetFileName().GetFullPath(), path);
    }
}

void FileViewTree::OnSetActive(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = GetSingleSelection();
    DoSetProjectActive(item);
}

void FileViewTree::DoSetProjectActive(wxTreeItemId& item)
{
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeProject) {

            wxString curActiveProj = ManagerST::Get()->GetActiveProjectName();

            // find previous active project and remove its bold style
            wxTreeItemIdValue cookie;
            wxTreeItemId child = GetFirstChild(GetRootItem(), cookie);
            bool fromPlugin = false;
            int iconIndex = wxNOT_FOUND;
            while(child.IsOk()) {
                FilewViewTreeItemData* childData = static_cast<FilewViewTreeItemData*>(GetItemData(child));
                if(childData && childData->GetData().GetDisplayName() == curActiveProj) {

                    DoGetProjectIconIndex(childData->GetData().GetDisplayName(), iconIndex, fromPlugin);
                    SetItemBold(child, false);
                    SetItemImage(child, iconIndex);
                    SetItemImage(child, iconIndex, wxTreeItemIcon_Selected);
                    SetItemImage(child, iconIndex, wxTreeItemIcon_SelectedExpanded);
                    break;
                }
                child = GetNextChild(GetRootItem(), cookie);
            }

            ManagerST::Get()->SetActiveProject(data->GetData().GetDisplayName());
            SetItemBold(item);
            DoGetProjectIconIndex(data->GetData().GetDisplayName(), iconIndex, fromPlugin);
            SetItemImage(item, fromPlugin ? iconIndex : ACTIVE_PROJECT_IMG_IDX);
            SetItemImage(item, fromPlugin ? iconIndex : ACTIVE_PROJECT_IMG_IDX, wxTreeItemIcon_Selected);
            SetItemImage(item, fromPlugin ? iconIndex : ACTIVE_PROJECT_IMG_IDX, wxTreeItemIcon_SelectedExpanded);
        }
    }
}

void FileViewTree::OnRemoveVirtualFolder(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = GetSingleSelection();
    DoRemoveVirtualFolder(item);
}

void FileViewTree::OnRemoveItem(wxCommandEvent& WXUNUSED(event)) { DoRemoveItems(); }

void FileViewTree::DoRemoveItems()
{
    wxArrayTreeItemIds items;
    size_t num = GetMultiSelection(items);
    if(!num) {
        return;
    }

    bool ApplyToEachFileRemoval = false;
    bool ApplyToEachFileDeletion = false;
    bool AlsoDeleteFromDisc = false;

    // We need to get the item names and data first. Why? Because if they're projects and multiple,
    // removing the second one segs in GetItemText(item) or data->GetData() as 'item' is no longer valid.
    // Why doesn't this happen for files and VDs too? Good question :/
    wxArrayString namearray;
    wxArrayInt kindarray;
    std::vector<FilewViewTreeItemData*> itemdata;
    for(size_t i = 0; i < num; i++) {
        wxTreeItemId item = items.Item(i);
        if(item.IsOk()) {
            namearray.Add(GetItemText(item));
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            itemdata.push_back(data);
            // Store the kind too, as this would become an invalid value
            kindarray.Add(data->GetData().GetKind());
        }
    }

    wxArrayString filesRemoved;
    for(size_t i = 0; i < num; i++) {
        wxTreeItemId item = items.Item(i);
        if(!item.IsOk()) {
            continue;
        }

        wxString name = namearray.Item(i);
        FilewViewTreeItemData* data = itemdata.at(i);

        if(data) {
            switch(kindarray.Item(i)) {
            case ProjectItem::TypeFile: {
                int result = wxID_YES;
                if(ApplyToEachFileRemoval == false) {
                    wxString message;
                    message << _("Are you sure you want remove '") << name << wxT("' ?");
                    if((num > 1) && ((i + 1) < num)) {

                        // For multiple selections, use a YesToAll dialog
                        wxRichMessageDialog dlg(wxTheApp->GetTopWindow(),
                                                message,
                                                _("Confirm"),
                                                wxYES_NO | wxYES_DEFAULT | wxCANCEL | wxCENTER | wxICON_QUESTION);
                        dlg.ShowCheckBox(_("Remember my answer and apply it all files"), false);
                        result = dlg.ShowModal();
                        ApplyToEachFileRemoval = dlg.IsCheckBoxChecked();

                    } else {
                        result = wxMessageBox(message, _("Are you sure?"), wxYES_NO | wxICON_QUESTION, this);
                    }
                }
                if(result == wxID_CANCEL || (result == wxID_NO && ApplyToEachFileRemoval == true)) {
                    return; // Assume Cancel or No+ApplyToEachFileRemoval means for folders etc too, not just files
                }
                if(result == wxID_YES || result == wxYES) {
                    wxTreeItemId parent = GetItemParent(item);
                    if(parent.IsOk()) {
                        wxString path = GetItemPath(parent);

                        // Remove the file. Do not fire an event here, we will send a "bulk" event
                        // with a list of all files removed
                        wxString fullpathOfFileRemoved;
                        if(ManagerST::Get()
                               ->RemoveFile(data->GetData().GetFile(), path, fullpathOfFileRemoved, false)) {
                            filesRemoved.Add(fullpathOfFileRemoved);
                        }

                        wxString file_name(data->GetData().GetFile());
                        Delete(item);
                        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);

                        int DeleteThisItemFromDisc = false;
                        if(ApplyToEachFileDeletion == false) {
                            wxString message;
                            message << _("Do you also want to delete the file '") << name << _("' from disc?");
                            if((num > 1) && ((i + 1) < num)) {
                                // For multiple selections, use a YesToAll dialog
                                wxRichMessageDialog dlg(wxTheApp->GetTopWindow(),
                                                        message,
                                                        _("Confirm"),
                                                        wxYES_NO | wxYES_DEFAULT | wxCANCEL | wxCENTER |
                                                            wxICON_QUESTION);
                                dlg.ShowCheckBox(_("Remember my answer and apply it all files"), false);
                                DeleteThisItemFromDisc = dlg.ShowModal();
                                ApplyToEachFileDeletion = dlg.IsCheckBoxChecked();
                            } else {
                                DeleteThisItemFromDisc =
                                    wxMessageBox(message, _("Are you sure?"), wxYES_NO | wxICON_QUESTION, this);
                            }
                        }

                        if((DeleteThisItemFromDisc == wxID_YES || DeleteThisItemFromDisc == wxYES) ||
                           AlsoDeleteFromDisc) {
                            AlsoDeleteFromDisc = ApplyToEachFileDeletion; // If we're here, ApplyToAll means delete all

                            wxString message(_("An error occurred during file removal. Maybe it has been already "
                                               "deleted or you don't have the necessary permissions"));
                            if(wxDirExists(name)) {
                                if(!wxRmdir(name)) {
                                    wxMessageBox(message, _("Error"), wxOK | wxICON_ERROR, this);
                                }
                            } else {
                                if(wxFileName::FileExists(file_name) && !wxRemoveFile(file_name)) {
                                    wxMessageBox(message, _("Error"), wxOK | wxICON_ERROR, this);
                                }
                            }
                        }
                    }
                }
            } break;
            case ProjectItem::TypeVirtualDirectory:
                DoRemoveVirtualFolder(item);
                break;
            case ProjectItem::TypeProject:
                DoRemoveProject(name);
                break;
            default:
                break;
            }
        }
    }

    // Notify plugins if we actually removed files
    if(filesRemoved.IsEmpty() == false) {
        clCommandEvent evtFileRemoved(wxEVT_PROJ_FILE_REMOVED);
        evtFileRemoved.SetStrings(filesRemoved);
        evtFileRemoved.SetEventObject(this);
        EventNotifier::Get()->ProcessEvent(evtFileRemoved);
    }
}

void FileViewTree::DoRemoveVirtualFolder(wxTreeItemId& item)
{
    wxString name = GetItemText(item);
    wxString message(wxT("'") + name + wxT("'"));
    message << _(" and all its content will be removed from the project.");

    if(wxMessageBox(message, _("CodeLite"), wxYES_NO | wxICON_WARNING) == wxYES) {
        wxString path = GetItemPath(item);
        ManagerST::Get()->RemoveVirtualDirectory(path);

        DeleteChildren(item);
        Delete(item);
        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
    }
}

void FileViewTree::OnNewVirtualFolder(wxCommandEvent& WXUNUSED(event))
{
    static int count = 0;
    wxString defaultName(wxT("NewDirectory"));
    defaultName << count++;

    wxTreeItemId item = GetSingleSelection();
    NewVirtualFolderDlg dlg(clMainFrame::Get(), GetItemPath(item));
    if(dlg.ShowModal() == wxID_OK) {
        DoAddVirtualFolder(item, dlg.GetName());
        if(dlg.GetCreateOnDisk()) {
            // Create the path on the file system, but don't complain if it is already there
            wxFileName::Mkdir(dlg.GetDiskPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
    }
}

wxTreeItemId FileViewTree::DoAddVirtualFolder(wxTreeItemId& parent, const wxString& text)
{
    wxString path = GetItemPath(parent) + wxT(":") + text;

    // Virtual directory already exists?
    if(ManagerST::Get()->AddVirtualDirectory(path, true) == Manager::VD_EXISTS) return wxTreeItemId();

    wxTreeItemId item;
    ProjectItem itemData(path, text, wxEmptyString, ProjectItem::TypeVirtualDirectory);
    item = AppendItem(parent,                    // parent
                      itemData.GetDisplayName(), // display name
                      GetIconIndex(itemData),    // item image index
                      GetIconIndex(itemData),    // selected item image
                      new FilewViewTreeItemData(itemData));

    SortItem(parent);
    Expand(parent);
    SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
    return item;
}

wxString FileViewTree::GetItemPath(wxTreeItemId& item) const
{
    std::deque<wxString> queue;
    wxString text = GetItemText(item);
    queue.push_front(text);

    wxTreeItemId p = GetItemParent(item);
    while(p.IsOk() && p != GetRootItem()) {

        text = GetItemText(p);
        queue.push_front(text);
        p = GetItemParent(p);
    }

    wxString path;
    size_t count = queue.size();
    for(size_t i = 0; i < count; i++) {
        path += queue.front();
        path += wxT(":");
        queue.pop_front();
    }

    if(!queue.empty()) {
        path += queue.front();
    } else {
        path = path.BeforeLast(wxT(':'));
    }

    return path;
}

void FileViewTree::OnLocalPrefs(wxCommandEvent& event)
{
    if(!ManagerST::Get()->IsWorkspaceOpen()) {
        return; // Probably not possible, but...
    }

    wxXmlNode* lwsnode = LocalWorkspaceST::Get()->GetLocalWorkspaceOptionsNode();
    // Don't check lwsnode: it'll be NULL if there are currently no local workspace options

    // Start by getting the global settings
    OptionsConfigPtr higherOptions = EditorConfigST::Get()->GetOptions();

    // If we're setting workspace options, run the dialog and return
    if(event.GetId() == XRCID("local_workspace_prefs")) {
        EditorSettingsLocal dlg(higherOptions, lwsnode, pLevel_workspace, this);
        if(dlg.ShowModal() == wxID_OK && LocalWorkspaceST::Get()->SetWorkspaceOptions(dlg.GetLocalOpts())) {
            clMainFrame::Get()->GetMainBook()->ApplySettingsChanges();
            // Notify plugins that some settings have changed
            PostCmdEvent(wxEVT_EDITOR_SETTINGS_CHANGED);
        }
        return;
    }

    // Otherwise we're getting project prefs
    wxTreeItemId item = GetSingleSelection();
    if(!item.IsOk()) {
        return;
    }

    wxXmlNode* lpnode = LocalWorkspaceST::Get()->GetLocalProjectOptionsNode(GetItemText(item));
    // Don't check lpnode: it'll be NULL if there are currently no local project options
    // Merge any local workspace options with the global ones inside 'higherOptions'
    LocalOptionsConfig wsOC(higherOptions, lwsnode);

    EditorSettingsLocal dlg(higherOptions, lpnode, pLevel_project, this);
    if(dlg.ShowModal() == wxID_OK &&
       LocalWorkspaceST::Get()->SetProjectOptions(dlg.GetLocalOpts(), GetItemText(item))) {
        clMainFrame::Get()->GetMainBook()->ApplySettingsChanges();
        // Notify plugins that some settings have changed
        PostCmdEvent(wxEVT_EDITOR_SETTINGS_CHANGED);
    }
}

void FileViewTree::OnProjectProperties(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = GetSingleSelection();
    if(!item.IsOk()) {
        return;
    }
    wxString projectName(GetItemText(item));
    clMainFrame::Get()->GetWorkspaceTab()->OpenProjectSettings(projectName);
}

void FileViewTree::DoRemoveProject(const wxString& name)
{
    wxString message(_("You are about to remove project '"));
    message << name << wxT("' ");
    message << _(" from the workspace, click 'Yes' to proceed or 'No' to abort.");
    if(wxMessageBox(message, _("Confirm"), wxYES_NO) == wxYES) {
        ManagerST::Get()->RemoveProject(name, true);
        // SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED); -- sent by WorkspaceTab
    }
}

int FileViewTree::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
    // used for SortChildren, reroute to our sort routine
    FilewViewTreeItemData* a = (FilewViewTreeItemData*)GetItemData(item1),
                           * b = (FilewViewTreeItemData*)GetItemData(item2);
    if(a && b) return OnCompareItems(a, b);

    return 0;
}

int FileViewTree::OnCompareItems(const FilewViewTreeItemData* a, const FilewViewTreeItemData* b)
{
    // if dir and other is not, dir has preference
    if(a->GetData().GetKind() == ProjectItem::TypeVirtualDirectory && b->GetData().GetKind() == ProjectItem::TypeFile)
        return -1;
    else if(b->GetData().GetKind() == ProjectItem::TypeVirtualDirectory &&
            a->GetData().GetKind() == ProjectItem::TypeFile)
        return 1;

    // else let ascii fight it out
    return a->GetData().GetDisplayName().CmpNoCase(b->GetData().GetDisplayName());
}

void FileViewTree::OnSaveAsTemplate(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString name = GetItemText(item);
        ProjectPtr proj = ManagerST::Get()->GetProject(name);
        if(proj) {
            NameAndDescDlg dlg(clMainFrame::Get(), PluginManager::Get(), name);
            if(dlg.ShowModal() == wxID_OK) {
                wxString newName = dlg.GetName();
                wxString desc = dlg.GetDescription();
                wxString type = dlg.GetType();

                newName = newName.Trim().Trim(false);
                desc = desc.Trim().Trim(false);

                if(newName.IsEmpty() == false) {
                    wxString tmplateDir =
                        ManagerST::Get()->GetStartupDirectory() + wxT("/templates/projects/") + newName + wxT("/");
                    Mkdir(tmplateDir);

                    Project newProj(*proj);
                    newProj.SetProjectInternalType(type);
                    newProj.CopyTo(tmplateDir, newName, desc);
                }
            }
        }
    }
}

void FileViewTree::OnBuildOrder(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        DependenciesDlg dlg(clMainFrame::Get(), GetItemText(item));
        dlg.ShowModal();
    }
}

void FileViewTree::OnClean(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName = GetItemText(item);
        wxString conf;

        // get the selected configuration to be built
        BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) {
            conf = bldConf->GetName();
        }
        QueueCommand buildInfo(projectName, conf, false, QueueCommand::kClean);

        if(bldConf && bldConf->IsCustomBuild()) {
            buildInfo.SetKind(QueueCommand::kCustomBuild);
            buildInfo.SetCustomBuildTarget(wxT("Clean"));
        }
        ManagerST::Get()->PushQueueCommand(buildInfo);
        ManagerST::Get()->ProcessCommandQueue();
    }
}

void FileViewTree::OnBuild(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName = GetItemText(item);

        wxString conf;
        // get the selected configuration to be built
        BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) {
            conf = bldConf->GetName();
        }

        QueueCommand buildInfo(projectName, conf, false, QueueCommand::kBuild);
        if(bldConf && bldConf->IsCustomBuild()) {
            buildInfo.SetKind(QueueCommand::kCustomBuild);
            buildInfo.SetCustomBuildTarget(wxT("Build"));
        }
        ManagerST::Get()->PushQueueCommand(buildInfo);
        ManagerST::Get()->ProcessCommandQueue();
    }
}

void FileViewTree::OnCompileItem(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeFile) {
            Manager* mgr = ManagerST::Get();
            wxTreeItemId parent = GetItemParent(item);
            if(parent.IsOk()) {
                wxString logmsg;
                wxString path = GetItemPath(parent);
                wxString proj = path.BeforeFirst(wxT(':'));
                logmsg << _("Compiling file: ") << data->GetData().GetFile() << _(" of project ") << proj << wxT("\n");
                mgr->CompileFile(proj, data->GetData().GetFile());
            }
        }
    }
}

void FileViewTree::OnPreprocessItem(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeFile) {
            Manager* mgr = ManagerST::Get();
            wxTreeItemId parent = GetItemParent(item);
            if(parent.IsOk()) {
                wxString logmsg;
                wxString path = GetItemPath(parent);
                wxString proj = path.BeforeFirst(wxT(':'));
                logmsg << _("Preprocessing file: ") << data->GetData().GetFile() << _(" of project ") << proj
                       << wxT("\n");
                mgr->CompileFile(proj, data->GetData().GetFile(), true);
            }
        }
    }
}

void FileViewTree::OnStopBuild(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ManagerST::Get()->StopBuild();
}

void FileViewTree::OnItemActivated(wxTreeEvent& event)
{
    if(event.GetKeyCode() == WXK_RETURN) {
        wxArrayTreeItemIds items;
        size_t num = GetMultiSelection(items);
        if(num > 0) {
            for(size_t i = 0; i < num; i++) {
                wxTreeItemId item = items.Item(i);
                DoItemActivated(item, event);
            }
        }
    } else if(event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE) {
        DoRemoveItems();
    } else {
        event.Skip();
    }
}

size_t FileViewTree::GetMultiSelection(wxArrayTreeItemIds& arr)
{
    if(HasFlag(wxTR_MULTIPLE)) {
        // we are using multiple selection tree
        return GetSelections(arr);
    } else {
        wxTreeItemId sel = GetSelection();
        if(sel.IsOk()) {
            arr.Add(sel);
            return 1;
        }
        arr.Clear();
        return 0;
    }
}

void FileViewTree::OnRetagProject(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName = GetItemText(item);
        ManagerST::Get()->RetagProject(projectName, true);
    }
}

void FileViewTree::OnRetagWorkspace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ManagerST::Get()->RetagWorkspace(TagsManager::Retag_Quick);
}

void FileViewTree::OnItemBeginDrag(wxTreeEvent& event)
{
    wxArrayTreeItemIds selections;
    size_t num = GetMultiSelection(selections);

    m_draggedItems.Clear();
    for(size_t n = 0; n < num; ++n) {
        wxTreeItemId item = selections[n];
        if(item.IsOk() && item != GetRootItem()) {
            // If it's a file, add it to the array
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(data && data->GetData().GetKind() == ProjectItem::TypeFile) {
                m_draggedItems.Add(item);
            }
        }
    }

    // Allow the event only if there were any valid selections
    if(m_draggedItems.GetCount() > 0) {
        event.Allow();
    }
}

void FileViewTree::OnItemEndDrag(wxTreeEvent& event)
{
    wxTreeItemId itemDst = event.GetItem();
    if(!itemDst.IsOk()) {
        return;
    }

    wxString targetVD, fromVD;
    while(true) {
        if(!itemDst.IsOk()) {
            return;
        }
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(itemDst));
        if(data && data->GetData().GetKind() == ProjectItem::TypeVirtualDirectory) {
            break; // Found a vd, so break out of the while loop
        }
        // We're only allowed to drag items between virtual folders, so find the parent folder
        itemDst = GetItemParent(itemDst);
    }

    wxTreeItemId target = itemDst;
    if(target.IsOk()) {
        targetVD = GetItemPath(target);
    } else {
        return;
    }

    for(size_t n = 0; n < m_draggedItems.GetCount(); ++n) {
        wxTreeItemId itemSrc = m_draggedItems.Item(n);
        wxTreeItemId fromItem = GetItemParent(itemSrc);
        if(fromItem.IsOk()) {
            fromVD = GetItemPath(fromItem);
        } else {
            continue;
        }

        if(fromVD == targetVD) {
            // Not much point dropping onto the same virtual dir
            continue;
        }

        // the file name to remove
        FilewViewTreeItemData* srcData = static_cast<FilewViewTreeItemData*>(GetItemData(itemSrc));

        // no tree-item-data? skip this one
        if(!srcData) continue;

        wxString filename = srcData->GetData().GetFile();

        ProjectItem itemData = srcData->GetData();

        // call the manager to remove them in the underlying project
        if(ManagerST::Get()->MoveFileToVD(filename, fromVD, targetVD)) {
            // remove the item from its current node, and place it under the
            // new parent node
            AppendItem(target,                    // parent
                       itemData.GetDisplayName(), // display name
                       GetIconIndex(itemData),    // item image index
                       GetIconIndex(itemData),    // selected item image
                       new FilewViewTreeItemData(itemData));
            Delete(itemSrc);
            Expand(target);
            SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
        }
    }
}

void FileViewTree::OnBuildProjectOnly(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();

    if(item.IsOk()) {
        wxString projectName = GetItemText(item);
        wxCommandEvent e(wxEVT_CMD_BUILD_PROJECT_ONLY);
        e.SetString(projectName);
        EventNotifier::Get()->AddPendingEvent(e);
    }
}

void FileViewTree::OnCleanProjectOnly(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();

    if(item.IsOk()) {
        wxString projectName = GetItemText(item);
        wxCommandEvent e(wxEVT_CMD_CLEAN_PROJECT_ONLY);
        e.SetString(projectName);
        EventNotifier::Get()->AddPendingEvent(e);
    }
}

void FileViewTree::ExpandToPath(const wxString& project, const wxFileName& fileName)
{
    wxTreeItemId root = GetRootItem();
    if(!root.IsOk()) return;

    CL_DEBUG1(" ===> [workspace] Expand to path for " + project + "::" + fileName.GetFullPath());

    wxTreeItemIdValue cookie;
    for(wxTreeItemId child = GetFirstChild(root, cookie); child.IsOk(); child = GetNextChild(root, cookie)) {
        FilewViewTreeItemData* childData = static_cast<FilewViewTreeItemData*>(GetItemData(child));
        if(childData->GetData().GetDisplayName() == project) {
            wxTreeItemId fileItem =
                fileName.GetName().IsEmpty() ?
                    child :
                    FindItemByPath(child, ManagerST::Get()->GetProjectCwd(project), fileName.GetFullPath());
            if(fileItem.IsOk()) {
                // Now we're using a wxTR_MULTIPLE tree, we need to unselect here, otherwise all project files get
                // selected
                // And,no, SelectItem(fileItem, false) isn't the answer: in 2.8 it toggles (a wx bug) and the 'selected'
                // tab ends up unselected
                if(HasFlag(wxTR_MULTIPLE)) {
                    UnselectAll();
                }

                SelectItem(fileItem);

                if(IsVisible(fileItem) == false) {
                    EnsureVisible(fileItem);
                }
            } else {
                wxString message;
                message << _("Failed to find file: ") << fileName.GetFullPath() << _(" in FileView.");
                wxLogMessage(message);
            }
            break;
        }
    }
    CL_DEBUG1(" <=== [workspace] Expand to path for " + project + "::" + fileName.GetFullPath());
}

wxTreeItemId FileViewTree::FindItemByPath(wxTreeItemId& parent, const wxString& projectPath, const wxString& fileName)
{
    if(!parent.IsOk()) return wxTreeItemId();

    if(!ItemHasChildren(parent)) return wxTreeItemId();

    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetFirstChild(parent, cookie);
    while(child.IsOk()) {
        FilewViewTreeItemData* childData = static_cast<FilewViewTreeItemData*>(GetItemData(child));
        wxFileName fn(childData->GetData().GetFile());
        fn.MakeAbsolute(projectPath);
        if(fn.GetFullPath().CmpNoCase(fileName) == 0) {
            return child;
        }

        if(ItemHasChildren(child)) {
            wxTreeItemId res = FindItemByPath(child, projectPath, fileName);
            if(res.IsOk()) {
                return res;
            }
        }
        child = GetNextChild(parent, cookie);
    }
    return wxTreeItemId();
}

wxTreeItemId FileViewTree::ItemByFullPath(const wxString& fullPath)
{
    if(!ItemHasChildren(GetRootItem())) return wxTreeItemId();

    wxTreeItemId parent = GetRootItem();
    wxArrayString texts = wxStringTokenize(fullPath, wxT(":"), wxTOKEN_STRTOK);
    for(size_t i = 0; i < texts.GetCount(); i++) {
        parent = DoGetItemByText(parent, texts.Item(i));
        if(parent.IsOk() == false) {
            return wxTreeItemId();
        }
    }
    return parent;
}

void FileViewTree::OnImportDirectory(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    if(!item.IsOk()) {
        return;
    }

    wxString vdPath = GetItemPath(item);
    wxString project;
    project = vdPath.BeforeFirst(wxT(':'));
    ProjectPtr proj = ManagerST::Get()->GetProject(project);

    bool extlessFiles(false);
    wxStringBoolMap_t dirs;
    wxArrayString files;
    wxArrayString all_files;
    wxString filespec;

    ImportFilesDialogNew dlg(clMainFrame::Get());
    if(dlg.ShowModal() != wxID_OK) return;

    extlessFiles = dlg.ExtlessFiles();
    dlg.GetDirectories(dirs);
    filespec = dlg.GetFileMask();

    // get list of all files based on the checked directories
    wxStringBoolMap_t::const_iterator iter = dirs.begin();
    for(; iter != dirs.end(); ++iter) {
        int flags = iter->second ? (wxDIR_FILES | wxDIR_DIRS) : (wxDIR_FILES);
        wxDir::GetAllFiles(iter->first, &all_files, "", flags);
    }

    DoImportFolder(proj, dlg.GetBaseDir(), all_files, filespec, extlessFiles);
}

void FileViewTree::DoImportFolder(ProjectPtr proj,
                                  const wxString& baseDir,
                                  const wxArrayString& all_files,
                                  const wxString& filespec,
                                  bool extlessFiles)
{
    wxStringTokenizer tok(filespec, wxT(";"));
    wxStringSet_t specMap;
    while(tok.HasMoreTokens()) {
        wxString v = tok.GetNextToken();
        // Cater for *.*, and also for idiots asking for *.foo;*.*
        if(v == wxT("*.*")) {
            // Remove any previous entries, and stop looking for more: an empty specMap signals *.*
            specMap.clear();
            break;
        }
        v = v.AfterLast(wxT('*'));
        v = v.AfterLast(wxT('.')).MakeLower();
        specMap.insert(v);
    }

    // filter non interesting files
    wxArrayString files;
    for(size_t i = 0; i < all_files.GetCount(); i++) {
        wxFileName fn(all_files.Item(i));

        /* always excluded by default */
        const wxArrayString& dirs = fn.GetDirs();
        bool cont = true;
        for(size_t j = 0; j < dirs.GetCount() && cont; j++) {
            wxString filepath = fn.GetPath();
            if(dirs.Item(j) == wxT(".svn") || dirs.Item(j) == wxT(".cvs") || dirs.Item(j) == wxT(".arch-ids") ||
               dirs.Item(j) == wxT("arch-inventory") || dirs.Item(j) == wxT("autom4te.cache") ||
               dirs.Item(j) == wxT("BitKeeper") || dirs.Item(j) == wxT(".bzr") || dirs.Item(j) == wxT(".bzrignore") ||
               dirs.Item(j) == wxT("CVS") || dirs.Item(j) == wxT(".cvsignore") || dirs.Item(j) == wxT("_darcs") ||
               dirs.Item(j) == wxT(".deps") || dirs.Item(j) == wxT("EIFGEN") || dirs.Item(j) == wxT(".git") ||
               dirs.Item(j) == wxT(".hg") || dirs.Item(j) == wxT("PENDING") || dirs.Item(j) == wxT("RCS") ||
               dirs.Item(j) == wxT("RESYNC") || dirs.Item(j) == wxT("SCCS") || dirs.Item(j) == wxT("{arch}")) {
                cont = false;
                break;
            }
        }

        // skip the directory?
        if(!cont) continue;

        if(specMap.empty()) {
            files.Add(all_files.Item(i));

        } else if(fn.GetExt().IsEmpty() & extlessFiles) {
            files.Add(all_files.Item(i));

        } else if(specMap.find(fn.GetExt().MakeLower()) != specMap.end()) {
            files.Add(all_files.Item(i));
        }
    }

    wxString path = baseDir;
    //{ Fixe bug 2847625
    if(path.EndsWith(wxT("/")) || path.EndsWith(wxT("\\"))) {
        path.RemoveLast();
    } //} Fixe bug 2847625

    wxFileName rootPath(path);

    // loop over the files and construct for each file a record with
    // the following information:
    // -virtual directory (full path, starting from project level)
    // -display name
    // -full path of the file
    proj->BeginTranscation();
    {
        // Create a progress dialog
        clProgressDlg* prgDlg = new clProgressDlg(NULL, _("Importing files ..."), wxT(""), (int)files.GetCount());

        // get list of files
        std::vector<wxFileName> vExistingFiles;
        wxArrayString existingFiles;

        proj->GetFiles(vExistingFiles, true);
        for(size_t i = 0; i < vExistingFiles.size(); i++) {
            existingFiles.Add(vExistingFiles.at(i).GetFullPath());
        }

        for(size_t i = 0; i < files.GetCount(); i++) {
            wxFileName fn(files.Item(i));

            // if the file already exist, skip it
            if(existingFiles.Index(fn.GetFullPath()) != wxNOT_FOUND) {
                continue;
            }

            FileViewItem fvitem;
            fvitem.fullpath = fn.GetFullPath();
            fvitem.displayName = fn.GetFullName();

            fn.MakeRelativeTo(path);

            wxString relativePath = fn.GetPath();
            relativePath.Replace(wxT("/"), wxT(":"));
            relativePath.Replace(wxT("\\"), wxT(":"));

            if(relativePath.IsEmpty()) {
                // the file is probably under the root, add it under
                // a virtual directory with the name of the target
                // root folder
                relativePath = rootPath.GetName();
            }
            relativePath.Append(wxT(":"));

            fvitem.virtualDir = relativePath;
            DoAddItem(proj, fvitem);

            wxString msg;
            msg << _("Adding file: ") << fn.GetFullPath();
            prgDlg->Update((int)i, msg);
        }
        m_itemsToSort.clear();
        prgDlg->Destroy();
    }

    // save the project file to disk
    proj->CommitTranscation();

    // reload the project
    wxString curr_proj_name(proj->GetName());
    bool was_active(ManagerST::Get()->GetActiveProjectName() == curr_proj_name);
    ManagerST::Get()->RemoveProject(proj->GetName(), false); // Don't notify about this action
    ManagerST::Get()->AddProject(proj->GetFileName().GetFullPath());

    // restore the active project
    if(was_active) {
        MarkActive(curr_proj_name);
    }
}
void FileViewTree::OnReconcileProject(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxString projectName;

    // Allow the selected project to be reconciled, even if it's inactive
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* fvid = dynamic_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(fvid && fvid->GetData().GetKind() == ProjectItem::TypeProject) {
            ManagerST::Get()->ReconcileProject(fvid->GetData().GetDisplayName());
        }
    }
}

void FileViewTree::RedefineProjFiles(ProjectPtr proj, const wxString& path, std::vector<wxString>& files)
{
    wxFileName rootPath(path);

    // loop over the files and construct for each file a record with
    // the following information:
    // -virtual directory (full path, starting from project level)
    // -display name
    // -full path of the file
    proj->BeginTranscation();
    {
        // Create a progress dialog
        clProgressDlg* prgDlg = new clProgressDlg(NULL, _("Importing files ..."), wxT(""), (int)files.size());

        proj->ClearAllVirtDirs();

        wxString relativePath;
        for(size_t i = 0; i < files.size(); i++) {
            wxFileName fn(files[i]);

            FileViewItem fvitem;
            fvitem.fullpath = fn.GetFullPath();
            fvitem.displayName = fn.GetFullName();

            fn.MakeRelativeTo(path);

            // anchor all files to a base folder
            relativePath = rootPath.GetName() + wxT(":") + fn.GetPath() + wxT(":");
            relativePath.Replace(wxT("/"), wxT(":"));
            relativePath.Replace(wxT("\\"), wxT(":"));

            fvitem.virtualDir = relativePath;
            DoAddItem(proj, fvitem);

            wxString msg;
            msg << _("Adding file: ") << fn.GetFullPath();
            prgDlg->Update((int)i, msg);
        }
        m_itemsToSort.clear();
        prgDlg->Destroy();
    }

    // save the project file to disk
    proj->CommitTranscation();

    // reload the project
    wxString curr_proj_name(proj->GetName());
    bool was_active(ManagerST::Get()->GetActiveProjectName() == curr_proj_name);
    ManagerST::Get()->RemoveProject(proj->GetName(), false);
    ManagerST::Get()->AddProject(proj->GetFileName().GetFullPath());

    // restore the active project
    if(was_active) {
        MarkActive(curr_proj_name);
    }
}

void FileViewTree::DoAddItem(ProjectPtr proj, const FileViewItem& item)
{
    if(!proj) {
        return;
    }

    // first add the virtual directory, if it already exist,
    // this function does nothing
    proj->CreateVirtualDir(item.virtualDir, true);

    // add the file.
    // For performance reasons, we dont go through the Workspace API
    // but directly through the project API
    proj->FastAddFile(item.fullpath, item.virtualDir);
}

void FileViewTree::OnRunPremakeStep(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName = GetItemText(item);
        ManagerST::Get()->RunCustomPreMakeCommand(projectName);
    }
}

void FileViewTree::OnRenameItem(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeFile) {
            wxTreeItemId parent = GetItemParent(item);
            if(parent.IsOk()) {

                wxString path = GetItemPath(parent);
                wxString proj = path.BeforeFirst(wxT(':'));

                ProjectPtr p = ManagerST::Get()->GetProject(proj);
                if(p) {
                    // prompt user for new name
                    wxString newName = wxGetTextFromUser(_("New file name:"), _("Rename file:"), GetItemText(item));
                    if(newName.IsEmpty() == false) {

                        wxFileName tmp(data->GetData().GetFile());
                        tmp.SetFullName(newName);

                        if(tmp.FileExists()) {
                            wxMessageBox(
                                _("A File with that name already exists!"), _("CodeLite"), wxICON_WARNING | wxOK);
                            return;
                        }

                        // rename the file (this will erase it from the symbol database and will
                        // also close the editor that it is currently opened in (if any))
                        if(ManagerST::Get()->RenameFile(data->GetData().GetFile(), tmp.GetFullPath(), path)) {
                            // update the item's info
                            data->SetDisplayName(tmp.GetFullName());
                            data->SetFile(tmp.GetFullPath());

                            // rename the tree item
                            SetItemText(item, tmp.GetFullName());

                            SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
                        }
                    }
                } // p
            }     // parent.IsOk()
        }         // TypeFile
    }             // item.IsOk()
}

void FileViewTree::OnRenameVirtualFolder(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        // got the item, prompt user for new name
        wxString newName =
            wxGetTextFromUser(_("New virtual folder name:"), _("Rename virtual folder:"), GetItemText(item));
        if(newName.IsEmpty() || newName == GetItemText(item)) {
            // user clicked cancel
            return;
        }

        // locate the project
        wxString path = GetItemPath(item);
        wxString proj = path.BeforeFirst(wxT(':'));

        path = path.AfterFirst(wxT(':'));
        ProjectPtr p = ManagerST::Get()->GetProject(proj);
        if(!p) {
            wxLogMessage(_("failed to rename virtual folder: ") + path + _(", reason: could not locate project ") +
                         proj);
            return;
        }

        if(!p->RenameVirtualDirectory(path, newName)) {
            wxLogMessage(_("failed to rename virtual folder: ") + path);
            return;
        }
        SetItemText(item, newName);
        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
    }
}

void FileViewTree::OnReBuild(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName = GetItemText(item);

        wxString conf;
        // get the selected configuration to be built
        BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) {
            conf = bldConf->GetName();
        }

        // Custom build supports the 'Rebuild' target
        if(bldConf && bldConf->IsCustomBuild()) {
            QueueCommand buildInfo(projectName, conf, false, QueueCommand::kRebuild);
            if(bldConf && bldConf->IsCustomBuild()) {
                buildInfo.SetKind(QueueCommand::kCustomBuild);
                buildInfo.SetCustomBuildTarget(wxT("Rebuild"));
            }

            ManagerST::Get()->PushQueueCommand(buildInfo);
            ManagerST::Get()->ProcessCommandQueue();

        } else {
            clMainFrame::Get()->RebuildProject(projectName);
        }
    }
}

wxTreeItemId FileViewTree::DoGetItemByText(const wxTreeItemId& parent, const wxString& text)
{
    if(!parent.IsOk()) {
        return wxTreeItemId();
    }

    if(!ItemHasChildren(parent)) {
        return wxTreeItemId();
    }

    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetFirstChild(parent, cookie);
    while(child.IsOk()) {
        if(GetItemText(child) == text) {
            return child;
        }
        child = GetNextChild(parent, cookie);
    }
    return wxTreeItemId();
}

bool FileViewTree::CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName)
{
    // try to locate that VD first, if it exists, do nothing
    wxTreeItemId item = ItemByFullPath(wxString::Format(wxT("%s:%s"), parentPath.c_str(), vdName.c_str()));
    if(item.IsOk()) {
        return true;
    }

    wxString project = parentPath.BeforeFirst(wxT(':'));
    wxString parentVDs = parentPath.AfterFirst(wxT(':'));
    wxArrayString vds = ::wxStringTokenize(parentVDs, wxT(":"), wxTOKEN_STRTOK);

    wxTreeItemId curItem = ItemByFullPath(project);
    if(!curItem.IsOk())
        // Could not locate the project item...
        return false;

    wxString path = project;
    for(size_t i = 0; i < vds.GetCount(); i++) {
        path << wxT(":") << vds.Item(i);
        wxTreeItemId tmpItem = ItemByFullPath(path);
        if(!tmpItem.IsOk()) {
            curItem = DoAddVirtualFolder(curItem, vds.Item(i));
            if(curItem.IsOk() == false) {
                // failed to add virtual directory
                break;
            }
        } else {
            curItem = tmpItem;
        }
    }

    if(!curItem.IsOk()) return false;

    DoAddVirtualFolder(curItem, vdName);
    return true;
}

void FileViewTree::MarkActive(const wxString& projectName)
{
    // find previous active project and remove its bold style
    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetFirstChild(GetRootItem(), cookie);
    while(child.IsOk()) {
        FilewViewTreeItemData* childData = static_cast<FilewViewTreeItemData*>(GetItemData(child));
        if(childData->GetData().GetDisplayName() == projectName) {
            DoSetProjectActive(child);
            break;
        }
        child = GetNextChild(GetRootItem(), cookie);
    }
}

bool FileViewTree::CreateAndAddFile(const wxString& filename, const wxString& vdFullPath)
{
    wxTreeItemId item = ItemByFullPath(vdFullPath);
    return DoAddNewItem(item, filename, vdFullPath);
}

bool FileViewTree::DoAddNewItem(wxTreeItemId& item, const wxString& filename, const wxString& vdFullpath)
{
    if(item.IsOk() == false) {
        return false;
    }

    ManagerST::Get()->AddNewFileToProject(filename, vdFullpath);

    // Add the tree node
    wxFileName fnFileName(filename);
    wxString path(vdFullpath);

    path += wxT(":");
    path += fnFileName.GetFullName();
    ProjectItem projItem(path, fnFileName.GetFullName(), fnFileName.GetFullPath(), ProjectItem::TypeFile);

    wxTreeItemId hti = AppendItem(item,                      // parent
                                  projItem.GetDisplayName(), // display name
                                  GetIconIndex(projItem),    // item image index
                                  GetIconIndex(projItem),    // selected item image
                                  new FilewViewTreeItemData(projItem));
    wxUnusedVar(hti);
    SortItem(item);
    Expand(item);
    SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
    return true;
}

void FileViewTree::OnRebuildProjectOnly(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTreeItemId item = GetSingleSelection();
    if(item.IsOk()) {
        wxString projectName = GetItemText(item);

        wxString conf;
        // get the selected configuration to be built
        BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) {
            conf = bldConf->GetName();
        }

        QueueCommand info(projectName, conf, true, QueueCommand::kRebuild);
        if(bldConf && bldConf->IsCustomBuild()) {
            info.SetKind(QueueCommand::kCustomBuild);
            info.SetCustomBuildTarget(wxT("Rebuild"));
        }

        ManagerST::Get()->PushQueueCommand(info);
        ManagerST::Get()->ProcessCommandQueue();
    }
}

void FileViewTree::OnLocalWorkspaceSettings(wxCommandEvent& e)
{
    if(ManagerST::Get()->IsWorkspaceOpen()) {
        WorkspaceSettingsDlg dlg(clMainFrame::Get(), LocalWorkspaceST::Get());
        if(dlg.ShowModal() == wxID_OK) {
            clMainFrame::Get()->SelectBestEnvSet();
            // Update the new paths
            ManagerST::Get()->UpdateParserPaths(true);
        }
    }
}

void FileViewTree::OnRetagInProgressUI(wxUpdateUIEvent& event)
{
    event.Enable(!ManagerST::Get()->GetRetagInProgress());
}

void FileViewTree::OnOpenWithDefaultApplication(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    GetMultiSelection(items);

    wxMimeTypesManager* mgr = wxTheMimeTypesManager;
    for(size_t i = 0; i < items.GetCount(); i++) {
        wxTreeItemId item = items.Item(i);
        FilewViewTreeItemData* itemData = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(itemData && itemData->GetData().GetKind() == ProjectItem::TypeFile) {

            wxFileName fn(itemData->GetData().GetFile());
            wxFileType* type = mgr->GetFileTypeFromExtension(fn.GetExt());
            bool bFoundCommand = false;
            wxUnusedVar(bFoundCommand);

            if(type) {

                wxString cmd = type->GetOpenCommand(fn.GetFullPath());
                delete type;

                if(!cmd.IsEmpty()) {
                    bFoundCommand = true;
                    wxExecute(cmd);
                }
            }

#ifdef __WXGTK__
            if(!bFoundCommand && itemData && itemData->GetData().GetKind() == ProjectItem::TypeFile) {
                // All hell break loose, try xdg-open
                wxString cmd = wxString::Format(wxT("xdg-open \"%s\""), fn.GetFullPath().c_str());
                wxExecute(cmd);
            }
#endif
        }
    }
}

ProjectPtr FileViewTree::GetSelectedProject() const
{
    wxArrayTreeItemIds selections;
    size_t count = GetSelections(selections);
    if(count == 0) {
        return NULL;
    }

    wxString errMsg;
    for(size_t i = 0; i < count; i++) {
        FilewViewTreeItemData* itemData = dynamic_cast<FilewViewTreeItemData*>(GetItemData(selections.Item(i)));
        if(itemData && itemData->GetData().GetKind() == ProjectItem::TypeProject) {
            return WorkspaceST::Get()->FindProjectByName(GetItemText(selections.Item(i)), errMsg);
        }
    }

    // None of the selected items is not a project
    // return the project parent of the first item
    wxTreeItemId item = selections.Item(0);
    while(item.IsOk() && item != GetRootItem()) {
        FilewViewTreeItemData* itemData = dynamic_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(!itemData) {
            return NULL;
        } else if(itemData->GetData().GetKind() == ProjectItem::TypeProject) {
            return WorkspaceST::Get()->FindProjectByName(GetItemText(item), errMsg);
        }
        item = GetItemParent(item);
    }
    return NULL;
}

void FileViewTree::OnBuildTree(wxCommandEvent& e)
{
    e.Skip();
    BuildTree();
}

void FileViewTree::OnBuildProjectOnlyInternal(wxCommandEvent& e)
{
    e.Skip();
    wxString projectName = e.GetString();
    if(projectName.IsEmpty()) {
        projectName = ManagerST::Get()->GetActiveProjectName();
    }

    wxString conf;
    // get the selected configuration to be built
    BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
    if(bldConf) {
        conf = bldConf->GetName();
    }

    QueueCommand info(projectName, conf, true, QueueCommand::kBuild);
    if(bldConf && bldConf->IsCustomBuild()) {
        info.SetKind(QueueCommand::kCustomBuild);
        info.SetCustomBuildTarget(wxT("Build"));
    }
    ManagerST::Get()->PushQueueCommand(info);
    ManagerST::Get()->ProcessCommandQueue();
}

void FileViewTree::OnCleanProjectOnlyInternal(wxCommandEvent& e)
{
    e.Skip();
    wxString projectName = e.GetString();
    if(projectName.IsEmpty()) {
        projectName = ManagerST::Get()->GetActiveProjectName();
    }

    wxString conf;
    // get the selected configuration to be built
    BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
    if(bldConf) {
        conf = bldConf->GetName();
    }

    QueueCommand info(projectName, conf, true, QueueCommand::kClean);
    if(bldConf && bldConf->IsCustomBuild()) {
        info.SetKind(QueueCommand::kCustomBuild);
        info.SetCustomBuildTarget(wxT("Clean"));
    }

    ManagerST::Get()->PushQueueCommand(info);
    ManagerST::Get()->ProcessCommandQueue();
}

void FileViewTree::OnExcludeFromBuild(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxArrayTreeItemIds selections;
    size_t count = GetSelections(selections);
    for(size_t selectionIndex = 0; selectionIndex < count; selectionIndex++) {
        wxTreeItemId item = selections[selectionIndex];
        if(item.IsOk()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(data->GetData().GetKind() == ProjectItem::TypeFile) {
                Manager* mgr = ManagerST::Get();
                wxTreeItemId parent = GetItemParent(item);
                if(parent.IsOk()) {
                    wxString path = GetItemPath(parent);
                    wxString proj = path.BeforeFirst(wxT(':'));
                    ProjectPtr p = mgr->GetProject(proj);
                    if(p) {
                        wxString vdPath = path.AfterFirst(':');
                        wxString filename = data->GetData().GetFile();

                        BuildConfigPtr buildConf = WorkspaceST::Get()->GetProjBuildConf(proj, "");
                        if(!buildConf) {
                            return;
                        }

                        wxString current_build_config = buildConf->GetName();

                        wxArrayString configs = p->GetExcludeConfigForFile(filename, vdPath);

                        if(e.IsChecked()) {
                            configs.Add(current_build_config);
                            SetItemTextColour(item, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));

                        } else {
                            int where = configs.Index(current_build_config);
                            if(where != wxNOT_FOUND) {
                                configs.RemoveAt(where);
                            }

                            SetItemTextColour(item, DrawingUtils::GetOutputPaneFgColour());
                        }
                        p->SetExcludeConfigForFile(filename, vdPath, configs);
                    }
                }
            }
        }
    }
}

void FileViewTree::OnExcludeFromBuildUI(wxUpdateUIEvent& event)
{
    // by default enable it
    event.Check(IsFileExcludedFromBuild(GetSingleSelection()));
}

bool FileViewTree::IsFileExcludedFromBuild(const wxTreeItemId& item) const
{
    if(item.IsOk()) {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeFile) {
            Manager* mgr = ManagerST::Get();
            wxTreeItemId parent = GetItemParent(item);
            if(parent.IsOk()) {
                wxString path = GetItemPath(parent);
                wxString proj = path.BeforeFirst(wxT(':'));
                ProjectPtr p = mgr->GetProject(proj);
                if(p) {

                    BuildConfigPtr buildConf = WorkspaceST::Get()->GetProjBuildConf(proj, "");
                    if(!buildConf) {
                        return false;
                    }

                    wxString vdPath = path.AfterFirst(':');
                    wxString filename = data->GetData().GetFile();
                    wxArrayString configs = p->GetExcludeConfigForFile(filename, vdPath);

                    return configs.Index(buildConf->GetName()) != wxNOT_FOUND;
                }
            }
        }
    }
    return false;
}

void FileViewTree::OnSelectionChanged(wxTreeEvent& e)
{
    e.Skip();
    if(!e.GetItem().IsOk()) return;

    FilewViewTreeItemData* data = dynamic_cast<FilewViewTreeItemData*>(GetItemData(e.GetItem()));
    if(data && data->GetData().GetKind() == ProjectItem::TypeProject) {
        wxCommandEvent evtProjectSelected(wxEVT_PROJECT_TREEITEM_CLICKED);
        evtProjectSelected.SetString(data->GetData().GetDisplayName());
        EventNotifier::Get()->AddPendingEvent(evtProjectSelected);
    }
}

void FileViewTree::DoGetProjectIconIndex(const wxString& projectName, int& iconIndex, bool& fromPlugin)
{
    fromPlugin = false;
    clColourEvent event(wxEVT_WORKSPACE_VIEW_CUSTOMIZE_PROJECT);
    event.SetEventObject(this);
    // set the project name
    event.SetString(projectName);
    if(EventNotifier::Get()->ProcessEvent(event)) {
        iconIndex = event.GetInt();
        fromPlugin = true;

    } else {
        iconIndex = PROJECT_IMG_IDX;
    }
}

void FileViewTree::OnRenameProject(wxCommandEvent& event)
{
    CHECK_COND_RET(WorkspaceST::Get()->IsOpen());
    wxTreeItemId item = GetSingleSelection();
    CHECK_ITEM_RET(item);

    FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
    if(data->GetData().GetKind() == ProjectItem::TypeProject) {
        wxString newname = ::wxGetTextFromUser(_("Project new name:"), _("Rename project"));
        newname.Trim().Trim(false);
        CHECK_COND_RET(!newname.IsEmpty());
        if(data->GetData().GetDisplayName() == newname) return;

        // Calling 'RenameProject' will trigger a wxEVT_PROJ_RENAMED event
        WorkspaceST::Get()->RenameProject(data->GetData().GetDisplayName(), newname);

        // Update the display name
        SetItemText(item, newname);

        // Update the user data
        data->GetData().SetDisplayName(newname);
    }
}

void FileViewTree::OnFolderDropped(clCommandEvent& event)
{
    // User dragged a folder into our workspace
    const wxArrayString& folders = event.GetStrings();
    if(folders.size() != 1) {
        ::wxMessageBox(_("You can only drag one folder at a time"), "CodeLite", wxOK | wxCENTER | wxICON_ERROR);
        return;
    }

    bool reloadWorkspaceIsNeeded(false);
    const wxString& folder = folders.Item(0);
    wxFileName workspaceFileName(folder, "");
    wxString errMsg;
    if(!WorkspaceST::Get()->IsOpen()) {

        wxFileName fnWorkspace(folder, "");

        workspaceFileName.SetName(workspaceFileName.GetDirs().Last());
        workspaceFileName.SetExt("workspace");

        // Create an empty workspace
        if(!WorkspaceST::Get()->CreateWorkspace(fnWorkspace.GetDirs().Last(), folder, errMsg)) {
            ::wxMessageBox(_("Failed to create workspace:\n") + errMsg, "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
            return;
        }

        // Create an empty project with sensible defaults
        ProjectData pd;
        CompilerPtr cmp = BuildSettingsConfigST::Get()->GetDefaultCompiler(COMPILER_DEFAULT_FAMILY);
        if(cmp) {
            pd.m_cmpType = cmp->GetName();
        } else {
            pd.m_cmpType = "gnu g++"; // Default :/
        }

        pd.m_name = fnWorkspace.GetDirs().Last();
        pd.m_path = folder;

        // Set a default empty project
        pd.m_srcProject.Reset(new Project());

// Use sensible debugger defaults
#ifdef __WXMAC__
        pd.m_debuggerType = "LLDB Debugger";
#else
        pd.m_debuggerType = "GNU gdb debugger";
#endif
        ManagerST::Get()->CreateProject(pd);
        reloadWorkspaceIsNeeded = true;
    }

    // to which project should we import the folder?
    wxArrayString projects;
    WorkspaceST::Get()->GetProjectList(projects);
    if(projects.IsEmpty()) {
        ::wxMessageBox(
            _("Can't import files to workspace without projects"), "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
    
    wxString projectName;
    if(projects.GetCount() > 1) {
        int selection = projects.Index(WorkspaceST::Get()->GetActiveProjectName());
        projectName = ::wxGetSingleChoice(_("Select project:"), _("Import files to project"), projects, selection);
    } else {
        // single project, just add it
        projectName = projects.Item(0);
    }

    // user cancelled?
    if(projectName.IsEmpty()) return;
    ProjectPtr pProj = WorkspaceST::Get()->GetProject(projectName);
    CHECK_PTR_RET(pProj);

    wxArrayString all_files;
    wxDir::GetAllFiles(folder, &all_files, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);

    ImportFilesSettings ifs;
    DoImportFolder(pProj, folder, all_files, ifs.GetFileMask(), ifs.GetFlags() & IFS_INCLUDE_FILES_WO_EXT);

    if(reloadWorkspaceIsNeeded) {
        // Now that we have created a workspace + one project reload the workspace
        wxCommandEvent evtOpenworkspace(wxEVT_MENU, XRCID("switch_to_workspace"));
        evtOpenworkspace.SetString(workspaceFileName.GetFullPath());
        evtOpenworkspace.SetEventObject(clMainFrame::Get());
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
    }

    // And trigger a full reparse of the workspace
    wxCommandEvent evtOpenworkspace(wxEVT_MENU, XRCID("full_retag_workspace"));
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
}

void FileViewTree::FolderDropped(const wxArrayString& folders)
{
    clCommandEvent dummy;
    dummy.SetStrings(folders);
    OnFolderDropped(dummy);
}
