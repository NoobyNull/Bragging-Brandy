# 3D Model Management Utility - Setup Guide

This guide will help you set up the development environment for the 3D Model Management Utility.

## Prerequisites

### Windows

1. **Visual Studio 2022** with C++ workload
   - Download from: https://visualstudio.microsoft.com/downloads/
   - Install the "Desktop development with C++" workload

2. **CMake 3.16+**
   - Download from: https://cmake.org/download/
   - Add to PATH environment variable

3. **Qt6 6.2+**
   - Download from: https://www.qt.io/download
   - Select Qt 6.2.0 for Windows (MSVC 2019 64-bit)
   - Install to: `C:\Qt\6.2.0\msvc2019_64`

### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build git
sudo apt install -y qt6-base-dev qt6-tools-dev libqt6opengl6-dev libqt6sql6-sqlite
sudo apt install -y libgl1-mesa-dev libglu1-mesa-dev mesa-common-dev
sudo apt install -y libassimp-dev libsqlite3-dev pkg-config
```

### Linux (Fedora/CentOS/RHEL)

```bash
sudo dnf install -y gcc-c++ cmake ninja-build git
sudo dnf install -y qt6-qtbase-devel qt6-qttools-devel qt6-qtopengl-devel qt6-qtsql-devel
sudo dnf install -y mesa-libGL-devel mesa-libGLU-devel
sudo dnf install -y assimp-devel sqlite-devel pkgconf
```

### Linux (Arch Linux)

```bash
sudo pacman -S --noconfirm base-devel cmake ninja git
sudo pacman -S --noconfirm qt6-base qt6-tools qt6-5compat
sudo pacman -S --noconfirm mesa
sudo pacman -S --noconfirm assimp sqlite pkgconf
```

### macOS

1. **Xcode Command Line Tools**
   ```bash
   xcode-select --install
   ```

2. **Homebrew** (if not installed)
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

3. **Dependencies**
   ```bash
   brew update
   brew install qt@6 cmake ninja assimp sqlite pkg-config
   ```

## Automated Setup

### Windows

Run the automated setup script as Administrator:

```cmd
tools\setup-dependencies.bat
```

This script will:
- Install Visual Studio 2022 Build Tools (if needed)
- Install CMake (if needed)
- Download and install Qt6 6.2.0
- Build the Assimp library
- Configure and build the project

### Linux/macOS

Run the automated setup script:

```bash
./tools/setup-dependencies.sh
```

This script will:
- Detect your Linux distribution or macOS
- Install required system packages
- Build the Assimp library from source (if needed)
- Configure and build the project

## Manual Setup

If you prefer to install dependencies manually:

### 1. Install Qt6

- Download Qt6 6.2.0+ from https://www.qt.io/download
- Install with the following modules:
  - Qt 6.2.0 Core
  - Qt 6.2.0 Widgets
  - Qt 6.2.0 OpenGL
  - Qt 6.2.0 SQL (SQLite)
  - Qt 6.2.0 Charts
  - Development tools and documentation

### 2. Install CMake

- Download CMake 3.16+ from https://cmake.org/download/
- Install and add to PATH

### 3. Build Assimp Library

```bash
cd dependencies
git clone https://github.com/assimp/assimp.git
cd assimp
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install
cmake --build build --config Release
sudo cmake --install build  # Linux/macOS
cmake --install build       # Windows
cd ../..
```

### 4. Configure Project

```bash
mkdir build
cd build

# Windows (with Visual Studio 2019)
cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.2.0\msvc2019_64" ..

# Linux/macOS
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/usr/lib/cmake/Qt6" ..
```

### 5. Build Project

```bash
# Windows
cmake --build . --config Release

# Linux/macOS
cmake --build . --config Release -j$(nproc)
```

## Environment Variables

### Windows

Add to PATH:
```
C:\Qt\6.2.0\msvc2019_64\bin
C:\Program Files\CMake\bin
```

Set environment variables:
```
Qt6_DIR=C:\Qt\6.2.0\msvc2019_64
CMAKE_PREFIX_PATH=C:\Qt\6.2.0\msvc2019_64
```

### Linux/macOS

Add to ~/.bashrc or ~/.zshrc:
```bash
export Qt6_DIR="/usr/lib/cmake/Qt6"
export CMAKE_PREFIX_PATH="/usr/lib/cmake/Qt6:$CMAKE_PREFIX_PATH"
```

## Building the Application

After successful setup:

### Windows
```cmd
cd build
cmake --build . --config Release
build\bin\Release\3DModelManagementUtility.exe
```

### Linux
```bash
cd build
cmake --build . --config Release
./bin/3DModelManagementUtility
```

### macOS
```bash
cd build
cmake --build . --config Release
./bin/3DModelManagementUtility.app/Contents/MacOS/3DModelManagementUtility
```

## Command Line Options

The application supports the following command line options:

```bash
3DModelManagementUtility.exe [options]

Options:
  --reset-database      Reset the project database
  --reset-settings      Restore default settings
  --import <path>       Import project or model file
  --export <path>       Export current project
  --gpu <adapter>       Specify GPU adapter (auto|cpu|dgpu|adapterId)
  --repair <mode>       Set repair behavior (auto|assist|off)
  --theme <style>       Set UI theme (light|dark|system)
```

## Troubleshooting

### Common Issues

1. **Qt6 not found**
   - Ensure Qt6 is installed in the correct location
   - Check that Qt6_DIR environment variable is set correctly
   - Verify PATH includes Qt6 bin directory

2. **CMake configuration fails**
   - Ensure CMake 3.16+ is installed and in PATH
   - Check that all Qt6 modules are installed
   - Verify Visual Studio 2022 is properly installed (Windows)

3. **Build fails**
   - Clean build directory: `rm -rf build` (Linux/macOS) or `rd /s /q build` (Windows)
   - Regenerate with `cmake -B build`
   - Check that all dependencies are installed

4. **Runtime errors**
   - Ensure all DLLs are available (Windows)
   - Check OpenGL drivers are up to date
   - Verify sufficient disk space for database and cache

### Getting Help

- Check the [Build Guide](docs/build-guide.md) for detailed instructions
- Review the [Troubleshooting Guide](docs/troubleshooting.md)
- Report issues with detailed system information

## Development Tools (Optional)

For enhanced development experience:

### Code Formatting
```bash
# Install clang-format
sudo apt install clang-format  # Linux
brew install clang-format      # macOS
```

### Static Analysis
```bash
# Install cppcheck
sudo apt install cppcheck      # Linux
brew install cppcheck          # macOS
```

### Documentation
```bash
# Install Doxygen
sudo apt install doxygen       # Linux
brew install doxygen           # macOS
```

## Next Steps

After successful setup:

1. Run the application to verify everything works
2. Review the [User Guide](docs/user-guide.md) for usage instructions
3. Check the [Contributing Guide](docs/contributing.md) for development workflow
4. Explore the [API Documentation](docs/api/) for extension development

---

*For issues and questions, please refer to the project documentation or create an issue in the repository.*