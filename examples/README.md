# Examples

This directory contains examples demonstrating various aspects of the CNC Catalogue & 3D Visualization Environment.

## Categories

### Basic Usage
- **[simple-viewer](../examples/simple-viewer/)**: Basic 3D model viewer widget
- **[project-organizer](../examples/project-organizer/)**: Simple project management
- **[mesh-repair-demo](../examples/mesh-repair-demo/)**: Mesh repair workflow

### Advanced Features
- **[custom-widget](../examples/custom-widget/)**: Creating custom widgets
- **[plugin-example](../examples/plugin-example/)**: Plugin development
- **[performance-test](../examples/performance-test/)**: Performance benchmarking

### Integration Examples
- **[cad-integration](../examples/cad-integration/)**: CAD software integration
- **[batch-processing](../examples/batch-processing/)**: Batch processing workflows
- **[custom-formats](../examples/custom-formats/)**: Custom file format support

## Running Examples

### Prerequisites
- Built application or development environment
- Qt 6.2+ installed
- CMake 3.16+

### Build and Run

```bash
# Navigate to example directory
cd examples/simple-viewer

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Run
build/Release/simple-viewer.exe
```

## Contributing Examples

When adding new examples:

1. **Create subdirectory** under appropriate category
2. **Include CMakeLists.txt** for easy building
3. **Add comprehensive README.md** with:
   - Purpose and scope
   - Prerequisites
   - Build instructions
   - Usage examples
   - Code walkthrough
4. **Document learning objectives**
5. **Include sample data** if needed

## Example Template

```
examples/
└── category/
    └── example-name/
        ├── CMakeLists.txt
        ├── README.md
        ├── src/
        │   ├── main.cpp
        │   ├── ExampleWidget.h
        │   └── ExampleWidget.cpp
        ├── data/
        │   └── sample-models/
        └── docs/
            └── screenshots/
```

See [Contributing Guide](../docs/contributing.md) for detailed guidelines.