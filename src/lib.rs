#![feature(core_ffi_c)]
#![feature(vec_into_raw_parts)]
extern crate core;

use core::ffi::c_char;
use std::collections::HashMap;
use std::ffi::CStr;
use std::error::Error;
use std::ffi::CString;
use std::hash::Hash;
use std::iter::Zip;
use std::path::PathBuf;
use std::sync::Arc;
use zip::ZipArchive;
use zip::read::ZipFile;
use std::fs::File;
use std::io;
use std::io::{Cursor, Read};
use zip::result::ZipResult;

unsafe fn cstr_to_rust_str(cstr: *const c_char) -> String{
    CStr::from_ptr(cstr).to_str().unwrap().to_string()
}

unsafe fn rust_str_to_cstr(s: String) -> *const c_char{
    CString::new(s).unwrap().into_raw()
}

#[repr(C)]
#[derive(Clone, Copy)]
struct ZipFileInfo{
    name: *const c_char,
    size: u64,
    date_modified: *const c_char,
    index: usize,
    is_dir: bool
}

struct ZipFileArchive{
    zipfile: zip::ZipArchive<File>,
    zipmap: HashMap<String, Vec<ZipFileInfo>>,
    //zipmap_index: HashMap<String, Vec<usize>>
}



#[repr(C)]
struct CppResult{
    isErr: bool,
    val: *mut ()
}
#[repr(C)]
struct CppArray{
    ptr: *mut *mut (),
    size: usize,
    cap: usize
}

unsafe fn extract_file(file: &mut ZipFile, out_path: PathBuf) -> String{
    let mut data = Vec::new();
    file.read_to_end(&mut data);
    if let Err(e) = std::fs::write(out_path.join(file.mangled_name().file_name().unwrap()), data){
        return format!("Failed to write {} With error: {}", out_path.clone().to_str().unwrap(), e.to_string())
    }
    format!("Wrote {}", out_path.clone().to_str().unwrap())
}
impl ZipFileArchive{
    unsafe fn new(path: String) -> CppResult {
        let mut f = File::open(path.clone());

        if let Err(e) = f{
            return CppResult{
                isErr: true,
                val: CString::new(e.to_string()).unwrap().into_raw() as   *mut ()
            }
        }
        let mut f = f.unwrap();
        let mut zip = zip::ZipArchive::new(f);
        if let Ok(mut z) = zip {
            let zfa = ZipFileArchive{zipfile: z, zipmap: HashMap::new()};
            let x = Box::new(zfa);
            let ptr = Box::into_raw(x) as   *mut ();
            return CppResult{
                isErr: false,
                val: ptr as   *mut ()
            }
        }
        else{
            return CppResult{
                isErr: true,
                val: CString::new(zip.unwrap_err().to_string()).unwrap().into_raw() as   *mut ()
            }
        }
    }
    unsafe fn load(&mut self) -> CppResult{
        
        let mut zipf = &mut self.zipfile;

        let mut currentdir = String::new();
        let mut currentlist = Vec::new();
        let mut current_parent = String::new();
        
            for i in 0..zipf.len(){
                let mut file = zipf.by_index(i as usize).unwrap();
                let mut name = file.name().to_string();
                let name = rust_str_to_cstr(name);
                let last_modified = rust_str_to_cstr(file.last_modified().to_time().unwrap().to_string());
                let size = file.size();
                let mut file_parent = file.mangled_name().parent().unwrap().to_str().unwrap().to_string();
                if file_parent == ""{
                    file_parent = "\\".to_string();
                }
               // println!("{}", file_parent.clone());

                    if current_parent != file_parent{
                        if let Some(list) = self.zipmap.get(&file_parent){
                            currentlist = list.clone();
                        }
                        else{
                            currentlist.clear();
                        }
                        current_parent = file_parent;
                    }
                    currentlist.push(ZipFileInfo{
                        name: name,
                        size: size,
                        date_modified: last_modified,
                        index: i,
                        is_dir: file.is_dir()
                    });
                    self.zipmap.insert(current_parent.clone(), currentlist.clone());
                
    }
    return CppResult{
        isErr: false,
        val: CString::new("").unwrap().into_raw() as   *mut ()
    }
}
    fn extract(&mut self, index: usize, out_dir: *mut c_char) -> CppResult{ 
        unsafe{
            let mut zipf = &mut self.zipfile;
            let out_path = PathBuf::from(cstr_to_rust_str(out_dir).replace("/", "\\"));
            let mut file = zipf.by_index(index).unwrap();
           let res = extract_file(&mut file, out_path);

           return CppResult{
               isErr: false,
               val: CString::new(res).unwrap().into_raw() as *mut ()
           }
            
        }
    }
    fn extract_all(&mut self, out_dir: *mut c_char) -> CppResult{
        unsafe{
            let mut zipf = &mut self.zipfile;
            let dir_path = PathBuf::from(cstr_to_rust_str(out_dir));
            if let Err(e) = zipf.extract(dir_path){
                return CppResult { isErr: false, val: CString::new(e.to_string()).unwrap().into_raw() as   *mut ()}
            }
            CppResult { isErr: false, val: 0 as *mut () }
        }
    }
    fn is_directory(&mut self, dir: *const c_char) -> bool{
        unsafe{
            println!("{}", cstr_to_rust_str(dir));
            let mut zipf = &mut self.zipfile;
            let mut file = zipf.by_name(&cstr_to_rust_str(dir)).unwrap();
            file.is_dir() 
        }
    }
    fn is_directory_index(&mut self, index: usize) -> bool{
        unsafe{
            let mut zipf = &mut self.zipfile;
            let mut file = zipf.by_index(index).unwrap();
            file.is_dir() 
        }
    }
    fn list_files_in_dir(&mut self, dir: *const c_char) -> CppArray{
        unsafe{
            let zipf = &mut self.zipfile;
            let dir = cstr_to_rust_str(dir);
            println!("{}", dir);
          //  println!("{}", dir);
                       // let mut folder = folder.file_name().unwrap().to_str().unwrap();
            let mut file = zipf.by_name(&dir);
            let mut files = self.zipmap.get(&dir);
            let mut list = Vec::new();
            if let Some(files) = files{
                list = files.clone();
            }
            let mut parts = list.to_vec().into_raw_parts();
            CppArray { ptr: parts.0 as *mut *mut (), size: parts.1, cap: parts.2 }
        }
    
    }
    fn list_files_in_dir_index(&mut self, dir: *const c_char) -> CppArray{
        unsafe{
            let zipf = &mut self.zipfile;
            let dir = cstr_to_rust_str(dir);
            let mut files = self.zipmap.get(&dir);
            let mut list = Vec::new();
            if let Some(files) = files{
                list = files.clone();
            }
            let mut parts = list.to_vec().into_raw_parts();
            CppArray { ptr: parts.0 as *mut *mut (), size: parts.1, cap: parts.2 }
        }
    }
    fn list_all(&mut self) -> CppArray{
        let mut zipf = &mut self.zipfile;
        let mut list = Vec::new();
        for i in 0..zipf.len(){
            let mut file = zipf.by_index(i as usize).unwrap();
            let name = CString::new(file.name().to_string()).unwrap();
            println!("Name: {} Id: {}", file.name(), i);
            list.push(name.into_raw());
        }
        let mut parts = list.into_raw_parts();
        CppArray { ptr: parts.0 as *mut *mut (), size: parts.1, cap: parts.2 }
    }

}

