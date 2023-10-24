#include "pm.h"
#include <napi.h>

v8::Persistent<v8::Value> notificationCallback;
bool isNotificationRegistered = false;

v8::Handle<v8::Value> RegisterNotifications(const v8::Arguments& args) {
  v8::HandleScope scope;

  v8::Local<v8::Value> callback;

  if (args.Length() == 0)
  {
    return scope.Close(v8::ThrowException(v8::Exception::TypeError(v8::String::New("First argument must be a function"))));
  }

  if (args.Length() == 1)
  {
    // callback
    if(!args[0]->IsFunction())
    {
        return scope.Close(v8::ThrowException(v8::Exception::TypeError(v8::String::New("First argument must be a function"))));
    }

    callback = args[0];
  }

  notificationCallback = v8::Persistent<v8::Value>::New(callback);
  isNotificationRegistered = true;

  return scope.Close(v8::Undefined());
}

void Notify(char* msg) {
  if (isNotificationRegistered) {
    v8::Handle<v8::Value> argv[1];
    argv[0] = v8::String::New(msg);
    v8::Function::Cast(*notificationCallback)->Call(v8::Context::GetCurrent()->Global(), 1, argv);
  }
}

v8::Handle<v8::Value> StartMonitoring(const v8::Arguments& args) {
  v8::HandleScope scope;

  Start();

  return scope.Close(v8::Undefined());
}

v8::Handle<v8::Value> StopMonitoring(const v8::Arguments& args) {
  v8::HandleScope scope;

  Stop();

  return scope.Close(v8::Undefined());
}

extern "C" {
  void init (v8::Handle<v8::Object> target)
  {
    NODE_SET_METHOD(target, "registerNotifications", RegisterNotifications);
    NODE_SET_METHOD(target, "startMonitoring", StartMonitoring);
    NODE_SET_METHOD(target, "stopMonitoring", StopMonitoring);
    InitPM();
  }
}

NODE_MODULE(pm, init);
