#pragma once

#include "ModelLoader.h"
#include <QObject>
#include <QString>
#include <QVector>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QTimer>

/**
 * @brief Level of Detail rendering system for performance optimization
 *
 * Automatically adjusts mesh detail based on distance from camera,
 * screen space size, and performance requirements.
 */
class LODRenderer : public QObject
{
    Q_OBJECT

public:
    explicit LODRenderer(QObject* parent = nullptr);
    virtual ~LODRenderer() = default;

    // LOD configuration
    struct LODLevel {
        QString name;
        float distanceThreshold;    // Distance at which this LOD becomes active
        float screenSpaceThreshold; // Screen space size threshold
        float qualityMultiplier;    // Quality multiplier (0.0 - 1.0)
        int maxTriangles;          // Maximum triangles for this level
        bool useNormals;           // Whether to use normal vectors
        bool useTextures;          // Whether to use textures
        bool useLighting;          // Whether to use lighting calculations

        LODLevel(const QString& name = "Level",
                float distance = 100.0f,
                float quality = 1.0f,
                int maxTris = 1000000)
            : name(name), distanceThreshold(distance), qualityMultiplier(quality),
              maxTriangles(maxTris), useNormals(true), useTextures(true), useLighting(true) {}
    };

    // LOD management
    virtual void addLODLevel(const LODLevel& level) = 0;
    virtual void removeLODLevel(const QString& name) = 0;
    virtual void clearLODLevels() = 0;
    virtual QList<LODLevel> getLODLevels() const = 0;

    // Rendering configuration
    virtual void setCameraPosition(const QVector3D& position) = 0;
    virtual void setCameraParameters(float fov, float aspectRatio, float nearPlane, float farPlane) = 0;
    virtual void setViewportSize(const QSize& size) = 0;

    // Model LOD processing
    virtual void processModelForLOD(ModelData& model) = 0;
    virtual MeshData generateLODVariant(const MeshData& originalMesh, const LODLevel& lodLevel) = 0;

    // Adaptive quality
    virtual void setAdaptiveQuality(bool enabled) = 0;
    virtual void setTargetFPS(float fps) = 0;
    virtual void setQualityBias(float bias) = 0; // -1.0 = performance, 1.0 = quality

    // Performance monitoring
    virtual PerformanceMetrics getRenderingMetrics() const = 0;
    virtual void resetPerformanceMetrics() = 0;

    // Real-time adaptation
    virtual void updateAdaptiveLOD() = 0;
    virtual void forceLODUpdate() = 0;

signals:
    // LOD events
    void lodLevelChanged(const QString& meshName, const QString& lodLevel);
    void qualityAdapted(float newQuality);
    void performanceWarning(const QString& warning);

    // Rendering events
    void frameRendered(int triangleCount, float renderTimeMs);
    void lodSwitched(const QString& reason);

protected:
    // LOD generation algorithms
    virtual MeshData generateDecimatedMesh(const MeshData& original, float qualityFactor) = 0;
    virtual MeshData generateSimplifiedMesh(const MeshData& original, int targetTriangles) = 0;

    // Distance and screen space calculations
    virtual float calculateDistanceToCamera(const QVector3D& meshCenter) const = 0;
    virtual float calculateScreenSpaceSize(const QVector3D& meshCenter, const QVector3D& meshExtents) const = 0;

    // Performance-based adaptation
    virtual void adaptLODToPerformance(float currentFPS) = 0;
    virtual void adaptLODToMemory(qint64 availableMemory) = 0;

    // LOD selection
    virtual QString selectLODLevel(const QString& meshName, float distance, float screenSize) = 0;
    virtual void switchMeshLOD(MeshData& mesh, const QString& lodLevel) = 0;

    // Camera and viewport
    QVector3D m_cameraPosition;
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
    QSize m_viewportSize;

    // LOD levels
    QList<LODLevel> m_lodLevels;
    QMap<QString, QString> m_currentLODLevels; // mesh name -> current LOD level

    // Adaptive settings
    bool m_adaptiveQuality;
    float m_targetFPS;
    float m_qualityBias;

    // Performance tracking
    QTimer* m_performanceTimer;
    float m_currentFPS;
    qint64 m_frameCount;
    qint64 m_lastFrameTime;
    PerformanceMetrics m_renderingMetrics;
};