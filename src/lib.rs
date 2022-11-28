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


struct ZipFileArchive{
    zipfile: zip::ZipArchive<File>
}

#[repr(C)]
struct CppResult{
    isErr: bool,
    val: *mut *mut ()
}
#[repr(C)]
struct ArrayStruct{
    ptr: *mut *mut (),
    size: usize,
    cap: usize
}

unsafe fn extract_file(file: &mut ZipFile, out_path: PathBuf) -> String{
    let mut data = Vec::new();
    file.read_to_end(&mut data);
    if let Err(e) = std::fs::write(out_path.clone(), data){
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
                val: CString::new(e.to_string()).unwrap().into_raw() as *mut *mut ()
            }
        }
        let mut f = f.unwrap();
        let mut zip = zip::ZipArchive::new(f);
        if let Ok(z) = zip {
            let x = Box::new(z);
            let ptr = Box::into_raw(x) as *mut ();
            return CppResult{
                isErr: false,
                val: ptr as *mut *mut ()
            }
        }
        else{
            return CppResult{
                isErr: true,
                val: CString::new(zip.unwrap_err().to_string()).unwrap().into_raw() as *mut *mut ()
            }
        }
    }
    fn extract(&mut self, id: usize, out_dir: *mut c_char, single_file: bool) -> CppResult{ 
        unsafe{
            let mut zipf = &mut self.zipfile;
            let out_path = PathBuf::from(CStr::from_ptr(out_dir).to_str().unwrap().to_string().replace("/", "\\"));
            let mut is_dir = false;
            let mut num = id;
                while !is_dir{
                    let mut file = zipf.by_index(num).unwrap();
                    if file.is_dir() && num == id{
                        if single_file{
                            break;
                        }
                        num+=1;
                        continue;
                    }        
                    let path = out_path.join(file.mangled_name().file_name().unwrap());
                    extract_file(&mut file, path);
                    num+=1;
                    is_dir = file.is_dir();
                }
                CppResult{
                    isErr: false,
                    val: CString::new(out_path.to_str().unwrap()).unwrap().into_raw() as *mut *mut ()
                }
        }
    }

    fn extract_all(&mut self, out_dir: *mut c_char) -> CppResult{
        unsafe{
            let mut zipf = &mut self.zipfile;
            let dir_path = PathBuf::from(CStr::from_ptr(out_dir).to_str().unwrap().to_string());
            if let Err(e) = zipf.extract(dir_path){
                return CppResult { isErr: false, val: CString::new(e.to_string()).unwrap().into_raw() as *mut *mut ()}
            }
            CppResult { isErr: false, val: 0 as *mut *mut () }
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

#[no_mangle]
unsafe extern "C" fn zfa_extract(zfa: *mut ZipFileArchive, file: usize, out_dir: *mut *mut c_char, single_file: bool) -> CppResult{
    (*zfa).extract(file, *out_dir, single_file)
}

#[no_mangle]
unsafe extern "C" fn zfa_extract_all(zfa: *mut ZipFileArchive, out_dir: *mut *mut c_char) -> CppResult{
    (*zfa).extract_all(*out_dir)
}