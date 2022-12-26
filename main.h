#include <iostream>
#include <vector>
#include <map>
#include <wx/listctrl.h>
#include <wx/wxprec.h>
#include <wx/bitmap.h>

struct ZipFileArchive{

};

struct ZipFileInfo{
    const char* name;
    size_t size;
    const char* dateModified;
    size_t index;
    bool isDir;
};

struct CppResult{
    bool isErr;
    void* val;
};

struct CppArray{
    void** ptr;
    size_t size;
    size_t cap;
};
//WxWidgets dir examples: test/streams: 8836, test/benchmarks: 8861
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
    void OnFileRightClicked(wxCommandEvent& ev);
    std::vector<long> getSelectedItems();
    bool isSelectedItem;
    wxListCtrl* listCtrl;
    ZipFileArchive* zfa;
    std::vector<ZipFileInfo> zipArr;
    long fileId;
    std::string currentDir = "\\";
    wxImageList imageList {48, 48};
    wxImageList imageListSmall {16, 16};
    wxListBox* listBox;
    bool isListboxActive;
};

extern "C" CppResult zfa_new(const char*);
extern "C" CppResult zfa_load(ZipFileArchive*);
extern "C" CppResult zfa_extract(ZipFileArchive*, size_t, char*);
extern "C" CppResult zfa_extract_all(ZipFileArchive*, char*);
extern "C" CppArray zfa_listall(ZipFileArchive*);
extern "C" CppArray zfa_list_files_in_dir(ZipFileArchive*, const char*);
extern "C" CppArray zfa_list_files_in_dir_index(ZipFileArchive*, const char*);
extern "C" bool zfa_isdir(ZipFileArchive*, const char*);
extern "C" bool zfa_isdir_index(ZipFileArchive*, size_t);


/*
int main(int args, char** argv){
    CppResult zfa_ = zfa_new(argv[1]);
    if(zfa_.isErr){
        std::cout<<(const char*) zfa_.val;
    }
    ZipFileArchive* zfa = (ZipFileArchive*) zfa_.val;
    char* path = "C:\\Users\\m\\Downloads\\ryujinxggfgggdg";
    //CppResult res = zfa_extract_all(zfa, &path);
    //std::cout<<(char*) res.val;
  CppArray ListDir = zfa_list_files_in_dir(zfa, 8861);
  char** arr = (char**) ListDir.ptr;
  for(int i=0;i<ListDir.size;i++){
    std::cout<<arr[i]<<std::endl;
  }
 
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
*/