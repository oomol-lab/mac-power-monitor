mod macos;
mod node;
mod listener;

pub use listener::{
    start,
    stop,
    register_on_will_sleep,
    register_on_will_wake,
    unregister,
    unregister_on_will_sleep,
    unregister_on_will_wake,
};