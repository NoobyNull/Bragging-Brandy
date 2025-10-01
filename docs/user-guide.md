# 3D Model Management Utility - User Guide

## 🎯 Welcome

Welcome to the **3D Model Management Utility** - a powerful, intuitive system for organizing, visualizing, and preparing 3D models for manufacturing and design workflows.

### What You Can Do

- **📦 Manage 3D Models**: Import, organize, and search thousands of 3D models effortlessly
- **🎨 Visualize Models**: View 3D models with professional OpenGL-accelerated rendering
- **🏷️ Smart Organization**: Use intelligent tagging and auto-suggest for easy model discovery
- **⚙️ CNC Integration**: Optimize materials, calculate feed/speeds, and generate cutting paths
- **📊 Performance**: Handle large models (500MB+) with smooth, responsive interaction

## 🚀 Quick Start

### Installation

1. **Download and install** the application using the [Build Guide](build-guide.md)
2. **Launch the application** - it will create necessary directories and database
3. **Import your first models** using drag-and-drop or the Import menu

### First Steps

1. **Create a Project**: Use the Project Canvas to organize your models
2. **Import Models**: Drag 3D files (STL, OBJ, PLY, etc.) into the application
3. **Add Tags**: Use intelligent tagging to categorize your models
4. **Search and Find**: Use real-time search to locate models instantly

## 📖 Core Workflows

### Model Management

#### Importing Models
```text
1. Click "Import Models" in the toolbar or File menu
2. Select STL, OBJ, PLY, 3MF, or other supported formats
3. Models are automatically analyzed and indexed
4. Thumbnails are generated for visual browsing
```

#### Organizing with Tags
```text
1. Select models in the grid view
2. Right-click and choose "Tag Models"
3. Type to get intelligent tag suggestions
4. Use hierarchical tags for better organization
```

#### Searching Models
```text
1. Use the search bar in any canvas
2. Type to get instant results across all metadata
3. Filter by tags, materials, or dimensions
4. Use fuzzy matching for typo tolerance
```

### 3D Visualization

#### Basic Navigation
```text
- **Rotate**: Left-click and drag
- **Pan**: Middle-click and drag
- **Zoom**: Mouse wheel or right-click and drag
- **Focus**: Press F to focus on selected model
```

#### View Presets
```text
- **1**: Perspective view
- **2**: Orthographic view
- **3**: Top view
- **4**: Front view
- **5**: Right view
```

#### Shading Modes
```text
- **W**: Wireframe mode
- **S**: Shaded mode
- **T**: Textured mode
- **R**: Rendered mode
```

### CNC Integration

#### Material Optimization
```text
1. Switch to "Cut List Canvas"
2. Add parts from your model library
3. Select material type and sheet size
4. Run genetic algorithm optimization
5. Export optimized cutting layout
```

#### Feed and Speed Calculation
```text
1. Switch to "CNC Tools Canvas"
2. Select tool from library
3. Choose material type
4. Enter cutting parameters
5. Get optimized feed and speed values
```

## 🎨 User Interface

### Canvas System

The application uses a **tabbed canvas system** where different workspaces are organized into specialized canvases:

#### Project Canvas (Default)
- **Model Management**: Import, organize, and search 3D models
- **Thumbnail Grid**: Visual browsing of model library
- **Tag Management**: Intelligent tagging and categorization
- **Project Organization**: Hierarchical project structure

#### Design Canvas
- **3D Visualization**: Professional OpenGL-accelerated model viewing
- **Camera Controls**: Interactive navigation with presets
- **Material Display**: Realistic rendering with lighting
- **Measurement Tools**: Dimensioning and analysis

#### Cut List Canvas
- **Material Optimization**: Genetic algorithm nesting optimization
- **Visual Nesting**: 2D layout visualization with drag-and-drop
- **Cost Analysis**: Material efficiency and cost calculation
- **Export Options**: G-Code, DXF, and PDF report generation

#### CNC Tools Canvas
- **Tool Library**: Industry-standard .tools JSON file management
- **Feed/Speed Calculator**: Physics-based machining parameter calculation
- **Tool Wear Tracking**: Usage monitoring and replacement scheduling
- **Material Database**: Comprehensive material properties

#### Settings Canvas
- **Application Settings**: Theme, language, and general preferences
- **Performance Tuning**: Memory, cache, and processing optimization
- **CNC Configuration**: Manufacturing-specific settings
- **Import/Export**: Settings backup and sharing

### Keyboard Shortcuts

#### General
- **Ctrl+N**: New project
- **Ctrl+O**: Open project
- **Ctrl+S**: Save project
- **Ctrl+I**: Import models
- **Ctrl+E**: Export project
- **Ctrl+Q**: Exit application

#### Navigation
- **Ctrl+Tab**: Next canvas
- **Ctrl+Shift+Tab**: Previous canvas
- **F11**: Toggle fullscreen
- **F1**: Show help

#### 3D Navigation
- **F**: Focus on model
- **1-5**: View presets (Perspective, Orthographic, Top, Front, Right)
- **W/S/T/R**: Shading modes (Wireframe, Shaded, Textured, Rendered)
- **Space**: Play/pause animation

## ⚙️ Configuration

### Application Settings

#### Theme and Appearance
```text
1. Open Settings Canvas
2. Go to "General" tab
3. Select theme: Light, Dark, or System
4. Choose language preference
5. Configure auto-save and backup options
```

