# Performance Guide

## 🚀 Performance Overview

The 3D Model Management Utility is designed for high performance with large models and complex operations. This guide covers performance optimization, benchmarking, and troubleshooting.

## 📊 Performance Specifications

### Guaranteed Performance
- **Model Loading**: ≤10 seconds for models up to 500MB (progressive loading)
- **Memory Usage**: ≤2GB RAM for 500MB models (efficient streaming)
- **Interactive Performance**: 30+ FPS during navigation with LOD rendering
- **Search Performance**: ≤100ms for real-time model discovery
- **Thumbnail Generation**: ≤500ms per thumbnail with background processing
- **Mesh Repair**: ≤5 seconds for complex model repair

### System Requirements
- **Minimum**: Windows 10/11, 8GB RAM, OpenGL 4.6+ GPU
- **Recommended**: 16GB RAM, dedicated GPU with 4GB VRAM
- **Optimal**: 32GB RAM, high-end GPU with 8GB+ VRAM

## ⚡ Performance Optimization

### Memory Optimization

#### Automatic Memory Management
```cpp
// Configure memory limits
settings->setMemoryLimit(2048); // 2GB limit
settings->setCacheSize(1000);   // 1GB cache

// Enable progressive loading
settings->setProgressiveLoading(true);

// Monitor memory usage
qint64 memoryUsage = getCurrentMemoryUsage();
if (memoryUsage > memoryLimit * 0.9) {
    optimizeMemory();
}
```

#### Large Model Handling
```cpp
// Enable streaming for large models
ProgressiveLoader loader;
loader.setMaxMemoryUsage(2LL * 1024 * 1024 * 1024); // 2GB
loader.setChunkSize(10000); // 10k vertices per chunk

// Load large model progressively
QFuture<ModelData> future = loader.loadProgressiveAsync(largeModelPath);
```

### GPU Optimization

#### Hardware Acceleration
```cpp
// Enable hardware acceleration
settings->setHardwareAcceleration(true);

// Configure GPU adapter
settings->setGPUAdapter("dgpu"); // Use dedicated GPU

// Monitor GPU memory
PerformanceMetrics gpuMetrics = getGPUMetrics();
```

#### LOD Optimization
```cpp
// Configure LOD distances
LODRenderer lodRenderer;
LODLevel high("High", 10.0f, 1.0f, 500000);
LODLevel medium("Medium", 50.0f, 0.7f, 100000);
LODLevel low("Low", 200.0f, 0.4f, 25000);

lodRenderer.addLODLevel(high);
lodRenderer.addLODLevel(medium);
lodRenderer.addLODLevel(low);
```

### Search Optimization

#### Index Optimization
```cpp
// Rebuild search index for better performance
SearchService searchService;
searchService.rebuildIndex();

// Configure search options
QVariantMap searchOptions;
searchOptions["fuzzy_threshold"] = 0.6;
searchOptions["max_results"] = 100;
searchOptions["search_delay"] = 100;

searchService.setSearchOptions(searchOptions);
```

#### Cache Optimization
```cpp
// Configure search cache
CacheManager cacheManager;
cacheManager.setMaxCacheSize(500 * 1024 * 1024); // 500MB
cacheManager.setCleanupInterval(3600); // 1 hour
```

## 📈 Benchmarking

### Built-in Benchmarks

#### Model Loading Benchmark
```bash
# Run model loading benchmark
3dmodelmanager --benchmark loading --model-size 100MB
3dmodelmanager --benchmark loading --model-size 500MB
```

#### Search Performance Benchmark
```bash
# Run search benchmark
3dmodelmanager --benchmark search --query-count 1000
3dmodelmanager --benchmark search --database-size 10000
```

#### Rendering Performance Benchmark
```bash
# Run rendering benchmark
3dmodelmanager --benchmark rendering --model-count 10
3dmodelmanager --benchmark rendering --complexity high
```

### Custom Benchmarks

