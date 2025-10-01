#include "ModelService.h"
#include "DatabaseManager.h"
#include "FileSystemManager.h"
#include "CacheManager.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFuture>
#include <QtConcurrent>
#include <QCoreApplication>
#include <QDebug>
#include <QRegularExpression>

ModelService::ModelService(QObject* parent)
    : QObject(parent)
{
    // Initialize supported formats
    m_supportedFormats << "stl" << "obj" << "ply" << "3mf" << "fbx" << "dae" << "gltf" << "glb";

    qRegisterMetaType<ModelMetadata>("ModelMetadata");
    qRegisterMetaType<QList<ModelMetadata>>("QList<ModelMetadata>");
}

QFuture<ModelMetadata> ModelService::loadModelAsync(const QString& filepath)
{
    return QtConcurrent::run([this, filepath]() -> ModelMetadata {
        QFileInfo fileInfo(filepath);

        if (!fileInfo.exists()) {
            emit errorOccurred("Load Model", filepath, "File does not exist");
            return ModelMetadata();
        }

        if (!isValidModelFile(filepath)) {
            emit errorOccurred("Load Model", filepath, "Unsupported file format");
            return ModelMetadata();
        }

        // Generate model ID
        QString modelId = generateModelId();

        // Create model metadata
        ModelMetadata model(modelId);
        model.filename = fileInfo.fileName();
        model.fileSize = fileInfo.size();
        model.importDate = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

        // Extract mesh statistics (placeholder for now)
        QVariantMap meshStats;
        meshStats["vertex_count"] = 0;
        meshStats["triangle_count"] = 0;
        meshStats["bounds"] = QVariantMap{
            {"x", 0.0}, {"y", 0.0}, {"z", 0.0}
        };
        model.meshStats = meshStats;

        // Copy file to storage
        FileSystemManager* fsManager = qobject_cast<FileSystemManager*>(parent());
        if (fsManager) {
            QString storedPath = fsManager->copyModelToStorage(filepath, model.filename);
            if (storedPath.isEmpty()) {
                emit errorOccurred("Load Model", filepath, "Failed to copy file to storage");
                return ModelMetadata();
            }
        }

        // Store in database
        DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
        if (dbManager) {
            if (!dbManager->insertModel(model)) {
                emit errorOccurred("Load Model", filepath, "Failed to store model metadata");
                return ModelMetadata();
            }
        }

        emit modelLoaded(model);
        return model;
    });
}

bool ModelService::saveModel(const ModelMetadata& model, const QString& filepath)
{
    // For now, this is a placeholder
    // In a full implementation, this would export the model in the specified format
    qWarning() << "Model export not yet implemented";
    return false;
}

QList<ModelMetadata> ModelService::getAllModels() const
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (dbManager) {
        return dbManager->getAllModels();
    }
    return QList<ModelMetadata>();
}

ModelMetadata ModelService::getModel(const QUuid& id) const
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (dbManager) {
        return dbManager->getModel(id);
    }
    return ModelMetadata(id);
}

bool ModelService::updateModelMetadata(const ModelMetadata& model)
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (dbManager) {
        bool success = dbManager->updateModel(model);
        if (success) {
            emit modelUpdated(model);
        }
        return success;
    }
    return false;
}

bool ModelService::deleteModel(const QUuid& id)
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    FileSystemManager* fsManager = qobject_cast<FileSystemManager*>(parent());

    if (dbManager && dbManager->deleteModel(id)) {
        if (fsManager) {
            fsManager->deleteModelFromStorage(id.toString());
        }

        // Clear from cache
        CacheManager* cacheManager = qobject_cast<CacheManager*>(parent());
        if (cacheManager) {
            cacheManager->removeData(id.toString());
        }

        emit modelDeleted(id);
        return true;
    }

    return false;
}

QList<ModelMetadata> ModelService::searchModels(const QString& query,
                                              const QStringList& tags,
                                              const QVariantMap& filters) const
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (dbManager) {
        return dbManager->searchModels(query, tags);
    }
    return QList<ModelMetadata>();
}

bool ModelService::tagModels(const QList<QUuid>& modelIds, const QStringList& tags)
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (dbManager) {
        bool success = true;
        for (const QUuid& id : modelIds) {
            ModelMetadata model = dbManager->getModel(id);
            QStringList currentTags = model.tags;

            // Add new tags
            for (const QString& tag : tags) {
                if (!currentTags.contains(tag)) {
                    currentTags.append(tag);
                }
            }

            model.tags = currentTags;
            if (!dbManager->updateModel(model)) {
                success = false;
            }
        }

        if (success) {
            emit modelsTagged(modelIds, tags);
        }

        return success;
    }
    return false;
}

