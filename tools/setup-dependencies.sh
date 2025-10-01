#!/bin/bash

# Setup script for 3D Model Management Utility dependencies
# This script installs Qt6, CMake, and other required dependencies on Linux/macOS

set -e  # Exit on any error

echo "Setting up 3D Model Management Utility dependencies..."
echo "======================================================="

# Detect platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="linux"
    echo "Detected Linux platform"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macos"
    echo "Detected macOS platform"
else
    echo "Unsupported platform: $OSTYPE"
    exit 1
fi

# Create directories
echo "Creating directories..."
mkdir -p build
mkdir -p dependencies
mkdir -p dependencies/downloads

# Function to install packages on Ubuntu/Debian
install_ubuntu_deps() {
    echo "Installing Ubuntu/Debian dependencies..."

    # Update package index
    sudo apt update

    # Install build tools
    sudo apt install -y build-essential cmake ninja-build git

    # Install Qt6 dependencies
    sudo apt install -y qt6-base-dev qt6-tools-dev libqt6opengl6-dev libqt6sql6-sqlite

    # Install OpenGL development libraries
    sudo apt install -y libgl1-mesa-dev libglu1-mesa-dev mesa-common-dev

    # Install additional libraries
    sudo apt install -y libassimp-dev libsqlite3-dev pkg-config

    # Install optional development tools
    sudo apt install -y clang-tidy cppcheck doxygen
}

# Function to install packages on Fedora/CentOS/RHEL
install_redhat_deps() {
    echo "Installing RedHat-based distribution dependencies..."

    # Install build tools
    sudo dnf install -y gcc-c++ cmake ninja-build git

    # Install Qt6
    sudo dnf install -y qt6-qtbase-devel qt6-qttools-devel qt6-qtopengl-devel qt6-qtsql-devel

    # Install OpenGL development libraries
    sudo dnf install -y mesa-libGL-devel mesa-libGLU-devel

    # Install additional libraries
    sudo dnf install -y assimp-devel sqlite-devel pkgconf

    # Install optional development tools
    sudo dnf install -y clang-analyzer cppcheck doxygen
}

# Function to install packages on Arch Linux
install_arch_deps() {
    echo "Installing Arch Linux dependencies..."

    # Update package index
    sudo pacman -Sy

    # Install build tools
    sudo pacman -S --noconfirm base-devel cmake ninja git

    # Install Qt6
    sudo pacman -S --noconfirm qt6-base qt6-tools qt6-5compat

    # Install OpenGL development libraries
    sudo pacman -S --noconfirm mesa

    # Install additional libraries
    sudo pacman -S --noconfirm assimp sqlite pkgconf

    # Install optional development tools
    sudo pacman -S --noconfirm clang cppcheck doxygen
}

# Function to install packages on macOS
install_macos_deps() {
    echo "Installing macOS dependencies..."

    # Install Xcode command line tools if not present
    if ! xcode-select -p &>/dev/null; then
        echo "Installing Xcode command line tools..."
        xcode-select --install
        # Wait for installation to complete
        read -p "Press Enter after Xcode command line tools installation is complete..."
    fi

    # Install Homebrew if not present
    if ! command -v brew &> /dev/null; then
        echo "Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi

    # Install dependencies via Homebrew
    brew update

    # Install Qt6
    brew install qt@6

    # Install CMake
    brew install cmake ninja

    # Install other dependencies
    brew install assimp sqlite pkg-config

    # Install optional development tools
    brew install clang-format cppcheck doxygen
}

# Install platform-specific dependencies
case $PLATFORM in
    "linux")
        # Detect Linux distribution
        if command -v apt &> /dev/null; then
            install_ubuntu_deps
        elif command -v dnf &> /dev/null; then
            install_redhat_deps
        elif command -v pacman &> /dev/null; then
            install_arch_deps
        else
            echo "Unsupported Linux distribution"
            echo "Please install the following packages manually:"
            echo "  - build-essential, cmake, ninja-build, git"
            echo "  - qt6-base-dev, qt6-tools-dev, libqt6opengl6-dev"
            echo "  - libgl1-mesa-dev, libassimp-dev, libsqlite3-dev"
            exit 1
        fi
        ;;
    "macos")
        install_macos_deps
        ;;
esac

# Clone and build Assimp if not available
if ! pkg-config --exists assimp; then
    echo "Building Assimp from source..."

    cd dependencies
    if [ ! -d "assimp" ]; then
        git clone https://github.com/assimp/assimp.git
    fi

    cd assimp
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install
    cmake --build build --config Release -j$(nproc)
    sudo cmake --install build
    cd ../..
fi

# Set up environment variables
echo "Setting up environment variables..."

# Update PATH and other variables for Qt6
if [[ "$PLATFORM" == "linux" ]]; then
    if [ -d "/usr/include/qt6" ]; then
        export Qt6_DIR="/usr/lib/cmake/Qt6"
    fi
elif [[ "$PLATFORM" == "macos" ]]; then
    export Qt6_DIR="$(brew --prefix qt@6)/lib/cmake/Qt6"
fi

# Create CMake build configuration
echo "Configuring project with CMake..."

# Configure with CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$Qt6_DIR"

if [ $? -eq 0 ]; then
    echo "CMake configuration: PASSED"

    # Build the project
    echo "Building project..."
    cmake --build build --config Release -j$(nproc)

    if [ $? -eq 0 ]; then
        echo "Build: PASSED"
        echo ""
        echo "======================================================="
        echo "Setup completed successfully!"
        echo ""
        if [[ "$PLATFORM" == "linux" ]]; then
            echo "You can now run the application with: ./build/bin/3DModelManagementUtility"
        elif [[ "$PLATFORM" == "macos" ]]; then
            echo "You can now run the application with: ./build/bin/3DModelManagementUtility.app/Contents/MacOS/3DModelManagementUtility"
        fi
        echo "======================================================="
    else
        echo "Build: FAILED"
        echo "Check the build output for errors"
        exit 1
    fi
else
    echo "CMake configuration: FAILED"
    echo "Check that all dependencies are installed correctly"
    exit 1
fi

echo ""
echo "Setup script completed successfully."