#[no_mangle]
unsafe extern "C" fn zfa_new(path: *const c_char) -> CppResult{
    let map:HashMap<String, Vec<*const c_char>> = HashMap::new();
    ZipFileArchive::new(cstr_to_rust_str(path))
}

#[no_mangle]
unsafe extern "C" fn zfa_load(zfa: *mut ZipFileArchive) -> CppResult{
    (*zfa).load()
}

#[no_mangle]
unsafe extern "C" fn zfa_list_files_in_dir(zfa: *mut ZipFileArchive, index: *const c_char) -> CppArray{
    (*zfa).list_files_in_dir(index)
}

#[no_mangle]
unsafe extern "C" fn zfa_listall(zfa: *mut ZipFileArchive) -> CppArray{
    (*zfa).list_all()
}

#[no_mangle]
unsafe extern "C" fn zfa_extract(zfa: *mut ZipFileArchive, file: usize, out_dir: *mut c_char) -> CppResult{
    (*zfa).extract(file, out_dir)
}

#[no_mangle]
unsafe extern "C" fn zfa_extract_all(zfa: *mut ZipFileArchive, out_dir: *mut c_char) -> CppResult{
    (*zfa).extract_all(out_dir)
}

#[no_mangle]
unsafe extern "C" fn zfa_isdir(zfa: *mut ZipFileArchive, dir: *const c_char) -> bool{
    (*zfa).is_directory(dir)
}

#[no_mangle]
unsafe extern "C" fn zfa_isdir_index(zfa: *mut ZipFileArchive, dir: usize) -> bool{
    (*zfa).is_directory_index(dir)
}

#[no_mangle]
unsafe extern "C" fn zfa_list_files_in_dir_index(zfa: *mut ZipFileArchive, index: *const c_char) -> CppArray{
    (*zfa).list_files_in_dir_index(index)
}