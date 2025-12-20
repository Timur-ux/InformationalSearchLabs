#!/usr/bin/env bash

# 
# This script used for buildding certain service from sources
# First arg is the serviceName(aka dbService, indexService, etc)
# Build release version
#

cmake -S . -B/build -DCMAKE_BUILD_TYPE=RELEASE
cmake --build /build -- -j$(nproc)

