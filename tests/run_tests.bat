@echo off
REM Test runner script for 3D Model Management Utility
REM This script runs the comprehensive test suite

echo Running 3D Model Management Utility Tests
echo ==========================================
echo.

REM Set up environment
set TEST_DIR=%~dp0
set BUILD_DIR=..\build
set TEST_BUILD_DIR=%BUILD_DIR%\tests

REM Check if test executable exists
if not exist "%TEST_BUILD_DIR%\3DModelManagementUtilityTests.exe" (
    echo Test executable not found. Building tests...
    echo.

    REM Configure with CMake
    echo Configuring tests with CMake...
    cmake -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=Debug

    if errorlevel 1 (
        echo CMake configuration failed
        pause
        exit /b 1
    )

    REM Build tests
    echo Building tests...
    cmake --build "%BUILD_DIR%" --config Debug --target 3DModelManagementUtilityTests

    if errorlevel 1 (
        echo Test build failed
        pause
        exit /b 1
    )
)

echo Test executable found. Running tests...
echo.

REM Run all tests
echo Running all tests...
"%TEST_BUILD_DIR%\3DModelManagementUtilityTests.exe"

if errorlevel 1 (
    echo Some tests failed
) else (
    echo All tests passed!
)

echo.

REM Run specific test suites
echo Running unit tests...
"%TEST_BUILD_DIR%\3DModelManagementUtilityTests.exe" -unittest

echo.

echo Running integration tests...
"%TEST_BUILD_DIR%\3DModelManagementUtilityTests.exe" -integration

echo.

echo Running performance tests...
"%TEST_BUILD_DIR%\3DModelManagementUtilityTests.exe" -performance

echo.

echo Test execution completed.
pause