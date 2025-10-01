# 📜 The Constitution of the CNC Catalogue & 3D Visualization Environment

## Preamble

We, the designers and stewards of this system, establish this constitution to govern the creation of a cross-platform, Qt‑based 3D Model Management Utility. **Its primary mission is the efficient tagging, organization, and discovery of 3D media assets**, with secondary capabilities for visualization, optimization, and CNC workflow preparation.

**Core Philosophy**: This is fundamentally a model management system where 3D visualization and CNC tools enhance but never complicate the primary workflow of finding, tagging, and organizing 3D models.

This environment shall be local‑first, fluid, intuitive, and performance‑guaranteed. It shall serve digital artists, designers, engineers, and CNC operators with effortless model management. It shall never stall, never hide its workings, and never betray the sovereignty of its users' data.

**Interface Priority**: Fluid, responsive interaction is paramount. Every operation must feel instantaneous and intuitive, with zero learning curve for basic model management tasks.

**Performance Priority**: Large model handling (500MB+) with sub‑10 second load times through progressive loading, LOD rendering, and efficient memory management.

## Article I – Pages & Widgets

### Pages as Canvases

A Page is a workspace surface.

Users may create, rename, duplicate, and delete Pages.

Pages persist across sessions and can be exported/imported.

### Widgets as Universal Units

Everything is a widget.

Examples: 3D Viewer, Metadata Editor, Material Library, Mesh Repair Report, File Browser, Search Panel, Background Scene Selector, Thumbnail Grid, Log Viewer.

Widgets can be added, removed, docked, nested, floated, resized, minimized, maximized.

### Docking & Layout

Widgets snap into zones (left, right, top, bottom, center).

Widgets can be tabbed together.

Widgets can pop out into floating windows, movable across monitors.

Layouts are saved per Page and globally.

### Canvas System

The application uses a tabbed canvas architecture where different workflows are organized into specialized canvases:

- **Project Canvas**: Basic project management and 3D visualization
- **Design Canvas**: Advanced 3D modeling and design operations
- **Cut List Canvas**: Material optimization and nesting
- **CNC Tools Canvas**: Tool library and feed/speed optimization
- **CNC Prep Canvas**: CNC-specific preparation and toolpath generation
- **Analysis Canvas**: Performance analysis and reporting
- **Settings Canvas**: Application and project configuration

Each canvas is composed of widgets optimized for its specific workflow, while maintaining the ability to share widgets and data across canvases.

## Article II – Visualization

### 3D Model Viewer Widget

OpenGL 4.6+ renderer with Qt OpenGL widget.

Supports pan, rotate, zoom, reset view, and 6‑axis space mouse input.

**Large Model Performance**:
- Progressive loading for models >100MB
- Level-of-detail (LOD) rendering for interactive performance
- Memory-efficient mesh streaming
- Sub‑10 second load time for models ≤500MB

### Background Scenes

Options: solid color, gradient, grid plane, HDRI environment, custom image.

HDRI maps provide image‑based lighting (IBL).

Background adapts to Light/Dark/System theme.

User controls: HDRI intensity, rotation, ground plane toggle.

### Material Rendering

PBR‑lite shaders for wood, acrylic, aluminum, etc.

Nested library: Species → Type.

Materials applied in real time.

## Article XIX – Cut List Optimizer Canvas

### Purpose

Optimize material usage and minimize waste through intelligent nesting algorithms and geometric bin packing.

### Parts List Widget

Hierarchical organization of project parts with quantities and material requirements.

Drag-and-drop from project browser.

Batch quantity adjustments and material assignments.

### Material Optimizer Widget

**Nesting Algorithms**:
- Genetic algorithm optimization
- Geometric bin packing strategies
- Grain direction and kerf width constraints

**Optimization Parameters**:
- Sheet size configuration (4x8, 5x10, custom)
- Material thickness and type
- Minimum distances and edge requirements
- Rotation and mirroring options

**Real-time Feedback**:
- Efficiency percentage calculation
- Waste area visualization
- Cut count and estimated time
- Material cost estimation

### Visual Preview Widget

2D layout visualization with:
- Color-coded parts identification
- Waste area highlighting
- Cut path preview
- Zoom and pan controls

### Nesting Results Widget

