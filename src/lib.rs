#![feature(core_ffi_c)]
#![feature(vec_into_raw_parts)]
extern crate core;

use core::ffi::c_char;
use std::ffi::CStr;
use std::error::Error;
use std::ffi::CString;
use std::iter::Zip;
use std::path::PathBuf;
use zip::ZipArchive;
use zip::read::ZipFile;
use std::fs::File;
use std::io;
use std::io::{Cursor, Read};
use zip::result::ZipResult;

static mut extract: bool = false;
static mut out_path: String =  String::new();


struct ZipFileArchive{
    zipfile: zip::ZipArchive<File>
}

#[repr(C)]
struct CppResult{
    isErr: bool,
    val: *mut ()
}
#[repr(C)]
struct ArrayStruct{
    ptr: *mut *mut (),
    size: usize,
    cap: usize
}
impl ZipFileArchive{
    unsafe fn new(path: String) -> CppResult {
        let mut f = File::open(path.clone());
        let mut f = f.unwrap();
        let mut zip = zip::ZipArchive::new(f);
        if let Ok(z) = zip {
            let x = Box::new(z);
            let ptr = Box::into_raw(x) as *mut ();
            return CppResult{
                isErr: false,
                val: ptr
            }
        }
        else{
            return CppResult{
                isErr: true,
                val: 0 as *mut ()
            }
        }
    }
    fn extract_folder(&mut self, directory: *mut c_char) -> io::Result<u32>{ 
        unsafe{
            let mut zipf = &mut self.zipfile;
            let mut i = 0;
            let mut file = zipf.by_index(i).unwrap();
            let dir_path = PathBuf::from(CString::from_raw(directory).to_str().unwrap().to_string());
                let mut is_dir = false;
                if let Err(e) = std::fs::create_dir_all(dir_path.join(file.name().replace("/", "\\"))){
                    return Err(e)
                }
                while !is_dir{
                    let path = dir_path.join(file.name().replace("/", "\\"));
                    let mut vec = Vec::new();
                    file.read_to_end(&mut vec);
                    std::fs::write(path, vec);
                    i+=1;
                    is_dir = file.is_dir();
                }
            
            Ok(0)
        }

    }
    fn extract_all(&mut self, out_dir: *mut c_char) -> io::Result<u32>{
        unsafe{
            let mut zipf = &mut self.zipfile;
            let len = zipf.len();
            let dir_path = PathBuf::from(CString::from_raw(out_dir).to_str().unwrap().to_string());
            for i in 0..len {
                let mut file = zipf.by_index(i as usize).unwrap();
                let path = dir_path.join(file.name().replace("/", "\\"));
                if file.is_dir(){
                    std::fs::create_dir_all(dir_path.join(file.name().replace("/", "\\")));
                }
                else{
                    let mut vec = Vec::new();
                    file.read_to_end(&mut vec);
                    std::fs::write(path, vec);
                }
            }
            Ok(0)
        }
    }
    fn list_all(&mut self) -> ArrayStruct{
        let mut zipf = &mut self.zipfile;
        let mut list = Vec::new();
        for i in 0..zipf.len(){
            let mut file = zipf.by_index(i as usize).unwrap();
            let name = CString::new(file.name().to_string()).unwrap();
            list.push(name.into_raw());
        }
        let mut parts = list.into_raw_parts();
        ArrayStruct { ptr: parts.0 as *mut *mut (), size: parts.1, cap: parts.2 }
    }

}

#[no_mangle]
unsafe extern "C" fn zfa_new(path: *const c_char) -> CppResult{
    let path = CStr::from_ptr(path);
    ZipFileArchive::new(path.to_str().unwrap().to_string())
}

#[no_mangle]
unsafe extern "C" fn zfa_listall(zfa: *mut ZipFileArchive) -> ArrayStruct{
    (*zfa).list_all()
}