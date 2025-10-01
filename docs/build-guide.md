# Build Guide

This guide covers setting up the development environment and building the CNC Catalogue & 3D Visualization Environment from source.

## Prerequisites

### Required Software

- **Operating System**: Windows 10/11, Linux (Ubuntu 20.04+), or macOS 10.15+
- **C++ Compiler**:
  - **Windows**: Visual Studio 2022 with Desktop C++ workload
  - **Linux**: GCC 9+ or Clang 10+
  - **macOS**: Xcode 12+ with command line tools
- **Qt 6.2+**: Open Source or Commercial license
  - Qt Widgets module (required)
  - Qt Charts module (optional, for analytics widgets)
  - Qt OpenGL module (automatically included)
- **CMake 3.16+**: For build system configuration
- **Git**: For version control

### Optional Dependencies

- **Assimp Library**: Automatically downloaded via CMake (for 3D model loading)
- **OpenCL**: For GPU-accelerated mesh processing (optional)
- **Doxygen**: For API documentation generation

### Supported File Formats

**3D Models**: STL, OBJ, PLY, 3MF (via Assimp)
**Tool Databases**: Standard .tools JSON files from CNC manufacturers
**Project Files**: Custom JSON format for metadata and organization

## Installation Steps

### 1. Install Visual Studio 2022

1. Download from [visualstudio.microsoft.com](https://visualstudio.microsoft.com/downloads/)
2. Select "Desktop development with C++" workload
3. Include optional components:
   - C++ CMake tools for Windows
   - C++ Clang tools for Windows (optional)
   - Windows 10/11 SDK

### 2. Install Qt 6.2+

1. Download from [qt.io/download](https://qt.io/download)
2. Choose Qt Online Installer
3. Select Qt 6.2+ with your preferred compiler:
   - **Windows**: MSVC 2019/2022 compiler
   - **Linux**: GCC compiler
   - **macOS**: clang compiler
4. Include these modules:
   - Qt Charts (for analytics widgets)
   - Qt WebEngine (for help system)
   - Developer and Designer Tools
   - Qt OpenGL module (automatically included)

### 3. Install CMake

1. Download from [cmake.org/download](https://cmake.org/download/)
2. Choose appropriate installer for your platform
3. Add CMake to system PATH during installation

### 4. Install Optional OpenCL Support

For GPU-accelerated mesh processing (optional):

**NVIDIA GPUs**: OpenCL is included with graphics drivers
**Intel GPUs**: OpenCL is included with graphics drivers
**AMD GPUs**: Install AMD OpenCL drivers from [amd.com](https://www.amd.com/en/support)

## Building the Project

### Clone the Repository

```bash
git clone <repository-url>
cd cnc-catalogue
```

### Configure with CMake

```bash
# Create build directory
mkdir build
cd build

# Configure for Release build
cmake -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_PREFIX_PATH="C:/Qt/6.2.4/msvc2019_64" ^
      -G "Visual Studio 16 2019" ^
      ..

# For Visual Studio 2022
cmake -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_PREFIX_PATH="C:/Qt/6.2.4/msvc2019_64" ^
      -G "Visual Studio 17 2022" ^
      ..
```

#### CMake Options

| Option | Description | Default |
|--------|-------------|---------|
| `CMAKE_BUILD_TYPE` | Build configuration | Release |
| `CMAKE_PREFIX_PATH` | Qt installation path | Required |
| `ENABLE_OPENCL` | Enable OpenCL GPU acceleration | OFF |
| `ENABLE_TESTS` | Build unit tests | OFF |
| `ENABLE_DOCS` | Generate documentation | OFF |
| `ASSIMP_BUILD_SHARED` | Use shared Assimp library | ON |

### Build the Application

```bash
# Build all targets
cmake --build . --config Release

# Build specific target
cmake --build . --config Release --target cnc-catalogue

# Build tests (if enabled)
cmake --build . --config Release --target tests
```

### Install the Application

```bash
# Install to system
cmake --install . --config Release

# Or specify custom path
cmake --install . --config Release --prefix "C:/Program Files/CNC Catalogue"
```

## Development Setup

### IDE Integration

#### Visual Studio Code

1. Install extensions:
   - C/C++ (Microsoft)
   - CMake Tools (Microsoft)
   - Qt Tools (The Qt Company)

2. Configure CMake Tools:
   ```json
   {
     "cmake.configureSettings": {
       "CMAKE_PREFIX_PATH": "C:/Qt/6.2.4/msvc2019_64"
     }
   }
   ```

#### Qt Creator

1. Open project as CMake project
2. Configure kit with MSVC 2019/2022
3. Set Qt version in Projects > Manage Kits

### Debugging Configuration

#### Visual Studio

1. Set as Startup Project: `cnc-catalogue`
2. Configuration: Release (for performance) or Debug (for development)
3. Debugging > Environment: Add Qt bin directory to PATH

#### Qt Creator

1. Projects > Run > Working directory: `$(builddir)`
2. Projects > Run > Environment: Add `PATH=C:/Qt/6.2.4/msvc2019_64/bin`

## Troubleshooting

### Common Issues

#### CMake cannot find Qt

```
CMake Error: Could not find Qt6 components
```

**Solution**: Verify `CMAKE_PREFIX_PATH` points to correct Qt installation

#### MSVC compiler not found

```
CMake Error: No C++ compiler found
```

**Solution**: Ensure Visual Studio C++ workload is installed

#### Missing DLLs at runtime

**Solution**: Copy Qt runtime DLLs or use windeployqt:

```bash
# Using windeployqt
C:/Qt/6.2.4/msvc2019_64/bin/windeployqt.exe Release/cnc-catalogue.exe
```

#### OpenGL not working

**Solution**: Update graphics drivers to latest version. Ensure GPU supports OpenGL 4.6+

#### Large model performance issues

**Solution**: Enable progressive loading in settings. Check available RAM (minimum 4GB recommended for 500MB models)

### Performance Optimization

#### Release Build Flags

```cmake
# In CMakeLists.txt or toolchain file
set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG /MT")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/OPT:REF /OPT:ICF")
```

#### Debug Build (for development)

```cmake
set(CMAKE_CXX_FLAGS_DEBUG "/Od /DDEBUG /MTd")
```

## Testing

### Running Tests

```bash
# Build and run all tests
ctest --build-config Release

# Run specific test suite
ctest --build-config Release -R mesh_repair

# Run with verbose output
ctest --build-config Release --verbose
```

### Performance Benchmarks

```bash
# Run performance tests
build/bin/tests --benchmark

# Generate performance report
build/bin/tests --benchmark --report
```

## Continuous Integration

### GitHub Actions Example

```yaml
name: Windows CI

on: [push, pull_request]

jobs:
  build:
    runs-on: windows-latest

    steps:
    - uses: actions/checkout@v2

    - name: Install Qt
      uses: jurplel/install-qt-action@v3
      with:
        version: '6.2.*'

    - name: Configure
      run: cmake -DCMAKE_BUILD_TYPE=Release

    - name: Build
      run: cmake --build . --config Release

    - name: Test
      run: ctest --build-config Release
```

## Getting Help

- **Documentation**: [API Reference](api/)
- **Issues**: [GitHub Issues](../../issues)
- **Discussions**: [GitHub Discussions](../../discussions)
- **Constitution**: [Project Principles](../CONSTITUTION.md)