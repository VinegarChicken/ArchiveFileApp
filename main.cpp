#include "main.h"



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
    zfa = (ZipFileArchive*) zfa_new("C:\\Users\\m\\Downloads\\ryujinx-1.1.232-win_x64.zip").val;
    CppResult zfaLoad = zfa_load(zfa);
    CppArray zipList = zfa_list_files_in_dir(zfa, "publish");
    char** arr = (char**) zipList.ptr;
    for(int i=0; i<zipList.size;i++){
        std::cout<<arr[i];
        std::cout<<" ";
        std::cout<<i<<std::endl;
    }
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello.../tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxPanel* panel = new wxPanel(this);
   // zipArr = (char**) zipList.ptr;
    listCtrl = new wxListCtrl(panel, ID_ARCHIVELIST, {10, 10}, {330, 200}, wxLC_REPORT|wxSIMPLE_BORDER);
    listCtrl->AppendColumn("Name", wxLIST_FORMAT_LEFT, 80);
   // for(int i=0; i<zipList.size;i++){
  //     listCtrl->InsertItem(i, zipArr[i]);
        //std::cout<<arr[i];
        //std::cout<<" ";
        //std::cout<<i<<std::endl;
  //  }
    
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
    fileId = getSelectedItems()[0];
    std::cout<<fileId<<std::endl;
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
    /*
      bool isDir = zfa_isdir(zfa, fileId);

    if(isDir){
        std::cout<<"Dir selected";
        listCtrl->DeleteAllItems();
        CppArray zipList = zfa_list_files_in_dir(zfa, fileId);
        zipArr = (char**) zipList.ptr;
        for(int i=0; i<zipList.size;i++){
        listCtrl->InsertItem(i, zipArr[i]);
        //std::cout<<arr[i];
        //std::cout<<" ";
        //std::cout<<i<<std::endl;
    }
    }
    else{
        std::cout<<"Dir not selected";
    }
    */
  
}
void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
