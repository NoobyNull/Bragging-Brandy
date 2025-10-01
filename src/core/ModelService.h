#pragma once

#include "BaseTypes.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QFuture>
#include <QDir>

/**
 * @brief Core service for 3D model management operations
 *
 * Handles model loading, saving, metadata management, and file operations.
 * All model-related operations go through this service.
 */
class ModelService : public QObject
{
    Q_OBJECT

public:
    explicit ModelService(QObject* parent = nullptr);
    virtual ~ModelService() = default;

    // Model loading and saving
    virtual QFuture<ModelMetadata> loadModelAsync(const QString& filepath) = 0;
    virtual bool saveModel(const ModelMetadata& model, const QString& filepath) = 0;

    // Model metadata operations
    virtual QList<ModelMetadata> getAllModels() const = 0;
    virtual ModelMetadata getModel(const QUuid& id) const = 0;
    virtual bool updateModelMetadata(const ModelMetadata& model) = 0;
    virtual bool deleteModel(const QUuid& id) = 0;

    // Model search and filtering
    virtual QList<ModelMetadata> searchModels(const QString& query,
                                            const QStringList& tags = QStringList(),
                                            const QVariantMap& filters = QVariantMap()) const = 0;

    // Batch operations
    virtual bool tagModels(const QList<QUuid>& modelIds, const QStringList& tags) = 0;
    virtual bool untagModels(const QList<QUuid>& modelIds, const QStringList& tags) = 0;
    virtual bool deleteModels(const QList<QUuid>& modelIds) = 0;

    // Model statistics
    virtual qint64 getTotalModelsCount() const = 0;
    virtual qint64 getTotalModelsSize() const = 0;
    virtual QStringList getAllTags() const = 0;
    virtual QVariantMap getModelStatistics() const = 0;

    // Import/Export operations
    virtual QList<ModelMetadata> importModels(const QStringList& filepaths,
                                             const QString& targetDirectory = QString()) = 0;
    virtual bool exportModels(const QList<QUuid>& modelIds,
                            const QString& format,
                            const QString& outputDirectory) = 0;

    // File system operations
    virtual QString getModelFilePath(const QUuid& id) const = 0;
    virtual QString getThumbnailPath(const QUuid& id) const = 0;
    virtual bool isModelLoaded(const QUuid& id) const = 0;

    // Performance monitoring
    virtual PerformanceMetrics getServiceMetrics() const = 0;

signals:
    // Model events
    void modelLoaded(const ModelMetadata& model);
    void modelDeleted(const QUuid& id);
    void modelUpdated(const ModelMetadata& model);
    void modelsImported(const QList<ModelMetadata>& models);

    // Progress events
    void importProgress(const QString& filename, int percentage);
    void exportProgress(const QString& filename, int percentage);

    // Error events
    void errorOccurred(const QString& operation, const QString& error, const QString& details);

protected:
    // Helper methods
    virtual QString generateModelId() const;
    virtual QString sanitizeFilename(const QString& filename) const;
    virtual bool validateModelFile(const QString& filepath) const;
    virtual QString detectModelFormat(const QString& filepath) const;

    // Supported formats
    QStringList m_supportedFormats;
};