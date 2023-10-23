#!/bin/bash
set -e

pnpm clean
tsc -p ./tsconfig.json

if [ ! -d "./swift-libs" ]; then
    echo "expect swift-libs folder (run pnpm build:swift)"
    exit 1
fi

cp -r swift-libs lib/swift-libs