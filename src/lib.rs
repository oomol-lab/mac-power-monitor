use std::env;

use napi::bindgen_prelude::*;
use napi_derive::napi;

mod sys {
    use libc::{c_int, c_void};

    #[link(name = "macos_notification")]
    extern "C" {
        pub fn registerNotifications() -> c_int;
        pub fn unregisterNotifications() -> c_void;
    }
}

#[napi]
fn test_sleep() -> i32 {
    0
}

/// module registration is done by the runtime, no need to explicitly do it now.
#[napi]
fn fibonacci(n: u32) -> u32 {
  match n {
    1 | 2 => 1,
    _ => fibonacci(n - 1) + fibonacci(n - 2),
  }
}

/// use `Fn`, `FnMut` or `FnOnce` traits to defined JavaScript callbacks
/// the return type of callbacks can only be `Result`.
#[napi]
fn get_cwd<T: Fn(String) -> Result<()>>(callback: T) {
  callback(env::current_dir().unwrap().to_string_lossy().to_string()).unwrap();
}

/// or, define the callback signature in where clause
#[napi]
fn test_callback<T>(callback: T)
where T: Fn(String) -> Result<()>
{}

/// async fn, require `async` feature enabled.
/// [dependencies]
/// napi = {version="2", features=["async"]}
#[napi]
async fn read_file_async(path: String) {

}