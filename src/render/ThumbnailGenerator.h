#pragma once

#include "../core/BaseTypes.h"
#include "ModelLoader.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QSize>
#include <QPixmap>
#include <QFuture>
#include <QTimer>
#include <QQueue>
#include <QSet>

/**
 * @brief Thumbnail generation and caching system
 *
 * Generates high-quality thumbnails for 3D models with background processing,
 * caching, and performance optimization. Performance target: ≤500ms per thumbnail.
 */
class ThumbnailGenerator : public QObject
{
    Q_OBJECT

public:
    explicit ThumbnailGenerator(QObject* parent = nullptr);
    virtual ~ThumbnailGenerator() = default;

    // Thumbnail configuration
    struct ThumbnailConfig {
        QSize size;              // Thumbnail size (e.g., 256x256)
        QString backgroundType;  // "solid", "gradient", "grid", "hdri"
        QColor backgroundColor;  // Background color for solid backgrounds
        QString hdriPath;        // HDRI environment map path
        QVector3D lightDirection; // Light direction vector
        float lightIntensity;    // Light intensity multiplier
        bool useShadows;         // Enable shadow rendering
        bool useWireframe;       // Show wireframe overlay
        float wireframeOpacity;  // Wireframe opacity (0.0 - 1.0)
        QString outputFormat;    // "PNG", "JPG", "WEBP"
        int quality;             // JPEG quality (1-100) or PNG compression

        ThumbnailConfig()
            : size(256, 256), backgroundType("gradient"), backgroundColor(64, 64, 64),
              lightDirection(1, 1, 1), lightIntensity(1.0f), useShadows(true),
              useWireframe(false), wireframeOpacity(0.3f), outputFormat("PNG"), quality(90) {}
    };

    // Thumbnail generation
    virtual QFuture<QPixmap> generateThumbnailAsync(const QString& modelId, const ThumbnailConfig& config = ThumbnailConfig()) = 0;
    virtual QPixmap generateThumbnail(const QString& modelId, const ThumbnailConfig& config = ThumbnailConfig()) = 0;
    virtual bool generateThumbnailToFile(const QString& modelId, const QString& outputPath, const ThumbnailConfig& config = ThumbnailConfig()) = 0;

    // Batch operations
    virtual void generateThumbnailsForModels(const QStringList& modelIds, const ThumbnailConfig& config = ThumbnailConfig()) = 0;
    virtual void regenerateAllThumbnails() = 0;
    virtual void regenerateThumbnailsForQuery(const QString& query) = 0;

    // Thumbnail management
    virtual QString getThumbnailPath(const QString& modelId, const QSize& size = QSize(256, 256)) const = 0;
    virtual bool thumbnailExists(const QString& modelId, const QSize& size = QSize(256, 256)) const = 0;
    virtual QPixmap loadThumbnail(const QString& modelId, const QSize& size = QSize(256, 256)) = 0;
    virtual void deleteThumbnail(const QString& modelId, const QSize& size = QSize(256, 256)) = 0;

    // Configuration
    virtual void setDefaultConfig(const ThumbnailConfig& config) = 0;
    virtual ThumbnailConfig getDefaultConfig() const = 0;
    virtual void setCacheDirectory(const QString& directory) = 0;
    virtual QString getCacheDirectory() const = 0;

    // Background processing
    virtual void setMaxConcurrentGenerations(int max) = 0;
    virtual int getMaxConcurrentGenerations() const = 0;
    virtual void setGenerationPriority(int priority) = 0;
    virtual void pauseGeneration() = 0;
    virtual void resumeGeneration() = 0;

    // Performance monitoring
    virtual PerformanceMetrics getGenerationMetrics() const = 0;
    virtual void resetPerformanceMetrics() = 0;

    // Cache management
    virtual void clearThumbnailCache() = 0;
    virtual void cleanupOldThumbnails(qint64 maxAgeSeconds = 604800) = 0; // Default 7 days
    virtual qint64 getCacheSize() const = 0;

signals:
    // Generation progress
    void thumbnailGenerated(const QString& modelId, const QString& thumbnailPath);
    void thumbnailGenerationFailed(const QString& modelId, const QString& error);
    void batchGenerationProgress(int completed, int total);

    // Cache events
    void thumbnailCacheCleared();
    void thumbnailCacheCleaned(qint64 freedBytes);

    // Performance events
    void generationQueueChanged(int queuedCount);
    void performanceMetricsUpdated(const PerformanceMetrics& metrics);

protected:
    // Thumbnail generation implementation
    virtual QPixmap renderModelThumbnail(const ModelData& model, const ThumbnailConfig& config) = 0;
    virtual QPixmap createPlaceholderThumbnail(const QString& modelName, const ThumbnailConfig& config) = 0;
    virtual QString generateThumbnailFilename(const QString& modelId, const QSize& size) const = 0;

    // Background rendering
    virtual void startBackgroundGeneration(const QString& modelId, const ThumbnailConfig& config) = 0;
    virtual void processGenerationQueue() = 0;

    // Image processing
    virtual QPixmap applyPostProcessing(const QPixmap& thumbnail, const ThumbnailConfig& config) = 0;
    virtual QPixmap resizeThumbnail(const QPixmap& source, const QSize& targetSize) = 0;

    // Cache management
    virtual QString getThumbnailCachePath() const = 0;
    virtual void ensureCacheDirectory() = 0;
    virtual QString calculateCacheKey(const QString& modelId, const QSize& size) const = 0;

    // Generation queue
    QQueue<QString> m_generationQueue;
    QSet<QString> m_generatingModels;
    QTimer* m_batchTimer;

    // Configuration
    ThumbnailConfig m_defaultConfig;
    QString m_cacheDirectory;
    int m_maxConcurrentGenerations;
    int m_generationPriority;

    // Performance tracking
    PerformanceMetrics m_generationMetrics;
    qint64 m_totalGenerationTime;
    int m_generationCount;
};