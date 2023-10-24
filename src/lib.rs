use std::{ptr, thread};
use std::sync::Mutex;
use libc::{c_int, c_void};
use napi_derive::napi;

mod macos;

static IS_RUNNING: Mutex<bool> = Mutex::new(false);

#[napi]
pub fn start_listen() {
    {
        let mut is_running = IS_RUNNING.lock().unwrap();
        if *is_running {
            return;
        }
        *is_running = true;
    }
    thread::spawn(|| unsafe {
        macos::startNotifications(&mut macos::Callback {
            canSleep: can_sleep,
            willSleep: will_sleep,
            willWake: will_wake,
        });
    });
}

#[napi]
pub fn stop_listen() {
    {
        let is_running = IS_RUNNING.lock().unwrap();
        if !*is_running {
            return;
        }
    }
    unsafe {
        macos::stopNotifications();
    }
}

fn can_sleep() -> c_int {
    println!("can sleep");
    1
}

fn will_sleep() -> *mut c_void {
    println!("will sleep");
    ptr::null_mut()
}

fn will_wake() -> *mut c_void {
    println!("will wake");
    ptr::null_mut()
}