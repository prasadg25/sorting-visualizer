@echo off
REM ============================================================
REM  build.bat  -  Build script for Sorting Algorithm Visualizer
REM  DSA Mini Project | B.Tech Computer Engineering
REM
REM  USAGE:
REM    1. Set SFML_DIR to your SFML installation path below
REM    2. Double-click this file or run from Command Prompt
REM ============================================================

SET "SFML_DIR=C:\SFML"
SET "GCC_DIR=%LOCALAPPDATA%\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.MSVCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin"
SET "GCC=%GCC_DIR%\g++.exe"

echo.
echo ============================================
echo   Sorting Algorithm Visualizer - Build
echo ============================================
echo.

REM Check if the configured WinLibs compiler is installed
IF NOT EXIST "%GCC%" (
    echo [ERROR] WinLibs GCC not found at "%GCC_DIR%".
    echo         Install it with: winget install --id BrechtSanders.WinLibs.POSIX.MSVCRT --exact
    pause
    exit /b 1
)

REM Check if SFML include folder exists
IF NOT EXIST "%SFML_DIR%\include\SFML" (
    echo [ERROR] SFML not found at %SFML_DIR%
    echo         Download SFML 2.6.x for MinGW 64-bit from https://www.sfml-dev.org/download.php
    echo         Extract to C:\SFML and re-run this script.
    pause
    exit /b 1
)

echo [INFO] Compiling...
echo.

%GCC% -std=c++17 -O2 -Wall ^
    main.cpp ^
    SortingAlgorithms.cpp ^
    Visualizer.cpp ^
    -I"%SFML_DIR%\include" ^
    -L"%SFML_DIR%\lib" ^
    -lsfml-graphics ^
    -lsfml-window ^
    -lsfml-system ^
    -lsfml-audio ^
    -o SortingVisualizer.exe

IF %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] Build complete: SortingVisualizer.exe
    echo.
    echo [INFO] Copying SFML and MinGW runtime DLLs...
    copy /Y "%SFML_DIR%\bin\sfml-graphics-2.dll"  . >nul 2>&1
    copy /Y "%SFML_DIR%\bin\sfml-window-2.dll"    . >nul 2>&1
    copy /Y "%SFML_DIR%\bin\sfml-system-2.dll"    . >nul 2>&1
    copy /Y "%SFML_DIR%\bin\sfml-audio-2.dll"     . >nul 2>&1
    copy /Y "%SFML_DIR%\bin\openal32.dll"          . >nul 2>&1
    copy /Y "%GCC_DIR%\libstdc++-6.dll" . >nul 2>&1
    copy /Y "%GCC_DIR%\libgcc_s_seh-1.dll" . >nul 2>&1
    copy /Y "%GCC_DIR%\libwinpthread-1.dll" . >nul 2>&1
    echo [INFO] DLLs copied.
    echo.
    echo [INFO] Launching SortingVisualizer.exe ...
    echo.
    SortingVisualizer.exe
) ELSE (
    echo.
    echo [FAILED] Build failed. Check errors above.
    pause
)
