#!/bin/bash
set -o pipefail

if [ -z $CI]; then
    pnpm run build;
else
    exit 0;
fi;
