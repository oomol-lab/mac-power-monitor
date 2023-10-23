use std::{ptr, thread};
use std::sync::Mutex;

use libc::{c_int, c_void};
use napi::{bindgen_prelude::*, JsUndefined, JsBoolean};
use napi::{JsNumber, Env};
use napi_derive::napi;
use lazy_static::lazy_static;

use crate::macos;
use crate::node::Node;

static IS_RUNNING: Mutex<bool> = Mutex::new(false);

lazy_static! {
    static ref CAN_SLEEP: Mutex<bool> = Mutex::new(true);
    static ref ON_WILL_SLEEP_NODE: Node = Node::new("onSleep");
    static ref ON_WILL_WAKE_NODE: Node = Node::new("onWake");
}

#[napi]
pub fn start() {
    {
        let mut is_running = IS_RUNNING.lock().unwrap();
        if *is_running {
            return;
        }
        *is_running = true;
    }
    thread::spawn(|| unsafe {
        macos::startNotifications(&mut macos::Callback {
            canSleep: on_can_sleep,
            willSleep: on_will_sleep,
            willWake: on_will_wake,
        });
    });
}

#[napi]
pub fn stop() {
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

#[napi]
pub fn can_sleep(env: Env) -> Result<JsBoolean> {
    env.get_boolean(*CAN_SLEEP.lock().unwrap())
}

#[napi]
pub fn set_can_sleep(env: Env, js_can_sleep: JsBoolean) -> Result<JsBoolean> {
    let mut can_sleep = CAN_SLEEP.lock().unwrap();
    let origin_can_sleep = *can_sleep;
    *can_sleep = js_can_sleep.get_value()?;
    env.get_boolean(origin_can_sleep)
}

#[napi]
pub fn register_on_will_sleep(env: Env, listener: JsFunction) -> Result<JsNumber> {
    ON_WILL_SLEEP_NODE.register(env, listener)
}

#[napi]
pub fn register_on_will_wake(env: Env, listener: JsFunction) -> Result<JsNumber> {
    ON_WILL_WAKE_NODE.register(env, listener)
}

#[napi]
pub fn unregister_on_will_sleep(env: Env, id: JsNumber) -> Result<JsUndefined> {
    ON_WILL_SLEEP_NODE.unregister(env, id)
}

#[napi]
pub fn unregister_on_will_wake(env: Env, id: JsNumber) -> Result<JsUndefined> {
    ON_WILL_WAKE_NODE.unregister(env, id)
}

#[napi]
pub fn unregister() {
    ON_WILL_SLEEP_NODE.clear();
    ON_WILL_WAKE_NODE.clear();
}

fn on_can_sleep() -> c_int {
    if *CAN_SLEEP.lock().unwrap() {
        1
    } else {
        0
    }
}

fn on_will_sleep() -> *mut c_void {
    ON_WILL_SLEEP_NODE.fire();
    ptr::null_mut()
}

fn on_will_wake() -> *mut c_void {
    ON_WILL_WAKE_NODE.fire();
    ptr::null_mut()
}