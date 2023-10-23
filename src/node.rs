use std::collections::HashMap;
use std::sync::Mutex;
use napi::{JsNumber, JsFunction, Env, JsUndefined};
use napi::bindgen_prelude::*;
use napi::threadsafe_function::{ThreadsafeFunctionCallMode, ThreadsafeFunction};

pub struct Node {
    name: String,
    core: Mutex<Core<Box<dyn Fn() -> Result<()> + 'static + Send>>>,
}

struct Core<F> {
    next_id: u32,
    listeners: HashMap<u32, F>,
}

impl Node {

    pub fn new(name: &str) -> Self {
        Self {
            name: name.to_owned(),
            core: Mutex::new(Core {
                next_id: 0,
                listeners: HashMap::new(),
            }),
        }
    }

    pub fn fire(&self) {
        for listener in self.core.lock().unwrap().listeners.values() {
            if let Err(err) = (*listener)() {
                eprintln!("fire some of {} failed: {}", self.name, err.to_string());
            }
        }
    }

    pub fn register(&self, env: Env, listener: JsFunction) -> Result<JsNumber> {
        let callback = |_| -> Result<Vec<()>> {
            Ok(vec![])
        };
        let listener: ThreadsafeFunction<()> = env.create_threadsafe_function(&listener, 0, callback)?;
        let id = self.core.lock().unwrap().register(Box::new(move || {
            let status = listener.call(Ok(()), ThreadsafeFunctionCallMode::Blocking);
            if matches!(status, Status::Ok) {
                Ok(())
            } else {
                Err(Error::from_reason(status.to_string()))
            }
        }));
        Ok(env.create_uint32(id)?)
    }

    pub fn unregister(&self, env: Env, id: JsNumber) -> Result<JsUndefined> {
        let id = id.get_uint32()?;
        self.core.lock().unwrap().unregister(id);
        env.get_undefined()
    }

    pub fn clear(&self) {
        self.core.lock().unwrap().clear();
    }

}

impl<F> Core<F> {

    #[inline]
    pub fn register(&mut self, target: F) -> u32 {
        let id = self.next_id;
        self.listeners.insert(id, target);
        self.next_id += 1;
        id
    }

    #[inline]
    pub fn unregister(&mut self, id: u32) {
        self.listeners.remove(&id);
    }

    #[inline]
    pub fn clear(&mut self) {
        self.listeners.clear();
    }
}