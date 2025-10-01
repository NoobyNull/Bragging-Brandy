# API Documentation

This directory contains comprehensive API documentation for the CNC Catalogue & 3D Visualization Environment.

## Structure

### Core APIs
- **[Core Library](core/)**: Fundamental data structures and utilities
- **[Mesh Processing](mesh/)**: 3D model loading, repair, and optimization
- **[Rendering](render/)**: DirectX 12 rendering pipeline
- **[Widgets](widgets/)**: Reusable UI components
- **[Project Management](projects/)**: Catalogue and metadata management

### Integration APIs
- **[Plugin System](plugins/)**: Extending the application with custom components
- **[File Formats](formats/)**: Supported import/export formats
- **[GPU Acceleration](gpu/)**: Hardware acceleration interfaces

## Documentation Formats

### Doxygen Documentation
Primary API documentation is generated using Doxygen:

```bash
# Generate documentation
doxygen Doxyfile

# Output in docs/api/doxygen/
```

### Architecture Decision Records
Important design decisions are documented in [ADRs](../../docs/adr/):

- [0001-use-qt-directx-architecture.md](../../docs/adr/0001-use-qt-directx-architecture.md)
- [0002-widget-based-ui-design.md](../../docs/adr/0002-widget-based-ui-design.md)

## Getting Started

### For Plugin Developers

1. **Review Plugin Architecture**: [Plugin System Guide](plugins/)
2. **Study Examples**: [Example Plugins](../../examples/plugins/)
3. **Follow Widget Guidelines**: [Widget Development](widgets/development.md)

### For Application Developers

1. **Core Concepts**: [Core Library Overview](core/)
2. **Mesh Processing**: [Working with 3D Models](mesh/)
3. **Custom Rendering**: [Graphics Pipeline](render/)

## API Stability

### Versioning
- **Stable APIs**: Marked as `@stable` in documentation
- **Beta APIs**: Marked as `@beta`, subject to change
- **Internal APIs**: Not documented, for internal use only

### Deprecation Policy
- Deprecated APIs marked with `@deprecated`
- Maintained for at least one major version
- Migration guides provided for breaking changes

## Examples

See [Examples Directory](../../examples/) for:
- Basic widget implementation
- Custom mesh processor
- Plugin development
- Performance optimization

## Support

For API-related questions:
- **Issues**: [GitHub Issues](../../../issues)
- **Discussions**: [API Discussion](../../../discussions/categories/api)
- **Email**: [api-support@cnccatalogue.dev](mailto:api-support@cnccatalogue.dev)