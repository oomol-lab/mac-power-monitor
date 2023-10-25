#!/bin/bash
set -o pipefail

platform_list=("x86_64-apple-darwin" "aarch64-apple-darwin")

for platform in "${platform_list[@]}"
do
    echo "napi build --platform --target $platform --release lib"
    napi build --platform --target $platform --release lib
done
