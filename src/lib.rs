use std::ptr;
use napi_derive::napi;

mod macos;

#[napi]
fn test_sleep() {
    println!("start");
    let mut callback = macos::Callback {
        canSleep: || {
            println!("call 1");
            return 0;
        },
        willSleep: || {
            println!("call 2");
            ptr::null_mut()
        },
        willWake: || {
            println!("call 3");
            ptr::null_mut()
        },
    };
    unsafe {
        macos::startNotifications(&mut callback)
    };
    println!("after");
}