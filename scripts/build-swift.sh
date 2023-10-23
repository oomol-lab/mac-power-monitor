#!/bin/bash
set -e

pnpm clean:swift
swift build --package-path ./MacPowerMonitor -c debug --arch arm64 --arch x86_64
mkdir -p ./swift-libs
mv ./MacPowerMonitor/.build/apple/Products/Debug/libMacPowerMonitor.dylib ./swift-libs/libMacPowerMonitor.dylib