#### Performance Testing Code
```cpp
void benchmarkModelLoading() {
    QElapsedTimer timer;
    QStringList testFiles = {"small.stl", "medium.stl", "large.stl"};

    for (const QString& file : testFiles) {
        timer.start();

        ModelService modelService;
        QFuture<ModelMetadata> future = modelService.loadModelAsync(file);

        // Wait for completion
        future.waitForFinished();

        qint64 elapsed = timer.elapsed();
        qInfo() << QString("Loaded %1 in %2ms").arg(file).arg(elapsed);

        // Verify performance requirements
        QVERIFY(elapsed < getMaxLoadingTime(file));
    }
}
```

#### Memory Usage Benchmark
```cpp
void benchmarkMemoryUsage() {
    qint64 initialMemory = getCurrentMemoryUsage();

    // Load multiple large models
    for (int i = 0; i < 10; ++i) {
        ModelService modelService;
        modelService.loadModelAsync(QString("large_model_%1.stl").arg(i));
    }

    // Wait for loading to complete
    QThread::sleep(5);

    qint64 peakMemory = getCurrentMemoryUsage();
    qint64 memoryIncrease = peakMemory - initialMemory;

    qInfo() << QString("Memory increase: %1MB").arg(memoryIncrease / 1024 / 1024);

    // Verify memory efficiency
    QVERIFY(memoryIncrease < 2LL * 1024 * 1024 * 1024); // Less than 2GB
}
```

## 🔧 Performance Tuning

### Application Settings

#### Memory Settings
```text
Settings → Performance → Memory
- Memory Limit: 2048 MB (adjust based on system RAM)
- Cache Size: 1000 MB (adjust based on usage patterns)
- Max File Size: 500 MB (adjust based on needs)
```

#### Processing Settings
```text
Settings → Performance → Processing
- Parallel Processing: Enabled (for multi-core systems)
- Progressive Loading: Enabled (for large model support)
- Hardware Acceleration: Enabled (for GPU support)
```

#### Visualization Settings
```text
Settings → 3D Visualization → Level of Detail
- LOD Distance: 100 units (adjust based on scene complexity)
- Hardware Acceleration: Enabled
- Background Processing: Enabled
```

### System-Level Optimization

#### Windows Optimization
```cmd
# Increase virtual memory
wmic computersystem where name="%computername%" set AutomaticManagedPagefile=False
wmic pagefileset where name="C:\\pagefile.sys" set InitialSize=8192,MaximumSize=16384

# Optimize for performance
powercfg -setactive 8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c
```

#### Linux Optimization
```bash
# Increase file limits
ulimit -n 65536

# Optimize virtual memory
echo 'vm.swappiness=10' >> /etc/sysctl.conf
echo 'vm.vfs_cache_pressure=50' >> /etc/sysctl.conf

# Set CPU governor to performance
cpufreq-set -g performance
```

#### GPU Optimization
```bash
# Update GPU drivers
# Ubuntu/Debian
sudo apt update && sudo apt upgrade

# NVIDIA specific
nvidia-settings --assign CurrentMetaMode="nvidia-auto-select +0+0 { ForceFullCompositionPipeline=On }"

# AMD specific
amdconfig --set-power-mode high
```

## 📊 Performance Monitoring

### Real-Time Monitoring

#### FPS Monitoring
```cpp
// Monitor rendering performance
DesignCanvas canvas;
connect(&canvas, &DesignCanvas::frameRendered, this, [](int triangleCount, float renderTimeMs) {
    float fps = 1000.0f / renderTimeMs;
    if (fps < 30) {
        qWarning() << "Low FPS detected:" << fps;
        // Reduce quality or LOD
    }
});
```

