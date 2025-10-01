#include "LODRenderer.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QElapsedTimer>
#include <QDebug>
#include <cmath>

LODRenderer::LODRenderer(QObject* parent)
    : QObject(parent)
    , m_cameraPosition(0, 0, 10)
    , m_fov(60.0f)
    , m_aspectRatio(16.0f / 9.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(1000.0f)
    , m_viewportSize(1920, 1080)
    , m_adaptiveQuality(true)
    , m_targetFPS(60.0f)
    , m_qualityBias(0.0f)
    , m_currentFPS(60.0f)
    , m_frameCount(0)
    , m_lastFrameTime(0)
{
    // Create performance monitoring timer
    m_performanceTimer = new QTimer(this);
    m_performanceTimer->setInterval(1000); // Update every second
    connect(m_performanceTimer, &QTimer::timeout, this, &LODRenderer::updatePerformanceMetrics);
    m_performanceTimer->start();

    // Initialize default LOD levels
    initializeDefaultLODLevels();

    qRegisterMetaType<QList<LODLevel>>("QList<LODLevel>");
}

LODRenderer::~LODRenderer()
{
    // Cleanup handled by Qt parent-child system
}

void LODRenderer::initializeDefaultLODLevels()
{
    // High detail (close objects)
    LODLevel high("High", 10.0f, 1.0f, 500000);
    high.useNormals = true;
    high.useTextures = true;
    high.useLighting = true;

    // Medium detail (medium distance)
    LODLevel medium("Medium", 50.0f, 0.7f, 100000);
    medium.useNormals = true;
    medium.useTextures = true;
    medium.useLighting = true;

    // Low detail (far objects)
    LODLevel low("Low", 200.0f, 0.4f, 25000);
    low.useNormals = true;
    low.useTextures = false;
    low.useLighting = false;

    // Ultra low detail (very far objects)
    LODLevel ultraLow("UltraLow", 1000.0f, 0.2f, 5000);
    ultraLow.useNormals = false;
    ultraLow.useTextures = false;
    ultraLow.useLighting = false;

    m_lodLevels << high << medium << low << ultraLow;
}

void LODRenderer::addLODLevel(const LODLevel& level)
{
    // Remove existing level with same name
    removeLODLevel(level.name);

    // Insert in order based on distance threshold
    auto insertPos = std::lower_bound(m_lodLevels.begin(), m_lodLevels.end(), level,
        [](const LODLevel& a, const LODLevel& b) {
            return a.distanceThreshold < b.distanceThreshold;
        });

    m_lodLevels.insert(insertPos, level);
}

void LODRenderer::removeLODLevel(const QString& name)
{
    for (int i = 0; i < m_lodLevels.size(); ++i) {
        if (m_lodLevels[i].name == name) {
            m_lodLevels.removeAt(i);
            break;
        }
    }
}

void LODRenderer::clearLODLevels()
{
    m_lodLevels.clear();
    m_currentLODLevels.clear();
}

QList<LODRenderer::LODLevel> LODRenderer::getLODLevels() const
{
    return m_lodLevels;
}

void LODRenderer::setCameraPosition(const QVector3D& position)
{
    m_cameraPosition = position;
    forceLODUpdate();
}

void LODRenderer::setCameraParameters(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    m_fov = fov;
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    forceLODUpdate();
}

void LODRenderer::setViewportSize(const QSize& size)
{
    m_viewportSize = size;
    forceLODUpdate();
}

void LODRenderer::processModelForLOD(ModelData& model)
{
    // Generate LOD variants for each mesh
    for (MeshData& mesh : model.meshes) {
        generateLODMeshVariants(mesh);
    }
}

MeshData LODRenderer::generateLODVariant(const MeshData& originalMesh, const LODLevel& lodLevel)
{
    MeshData lodMesh = originalMesh;

    // Apply quality multiplier
    float qualityFactor = lodLevel.qualityMultiplier;

    if (qualityFactor < 1.0f) {
        // Generate simplified version
        lodMesh = generateDecimatedMesh(originalMesh, qualityFactor);
    }

    // Apply LOD-specific settings
    if (!lodLevel.useNormals) {
        // Remove normals to save memory and computation
        for (Vertex& vertex : lodMesh.vertices) {
            vertex.normal = QVector3D(0, 0, 1); // Default normal
        }
    }

    if (!lodLevel.useTextures) {
        // Clear texture coordinates
        for (Vertex& vertex : lodMesh.vertices) {
            vertex.texCoord = QVector2D(0, 0);
        }
    }

    return lodMesh;
}

void LODRenderer::setAdaptiveQuality(bool enabled)
{
    m_adaptiveQuality = enabled;
}

void LODRenderer::setTargetFPS(float fps)
{
    m_targetFPS = qMax(15.0f, fps); // Minimum 15 FPS
}

void LODRenderer::setQualityBias(float bias)
{
    m_qualityBias = qBound(-1.0f, bias, 1.0f);
}

PerformanceMetrics LODRenderer::getRenderingMetrics() const
{
    return m_renderingMetrics;
}

void LODRenderer::resetPerformanceMetrics()
{
    m_renderingMetrics = PerformanceMetrics();
    m_frameCount = 0;
    m_currentFPS = 60.0f;
}

void LODRenderer::updateAdaptiveLOD()
{
    if (!m_adaptiveQuality) {
        return;
    }

    // Adapt based on current FPS
    adaptLODToPerformance(m_currentFPS);

    // Adapt based on memory availability
    adaptLODToMemory(getAvailableMemory());
}

void LODRenderer::forceLODUpdate()
{
    // Force update of all LOD selections
    // This would iterate through all visible meshes and update their LOD levels
    qDebug() << "Forcing LOD update for all meshes";
}

MeshData LODRenderer::generateDecimatedMesh(const MeshData& original, float qualityFactor)
{
    MeshData decimated = original;

    // Calculate target triangle count based on quality factor
    int targetTriangles = static_cast<int>(original.triangleCount * qualityFactor);

    if (targetTriangles >= original.triangleCount) {
        return decimated; // No decimation needed
    }

    return generateSimplifiedMesh(decimated, targetTriangles);
}

MeshData LODRenderer::generateSimplifiedMesh(const MeshData& original, int targetTriangles)
{
    MeshData simplified = original;

    if (targetTriangles >= original.triangleCount) {
        return simplified; // No simplification needed
    }

    // Simple mesh simplification by removing triangles
    float simplificationRatio = static_cast<float>(targetTriangles) / original.triangleCount;

    // Remove triangles based on simplification ratio
    int trianglesToRemove = original.indices.size() / 3 - targetTriangles;

    if (trianglesToRemove > 0) {
        // Remove every Nth triangle
        int step = original.indices.size() / (trianglesToRemove * 3);

        QVector<unsigned int> newIndices;
        for (int i = 0; i < original.indices.size(); i += step * 3) {
            if (newIndices.size() / 3 >= targetTriangles) {
                break;
            }

            // Add triangle if it's within our target
            if (i + 2 < original.indices.size()) {
                newIndices.append(original.indices[i]);
                newIndices.append(original.indices[i + 1]);
                newIndices.append(original.indices[i + 2]);
            }
        }

        simplified.indices = newIndices;
        simplified.triangleCount = newIndices.size() / 3;
    }

    return simplified;
}

float LODRenderer::calculateDistanceToCamera(const QVector3D& meshCenter) const
{
    return (meshCenter - m_cameraPosition).length();
}

float LODRenderer::calculateScreenSpaceSize(const QVector3D& meshCenter, const QVector3D& meshExtents) const
{
    // Calculate screen space size based on distance and mesh size
    float distance = calculateDistanceToCamera(meshCenter);

    if (distance < m_nearPlane) {
        return 1.0f; // Very close, maximum size
    }

    // Project mesh extents to screen space
    float meshRadius = meshExtents.length() * 0.5f;

    // Simple projection calculation
    float fovRadians = qDegreesToRadians(m_fov * 0.5f);
    float screenSpaceSize = (meshRadius / distance) / qTan(fovRadians);

    return qBound(0.0f, screenSpaceSize, 1.0f);
}

void LODRenderer::adaptLODToPerformance(float currentFPS)
{
    if (currentFPS < m_targetFPS * 0.8f) {
        // Performance is poor, reduce quality
        float qualityReduction = (m_targetFPS - currentFPS) / m_targetFPS;

        // Apply quality bias
        if (m_qualityBias > 0) {
            qualityReduction *= (1.0f - m_qualityBias * 0.5f); // Reduce quality reduction if quality-biased
        } else if (m_qualityBias < 0) {
            qualityReduction *= (1.0f + (-m_qualityBias) * 0.5f); // Increase quality reduction if performance-biased
        }

        // Reduce LOD distances to use lower quality levels sooner
        for (LODLevel& level : m_lodLevels) {
            level.distanceThreshold *= (1.0f - qualityReduction * 0.3f);
        }

        emit qualityAdapted(1.0f - qualityReduction);
        emit lodSwitched("Performance adaptation");

    } else if (currentFPS > m_targetFPS * 1.2f) {
        // Performance is good, can increase quality
        float qualityIncrease = (currentFPS - m_targetFPS) / m_targetFPS;

        // Apply quality bias
        if (m_qualityBias < 0) {
            qualityIncrease *= (1.0f + (-m_qualityBias) * 0.5f); // Increase quality more if performance-biased
        } else if (m_qualityBias > 0) {
            qualityIncrease *= (1.0f - m_qualityBias * 0.5f); // Reduce quality increase if quality-biased
        }

        // Increase LOD distances to use higher quality levels longer
        for (LODLevel& level : m_lodLevels) {
            level.distanceThreshold *= (1.0f + qualityIncrease * 0.3f);
        }

        emit qualityAdapted(1.0f + qualityIncrease);
        emit lodSwitched("Performance adaptation");
    }
}

void LODRenderer::adaptLODToMemory(qint64 availableMemory)
{
    // Adapt LOD based on available memory
    qint64 memoryThreshold = 500 * 1024 * 1024; // 500MB

    if (availableMemory < memoryThreshold) {
        // Low memory, reduce quality
        float memoryPressure = 1.0f - (availableMemory / static_cast<float>(memoryThreshold));

        for (LODLevel& level : m_lodLevels) {
            level.distanceThreshold *= (1.0f - memoryPressure * 0.5f);
            level.maxTriangles = static_cast<int>(level.maxTriangles * (1.0f - memoryPressure * 0.3f));
        }

        emit qualityAdapted(1.0f - memoryPressure);
        emit lodSwitched("Memory adaptation");
    }
}

QString LODRenderer::selectLODLevel(const QString& meshName, float distance, float screenSize)
{
    // Find the appropriate LOD level based on distance and screen space
    for (const LODLevel& level : m_lodLevels) {
        if (distance <= level.distanceThreshold && screenSize >= level.screenSpaceThreshold) {
            return level.name;
        }
    }

    // Return lowest quality level if no match
    return m_lodLevels.last().name;
}

void LODRenderer::switchMeshLOD(MeshData& mesh, const QString& lodLevel)
{
    // Switch mesh to specified LOD level
    // In a full implementation, this would swap vertex/index buffers

    QString currentLOD = m_currentLODLevels.value(mesh.name, "High");

    if (currentLOD != lodLevel) {
        m_currentLODLevels[mesh.name] = lodLevel;
        emit lodLevelChanged(mesh.name, lodLevel);

        qDebug() << QString("Switched mesh %1 from %2 to %3")
                    .arg(mesh.name).arg(currentLOD).arg(lodLevel);
    }
}

void LODRenderer::updatePerformanceMetrics()
{
    // Calculate current FPS
    qint64 currentTime = QDateTime::currentTimeMSecs();
    qint64 elapsed = currentTime - m_lastFrameTime;

    if (elapsed > 0) {
        float instantFPS = 1000.0f / elapsed;

        // Smooth FPS calculation
        if (m_frameCount == 0) {
            m_currentFPS = instantFPS;
        } else {
            m_currentFPS = m_currentFPS * 0.9f + instantFPS * 0.1f;
        }
    }

    m_lastFrameTime = currentTime;
    m_frameCount++;

    // Update performance metrics
    m_renderingMetrics.operationTimeMs = 1000.0f / m_currentFPS;
    m_renderingMetrics.memoryUsageBytes = getCurrentMemoryUsage();

    // Check for performance issues
    if (m_currentFPS < m_targetFPS * 0.7f) {
        emit performanceWarning(QString("Low FPS: %1 (target: %2)").arg(m_currentFPS).arg(m_targetFPS));
    }

    // Adapt LOD if adaptive quality is enabled
    if (m_adaptiveQuality) {
        updateAdaptiveLOD();
    }
}

qint64 LODRenderer::getAvailableMemory() const
{
    // Simplified memory check
    // In a full implementation, this would query system memory information
    return 1024 * 1024 * 1024; // 1GB default
}

qint64 LODRenderer::getCurrentMemoryUsage() const
{
    // Simplified memory usage calculation
    // In a full implementation, this would track actual GPU and system memory usage
    return 256 * 1024 * 1024; // 256MB default
}

void LODRenderer::generateLODMeshVariants(MeshData& mesh)
{
    // Generate LOD variants for the mesh
    for (const LODLevel& lodLevel : m_lodLevels) {
        if (lodLevel.qualityMultiplier < 1.0f) {
            MeshData lodVariant = generateLODVariant(mesh, lodLevel);

            // In a full implementation, this would store the LOD variant
            // For now, just update the mesh with the highest quality variant
            if (lodLevel.qualityMultiplier > 0.8f) {
                mesh = lodVariant;
            }
        }
    }
}