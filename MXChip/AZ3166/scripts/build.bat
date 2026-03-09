::  Copyright (c) Microsoft
::  Copyright (c) 2024 Eclipse Foundation
:: 
::  This program and the accompanying materials are made available 
::  under the terms of the MIT license which is available at
::  https://opensource.org/license/mit.
:: 
::  SPDX-License-Identifier: MIT
:: 
::  Contributors: 
::     Microsoft         - Initial version
::     Frédéric Desbiens - 2024 version.

@echo off
REM Build script optimisé pour le projet IoT DevKit (Windows)
REM Usage: build_fast.bat [clean|rebuild]

setlocal enabledelayedexpansion

REM Get the script directory and go up one level to AZ3166 dir
for %%I in ("%~dp0..") do set AZ3166_DIR=%%~fI
set BUILD_DIR=%AZ3166_DIR%\build
set NUM_JOBS=4

echo ==========================================
echo IoT DevKit - Fast Build Script (Windows)
echo ==========================================
echo AZ3166 Dir: %AZ3166_DIR%
echo Build Dir: %BUILD_DIR%
echo Parallel Jobs: %NUM_JOBS%
echo.

REM Parse arguments
if "%1"=="clean" (
    echo [INFO] Cleaning build directory...
    if exist "%BUILD_DIR%" (
        rmdir /s /q "%BUILD_DIR%"
    )
    mkdir "%BUILD_DIR%"
    echo [OK] Build directory cleaned
    echo.
)

if "%1"=="rebuild" (
    echo [INFO] Full rebuild...
    if exist "%BUILD_DIR%" (
        rmdir /s /q "%BUILD_DIR%"
    )
    mkdir "%BUILD_DIR%"
    echo [OK] Build directory cleaned
    echo.
)

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

REM Navigate to build directory
cd /d "%BUILD_DIR%"

REM Check if CMakeCache exists
if not exist "CMakeCache.txt" (
    echo [INFO] First build - configuring CMake...
    cmake -G Ninja ^
        -DCMAKE_BUILD_TYPE=Release ^
        -DCMAKE_C_FLAGS="-O3 -DNDEBUG" ^
        -DCMAKE_CXX_FLAGS="-O3 -DNDEBUG" ^
        ..
    echo [OK] CMake configured
    echo.
)

REM Build with parallel jobs
echo [INFO] Building with %NUM_JOBS% parallel jobs...

REM Try ninja first, fallback to cmake
where ninja >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    ninja -j %NUM_JOBS%
) else (
    cmake --build . --parallel %NUM_JOBS% --config Release
)

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    exit /b 1
)

echo.
echo ==========================================
echo [OK] Build completed successfully!
echo ==========================================
echo.
echo Output binary location:
echo %BUILD_DIR%\app\az3166_iot_devkit.elf