#### Memory Monitoring
```cpp
// Monitor memory usage
QTimer memoryTimer;
connect(&memoryTimer, &QTimer::timeout, this, []() {
    qint64 memoryUsage = getCurrentMemoryUsage();
    qint64 memoryLimit = getMemoryLimit();

    if (memoryUsage > memoryLimit * 0.9) {
        qWarning() << "High memory usage:" << memoryUsage / 1024 / 1024 << "MB";
        optimizeMemory();
    }
});
```

### Performance Metrics

#### Collection
```cpp
// Collect comprehensive performance metrics
PerformanceMetrics collectMetrics() {
    PerformanceMetrics metrics;

    // System metrics
    metrics.memoryUsageBytes = getCurrentMemoryUsage();
    metrics.cpuUsagePercent = getCurrentCPUUsage();

    // Application metrics
    metrics.operationTimeMs = getAverageOperationTime();
    metrics.cacheHitRate = getCacheHitRate();

    // Rendering metrics
    metrics.fps = getCurrentFPS();
    metrics.triangleCount = getCurrentTriangleCount();

    return metrics;
}
```

#### Analysis
```cpp
// Analyze performance trends
void analyzePerformanceTrends(const QList<PerformanceMetrics>& history) {
    // Calculate averages
    float avgFPS = calculateAverageFPS(history);
    qint64 avgMemory = calculateAverageMemory(history);

    // Detect trends
    if (avgFPS < 30) {
        recommendPerformanceOptimization();
    }

    if (avgMemory > getMemoryLimit() * 0.8) {
        recommendMemoryOptimization();
    }
}
```

## 🛠️ Troubleshooting Performance Issues

### Slow Model Loading

#### Diagnosis
```bash
# Check file size and format
3dmodelmanager --analyze-file model.stl

# Check system resources
3dmodelmanager --system-info

# Check memory usage
3dmodelmanager --memory-info
```

#### Solutions
```text
1. Enable progressive loading in settings
2. Increase memory limit if sufficient RAM available
3. Check disk I/O performance
4. Verify model file integrity
5. Consider model optimization before import
```

### Low FPS During Navigation

#### Diagnosis
```cpp
// Check rendering performance
connect(&canvas, &DesignCanvas::frameRendered, this, [](int triangles, float timeMs) {
    float fps = 1000.0f / timeMs;
    if (fps < 30) {
        qDebug() << "Low FPS:" << fps << "Triangles:" << triangles;
    }
});
```

#### Solutions
```text
1. Reduce LOD distance in settings
2. Enable hardware acceleration
3. Reduce model complexity if possible
4. Check GPU drivers and performance
5. Consider upgrading GPU for complex models
```

### High Memory Usage

#### Diagnosis
```cpp
// Monitor memory usage
QTimer memoryMonitor;
connect(&memoryMonitor, &QTimer::timeout, this, []() {
    qint64 memory = getCurrentMemoryUsage();
    qDebug() << "Memory usage:" << memory / 1024 / 1024 << "MB";
});
```

#### Solutions
```text
1. Reduce cache size in settings
2. Enable progressive loading
3. Clear thumbnail cache regularly
4. Increase system RAM if possible
5. Monitor for memory leaks
```

### Slow Search Performance

#### Diagnosis
```bash
# Benchmark search performance
3dmodelmanager --benchmark search --query-count 1000

# Check search index
3dmodelmanager --search-info
```

#### Solutions
```text
1. Rebuild search index
2. Reduce fuzzy matching threshold
3. Limit search results
4. Optimize database performance
5. Consider search query optimization
```

## 📈 Performance Benchmarks

### Standard Benchmarks

#### Model Loading Benchmark
```bash
# Small model (10MB)
3dmodelmanager --benchmark loading --size small --iterations 10

# Medium model (100MB)
3dmodelmanager --benchmark loading --size medium --iterations 5

# Large model (500MB)
3dmodelmanager --benchmark loading --size large --iterations 3
```

#### Search Benchmark
```bash
# Search performance with different database sizes
3dmodelmanager --benchmark search --models 1000 --queries 100
3dmodelmanager --benchmark search --models 10000 --queries 100
3dmodelmanager --benchmark search --models 50000 --queries 100
```

