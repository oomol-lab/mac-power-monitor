extern crate napi_build;
extern crate cc;

fn main() {
  cc::Build::new()
    .file("c/macos_notification.c")
    .flag("-fmodules")
    .flag("-Wno-deprecated-declarations")
    .compile("macos_notification");
  napi_build::setup();
}