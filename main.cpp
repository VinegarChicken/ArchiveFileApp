#include <iostream>
#include <vector>
#include <map>

struct CppResult{
    bool isErr;
    void* val;
};

struct ArrayStuct{
    void** ptr;
    size_t size;
    size_t cap;
};

struct ZipFileArchive{

};

extern "C" CppResult zfa_new(const char*);
extern "C" CppResult zfa_extract(ZipFileArchive*, size_t, char**, bool);
extern "C" CppResult zfa_extract_all(ZipFileArchive*, char**);
extern "C" ArrayStuct zfa_listall(ZipFileArchive*);


int main(int args, char** argv){
    CppResult zfa_ = zfa_new(argv[1]);
    if(zfa_.isErr){
        std::cout<<(const char*) zfa_.val;
    }
   ZipFileArchive* zfa = (ZipFileArchive*) zfa_.val;
    char* path = "C:\\Users\\m\\Downloads\\ryujinxggfgggdg";
    //CppResult res = zfa_extract_all(zfa, &path);
    //std::cout<<(char*) res.val;
  /*
  
  */
 zfa_extract(zfa, 8932, &path, false);
  ArrayStuct list = zfa_listall(zfa);
  std::map<int, char*> ZfaMap;
  std::map<int, char*>::iterator itr;
    char** arr = (char**) list.ptr;
    for(int i=0;i<list.size;i++){
        ZfaMap.insert(std::pair<int, char*>(i, arr[i]));
    }
    for(itr=ZfaMap.begin();itr!=ZfaMap.end();itr++){
        std::cout<<itr->first<<" "<<itr->second<<std::endl;
    }
  
    
    return 0;
}

/*// wxWidgets "Hello World" Program
 
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
extern "C" void hello_world(); 
class MyFrame : public wxFrame
{
public:
    MyFrame();
 
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};
 
enum
{
    ID_Hello = 1
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
 
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}
 
void MyFrame::OnHello(wxCommandEvent& event)
{
    hello_world();
    wxLogMessage("Hello world from wxWidgets!");
}
*/