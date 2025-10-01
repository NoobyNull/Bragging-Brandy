@echo off
REM Setup script for 3D Model Management Utility dependencies
REM This script installs Qt6, CMake, and other required dependencies on Windows

echo Setting up 3D Model Management Utility dependencies...
echo ===================================================

REM Check if we're running as administrator
net session >nul 2>&1
if %errorLevel% == 0 (
    echo Running with administrator privileges
) else (
    echo Warning: Not running as administrator. Some installations may fail.
    echo Please run as administrator for full installation.
    pause
)

REM Set up environment
setlocal enabledelayedexpansion

REM Create directories
echo Creating directories...
if not exist "build" mkdir build
if not exist "dependencies" mkdir dependencies
if not exist "dependencies\downloads" mkdir dependencies\downloads

REM Check for existing installations
echo Checking existing installations...

REM Check for Qt6
if exist "C:\Qt\6.9.3\mingw_64" (
    echo Qt6 6.9.3 already installed
    set QT_INSTALLED=1
) else (
    set QT_INSTALLED=0
)

REM Check for CMake
cmake --version >nul 2>&1
if %errorLevel% == 0 (
    echo CMake already installed
    set CMAKE_INSTALLED=1
) else (
    set CMAKE_INSTALLED=0
)

REM Check for Visual Studio 2022
if exist "C:\Program Files\Microsoft Visual Studio\2022" (
    echo Visual Studio 2022 found
    set VS_INSTALLED=1
) else (
    set VS_INSTALLED=0
)

REM Install Visual Studio 2022 if not present
if !VS_INSTALLED! == 0 (
    echo Installing Visual Studio 2022...
    echo This may take a while and require a restart.
    echo.

    REM Download VS Build Tools
    echo Downloading Visual Studio 2022 Build Tools...
    powershell -Command "Invoke-WebRequest -Uri 'https://aka.ms/vs/17/release/vs_buildtools.exe' -OutFile 'dependencies\downloads\vs_buildtools.exe'"

    if exist "dependencies\downloads\vs_buildtools.exe" (
        echo Installing Visual Studio 2022 Build Tools...
        start /wait dependencies\downloads\vs_buildtools.exe --quiet --wait --norestart --nocache ^
            --installPath "C:\BuildTools" ^
            --add Microsoft.VisualStudio.Workload.VCTools ^
            --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
            --add Microsoft.VisualStudio.Component.Windows10SDK.19041

        if !errorLevel! == 0 (
            echo Visual Studio 2022 installed successfully
        ) else (
            echo Failed to install Visual Studio 2022
            echo Please install manually from: https://visualstudio.microsoft.com/downloads/
        )
    ) else (
        echo Failed to download Visual Studio 2022 installer
    )
)

REM Install CMake if not present
if !CMAKE_INSTALLED! == 0 (
    echo Installing CMake...

    REM Download CMake
    powershell -Command "Invoke-WebRequest -Uri 'https://github.com/Kitware/CMake/releases/download/v3.21.0/cmake-3.21.0-windows-x86_64.msi' -OutFile 'dependencies\downloads\cmake.msi'"

    if exist "dependencies\downloads\cmake.msi" (
        echo Installing CMake...
        start /wait msiexec /i dependencies\downloads\cmake.msi /quiet /norestart

        if !errorLevel! == 0 (
            echo CMake installed successfully
        ) else (
            echo Failed to install CMake
        )
    ) else (
        echo Failed to download CMake installer
    )
) else (
    echo CMake already installed
)

REM Install Qt6 if not present
if !QT_INSTALLED! == 0 (
    echo ERROR: Qt6 6.9.3 not found at C:\Qt\6.9.3\mingw_64
    echo.
    echo Please install Qt 6.9.3 manually:
    echo 1. Download from: https://www.qt.io/download
    echo 2. Select "Qt 6.9.3" for Windows (MinGW 11.2.0 64-bit)
    echo 3. Install to: C:\Qt\6.9.3\mingw_64
    echo 4. Run this script again after installation
    echo.
    pause
    exit /b 1

    REM Download Qt6 installer
    powershell -Command "Invoke-WebRequest -Uri 'https://qt-mirror.dannhauer.de/official_releases/qt/6.2/6.2.0/qt-opensource-windows-x86_64-6.2.0.exe' -OutFile 'dependencies\downloads\qt6.exe'"

    if exist "dependencies\downloads\qt6.exe" (
        echo Installing Qt6...
        echo This may take a while...
        start /wait dependencies\downloads\qt6.exe ^
            --script tools\qt6-install-script.qs ^
            --verbose

        if !errorLevel! == 0 (
            echo Qt6 installed successfully
        ) else (
            echo Failed to install Qt6
            echo Please install manually from: https://www.qt.io/download
        )
    ) else (
        echo Failed to download Qt6 installer
    )
)

REM Install additional dependencies
echo Installing additional dependencies...

REM Install Assimp library
if not exist "dependencies\assimp" (
    echo Installing Assimp library...
    git clone https://github.com/assimp/assimp.git dependencies\assimp
    cd dependencies\assimp
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install
    cmake --build build --config Release
    cmake --install build
    cd ..\..
)

REM Set up environment variables
echo Setting up environment variables...

REM Add Qt6 to PATH
for /f "tokens=*" %%i in ('dir "C:\Qt\6.9.3\mingw_64\bin" /b /o:n 2^>nul') do (
    setx QT6_DIR "C:\Qt\6.9.3\mingw_64" /M
    goto :qt_found
)
:qt_found

REM Update PATH for current session
set PATH=%PATH%;C:\Qt\6.9.3\mingw_64\bin;C:\Program Files\CMake\bin

REM Verify installations
echo Verifying installations...

REM Verify Qt6
if exist "C:\Qt\6.9.3\mingw_64\bin\qmake.exe" (
    echo Qt6 verification: PASSED
) else (
    echo Qt6 verification: FAILED
)

REM Verify CMake
cmake --version >nul 2>&1
if !errorLevel! == 0 (
    echo CMake verification: PASSED
) else (
    echo CMake verification: FAILED
)

REM Build the project
echo Building the project...

REM Configure with CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\6.9.3\mingw_64"

if !errorLevel! == 0 (
    echo CMake configuration: PASSED

    REM Build the project
    cmake --build build --config Release

    if !errorLevel! == 0 (
        echo Build: PASSED
        echo.
        echo ===================================================
        echo Setup completed successfully!
        echo.
        echo You can now run the application with: build\bin\Release\3DModelManagementUtility.exe
        echo ===================================================
    ) else (
        echo Build: FAILED
        echo Check the build output for errors
    )
) else (
    echo CMake configuration: FAILED
    echo Check that all dependencies are installed correctly
)

echo.
echo Setup script completed.
pause