**Performance Metrics**:
- Material efficiency percentage
- Total waste area and percentage
- Number of cuts required
- Estimated machining time

**Export Capabilities**:
- G-code generation for cutting operations
- DXF export for external nesting software
- PDF reports with cutting diagrams
- CSV export for cost analysis

## Article XX – CNC Tool Library & Feed/Speed Canvas

### Purpose

Comprehensive tool management and machining parameter optimization for CNC operations.

### Tool Library Widget

**Industry Standard Integration**:
- Import/export standard .tools JSON files from manufacturers
- Support for major CNC tool manufacturer formats
- Automatic parsing of tool geometry and specifications

**Hierarchical Organization**:
- Tool types: End mills, drills, routers, custom
- Material specifications and geometry from manufacturer data
- Performance characteristics and cutting parameters

**Tool Database**:
- SQLite storage with search and filtering
- JSON import/export for manufacturer .tools files
- Wear tracking and tool life estimation
- Cost per tool and replacement scheduling

### Feed/Speed Calculator Widget

**Material Database**:
- Wood species (hardwood, softwood, plywood)
- Metals (aluminum, steel, brass)
- Plastics (acrylic, HDPE, PVC)
- Composites and custom materials

**Physics-Based Calculations**:
- Chip load optimization
- Surface speed calculations
- Feed rate recommendations
- Depth of cut limitations

**Safety Constraints**:
- Maximum chip load warnings
- Tool deflection limits
- Heat generation monitoring
- Machine capability boundaries

### Tool Preview Widget

3D visualization of cutting tools with:
- Accurate geometry representation
- Cutting edge highlighting
- Flute and helix angle display
- Tool holder compatibility

### Machining Simulator Widget

**Real-time Simulation**:
- Toolpath visualization
- Material removal animation
- Chip formation modeling
- Heat and force analysis

**Performance Monitoring**:
- Actual vs. theoretical feed/speed
- Tool wear estimation
- Surface finish prediction
- Cycle time optimization

## Article III – Mesh Repair & Optimization

### Repair Pipeline

Parse → Validate → Repair → Cache → Render.

### Operations

- Duplicate vertex weld (epsilon threshold)
- Non‑manifold edge cleanup
- Self‑intersection detection
- Normal/tangent recompute
- Degenerate triangle removal

### Performance

Repair budget: ≤1.5 seconds for ≤200 MB models.

Cache repaired meshes for instant reload.

### Compute

**Large Model Processing**:
- Background processing with QtConcurrent
- Memory-mapped file I/O for large datasets
- Progressive mesh processing for immediate user feedback
- CPU SIMD optimization (AVX2/AVX‑512)
- Optional GPU acceleration via OpenCL/OpenGL compute

### Repair Report Widget

Displays counts of fixes, thresholds, unresolved issues.

Exportable as JSON/Markdown.

## Article IV – Catalogue & Metadata

### Catalogue Widget

Organizes projects into hierarchical folders.

Each project contains models, G‑code, cutsheets, previews, notes.

### Metadata Editor Widget

Editable fields: project title, material, tooling, spindle/feed, cut depth, estimated time, tags, notes.

Batch editing and tagging supported.

### Search Widget

Full‑text search across metadata and filenames.

Filters by tags, material, tooling, file type.

Performance: ≤500 ms for 50k entries.

### Storage

SQLite database for metadata.

Binary cache for repaired meshes and thumbnails.

JSON for settings and layouts.

## Article V – Thumbnails

### Thumbnail Widget

Grid or list view of models/projects.

Auto‑generated on import.

### Generation

Resolution: 256×256 px (configurable).

Viewpoint: standardized isometric (user override possible).

Background: matches active scene.

Performance: ≤500 ms per thumbnail.

### Caching

Stored with checksum key.

Regenerated if background/material changes.

## Article VI – Logging

### Universal Logging

Every widget, every page, every process logs events.

Logs are structured, timestamped, and local‑first.

### Log Structure

Format: JSONL or SQLite log table.

Fields: timestamp, page ID, widget ID, event type, details, performance metrics.

### Log Viewer Widget

Dockable/floating widget to browse logs.

Filters by event type, widget, page, severity.

Exportable to JSON/CSV/text.

### Performance

Asynchronous logging; never blocks UI.

Configurable retention and verbosity.

## Article VII – Settings