#### Performance Settings
```text
1. Open Settings Canvas
2. Go to "Performance" tab
3. Set memory limit (default: 2048 MB)
4. Configure cache size (default: 1000 MB)
5. Enable/disable parallel processing
6. Set maximum file size limit
```

### CNC Configuration

#### Tool Library Setup
```text
1. Open CNC Tools Canvas
2. Click "Import" to load .tools JSON files
3. Add custom tools with specific parameters
4. Configure tool materials and cutting data
```

#### Material Database
```text
1. Open CNC Tools Canvas
2. Select material from dropdown
3. Add custom materials with properties
4. Configure machining parameters (SFM, chip load)
```

## 🔧 Advanced Features

### Large Model Handling

#### Progressive Loading
```text
- Large models (>100MB) load progressively
- Basic structure loads first for immediate interaction
- Detail increases as more data becomes available
- Cancel loading at any time
```

#### Memory Management
```text
- Automatic memory optimization for large models
- Streaming architecture for 500MB+ files
- Intelligent caching with automatic cleanup
- Performance monitoring with resource alerts
```

### Batch Processing

#### Model Processing Pipeline
```text
1. Import multiple models
2. Apply automatic tagging
3. Generate thumbnails
4. Run mesh repair if needed
5. Export in desired format
```

#### CNC Batch Operations
```text
1. Select multiple models for nesting
2. Run optimization algorithm
3. Generate cutting paths
4. Export G-Code for all parts
```

### Custom Workflows

#### Automation Scripts
```bash
#!/bin/bash
# Example automation script

# Import models
3dmodelmanager --import *.stl

# Repair models
3dmodelmanager --repair auto

# Generate thumbnails
3dmodelmanager --generate-thumbnails

# Optimize nesting
3dmodelmanager --optimize-nesting --output-dir ./output

# Generate G-Code
3dmodelmanager --generate-gcode
```

## 🆘 Troubleshooting

### Common Issues

#### Application Won't Start
```text
1. Check system requirements (Qt 6.2+, OpenGL 4.6+)
2. Verify GPU drivers are up to date
3. Check available disk space
4. Review log files for error details
```

#### Models Won't Import
```text
1. Verify file format is supported (STL, OBJ, PLY, 3MF, etc.)
2. Check file integrity and size
3. Ensure sufficient memory is available
4. Review import logs for specific errors
```

#### Performance Issues
```text
1. Reduce memory limit in settings
2. Enable progressive loading
3. Clear thumbnail cache
4. Check system resources (CPU, RAM, GPU)
```

#### CNC Export Problems
```text
1. Verify tool library is loaded
2. Check material properties
3. Validate part dimensions
4. Review export format settings
```

### Getting Help

#### Log Files
```text
- Application logs: ~/AppData/Local/3DModelManager/logs/ (Windows)
- Application logs: ~/.local/share/3DModelManager/logs/ (Linux)
- Application logs: ~/Library/Logs/3DModelManager/ (macOS)
```

#### Performance Monitoring
```text
1. Check FPS display in 3D view
2. Monitor memory usage in status bar
3. Review performance logs
4. Use built-in benchmarking tools
```

## 📊 Tips and Best Practices

### Model Organization
```text
- Use consistent naming conventions
- Apply tags immediately after import
- Create project hierarchies for complex work
- Use custom fields for specific metadata
```

### Performance Optimization
```text
- Enable progressive loading for large models
- Use appropriate LOD settings for your hardware
- Configure memory limits based on system RAM
- Clear caches regularly for optimal performance
```

### CNC Workflow
```text
- Always verify tool and material compatibility
- Use conservative safety margins for production
- Test cutting parameters on scrap material first
- Keep tool library up to date with actual tools
```

## 🎓 Learning Resources

### Video Tutorials
- **Getting Started**: Basic model import and organization
- **3D Visualization**: Camera controls and rendering options
- **CNC Integration**: Material optimization and G-Code generation
- **Advanced Features**: Custom workflows and automation

### Example Projects
- **Product Design**: Complete product development workflow
- **Manufacturing**: CNC machining preparation and optimization
- **Architecture**: Large model visualization and analysis
- **Education**: Learning 3D modeling and CNC concepts

## 🔄 Updates and Maintenance

### Software Updates
```text
1. Check for updates in Help menu
2. Backup settings before updating
3. Review changelog for new features
4. Update tool libraries and material data
```

### Database Maintenance
```text
1. Regular backups using --backup-database option
2. Monitor database size and performance
3. Archive old projects if needed
4. Optimize database periodically
```

### Performance Maintenance
```text
1. Clear thumbnail cache monthly
2. Review and update material database
3. Calibrate tool library with actual tools
4. Monitor system performance metrics
```

---

## 🎯 Next Steps

Now that you're familiar with the basics:

1. **Import Your Models**: Start building your 3D model library
2. **Explore Advanced Features**: Try the Design Canvas for 3D visualization
3. **Set Up CNC Integration**: Configure tool library and materials
4. **Customize Settings**: Tailor the application to your workflow
5. **Create Automation Scripts**: Streamline repetitive tasks

**Happy modeling and manufacturing!** 🚀

*For technical support and advanced features, check the [API Documentation](api/) and [Contributing Guide](contributing.md).*