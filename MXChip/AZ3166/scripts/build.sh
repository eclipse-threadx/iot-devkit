#!/bin/bash

#  Copyright (c) Microsoft
#  Copyright (c) 2024 Eclipse Foundation
#
#  This program and the accompanying materials are made available
#  under the terms of the MIT license which is available at
#  https://opensource.org/license/mit.
#
#  SPDX-License-Identifier: MIT
#
#  Contributors:
#     Microsoft         - Initial version
#     Frédéric Desbiens - 2024 version.

set -e

# Get the absolute path to the AZ3166 directory (go up 1 level from scripts dir)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
AZ3166_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${AZ3166_DIR}/build"
NUM_JOBS=$(nproc || echo 4)

echo "=========================================="
echo "IoT DevKit - Fast Build Script"
echo "=========================================="
echo "AZ3166 Dir: ${AZ3166_DIR}"
echo "Build Dir: ${BUILD_DIR}"
echo "Parallel Jobs: ${NUM_JOBS}"
echo ""

# Parse arguments
if [ "$1" == "clean" ]; then
    echo "[INFO] Cleaning build directory..."
    rm -rf "${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
    echo "[OK] Build directory cleaned"
    echo ""
fi

if [ "$1" == "rebuild" ]; then
    echo "[INFO] Full rebuild..."
    rm -rf "${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
    echo "[OK] Build directory cleaned"
    echo ""
fi

# Create build directory if it doesn't exist
if [ ! -d "${BUILD_DIR}" ]; then
    mkdir -p "${BUILD_DIR}"
fi

# Navigate to build directory
cd "${BUILD_DIR}"

# Check if CMakeCache exists for incremental build
if [ ! -f "CMakeCache.txt" ]; then
    echo "[INFO] First build - configuring CMake..."
    cmake -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_FLAGS="-O3 -DNDEBUG" \
        -DCMAKE_CXX_FLAGS="-O3 -DNDEBUG" \
        ..
    echo "[OK] CMake configured"
    echo ""
fi

# Build with parallel jobs
echo "[INFO] Building with ${NUM_JOBS} parallel jobs..."
START_TIME=$(date +%s)

if command -v ninja &> /dev/null; then
    ninja -j ${NUM_JOBS}
else
    cmake --build . --parallel ${NUM_JOBS} --config Release
fi

END_TIME=$(date +%s)
BUILD_TIME=$((END_TIME - START_TIME))

echo ""
echo "=========================================="
echo "[OK] Build completed successfully!"
echo "Build time: ${BUILD_TIME}s"
echo "=========================================="
echo ""
echo "Output binary location:"
echo "${BUILD_DIR}/app/az3166_iot_devkit"
