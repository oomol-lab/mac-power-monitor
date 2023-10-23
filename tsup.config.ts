import { defineConfig } from "tsup";

export default defineConfig({
    entry: ["src/index.ts"],
    outDir: "lib",
    format: ["cjs"],
    splitting: false,
    sourcemap: false,
    clean: false,
    treeshake: true,
    dts: true,
    minify: false,
});