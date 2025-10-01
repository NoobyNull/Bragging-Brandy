#pragma once

#include "BaseTypes.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>

/**
 * @brief File system manager for model storage and organization
 *
 * Handles file operations, directory management, and file system monitoring
 * for 3D models, thumbnails, and project data.
 */
class FileSystemManager : public QObject
{
    Q_OBJECT

public:
    explicit FileSystemManager(QObject* parent = nullptr);
    virtual ~FileSystemManager() = default;

    // Directory management
    virtual bool initializeDirectories() = 0;
    virtual QString getModelsDirectory() const = 0;
    virtual QString getThumbnailsDirectory() const = 0;
    virtual QString getProjectsDirectory() const = 0;
    virtual QString getCacheDirectory() const = 0;
    virtual QString getExportsDirectory() const = 0;

    // File operations
    virtual QString copyModelToStorage(const QString& sourcePath, const QString& filename = QString()) = 0;
    virtual bool deleteModelFromStorage(const QString& modelId) = 0;
    virtual QString getModelFilePath(const QString& modelId) const = 0;
    virtual QString getThumbnailPath(const QString& modelId) const = 0;

    // File information
    virtual QFileInfo getModelFileInfo(const QString& modelId) const = 0;
    virtual QFileInfo getThumbnailFileInfo(const QString& modelId) const = 0;
    virtual bool modelFileExists(const QString& modelId) const = 0;
    virtual bool thumbnailExists(const QString& modelId) const = 0;

    // Directory monitoring
    virtual void startMonitoring(const QString& directory) = 0;
    virtual void stopMonitoring(const QString& directory) = 0;

    // File validation
    virtual bool isValidModelFile(const QString& filepath) const = 0;
    virtual QStringList getSupportedModelExtensions() const = 0;
    virtual QString detectModelFormat(const QString& filepath) const = 0;

    // Storage management
    virtual qint64 getStorageUsage() const = 0;
    virtual qint64 getAvailableStorage() const = 0;
    virtual void cleanupCache(qint64 maxAgeSeconds = 86400) = 0;  // Default 24 hours
    virtual void optimizeStorage() = 0;

    // Import/Export support
    virtual QStringList scanForModels(const QString& directory, bool recursive = true) = 0;
    virtual bool exportModel(const QString& modelId, const QString& format, const QString& outputPath) = 0;
    virtual QStringList getExportFormats() const = 0;

signals:
    // File system events
    void modelFileAdded(const QString& filepath);
    void modelFileRemoved(const QString& filepath);
    void modelFileModified(const QString& filepath);
    void thumbnailGenerated(const QString& modelId, const QString& thumbnailPath);

    // Storage events
    void storageSpaceLow(qint64 availableBytes);
    void cacheCleaned(qint64 freedBytes);

    // Error events
    void fileOperationError(const QString& operation, const QString& filepath, const QString& error);

protected:
    // Helper methods
    virtual QString generateUniqueFilename(const QString& directory, const QString& baseName) const;
    virtual QString sanitizeFilename(const QString& filename) const;
    virtual bool createDirectoryStructure(const QString& basePath);
    virtual void updateStorageMetrics();

    // File system paths
    QString m_baseDirectory;
    QString m_modelsDirectory;
    QString m_thumbnailsDirectory;
    QString m_projectsDirectory;
    QString m_cacheDirectory;
    QString m_exportsDirectory;

    // Supported formats
    QStringList m_supportedExtensions;

    // Storage monitoring
    qint64 m_lastStorageCheck;
    qint64 m_storageUsageBytes;
    qint64 m_availableStorageBytes;
};