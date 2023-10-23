import { dirname, join } from "path";
import { Library } from "@lwahonen/ffi-napi";

const myLib = Library(
    join(dirname(__filename), "swift-libs", "libMacPowerMonitor.dylib"), {
        "add": ["int", ["int", "int"]],
    },
);

console.log("add", myLib.add(2, 4));