### Universal Settings Principle

Every option must be surfaced in Settings.

No hidden toggles.

### Control Types

- Checkboxes for binary toggles
- Radio buttons for exclusive choices
- Dropdowns for multiple options
- Sliders/numeric inputs for ranges
- File/color pickers for user assets

### Categories

General, Visualization, Mesh Repair, Catalogue, Thumbnails, Docking & Pages, GPU & Performance, Logging, CLI Integration.

### UX

Windows 11 Fluent styling.

Tooltips for every option.

Reset‑to‑defaults button.

## Article VIII – CLI

### Switches

- `--reset-database`
- `--reset-settings`
- `--import <path>`
- `--export <path>`
- `--gpu <auto|cpu|dgpu|adapterId>`
- `--repair <auto|assist|off>`
- `--theme <light|dark|system>`

## Article IX – Loading & Responsiveness

### Loading Widgets

Any process >500 ms must show a loading widget.

Progress bar (deterministic) or spinner (indeterminate).

Always paired with status text.

### Responsiveness

UI must never freeze.

All long tasks are asynchronous.

Crashes and silent stalls are unacceptable.

## Article X – Performance Guarantees

**Large Model Performance**:
- Model load: ≤10 seconds for ≤500 MB (progressive loading)
- Mesh repair: ≤5 seconds for ≤500 MB models
- Memory usage: ≤2GB for 500MB models (efficient streaming)
- Interactive performance: 30+ FPS during navigation with LOD
- Background processing: Non-blocking UI during all operations

**Standard Performance**:
- Model load: ≤3 seconds for ≤100 MB
- Thumbnail generation: ≤500 ms
- Search: ≤500 ms for 50k entries
- Import indexing: ≤2 seconds per 100 files
- UI responsiveness: Zero blocking operations

## Article XI – Extensibility

Plugin system via Qt plugin architecture.

APIs for new widgets, file types, metadata fields, render passes.

Stable interfaces for catalogue access and GPU tasks.

## Article XII – Boundaries

The application shall not:

- Control CNC machines directly
- Generate or edit toolpaths for CNC controllers
- Provide version control or revision history
- Depend on cloud services for core functionality

**CNC-Related Operations** (Permitted):
- Cut list optimization and material nesting
- Feed and speed calculations for reference
- Tool library management and recommendations
- Machining simulation and analysis
- Cost estimation and time calculations

These operations are for planning and optimization purposes only, not for direct machine control.

## Article XIII – Governing Principles

**Primary Mission**:
- **Model Management First**: Tagging, organization, and discovery are the core purpose
- **Fluid Interface**: Every interaction must feel instantaneous and intuitive
- **Zero Learning Curve**: Basic model management requires no training

**Technical Excellence**:
- **Visualization Support**: Models viewable, optimized, repaired as needed
- **Local‑first sovereignty**: Data remains under user control
- **Transparency**: All operations visible through comprehensive logging
- **Performance**: Large model handling with progressive loading and LOD
- **Modularity**: Canvas + widget system for flexible workflows

**Secondary Capabilities**:
- **CNC Integration**: Cut list optimization and feed/speed calculation for planning
- **Material Efficiency**: Waste reduction through intelligent nesting algorithms
- **Safety First**: Conservative recommendations with safety margins

**User Experience**:
- **Effortless Operation**: Drag-and-drop, contextual actions, smart defaults
- **Instant Feedback**: Real-time search, live thumbnails, progress indicators
- **Contextual Help**: Tooltips, inline documentation, progressive disclosure
- **Accessibility**: Intuitive controls, keyboard shortcuts, screen reader support
- **Reliability**: No freezes, no silent failures, graceful error recovery

## Article XIV – Development Philosophy

### Code Quality Standards

**Readability First**
- Self-documenting code over comments
- Clear variable and function names
- Consistent formatting and style
- Meaningful commit messages

**Modern C++ Practices**
- RAII (Resource Acquisition Is Initialization)
- Smart pointers over raw pointers
- Const-correctness where applicable
- Move semantics for performance
- STL algorithms over manual loops

**Qt Best Practices**
- QObject hierarchy for memory management
- Signal-slot connections for loose coupling
- Model-View-Delegate pattern for data presentation
- QOpenGLWidget for hardware-accelerated 3D rendering

