# Contributing Guide

We welcome contributions from the CNC and 3D printing community! This guide outlines how to contribute effectively to the CNC Catalogue & 3D Visualization Environment.

## Ways to Contribute

### 🐛 Bug Reports
- **Reproduction Steps**: Clear, step-by-step instructions
- **Expected vs Actual**: What should happen vs what happens
- **Environment**: Windows version, GPU, Qt version, application version
- **Logs**: Include relevant log entries from the Log Viewer widget

### 💡 Feature Requests
- **Use Case**: Describe the problem you're trying to solve
- **Proposed Solution**: How you think it should work
- **Alternatives**: Other approaches you've considered
- **Impact**: How this affects your workflow

### 📝 Documentation Improvements
- **Clarifications**: Unclear instructions or concepts
- **Missing Information**: Gaps in current documentation
- **Examples**: Additional use cases or tutorials
- **Translations**: Help translate to other languages

### 🔧 Code Contributions
- **Bug Fixes**: Implement solutions for reported issues
- **New Features**: Add functionality that aligns with project goals
- **Performance**: Optimize existing code paths
- **Tests**: Improve test coverage and reliability

## Development Workflow

### Prerequisites

Before contributing code, ensure you have:

- **Development Environment**: [Build Guide](build-guide.md)
- **Git**: Familiarity with version control
- **C++**: Modern C++ (C++17 or later) knowledge
- **Qt**: Experience with Qt framework
- **DirectX**: Basic understanding (for graphics contributions)

### Getting Started

1. **Fork the Repository**: Create your own copy on GitHub
2. **Clone Locally**: `git clone <your-fork-url>`
3. **Create Branch**: `git checkout -b feature/your-feature-name`
4. **Set Up Development Environment**: Follow [Build Guide](build-guide.md)

### Development Standards

#### Code Style

**C++ Standards**
```cpp
// ✅ Good: Modern C++ style
auto result = std::make_unique<MeshProcessor>();
const auto& vertices = mesh.getVertices();
if (const auto repaired = repairMesh(mesh)) {
    return repaired.value();
}

// ❌ Avoid: C-style, raw pointers
MeshProcessor* processor = new MeshProcessor();
std::vector<Vertex>* verts = mesh->getVertices();
if (repairMesh(mesh, &result)) {
    return result;
}
```

**Qt Best Practices**
```cpp
// ✅ Good: QObject hierarchy, signals/slots
class MeshViewer : public QWidget {
    Q_OBJECT
public:
    explicit MeshViewer(QWidget* parent = nullptr);

signals:
    void meshLoaded(const QString& filename);
    void repairCompleted(int fixCount);

private slots:
    void onLoadButtonClicked();
    void onRepairFinished();
};

// ❌ Avoid: No parent, manual connections
class BadMeshViewer : public QWidget {
public:
    BadMeshViewer() {
        connect(button, SIGNAL(clicked()),
                this, SLOT(onClicked()));  // Old-style connect
    }
};
```

#### Commit Messages

```
feat: add mesh repair progress reporting

- Add progress callback to MeshRepair class
- Update UI to show repair progress
- Add cancellation support for long repairs
- Update tests for new progress API

Closes #123
```

