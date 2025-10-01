# Command Line Interface Guide

## 📋 Overview

The 3D Model Management Utility provides a comprehensive command-line interface for headless operation, automation, and batch processing. All features are accessible via CLI for integration into automated workflows.

## 🚀 Basic Usage

```bash
# Display help
3dmodelmanager --help

# Show version
3dmodelmanager --version

# Show system information
3dmodelmanager --system-info
```

## 💾 Database Operations

### Reset Database
```bash
# Reset the project database (WARNING: Deletes all data)
3dmodelmanager --reset-database
```

### Backup and Restore
```bash
# Backup database
3dmodelmanager --backup-database /path/to/backup.db

# Restore from backup
3dmodelmanager --restore-database /path/to/backup.db

# Backup with automatic timestamp
3dmodelmanager --backup-database
```

## ⚙️ Settings Management

### Reset Settings
```bash
# Reset all settings to defaults
3dmodelmanager --reset-settings
```

### Import/Export Settings
```bash
# Export current settings
3dmodelmanager --export-settings /path/to/settings.json

# Import settings
3dmodelmanager --import-settings /path/to/settings.json

# Export with automatic timestamp
3dmodelmanager --export-settings
```

## 📦 Model Operations

### Import Models
```bash
# Import single model
3dmodelmanager --import /path/to/model.stl

# Import multiple models
3dmodelmanager --import model1.stl model2.obj model3.ply

# Import all models in directory
3dmodelmanager --import /path/to/models/*.stl
```

### Export Models
```bash
# Export models to STL format
3dmodelmanager --export /path/to/export.stl --export-format stl

# Export to OBJ format
3dmodelmanager --export /path/to/export.obj --export-format obj

# Export with automatic filename
3dmodelmanager --export --export-format stl
```

### Repair Models
```bash
# Auto-repair all models
3dmodelmanager --repair auto

# Assisted repair mode
3dmodelmanager --repair assist

# Manual repair only
3dmodelmanager --repair manual
```

### Generate Thumbnails
```bash
# Generate thumbnails for all models
3dmodelmanager --generate-thumbnails

# Generate after import
3dmodelmanager --import models/*.stl --generate-thumbnails
```

## 🏭 CNC Operations

### Generate G-Code
```bash
# Generate G-Code files
3dmodelmanager --generate-gcode --output-dir /path/to/gcode/

# Generate with specific settings
3dmodelmanager --generate-gcode --material aluminum --tool endmill_025
```

### Optimize Nesting
```bash
# Run nesting optimization
3dmodelmanager --optimize-nesting --output-dir /path/to/output/

# Optimize with specific sheet size
3dmodelmanager --optimize-nesting --sheet-width 48 --sheet-height 96
```

## ⚡ Performance Options

### GPU Configuration
```bash
# Use specific GPU adapter
3dmodelmanager --gpu auto          # Automatic selection
3dmodelmanager --gpu cpu           # CPU-only rendering
3dmodelmanager --gpu dgpu          # Dedicated GPU
3dmodelmanager --gpu adapter_id    # Specific adapter ID
```

### Memory Management
```bash
# Set memory limit
3dmodelmanager --max-memory 4096   # 4GB limit
3dmodelmanager --max-memory 8192   # 8GB limit
```

### Hardware Acceleration
```bash
# Disable hardware acceleration
3dmodelmanager --disable-hw-accel
```

## 📊 Logging Options

### Verbose Logging
```bash
# Enable verbose logging
3dmodelmanager --verbose [operation]

# Enable debug logging
3dmodelmanager --debug [operation]

# Set custom log level
3dmodelmanager --log-level debug [operation]
3dmodelmanager --log-level info [operation]
```

### Log File Output
```bash
# Log to specific file
3dmodelmanager --log-file /path/to/log.txt [operation]

# Log with automatic timestamp
3dmodelmanager --log-file [operation]
```

## 🔄 Batch Operations

### Batch Scripts
```bash
# Execute batch script
3dmodelmanager --batch /path/to/batch_script.txt

# Example batch script content:
# import models/*.stl
# repair auto
# generate-thumbnails
# optimize-nesting
# generate-gcode
```