#### Rendering Benchmark
```bash
# Rendering performance with different model complexities
3dmodelmanager --benchmark rendering --complexity low --models 5
3dmodelmanager --benchmark rendering --complexity medium --models 3
3dmodelmanager --benchmark rendering --complexity high --models 1
```

### Custom Benchmarking

#### Benchmark Framework
```cpp
class PerformanceBenchmark {
public:
    void runLoadingBenchmark() {
        QStringList testFiles = getTestFiles();

        for (const QString& file : testFiles) {
            BenchmarkResult result = benchmarkFileLoading(file);
            recordResult("loading", file, result);
        }
    }

    void runSearchBenchmark() {
        QStringList testQueries = getTestQueries();

        for (const QString& query : testQueries) {
            BenchmarkResult result = benchmarkSearch(query);
            recordResult("search", query, result);
        }
    }

private:
    BenchmarkResult benchmarkFileLoading(const QString& filepath) {
        QElapsedTimer timer;
        timer.start();

        ModelService service;
        QFuture<ModelMetadata> future = service.loadModelAsync(filepath);
        future.waitForFinished();

        return BenchmarkResult(timer.elapsed(), getCurrentMemoryUsage(), true);
    }
};
```

#### Automated Benchmarking
```bash
#!/bin/bash
# Automated performance regression testing

echo "Starting performance benchmarks..."

# Run benchmarks
3dmodelmanager --benchmark loading --output loading_results.json
3dmodelmanager --benchmark search --output search_results.json
3dmodelmanager --benchmark rendering --output rendering_results.json

# Compare with baseline
if [ -f "baseline.json" ]; then
    3dmodelmanager --compare-performance baseline.json *_results.json
fi

echo "Benchmarks completed"
```

## 🔧 Performance Configuration

### Configuration Files

#### Performance Settings
```json
{
  "performance": {
    "memory_limit": 2048,
    "cache_size": 1000,
    "max_file_size": 500,
    "parallel_processing": true,
    "progressive_loading": true,
    "hardware_acceleration": true,
    "lod_distance": 100,
    "search_timeout": 100,
    "thumbnail_quality": 85
  }
}
```

#### Optimization Profiles
```json
{
  "profiles": {
    "high_performance": {
      "memory_limit": 4096,
      "cache_size": 2000,
      "parallel_processing": true,
      "hardware_acceleration": true,
      "lod_distance": 50
    },
    "memory_constrained": {
      "memory_limit": 1024,
      "cache_size": 500,
      "parallel_processing": false,
      "progressive_loading": true,
      "lod_distance": 200
    },
    "quality_focused": {
      "memory_limit": 8192,
      "cache_size": 4000,
      "hardware_acceleration": true,
      "lod_distance": 25,
      "thumbnail_quality": 95
    }
  }
}
```

## 📊 Performance Analysis

### Performance Reports

#### Generation
```cpp
// Generate comprehensive performance report
void generatePerformanceReport() {
    PerformanceReport report;

    // Collect metrics
    report.systemMetrics = collectSystemMetrics();
    report.applicationMetrics = collectApplicationMetrics();
    report.benchmarkResults = runBenchmarks();

    // Analyze performance
    report.analysis = analyzePerformance(report);

    // Generate recommendations
    report.recommendations = generateRecommendations(report);

    // Export report
    exportPerformanceReport(report, "performance_report.json");
}
```

#### Analysis
```cpp
// Analyze performance data
PerformanceAnalysis analyzePerformance(const PerformanceReport& report) {
    PerformanceAnalysis analysis;

    // Check against requirements
    if (report.benchmarkResults.loadingTime > 10000) {
        analysis.issues.append("Model loading slower than 10 seconds");
        analysis.severity = "High";
    }

    if (report.benchmarkResults.searchTime > 100) {
        analysis.issues.append("Search slower than 100ms");
        analysis.severity = "Medium";
    }

    if (report.benchmarkResults.memoryUsage > 2LL * 1024 * 1024 * 1024) {
        analysis.issues.append("Memory usage exceeds 2GB limit");
        analysis.severity = "High";
    }

    return analysis;
}
```

