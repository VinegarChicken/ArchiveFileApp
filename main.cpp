#include "main.h"
#include <filesystem>
#include "assets\file_16.xpm"
#include "assets\file_48.xpm"
#include "assets\folder_16.xpm"
#include "assets\folder_48.xpm"
#include <wx/display.h>
#include <io.h>

// wxWidgets "Open World" Program
    
// For compilers that support precompilation, includes "wx/wx.h".

std::string fileSizeString(long double fileSize){
    std::string fSizeType;
    std::string fsizeString;
    if(fileSize > 1000000000){
        fSizeType = "Mb";
        fsizeString = (std::to_string((size_t) fileSize / 1000000) + " ");
    }
    else{
        fSizeType = "Kb";
        fsizeString = (std::to_string((size_t) fileSize / 1000) + " ");
    }
    fsizeString.append(" " + fSizeType);
    return fsizeString;
}
enum
{
    ID_Open = 1,
    ID_ARCHIVELIST = 2
};
 
wxIMPLEMENT_APP(MyApp);
 
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show(true);
    return true;
}
 
MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Joel Zip File Extract", wxDefaultPosition, wxSize(1200, 800))
{
        wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_OPEN, "&Open.../tCtrl-O",
                     "Open a zip file.");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxPanel* panel = new wxPanel(this);
    textCtrl = new wxTextCtrl(panel, wxID_ANY, "Enter Path", {500, 0}, {200, 20});
    imageList.Add(wxBitmap (file48));
    imageList.Add(wxBitmap (folder48));
    imageListSmall.Add(wxBitmap(file16));
    imageListSmall.Add(wxBitmap(folder16));
    listCtrl = new wxListCtrl(panel, ID_ARCHIVELIST, {10, 30}, {1100, 700}, wxLC_REPORT|wxSIMPLE_BORDER|wxTE_PROCESS_ENTER);
    listCtrl->AppendColumn("Name", wxLIST_FORMAT_LEFT, 600);
    listCtrl->AppendColumn("Date Modified", wxLIST_FORMAT_LEFT, 200);
    listCtrl->AppendColumn("Type", wxLIST_FORMAT_LEFT, 200);
    listCtrl->AppendColumn("Size", wxLIST_FORMAT_LEFT, 100);
    listCtrl->SetImageList(&imageList, wxIMAGE_LIST_NORMAL);
    listCtrl->SetImageList(&imageListSmall, wxIMAGE_LIST_SMALL);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxCommandEventHandler(MyFrame::OnFileSelect), nullptr, this);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxCommandEventHandler(MyFrame::OnFileUnSelect), nullptr, this);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_LIST_ITEM_ACTIVATED, wxCommandEventHandler(MyFrame::OnFileClicked), nullptr, this);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler(MyFrame::OnFileRightClicked), nullptr, this);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_MOTION, wxMouseEventHandler(MyFrame::OnMouseMove), nullptr, this);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_TEXT_ENTER, wxCommandEventHandler(MyFrame::OnTextCtrlEnterPressed), nullptr, this);


    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar( menuBar );
 
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
 
    Bind(wxEVT_MENU, &MyFrame::OnOpenFileClicked, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::OnOpenFileClicked(wxCommandEvent& event)
{
     wxFileDialog
        openFileDialog(this, _("Open Zip file"), "", "",
                       "Zip files (*.zip)|*.zip", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if(openFileDialog.ShowModal() == wxID_CANCEL){
        return;
    }
    if(listCtrl->GetItemCount() != 0){
        listCtrl->DeleteAllItems();
        zipArr.clear();
    }
    zfa = (ZipFileArchive*) zfa_new(openFileDialog.GetPath().c_str()).val;
    CppResult zfaLoad = zfa_load(zfa);
    
    CppArray zipList = zfa_list_files_in_dir(zfa, "\\");
    CppArray zipListIndex = zfa_list_files_in_dir_index(zfa, "\\");

    ZipFileInfo* tmpArr = (ZipFileInfo*) zipList.ptr;
    

   
    for(int i=0; i<zipList.size;i++){
        ZipFileInfo info = tmpArr[i];
        std::string tmp = info.name;
        std::filesystem::path fname = tmp;
        if(info.isDir){
            listCtrl->InsertItem(i, fname.parent_path().filename().c_str());
            listCtrl->SetItem(i, 1, info.dateModified);
            listCtrl->SetItem(i, 2, "File Folder");
            listCtrl->SetItemImage(i, 1);
        }
        else{
            listCtrl->InsertItem(i, fname.filename().c_str());
            listCtrl->SetItem(i, 1, info.dateModified);
            listCtrl->SetItem(i, 2, fname.extension().u8string() + " File");
            listCtrl->SetItem(i, 3, fileSizeString(info.size));
            listCtrl->SetItemImage(i, 0);
        }
      //  info.name = tmp.c_str();
        zipArr.push_back(info);
       // std::cout<<tmp<<std::endl;
    }
    wxArrayString list;
    list.Add(wxString("Extract File..."));
    list.Add(wxString("Extract Current Directory..."));
    listBox = new wxListBox(listCtrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, list);
    listBox->Hide();
        listBox->Bind(wxEVT_LISTBOX_DCLICK, [&](wxCommandEvent& event) {
        long selectedItemListBox = listBox->GetSelection();
        long selectedItem = getSelectedItems()[0];
        ZipFileInfo fileInfo = zipArr[selectedItem];
        if(selectedItemListBox == 0){
                wxDirDialog dlg(listCtrl, "Choose a directory", "",
                wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
                if(dlg.ShowModal() != wxID_CANCEL){
                    zfa_extract(zfa, fileInfo.index, dlg.GetPath().c_str());
                }
            }
        if(selectedItemListBox == 1){
            wxDirDialog dlg(listCtrl, "Choose a directory", "",
                wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
                if(dlg.ShowModal() != wxID_CANCEL){
                    for(auto& currentFile: zipArr){
                        if (currentFile.isDir){
                            continue;
                        }
                        
                        zfa_extract(zfa, currentFile.index, dlg.GetPath().c_str());
                    }
                }
        }
        listBox->Hide();  
    });
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
void MyFrame::OnFileSelect(wxCommandEvent& ev){
    listBox->Hide();
}

void MyFrame::OnFileUnSelect(wxCommandEvent& ev){
    
}

std::vector<long> MyFrame::getSelectedItems(){
     long item = -1;
       std::vector<long> items;
for ( ;; )
{
    item = listCtrl->GetNextItem(item,
                                wxLIST_NEXT_ALL,
                                wxLIST_STATE_SELECTED);
    if ( item == -1 )
        break;
    items.push_back(item);
}
return items;
}


void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Open World example",
                 "About Open World", wxOK | wxICON_INFORMATION);
}
void MyFrame::OnTextCtrlEnterPressed(wxCommandEvent& ev){
    SetCurrentDirectory((const char*) textCtrl->GetLabelText().c_str());
}
void MyFrame::OnFileClicked(wxCommandEvent& ev){
    std::vector<long> selectedItem = getSelectedItems();
    if(selectedItem.size() != 0){
        SetCurrentDirectory(zipArr[selectedItem[0]].name);
    }
}


void MyFrame::SetCurrentDirectory(std::string path){
   // std::string selected = zipArr[path].name;
        std::replace(path.begin(), path.end(), '/', '\\');
   if(path.back() == '\\'){
       path.pop_back();
   }
   textCtrl->SetLabelText(path);
    CppArray zipList = zfa_list_files_in_dir(zfa, path.c_str());
    if(zipList.size != 0){
        listCtrl->DeleteAllItems();
        zipArr.clear();
        ZipFileInfo* tmpArr = (ZipFileInfo*) zipList.ptr;
        for(int i=0; i<zipList.size;i++){
        ZipFileInfo info = tmpArr[i];
        std::string tmp = info.name;
        std::filesystem::path fname = tmp;
        if(info.isDir){
            listCtrl->InsertItem(i, fname.parent_path().filename().c_str());
            listCtrl->SetItem(i, 1, info.dateModified);
            listCtrl->SetItem(i, 2, "File Folder");
            listCtrl->SetItemImage(i, 1);
        }
        else{
            listCtrl->InsertItem(i, fname.filename().c_str());
            listCtrl->SetItem(i, 1, info.dateModified);
            listCtrl->SetItem(i, 2, fname.extension().u8string() + " File");
            listCtrl->SetItem(i, 3, fileSizeString(info.size));
            listCtrl->SetItemImage(i, 0);
        }
      //  info.name = tmp.c_str();
        zipArr.push_back(info);
       // std::cout<<tmp<<std::endl;
       // std::cout<<tmp<<std::endl;
    }

    }
}
void MyFrame::OnFileRightClicked(wxListEvent& ev){
    long selectedItem = getSelectedItems()[0];
    if(!zipArr[selectedItem].isDir){
        listBox->Show();
        listBox->SetPosition(mousePos);
    }
    
       /*
       if(zfa_isdir_index(zfa, zipArr[selectedItem[0]].index)){
            for(auto& i:zipIndex){
                if (zfa_isdir_index(zfa, i)){
                    continue;
                }
                zfa_extract(zfa, i, "");
        }
        }
        else{
            zfa_extract(zfa, zipIndex[selectedItem[0]], "");
        }
    */
     

}

void MyFrame::OnMouseMove(wxMouseEvent& ev){
    mousePos.x = ev.GetX();
    mousePos.y = ev.GetY();
}