### Pipeline Operations
```bash
# Complete workflow pipeline
3dmodelmanager --batch workflow.txt

# Workflow script example:
echo "import models/*.stl" > workflow.txt
echo "repair auto" >> workflow.txt
echo "generate-thumbnails" >> workflow.txt
echo "optimize-nesting" >> workflow.txt
echo "generate-gcode" >> workflow.txt
```

## 🎯 Advanced Examples

### Complete CNC Workflow
```bash
#!/bin/bash
# Complete CNC preparation workflow

echo "Starting CNC workflow..."

# Import models
3dmodelmanager --import parts/*.stl --verbose

# Repair models
3dmodelmanager --repair auto --log-file repair.log

# Generate thumbnails
3dmodelmanager --generate-thumbnails

# Optimize nesting
3dmodelmanager --optimize-nesting --output-dir nesting_results/

# Generate G-Code
3dmodelmanager --generate-gcode --output-dir gcode_output/

echo "CNC workflow completed!"
```

### Quality Assurance Pipeline
```bash
#!/bin/bash
# Automated quality assurance pipeline

# Import test models
3dmodelmanager --import test_models/*.stl

# Run mesh repair
3dmodelmanager --repair auto

# Validate models
3dmodelmanager --validate-models

# Generate reports
3dmodelmanager --export-report qa_report.pdf

# Archive results
3dmodelmanager --backup-database qa_backup.db
```

### Production Deployment
```bash
#!/bin/bash
# Production environment setup

# Reset and configure database
3dmodelmanager --reset-database
3dmodelmanager --import-settings production_settings.json

# Import tool library
3dmodelmanager --import-tools tools/production_tools.json

# Import material database
3dmodelmanager --import-materials materials/production_materials.json

# Import production models
3dmodelmanager --import production_models/*.stl

# Generate optimized layouts
3dmodelmanager --optimize-nesting --output-dir production_output/

echo "Production environment ready!"
```

## 🔧 Configuration Files

### Settings File Format
```json
{
  "general": {
    "theme": "dark",
    "language": "en",
    "auto_save": true,
    "auto_save_interval": 5
  },
  "performance": {
    "memory_limit": 4096,
    "cache_size": 2048,
    "parallel_processing": true,
    "max_file_size": 500
  },
  "cnc": {
    "default_units": "inches",
    "safety_margin": 0.2,
    "tool_library_path": "/path/to/tools",
    "auto_calculate": true
  }
}
```

### Batch Script Format
```text
# Comment lines start with #
import models/*.stl
repair auto
generate-thumbnails
optimize-nesting
generate-gcode --output-dir ./output
export-report summary.pdf
```

## 📊 Output and Reports

### Console Output
```bash
$ 3dmodelmanager --import models/*.stl --generate-thumbnails
3D Model Management Utility v1.0.0
Importing 15 models...
Model import completed: 15 successful, 0 failed
Generating thumbnails...
Thumbnail generation completed: 15 generated, 0 failed
Operation completed successfully
```

### Log File Output
```text
[2025-10-01 10:30:15] INFO Application: 3D Model Management Utility starting
[2025-10-01 10:30:15] INFO Database: Database initialized successfully
[2025-10-01 10:30:16] INFO ModelService: Model loaded: part1.stl (1.2 MB)
[2025-10-01 10:30:16] DEBUG SearchService: Search index rebuilt: 150 models
[2025-10-01 10:30:17] INFO ThumbnailGenerator: Thumbnail generated: part1.png (45ms)
```

## 🆘 Error Handling

### Exit Codes
- **0**: Success
- **1**: General error
- **2**: Invalid arguments
- **3**: File not found
- **4**: Permission denied
- **5**: Operation failed

### Error Examples
```bash
$ 3dmodelmanager --import nonexistent.stl
Error: Import file does not exist: nonexistent.stl
Exit code: 3

$ 3dmodelmanager --export-format invalid
Error: Unsupported export format: invalid
Exit code: 2
```