**Commit Format**: Use [Conventional Commits](https://conventionalcommits.org/)
- `feat:`: New feature
- `fix:`: Bug fix
- `docs:`: Documentation changes
- `test:`: Test additions/changes
- `refactor:`: Code restructuring
- `perf:`: Performance improvements
- `ci:`: CI/CD changes

#### Testing Requirements

**Unit Tests**
- Every public method must have tests
- Mock external dependencies (GPU, file I/O)
- Test both success and failure paths
- Minimum 80% code coverage

**Integration Tests**
- Widget interaction testing
- File import/export workflows
- Performance benchmarks
- Cross-platform compatibility

**Example Test Structure**
```cpp
TEST(MeshRepair, HandlesLargeModels) {
    // Arrange
    auto largeModel = LoadTestModel("large_stl_file.stl");

    // Act
    auto result = repairMesh(largeModel);

    // Assert
    EXPECT_TRUE(result.has_value());
    EXPECT_LT(result->repairTime, std::chrono::seconds(2));
    EXPECT_EQ(result->fixCount, 15);
}
```

### Submitting Changes

#### Pull Request Process

1. **Create Pull Request**: From your feature branch to main
2. **Fill Template**: Use provided PR template
3. **Description**: Explain what and why, not how
4. **Testing**: Include test results and performance benchmarks
5. **Documentation**: Update relevant docs

#### Code Review Process

**Review Criteria**
- **Functionality**: Does it work as intended?
- **Performance**: Meets project guarantees?
- **Code Quality**: Follows project standards?
- **Tests**: Adequate test coverage?
- **Documentation**: Updated appropriately?

**Review Timeline**
- Initial review: Within 2 business days
- Follow-up reviews: Within 1 business day
- Approval: When all criteria met

#### PR Template

```markdown
## Description

Brief description of changes and motivation.

## Related Issues

Closes #123, relates to #456

## Testing

- [ ] Unit tests added/updated
- [ ] Integration tests pass
- [ ] Performance benchmarks meet requirements
- [ ] Manual testing completed

## Performance Impact

- Model load time: No change
- Memory usage: +2% for large models
- Repair time: -15% improvement

## Screenshots

Before/after screenshots if UI changes.

## Checklist

- [ ] Code follows project style guide
- [ ] Tests written and passing
- [ ] Documentation updated
- [ ] Performance requirements met
- [ ] Reviewed by at least one maintainer
```

## Issue Reporting

### Bug Report Template

```markdown
## Description

Brief description of the bug.

## Steps to Reproduce

1. Step one
2. Step two
3. Expected result
4. Actual result

## Environment

- OS: Windows 11 Pro
- GPU: NVIDIA RTX 3080
- Qt Version: 6.2.4
- Application Version: 1.0.0

## Logs

```
[Paste relevant log entries here]
```

## Additional Context

Any other relevant information.
```

### Feature Request Template

```markdown
## Problem Statement

Describe the problem you're trying to solve.

## Proposed Solution

How you think this should be implemented.

## Use Cases

- Primary use case
- Secondary use cases
- Edge cases

## Implementation Considerations

- Technical challenges
- Performance implications
- UI/UX impact

## Alternatives Considered

Other approaches you've thought about.
```

## Community Guidelines

### Communication

**Respectful Discourse**
- Be constructive in feedback
- Assume good intent
- Focus on solutions, not problems

**Inclusive Language**
- Use gender-neutral terms
- Be mindful of different skill levels
- Provide context for technical terms

### Getting Help

**Before Asking**
- Check existing documentation
- Search GitHub issues (open and closed)
- Review recent discussions

**When Asking**
- Be specific about your question
- Include relevant context
- Show what you've tried already

### Recognition

Contributors who make significant impacts may be:
- **Acknowledged** in release notes
- **Invited** to become maintainers
- **Featured** in community highlights

## Project Governance

### Decision Making

**Technical Decisions**
- Must be justified with benchmarks or testing
- Should align with project constitution
- Require maintainer approval for breaking changes

**Feature Inclusion**
- Must demonstrate clear user value
- Should not violate project boundaries
- Must have reasonable implementation complexity

### Maintainer Responsibilities

**Code Review**
- Review all pull requests
- Ensure standards compliance
- Test changes before merging

**Release Management**
- Coordinate release schedules
- Write release notes
- Manage version bumps

**Community Management**
- Respond to issues and discussions
- Moderate community spaces
- Organize community events

## Advanced Contributions

### Performance Contributions

**Benchmarking**
- Use provided benchmarking tools
- Compare against baseline measurements
- Document performance improvements

**Optimization Guidelines**
- Profile before optimizing
- Target bottlenecks, not micro-optimizations
- Maintain readability and maintainability

### Graphics Contributions

**DirectX Knowledge Required**
- Understanding of DirectX 12 pipeline
- Experience with compute shaders
- Knowledge of GPU architectures

**Testing Requirements**
- Cross-GPU compatibility testing
- Performance regression testing
- Visual quality validation

### Plugin Development

**API Design**
- Follow Qt plugin architecture
- Provide stable interfaces
- Include comprehensive documentation

**Distribution**
- Plugin metadata and versioning
- Dependency management
- Installation/uninstallation support

## Legal and Licensing

### Contributor License Agreement

By contributing, you agree that:
- Your contributions are your original work
- You have the right to contribute the work
- Your contributions are licensed under the project license

### Intellectual Property

- **Trademarks**: Respect project and third-party trademarks
- **Patents**: Be aware of software patents in your jurisdiction
- **Attribution**: Provide proper attribution for borrowed ideas

## Getting Help

### Resources
- **Constitution**: [Project Principles](../CONSTITUTION.md)
- **Build Guide**: [Development Setup](build-guide.md)
- **API Docs**: [API Reference](api/)
- **Code Style**: [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)

### Contact
- **Issues**: [GitHub Issues](../../../issues)
- **Discussions**: [GitHub Discussions](../../../discussions)
- **Email**: [maintainers@cnccatalogue.dev](mailto:maintainers@cnccatalogue.dev)

---

Thank you for contributing to the CNC Catalogue & 3D Visualization Environment! Your efforts help make this a better tool for the entire CNC community.