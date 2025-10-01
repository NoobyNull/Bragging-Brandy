#pragma once

#include "BaseTypes.h"
#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QVariantMap>
#include <QList>

/**
 * @brief Database manager for SQLite operations
 *
 * Handles all database operations including model metadata, project data,
 * tags, and application settings. Provides thread-safe access to the database.
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject* parent = nullptr);
    virtual ~DatabaseManager();

    // Database lifecycle
    virtual bool initialize(const QString& databasePath = QString()) = 0;
    virtual void close() = 0;
    virtual bool isInitialized() const = 0;

    // Model operations
    virtual bool insertModel(const ModelMetadata& model) = 0;
    virtual bool updateModel(const ModelMetadata& model) = 0;
    virtual bool deleteModel(const QUuid& id) = 0;
    virtual ModelMetadata getModel(const QUuid& id) const = 0;
    virtual QList<ModelMetadata> getAllModels() const = 0;
    virtual QList<ModelMetadata> searchModels(const QString& query,
                                            const QStringList& tags = QStringList()) const = 0;

    // Project operations
    virtual bool insertProject(const ProjectData& project) = 0;
    virtual bool updateProject(const ProjectData& project) = 0;
    virtual bool deleteProject(const QUuid& id) = 0;
    virtual ProjectData getProject(const QUuid& id) const = 0;
    virtual QList<ProjectData> getAllProjects() const = 0;

    // Tag operations
    virtual bool insertModelTags(const QUuid& modelId, const QStringList& tags) = 0;
    virtual bool insertProjectTags(const QUuid& projectId, const QStringList& tags) = 0;
    virtual QStringList getModelTags(const QUuid& modelId) const = 0;
    virtual QStringList getProjectTags(const QUuid& projectId) const = 0;
    virtual QStringList getAllTags() const = 0;
    virtual QMap<QString, int> getTagUsageCounts() const = 0;

    // Settings operations
    virtual bool saveSetting(const QString& key, const QVariant& value) = 0;
    virtual QVariant getSetting(const QString& key, const QVariant& defaultValue = QVariant()) const = 0;
    virtual QVariantMap getAllSettings() const = 0;
    virtual bool deleteSetting(const QString& key) = 0;

    // Database maintenance
    virtual bool optimize() = 0;
    virtual bool backup(const QString& backupPath) = 0;
    virtual bool restore(const QString& backupPath) = 0;
    virtual qint64 getDatabaseSize() const = 0;
    virtual QStringList getTableNames() const = 0;

    // Transaction management
    virtual bool beginTransaction() = 0;
    virtual bool commitTransaction() = 0;
    virtual bool rollbackTransaction() = 0;

    // Migration support
    virtual bool runMigrations() = 0;
    virtual QString getDatabaseVersion() const = 0;

signals:
    // Database events
    void databaseInitialized();
    void databaseClosed();
    void databaseError(const QString& error, const QString& details);

    // Data change events
    void modelInserted(const ModelMetadata& model);
    void modelUpdated(const ModelMetadata& model);
    void modelDeleted(const QUuid& id);
    void projectInserted(const ProjectData& project);
    void projectUpdated(const ProjectData& project);
    void projectDeleted(const QUuid& id);

protected:
    // Database schema
    virtual bool createTables() = 0;
    virtual bool createIndexes() = 0;

    // Migration system
    virtual bool migrateFromVersion(const QString& fromVersion) = 0;

    // Utility methods
    virtual QString sanitizeString(const QString& input) const;
    virtual QVariantMap modelToVariantMap(const ModelMetadata& model) const;
    virtual ModelMetadata variantMapToModel(const QVariantMap& map) const;
    virtual QVariantMap projectToVariantMap(const ProjectData& project) const;
    virtual ProjectData variantMapToProject(const QVariantMap& map) const;

    // Database connection
    QSqlDatabase m_database;
    QString m_databasePath;
    bool m_isInitialized;

    // Schema version
    static const QString CURRENT_SCHEMA_VERSION;
};