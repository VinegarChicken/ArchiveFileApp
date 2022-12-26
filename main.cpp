#include "main.h"
#include <filesystem>
#include "assets\file_16.xpm"
#include "assets\file_48.xpm"
#include "assets\folder_16.xpm"
#include "assets\folder_48.xpm"

// wxWidgets "Hello World" Program
    
// For compilers that support precompilation, includes "wx/wx.h".

 
enum
{
    ID_Hello = 1,
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
    zfa = (ZipFileArchive*) zfa_new("C:\\Users\\m\\Downloads\\wxWidgets-3.2.1.zip").val;
    CppResult zfaLoad = zfa_load(zfa);
    imageList.Add(wxBitmap (file48));
    imageList.Add(wxBitmap (folder48));
    imageListSmall.Add(wxBitmap(file16));
    imageListSmall.Add(wxBitmap(folder16));
   
    CppArray zipList = zfa_list_files_in_dir(zfa, "\\");
    CppArray zipListIndex = zfa_list_files_in_dir_index(zfa, "\\");
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello.../tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxPanel* panel = new wxPanel(this);
    ZipFileInfo* tmpArr = (ZipFileInfo*) zipList.ptr;
    listCtrl = new wxListCtrl(panel, ID_ARCHIVELIST, {10, 10}, {1100, 700}, wxLC_REPORT|wxSIMPLE_BORDER);
    listCtrl->AppendColumn("Name", wxLIST_FORMAT_LEFT, 600);
    listCtrl->AppendColumn("Date Modified", wxLIST_FORMAT_LEFT, 200);
    listCtrl->AppendColumn("Type", wxLIST_FORMAT_LEFT, 200);
    listCtrl->AppendColumn("Size", wxLIST_FORMAT_LEFT, 100);

    listCtrl->SetImageList(&imageList, wxIMAGE_LIST_NORMAL);
    listCtrl->SetImageList(&imageListSmall, wxIMAGE_LIST_SMALL);
    for(int i=0; i<zipList.size;i++){
        ZipFileInfo info = tmpArr[i];
        std::string tmp = info.name;
        std::filesystem::path fname = tmp;
        if(info.isDir){
            listCtrl->InsertItem(i, fname.parent_path().filename().c_str());
            listCtrl->SetItemImage(i, 1);
            
        }
        else{
            listCtrl->InsertItem(i, fname.filename().c_str());
            listCtrl->SetItemImage(i, 0);
        }
        std::replace(tmp.begin(), tmp.end(), '/', '\\');
        zipArr.push_back(info);
       // std::cout<<tmp<<std::endl;
    }
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxCommandEventHandler(MyFrame::OnFileSelect), nullptr, this);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxCommandEventHandler(MyFrame::OnFileUnSelect), nullptr, this);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_LIST_ITEM_ACTIVATED, wxCommandEventHandler(MyFrame::OnFileClicked), nullptr, this);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_LIST_ITEM_RIGHT_CLICK, wxCommandEventHandler(MyFrame::OnFileRightClicked), nullptr, this);
    

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar( menuBar );
 
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
 
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}
 
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
void MyFrame::OnFileSelect(wxCommandEvent& ev){
    if(isListboxActive){
        delete listBox;
        isListboxActive = false;
    } 
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
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}
void MyFrame::OnFileClicked(wxCommandEvent& ev){
    std::vector<long> selectedItem = getSelectedItems();
    
    if(selectedItem.size() != 0){
    std::string selected = zipArr[selectedItem[0]].name;
   if(selected.back() == '\\'){
       selected.pop_back();
   }
    //std::cout<<selected.c_str()<<std::endl;
    CppArray zipList = zfa_list_files_in_dir(zfa, selected.c_str());
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
            listCtrl->SetItemImage(i, 1);
            
        }
        else{
            listCtrl->InsertItem(i, fname.filename().c_str());
            listCtrl->SetItemImage(i, 0);
        }
        std::replace(tmp.begin(), tmp.end(), '/', '\\');
        zipArr.push_back(info);
       // std::cout<<tmp<<std::endl;
       currentDir = selected;
    }

    }
    }
    
}

void MyFrame::OnFileRightClicked(wxCommandEvent& event){
    std::vector<long> selectedItem = getSelectedItems();
    wxPoint mousePos = wxGetMousePosition();
    mousePos.x -= 150;
    mousePos.y -= 150;
    listBox = new wxListBox(listCtrl, wxID_ANY, mousePos);
    listBox->Bind(wxEVT_LEFT_UP, [&](wxMouseEvent& event) {
        isListboxActive = false;
        delete listBox;
      });
    isListboxActive = true;
     for (auto item : {"extract"}){
        listBox->Append(item);
      listBox->SetSelection(0);
    }
    zfa_extract(zfa, zipArr[selectedItem[0]].index, "");
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

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