bool ModelService::untagModels(const QList<QUuid>& modelIds, const QStringList& tags)
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (dbManager) {
        bool success = true;
        for (const QUuid& id : modelIds) {
            ModelMetadata model = dbManager->getModel(id);
            QStringList currentTags = model.tags;

            // Remove specified tags
            for (const QString& tag : tags) {
                currentTags.removeAll(tag);
            }

            model.tags = currentTags;
            if (!dbManager->updateModel(model)) {
                success = false;
            }
        }

        return success;
    }
    return false;
}

bool ModelService::deleteModels(const QList<QUuid>& modelIds)
{
    bool success = true;
    for (const QUuid& id : modelIds) {
        if (!deleteModel(id)) {
            success = false;
        }
    }
    return success;
}

qint64 ModelService::getTotalModelsCount() const
{
    QList<ModelMetadata> models = getAllModels();
    return models.count();
}

qint64 ModelService::getTotalModelsSize() const
{
    QList<ModelMetadata> models = getAllModels();
    qint64 totalSize = 0;
    for (const ModelMetadata& model : models) {
        totalSize += model.fileSize;
    }
    return totalSize;
}

QStringList ModelService::getAllTags() const
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (dbManager) {
        return dbManager->getAllTags();
    }
    return QStringList();
}

QVariantMap ModelService::getModelStatistics() const
{
    QVariantMap stats;
    stats["total_models"] = getTotalModelsCount();
    stats["total_size"] = getTotalModelsSize();
    stats["supported_formats"] = m_supportedFormats;

    // Calculate format distribution
    QVariantMap formatStats;
    QList<ModelMetadata> models = getAllModels();
    for (const ModelMetadata& model : models) {
        QString extension = QFileInfo(model.filename).suffix().toLower();
        formatStats[extension] = formatStats[extension].toInt() + 1;
    }
    stats["format_distribution"] = formatStats;

    return stats;
}

QList<ModelMetadata> ModelService::importModels(const QStringList& filepaths,
                                              const QString& targetDirectory)
{
    QList<ModelMetadata> importedModels;

    for (const QString& filepath : filepaths) {
        QFileInfo fileInfo(filepath);

        if (!fileInfo.exists() || !isValidModelFile(filepath)) {
            emit errorOccurred("Import Models", filepath, "Invalid file");
            continue;
        }

        // Load model asynchronously and wait for result
        QFuture<ModelMetadata> future = loadModelAsync(filepath);
        future.waitForFinished();

        ModelMetadata model = future.result();
        if (!model.id.isNull()) {
            importedModels.append(model);
        }
    }

    if (!importedModels.isEmpty()) {
        emit modelsImported(importedModels);
    }

    return importedModels;
}

bool ModelService::exportModels(const QList<QUuid>& modelIds,
                              const QString& format,
                              const QString& outputDirectory)
{
    // Placeholder for model export functionality
    qWarning() << "Model export not yet implemented";
    return false;
}

QString ModelService::getModelFilePath(const QUuid& id) const
{
    FileSystemManager* fsManager = qobject_cast<FileSystemManager*>(parent());
    if (fsManager) {
        return fsManager->getModelFilePath(id.toString());
    }
    return QString();
}

QString ModelService::getThumbnailPath(const QUuid& id) const
{
    CacheManager* cacheManager = qobject_cast<CacheManager*>(parent());
    if (cacheManager) {
        return cacheManager->getThumbnailPath(id.toString());
    }
    return QString();
}

bool ModelService::isModelLoaded(const QUuid& id) const
{
    return !getModel(id).id.isNull();
}

PerformanceMetrics ModelService::getServiceMetrics() const
{
    PerformanceMetrics metrics;
    metrics.operationType = "ModelService";
    metrics.operationTimeMs = 0;  // Would be calculated from actual operations
    metrics.memoryUsageBytes = 0;  // Would be tracked
    return metrics;
}

QString ModelService::generateModelId() const
{
    return QUuid::createUuid().toString();
}

QString ModelService::sanitizeFilename(const QString& filename) const
{
    QString sanitized = filename;
    // Remove or replace invalid characters
    sanitized.replace(QRegularExpression("[<>:\"|?*]"), "_");
    return sanitized;
}

bool ModelService::validateModelFile(const QString& filepath) const
{
    QFileInfo fileInfo(filepath);

    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        return false;
    }

    if (fileInfo.size() == 0) {
        return false;
    }

    QString extension = fileInfo.suffix().toLower();
    return m_supportedFormats.contains(extension);
}

QString ModelService::detectModelFormat(const QString& filepath) const
{
    QFileInfo fileInfo(filepath);
    return fileInfo.suffix().toLower();
}