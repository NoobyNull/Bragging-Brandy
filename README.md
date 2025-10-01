# 3D Model Management Utility

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)]()
[![Qt](https://img.shields.io/badge/Qt-6.2+-green.svg)]()
[![OpenGL](https://img.shields.io/badge/OpenGL-4.6+-purple.svg)]()

**A fluid, intuitive 3D model management system** built with Qt and OpenGL. **Primary mission: efficient tagging, organization, and discovery of 3D media assets.** Features 3D visualization and CNC workflow tools that enhance but never complicate the core model management experience.

**Bottom Line**: Find, tag, and organize your 3D models effortlessly. Everything else is secondary.

## 🎯 Fluid Interface Design

### Model Management First
The interface is optimized for the **80/20 rule** - 80% of users need only 20% of features for basic model management:

- **Instant Model Import**: Drag-and-drop with auto-tagging
- **Real-time Search**: Type to find models across all metadata
- **Visual Organization**: Thumbnail grids with batch selection
- **Smart Tagging**: Auto-suggest, hierarchical tags, bulk operations

### Tabbed Canvas Workflow
Different canvases for different expertise levels:

- **Project Canvas**: Simple model management for everyone
- **Design Canvas**: Advanced visualization for power users
- **Cut List/CNC Canvases**: Specialized tools when needed

### Responsiveness Requirements
- **Search**: ≤100ms for real-time model discovery
- **Navigation**: 60+ FPS for smooth interface interaction
- **Loading**: Progressive display within 2 seconds
- **Operations**: All background processed, UI never blocks

## 📈 Use Case Priority

1. **Digital Asset Manager**: Tag and find 3D models quickly
2. **Project Organizer**: Structure models into coherent collections
3. **3D Viewer**: Visualize and inspect models when needed
4. **CNC Planner**: Prepare models for manufacturing (optional)

## 🚀 Key Features

**Core Mission - Model Management**:
- **Fluid Interface**: Zero learning curve for basic model tagging and organization
- **Instant Search**: Real-time discovery across thousands of models and tags
- **Smart Tagging**: Auto-suggest tags, batch tagging, hierarchical organization
- **Visual Organization**: Thumbnail grids, project hierarchies, custom collections

**Performance & Reliability**:
- **Large Model Support**: Handle 500MB+ models with progressive loading (≤10s)
- **Responsive Design**: Never blocks UI, always shows progress, graceful cancellation
- **Memory Efficient**: Streaming architecture with intelligent caching
- **Cross-Platform**: Native performance on Windows, Linux, and macOS

**Enhanced Capabilities**:
- **3D Visualization**: OpenGL-accelerated viewing with LOD rendering
- **Mesh Repair**: Background processing for model optimization
- **CNC Integration**: Cut list optimization and feed/speed calculation with standard .tools JSON files
- **Project Management**: Structured organization with metadata and notes

**Technical Excellence**:
- **Local-First**: Complete data control, no cloud dependencies
- **Transparent Operation**: Comprehensive logging and performance metrics
- **Extensible Architecture**: Plugin system for specialized workflows

## 📋 Performance Guarantees

**Large Model Performance** (New!):
- **Model Loading**: ≤10 seconds for models up to 500MB (progressive loading)
- **Memory Usage**: ≤2GB RAM for 500MB models (efficient streaming)
- **Interactive Performance**: 30+ FPS during navigation with LOD rendering
- **Background Processing**: Non-blocking UI during all operations

**Interface Fluidity** (Primary Requirement):
- **Instant Search**: ≤100ms response for model discovery
- **Smooth Navigation**: 60+ FPS during interface interactions
- **Drag & Drop**: Immediate visual feedback for all operations
- **Progressive Loading**: Models appear usable within 2 seconds
- **Zero Blocking**: UI never freezes, all operations cancellable

**Model Management Performance**:
- **Model Loading**: ≤3 seconds for models up to 100MB
- **Search Performance**: ≤500ms across 50,000+ entries
- **Thumbnail Generation**: ≤500ms per model
- **Batch Operations**: Linear scaling with smart progress indication

**Advanced Features**:
- **Large Model Loading**: ≤10 seconds for 500MB models (progressive)
- **Mesh Repair**: ≤5 seconds for complex models
- **Memory Usage**: <2GB for 500MB models (streaming architecture)

## 🏗️ Architecture

### Core Technologies
- **Framework**: Qt 6.2+ with OpenGL, Widgets, and Charts modules
- **Interface**: Fluid, responsive design with Qt's graphics view framework
- **Search Engine**: Full-text search with real-time results and faceted filtering
- **Graphics**: OpenGL 4.6+ with LOD rendering for large model performance
- **3D Model Loading**: Assimp library optimized for 500MB+ file support
- **Background Processing**: QtConcurrent for non-blocking model operations
- **Database**: SQLite for fast metadata queries and tagging
- **Build System**: Modern CMake (3.16+) with target-based organization

### Interface Design Philosophy
- **Progressive Disclosure**: Show only what users need, when they need it
- **Contextual Actions**: Right-click menus, drag-and-drop, keyboard shortcuts
- **Visual Feedback**: Live thumbnails, progress bars, status indicators
- **Smart Defaults**: Auto-tagging, suggested metadata, intelligent organization

### Widget-Based Design
Every feature is implemented as a dockable, floating widget that can be:
- Added, removed, and rearranged freely
- Tabbed together for space efficiency
- Popped out into separate windows
- Nested for complex workflows

## 🛠️ Development Philosophy

### Code Quality
- **Readability First**: Self-documenting code with clear naming conventions
- **Modern C++**: RAII, smart pointers, const-correctness, move semantics
- **Qt Best Practices**: QObject hierarchy, signal-slot patterns, MVD architecture
- **Performance Focused**: Profile-guided optimization and caching strategies

### Testing Standards
- **Unit Tests**: ≥80% coverage for core functionality
- **Integration Tests**: Widget interaction and file I/O validation
- **Performance Tests**: Automated benchmarking for all guarantees
- **UI Tests**: User workflow automation and accessibility compliance

## 📚 Documentation

- **[Constitution](CONSTITUTION.md)**: Complete project principles and governance
- **[Build Guide](docs/build-guide.md)**: Development environment setup
- **[API Documentation](docs/api/)**: Comprehensive API reference
- **[User Guide](docs/user-guide.md)**: End-user documentation
- **[Contributing Guide](docs/contributing.md)**: Development workflow

## 🚦 Quick Start

### Prerequisites
- Windows 10/11 (x64)
- Visual Studio 2022 with C++ workload
- Qt 6.2+ (with MSVC 2019/2022 compiler)
- CMake 3.16+
- Git

### Building from Source

```bash
# Clone the repository
git clone <repository-url>
cd cnc-catalogue

# Configure with CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the application
cmake --build build --config Release

# Run the application
build/bin/cnc-catalogue.exe
```

### Command Line Options

```bash
cnc-catalogue.exe [options]

Options:
  --reset-database      Reset the project database
  --reset-settings      Restore default settings
  --import <path>       Import project or model file
  --export <path>       Export current project
  --gpu <adapter>       Specify GPU adapter (auto|cpu|dgpu|adapterId)
  --repair <mode>       Set repair behavior (auto|assist|off)
  --theme <style>       Set UI theme (light|dark|system)
```

## 🤝 Contributing

We welcome contributions from the community! Please see our [Contributing Guide](docs/contributing.md) for details on:

- Development workflow and standards
- Code review process
- Issue reporting and feature requests
- Testing requirements

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Qt Project**: For the excellent cross-platform framework
- **Microsoft**: For DirectX 12 and Windows APIs
- **3D Model Community**: For providing test cases and feedback
- **Open Source Contributors**: For tools, libraries, and inspiration

---

*Built with ❤️ for the CNC and 3D printing community*