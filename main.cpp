#include <iostream>
#include <vector>
#include <map>
#include "main.h"
#include <wx/listctrl.h>



// wxWidgets "Hello World" Program
 
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
 
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};
class MyFrame : public wxFrame
{
public:
    MyFrame();
 
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnFileSelect(wxCommandEvent& ev);
    void OnFileUnSelect(wxCommandEvent& ev);
    void OnFileClicked(wxCommandEvent& ev);
    std::vector<long> getSelectedItems();
    bool isSelectedItem;
    wxListCtrl* listCtrl;

};
 
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
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello.../tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxPanel* panel = new wxPanel(this);

    listCtrl = new wxListCtrl(panel, ID_ARCHIVELIST, {10, 10}, {330, 200}, wxLC_REPORT|wxSIMPLE_BORDER);
    listCtrl->AppendColumn("Name", wxLIST_FORMAT_LEFT, 80);
     listCtrl->InsertItem(0, "First");
     listCtrl->InsertItem(1, "Second");
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxCommandEventHandler(MyFrame::OnFileSelect), nullptr, this);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxCommandEventHandler(MyFrame::OnFileUnSelect), nullptr, this);
    listCtrl->Connect(ID_ARCHIVELIST, wxEVT_LIST_ITEM_ACTIVATED, wxCommandEventHandler(MyFrame::OnFileClicked), nullptr, this);
    
      //listCtrl->SetItem(0, 2, "5359");
     // listCtrl->SetItem(0, 3, "This is the first item");

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
    // this item is selected - do whatever is needed with it
}
return items;
}
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}
void MyFrame::OnFileClicked(wxCommandEvent& ev){
    auto items = this->getSelectedItems();
    for(auto& item:items){
        std::cout<<item;
    }
}
void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
