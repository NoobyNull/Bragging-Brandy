#!/bin/bash

# Test runner script for 3D Model Management Utility
# This script runs the comprehensive test suite

set -e  # Exit on any error

echo "Running 3D Model Management Utility Tests"
echo "=========================================="
echo

# Set up environment
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="../build"
TEST_BUILD_DIR="$BUILD_DIR/tests"

# Check if test executable exists
if [ ! -f "$TEST_BUILD_DIR/3DModelManagementUtilityTests" ]; then
    echo "Test executable not found. Building tests..."
    echo

    # Configure with CMake
    echo "Configuring tests with CMake..."
    cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug

    if [ $? -ne 0 ]; then
        echo "CMake configuration failed"
        exit 1
    fi

    # Build tests
    echo "Building tests..."
    cmake --build "$BUILD_DIR" --config Debug --target 3DModelManagementUtilityTests

    if [ $? -ne 0 ]; then
        echo "Test build failed"
        exit 1
    fi
fi

echo "Test executable found. Running tests..."
echo

# Run all tests
echo "Running all tests..."
"$TEST_BUILD_DIR/3DModelManagementUtilityTests"

TEST_RESULT=$?

if [ $TEST_RESULT -eq 0 ]; then
    echo "All tests passed!"
else
    echo "Some tests failed. Check output above for details."
fi

echo

# Run specific test suites
echo "Running unit tests..."
"$TEST_BUILD_DIR/3DModelManagementUtilityTests" -unittest

echo

echo "Running integration tests..."
"$TEST_BUILD_DIR/3DModelManagementUtilityTests" -integration

echo

echo "Running performance tests..."
"$TEST_BUILD_DIR/3DModelManagementUtilityTests" -performance

echo

echo "Test execution completed."
echo "Exit code: $TEST_RESULT"

exit $TEST_RESULT