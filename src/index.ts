import { Library } from "@lwahonen/ffi-napi";

const myLib = Library(
    "./MacPowerMonitor/.build/debug/libMacPowerMonitor.dylib", {
        "add": ["int", ["int", "int"]],
    },
);

console.log("add", myLib.add(2, 4));