## 🚨 Performance Alerts

### Automatic Alerts
```cpp
// Set up performance monitoring with alerts
void setupPerformanceAlerts() {
    // Memory alert
    connect(&memoryTimer, &QTimer::timeout, this, []() {
        qint64 memoryUsage = getCurrentMemoryUsage();
        qint64 memoryLimit = getMemoryLimit();

        if (memoryUsage > memoryLimit * 0.95) {
            emit performanceAlert("Critical memory usage", "High");
        }
    });

    // FPS alert
    connect(&fpsTimer, &QTimer::timeout, this, []() {
        float fps = getCurrentFPS();

        if (fps < 15) {
            emit performanceAlert("Very low FPS", "Critical");
        } else if (fps < 30) {
            emit performanceAlert("Low FPS", "Warning");
        }
    });
}
```

### Alert Configuration
```json
{
  "alerts": {
    "memory_threshold": 0.95,
    "fps_threshold": 30,
    "search_time_threshold": 100,
    "loading_time_threshold": 10000,
    "enable_alerts": true,
    "alert_cooldown": 300
  }
}
```

## 📈 Optimization Strategies

### For Large Models
```text
1. Enable progressive loading
2. Use appropriate LOD settings
3. Increase memory limit if sufficient RAM
4. Enable parallel processing
5. Consider model optimization before import
```

### For Many Models
```text
1. Optimize search index
2. Use thumbnail caching
3. Enable batch operations
4. Configure appropriate cache sizes
5. Consider database optimization
```

### For CNC Operations
```text
1. Use genetic algorithms for complex nesting
2. Enable parallel processing for large batches
3. Optimize material database
4. Use appropriate safety margins
5. Consider hardware acceleration
```

## 🔍 Performance Debugging

### Debug Mode
```bash
# Enable debug logging
3dmodelmanager --debug --verbose

# Log to file
3dmodelmanager --log-file debug.log --debug --verbose
```

### Performance Profiling
```cpp
// Profile specific operations
void profileOperation(const QString& operationName, const std::function<void()>& operation) {
    QElapsedTimer timer;
    timer.start();

    operation();

    qint64 elapsed = timer.elapsed();
    qDebug() << QString("Operation %1 took %2ms").arg(operationName).arg(elapsed);

    // Log to performance tracking
    logPerformance(operationName, elapsed);
}
```

### Memory Profiling
```cpp
// Track memory usage during operations
void profileMemoryUsage(const std::function<void()>& operation) {
    qint64 initialMemory = getCurrentMemoryUsage();

    operation();

    qint64 finalMemory = getCurrentMemoryUsage();
    qint64 memoryDelta = finalMemory - initialMemory;

    qDebug() << QString("Memory delta: %1MB").arg(memoryDelta / 1024 / 1024);
}
```

## 📋 Performance Checklist

### Pre-Deployment
- [ ] Verify all performance requirements are met
- [ ] Run comprehensive benchmark suite
- [ ] Test with representative models
- [ ] Validate memory usage patterns
- [ ] Check system resource utilization

### Production Monitoring
- [ ] Set up performance alerts
- [ ] Monitor memory usage trends
- [ ] Track operation timing
- [ ] Regular benchmark validation
- [ ] Performance regression testing

### Optimization
- [ ] Profile and optimize slow operations
- [ ] Tune memory and cache settings
- [ ] Optimize database performance
- [ ] Consider hardware upgrades if needed
- [ ] Regular performance reviews

---

*For detailed troubleshooting, check the [Troubleshooting Guide](troubleshooting.md). For system requirements and deployment, see the [Deployment Guide](deployment.md).*