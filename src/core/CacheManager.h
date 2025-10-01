#pragma once

#include "BaseTypes.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QCache>
#include <QTimer>
#include <QMutex>
#include <QDir>

// Cache policy enumeration
enum class CachePolicy {
    LRU,           // Least Recently Used
    LFU,           // Least Frequently Used
    FIFO,          // First In, First Out
    SizeBased,     // Evict when size limit reached
    TimeBased      // Evict based on age
};

/**
 * @brief Cache manager for thumbnails, model previews, and temporary data
 *
 * Provides efficient caching with automatic cleanup, memory management,
 * and performance monitoring for cached resources.
 */
class CacheManager : public QObject
{
    Q_OBJECT

public:
    explicit CacheManager(QObject* parent = nullptr);
    virtual ~CacheManager() = default;

    // Cache configuration
    virtual void setMaxCacheSize(qint64 maxBytes) = 0;
    virtual void setCacheDirectory(const QString& directory) = 0;
    virtual void setCleanupInterval(qint64 seconds) = 0;

    // Thumbnail operations
    virtual QString getThumbnailPath(const QString& modelId, const QSize& size = QSize(256, 256)) = 0;
    virtual bool thumbnailExists(const QString& modelId, const QSize& size = QSize(256, 256)) const = 0;
    virtual void storeThumbnail(const QString& modelId, const QPixmap& thumbnail, const QSize& size = QSize(256, 256)) = 0;
    virtual QPixmap loadThumbnail(const QString& modelId, const QSize& size = QSize(256, 256)) = 0;

    // Model preview operations
    virtual QString getModelPreviewPath(const QString& modelId) = 0;
    virtual bool modelPreviewExists(const QString& modelId) const = 0;
    virtual void storeModelPreview(const QString& modelId, const QByteArray& previewData) = 0;
    virtual QByteArray loadModelPreview(const QString& modelId) = 0;

    // Generic cache operations
    virtual void storeData(const QString& key, const QVariant& data, qint64 cost = 1) = 0;
    virtual QVariant retrieveData(const QString& key) = 0;
    virtual void removeData(const QString& key) = 0;
    virtual bool containsData(const QString& key) const = 0;

    // Cache management
    virtual void clearCache() = 0;
    virtual void clearExpiredCache() = 0;
    virtual void optimizeCache() = 0;

    // Cache statistics
    virtual qint64 getCacheSize() const = 0;
    virtual qint64 getMaxCacheSize() const = 0;
    virtual int getCacheHitCount() const = 0;
    virtual int getCacheMissCount() const = 0;
    virtual QStringList getCachedKeys() const = 0;

    // Cache policies
    virtual void setCachePolicy(CachePolicy policy) = 0;
    virtual CachePolicy getCachePolicy() const = 0;

    // Automatic cleanup
    virtual void startAutoCleanup() = 0;
    virtual void stopAutoCleanup() = 0;

signals:
    // Cache events
    void thumbnailCached(const QString& modelId, const QString& path);
    void modelPreviewCached(const QString& modelId, const QString& path);
    void cacheCleared();
    void cacheOptimized(qint64 freedBytes);

    // Performance events
    void cacheHit(const QString& key);
    void cacheMiss(const QString& key);
    void cacheSizeChanged(qint64 newSize);

protected:
    // Cache implementation
    virtual QString generateCacheKey(const QString& modelId, const QSize& size) const;
    virtual QString generatePreviewKey(const QString& modelId) const;
    virtual QString getCacheFilePath(const QString& key) const;
    virtual bool isCacheFileExpired(const QString& filepath, qint64 maxAgeSeconds) const;

    // Memory management
    virtual void ensureCacheSpace(qint64 requiredBytes);
    virtual void evictLeastRecentlyUsed();
    virtual qint64 calculateCacheCost(const QVariant& data) const;

    // File operations
    virtual bool writeCacheFile(const QString& filepath, const QByteArray& data);
    virtual QByteArray readCacheFile(const QString& filepath);

    // Cache monitoring
    QTimer* m_cleanupTimer;
    QMutex m_cacheMutex;

    // Cache configuration
    qint64 m_maxCacheSize;
    QString m_cacheDirectory;
    qint64 m_cleanupInterval;
    CachePolicy m_policy;

    // Cache statistics
    mutable int m_hitCount;
    mutable int m_missCount;
    mutable qint64 m_currentSize;

    // Cache storage
    QCache<QString, QVariant> m_memoryCache;
};