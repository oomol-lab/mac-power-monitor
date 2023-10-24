use libc::{c_void, c_int};

#[cfg(not(target_os = "macos"))]
compile_error!("This crate only works on macOS");

#[repr(C)]
#[allow(warnings)]
pub struct Callback {
    pub canSleep: fn() -> c_int,
    pub willSleep: fn() -> *mut c_void,
    pub willWake: fn() -> *mut c_void,
}

#[link(name = "macos_notification")]
extern "C" {
    #[allow(warnings)]
    pub fn startNotifications(callback: *mut Callback) -> c_int;
    pub fn stopNotifications() -> c_void;
}