**Large Model Handling**
- **Assimp**: Industry-standard 3D model loading (handles 500MB+ files)
- **Progressive Loading**: Stream large models for immediate feedback
- **LOD Rendering**: Level-of-detail for interactive performance
- **QtConcurrent**: Background processing without blocking UI
- **Memory Mapping**: Efficient I/O for large datasets

**Performance Considerations**
- Profile-guided optimization
- Lazy loading and caching strategies
- Asynchronous processing for I/O operations
- OpenGL compute shaders for mesh processing

### Testing Requirements

**Unit Tests**
- Every public function must have unit tests
- Mock objects for external dependencies
- Test coverage ≥80% for core functionality

**Integration Tests**
- Widget interaction testing
- File import/export validation
- Performance benchmark tests

**UI Tests**
- User workflow automation
- Cross-platform compatibility
- Accessibility compliance

### Documentation Standards

**Code Documentation**
- Doxygen comments for all public APIs
- Architecture decision records (ADRs)
- Performance analysis reports

**User Documentation**
- Built-in help system
- Video tutorials for complex workflows
- Context-sensitive tooltips

## Article XV – Project Governance

### Decision Making Process

**Technical Decisions**
- Performance improvements require benchmarking
- API changes require compatibility analysis
- New features require use case validation

**Architecture Changes**
- Reviewed by core maintainers
- Impact analysis on existing functionality
- Migration path for users

**Release Process**
- Semantic versioning (MAJOR.MINOR.PATCH)
- Release notes with upgrade instructions
- Beta testing for major features

### Contributing Guidelines

**Code Contributions**
- Issues must have approved design before implementation
- Pull requests require unit tests
- Code review by at least one maintainer

**Bug Reports**
- Must include reproduction steps
- Environment information required
- Severity classification (critical, major, minor)

**Feature Requests**
- Must demonstrate user value
- Implementation complexity assessment
- Alternative solution consideration

## Article XVI – Project Structure

### Repository Organization

```
├── src/                    # Source code
│   ├── core/              # Core functionality
│   ├── ui/                # User interface widgets and canvases
│   ├── render/            # Rendering engine
│   ├── repair/            # Mesh processing
│   ├── nesting/           # Cut list optimization algorithms
│   ├── tools/             # CNC tool library and feed/speed
│   ├── simulation/        # Machining simulation engine
│   └── utils/             # Utilities and helpers
├── tests/                 # Test suites
├── docs/                  # Documentation
├── tools/                 # Development tools
└── examples/              # Usage examples
```

### Build System

**CMake Configuration**
- Modern CMake (3.16+)
- Target-based build organization
- Feature flags for optional components

**Dependencies**
- Qt 6.2+ with OpenGL, Widgets, Charts modules (includes JSON parsing)
- Assimp library for 3D model loading
- OpenGL 4.6+ drivers
- SQLite for metadata storage
- Clipper2 library for polygon clipping and nesting
- Standard .tools JSON files from CNC tool manufacturers
- Optional: OpenCL for GPU-accelerated mesh processing
- Optional: CGAL for advanced geometric algorithms

## Article XVII – Community Standards

### Communication

**Respectful Discourse**
- Constructive feedback over criticism
- Patience with newcomers
- Recognition of diverse skill levels

**Inclusive Language**
- Gender-neutral terminology
- Accessible technical explanations
- Consideration for non-native speakers

### Support Philosophy

**User Empowerment**
- Enable self-service through good UX
- Comprehensive error messages
- Recovery suggestions for failures

**Learning Resources**
- Progressive disclosure of complexity
- Contextual help systems
- Community-contributed tutorials

## Article XVIII – Future Evolution

### Extensibility Vision

**Plugin Ecosystem**
- Third-party widget development
- Custom file format support
- Specialized repair algorithms

**Integration Capabilities**
- CAD software interoperability
- CNC controller communication (read-only)
- Cloud storage synchronization (optional)

### Technology Evolution

**Graphics Pipeline**
- Ray tracing support when beneficial
- Vulkan backend for cross-platform potential
- Advanced material representations

**User Experience**
- Voice control integration
- Gesture recognition support
- Multi-touch interaction

---

*This constitution is a living document, subject to evolution as the project grows and technology advances. Amendments require consensus among active maintainers and clear benefit to users.*