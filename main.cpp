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
    : wxFrame(nullptr, wxID_ANY, "Hello World")
{
    zfa = (ZipFileArchive*) zfa_new("C:\\Users\\m\\Downloads\\wxWidgets-3.2.1.zip").val;
    CppResult zfaLoad = zfa_load(zfa);
   // CppArray listall = zfa_listall(zfa);
    //char** allarr = (char**) listall.ptr;
    //for(int i=0; i<listall.size;i++){
       // std::cout<<allarr[i]<<std::endl;
    //}
    imageList.Add(wxBitmap (file48));
    imageList.Add(wxBitmap (folder48));
    imageListSmall.Add(wxBitmap(file16));
    imageListSmall.Add(wxBitmap(folder16));
    CppArray zipList = zfa_list_files_in_dir(zfa, "\\");
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello.../tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxPanel* panel = new wxPanel(this);
    char** tmpArr = (char**) zipList.ptr;
    listCtrl = new wxListCtrl(panel, ID_ARCHIVELIST, {10, 10}, {330, 200}, wxLC_REPORT|wxSIMPLE_BORDER);
    listCtrl->AppendColumn("Name", wxLIST_FORMAT_LEFT, 80);
    listCtrl->SetImageList(&imageList, wxIMAGE_LIST_NORMAL);
    listCtrl->SetImageList(&imageListSmall, wxIMAGE_LIST_SMALL);
    for(int i=0; i<zipList.size;i++){
        std::string tmp = tmpArr[i];
        std::filesystem::path fname = tmp;
        if(fname.has_filename()){
            listCtrl->InsertItem(i, fname.filename().c_str());
            listCtrl->SetItemImage(i, 0);
        }
        else{
            listCtrl->InsertItem(i, fname.parent_path().filename().c_str());
            listCtrl->SetItemImage(i, 1);
        }
        std::replace(tmp.begin(), tmp.end(), '/', '\\');
        zipArr.push_back(tmp);
       // std::cout<<tmp<<std::endl;
    }
    
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxCommandEventHandler(MyFrame::OnFileSelect), nullptr, this);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxCommandEventHandler(MyFrame::OnFileUnSelect), nullptr, this);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_LIST_ITEM_ACTIVATED, wxCommandEventHandler(MyFrame::OnFileClicked), nullptr, this);
    

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
    std::string selected = zipArr[selectedItem[0]];
   if(selected.back() == '\\'){
       selected.pop_back();
   }
    std::cout<<selected.c_str()<<std::endl;
    CppArray zipList = zfa_list_files_in_dir(zfa, selected.c_str());
    if(zipList.size != 0){
        listCtrl->DeleteAllItems();
        zipArr.clear();
        char** tmpArr = (char**) zipList.ptr;
        for(int i=0; i<zipList.size;i++){
        std::string tmp = tmpArr[i];
        std::filesystem::path fname = tmp;
        if(fname.has_filename()){
            listCtrl->InsertItem(i, fname.filename().c_str());
            listCtrl->SetItemImage(i, 0);
        }
        else{
            listCtrl->InsertItem(i, fname.parent_path().filename().c_str());
            listCtrl->SetItemImage(i, 1);
        }
        std::replace(tmp.begin(), tmp.end(), '/', '\\');
        zipArr.push_back(tmp);
        }

    }
    else{
        //std::cout<<"Dir not selected";
    }
    }
    

}
void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