## 🔍 Troubleshooting

### Common Issues

#### Import Failures
```bash
# Check file format support
3dmodelmanager --validate-format model.stl

# Check file integrity
3dmodelmanager --analyze-file model.stl

# Try with repair
3dmodelmanager --import model.stl --repair auto
```

#### Performance Issues
```bash
# Check memory usage
3dmodelmanager --memory-info

# Optimize settings
3dmodelmanager --max-memory 8192

# Clear caches
3dmodelmanager --clear-caches
```

#### CNC Export Problems
```bash
# Validate tool library
3dmodelmanager --validate-tools

# Check material database
3dmodelmanager --validate-materials

# Generate with conservative settings
3dmodelmanager --generate-gcode --safety-margin 0.3
```

## 📈 Performance Monitoring

### Memory Usage
```bash
# Display current memory usage
3dmodelmanager --memory-info

# Set memory limit
3dmodelmanager --max-memory 4096 [operation]
```

### Operation Timing
```bash
# Enable timing for operations
3dmodelmanager --verbose --log-level debug [operation]

# Check log file for detailed timing
cat application.log | grep "Performance:"
```

### System Resources
```bash
# Display system information
3dmodelmanager --system-info

# Check GPU status
3dmodelmanager --gpu-info
```

## 🔄 Automation Examples

### Daily Backup
```bash
#!/bin/bash
# Daily backup script

DATE=$(date +%Y%m%d)
BACKUP_DIR="/backups"

# Backup database
3dmodelmanager --backup-database "$BACKUP_DIR/models_$DATE.db"

# Export settings
3dmodelmanager --export-settings "$BACKUP_DIR/settings_$DATE.json"

# Cleanup old backups (keep 7 days)
find "$BACKUP_DIR" -name "*.db" -mtime +7 -delete
find "$BACKUP_DIR" -name "*.json" -mtime +7 -delete

echo "Daily backup completed: $DATE"
```

### Batch Model Processing
```bash
#!/bin/bash
# Process incoming models

INPUT_DIR="/incoming"
OUTPUT_DIR="/processed"
LOG_FILE="/logs/batch_$(date +%Y%m%d_%H%M%S).log"

# Process all STL files
3dmodelmanager --import "$INPUT_DIR"/*.stl \
               --repair auto \
               --generate-thumbnails \
               --export "$OUTPUT_DIR"/export.stl \
               --log-file "$LOG_FILE" \
               --verbose

# Generate report
3dmodelmanager --export-report "$OUTPUT_DIR/processing_report.pdf"

echo "Batch processing completed"
```

### CNC Production Run
```bash
#!/bin/bash
# CNC production preparation

PROJECT_NAME="production_run_001"
OUTPUT_DIR="/cnc_output/$PROJECT_NAME"

# Import production models
3dmodelmanager --import production_models/*.stl

# Optimize for current material
3dmodelmanager --optimize-nesting \
               --material aluminum_6061 \
               --sheet-size 48x96 \
               --output-dir "$OUTPUT_DIR"

# Generate G-Code
3dmodelmanager --generate-gcode \
               --tool endmill_025 \
               --output-dir "$OUTPUT_DIR/gcode"

# Generate documentation
3dmodelmanager --export-report "$OUTPUT_DIR/README.pdf"

echo "Production run prepared: $PROJECT_NAME"
```

## 📞 Getting Help

### Command Help
```bash
# General help
3dmodelmanager --help

# Help for specific option
3dmodelmanager --help | grep -A 5 -B 5 "import"

# Examples
3dmodelmanager --help | grep -A 10 "EXAMPLES:"
```

### Online Resources
- **Documentation**: Check the full [User Guide](../user-guide.md)
- **API Reference**: See [API Documentation](../api/)
- **Troubleshooting**: Check [Troubleshooting Guide](../troubleshooting.md)
- **Examples**: See [Examples Directory](../examples/)

---

*For more advanced usage and automation, check the [API Documentation](../api/) and [Contributing Guide](../contributing.md).*