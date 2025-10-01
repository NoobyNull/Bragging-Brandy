#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

// Schema version for migrations
const QString DatabaseManager::CURRENT_SCHEMA_VERSION = "1.0.0";

DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent)
    , m_isInitialized(false)
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
}

DatabaseManager::~DatabaseManager()
{
    close();
}

bool DatabaseManager::initialize(const QString& databasePath)
{
    if (m_isInitialized) {
        qWarning() << "Database already initialized";
        return true;
    }

    QString dbPath = databasePath;
    if (dbPath.isEmpty()) {
        // Use default path in application data directory
        QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        dbPath = dataDir + "/models.db";
    }

    m_databasePath = dbPath;
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        qCritical() << "Failed to open database:" << m_database.lastError().text();
        emit databaseError("Database Open Failed", m_database.lastError().text());
        return false;
    }

    // Enable foreign keys and WAL mode for better performance
    QSqlQuery pragmaQuery(m_database);
    pragmaQuery.exec("PRAGMA foreign_keys = ON");
    pragmaQuery.exec("PRAGMA journal_mode = WAL");
    pragmaQuery.exec("PRAGMA synchronous = NORMAL");
    pragmaQuery.exec("PRAGMA cache_size = 10000");
    pragmaQuery.exec("PRAGMA temp_store = MEMORY");

    // Create tables and indexes
    if (!createTables()) {
        qCritical() << "Failed to create database tables";
        return false;
    }

    if (!createIndexes()) {
        qCritical() << "Failed to create database indexes";
        return false;
    }

    // Run migrations if needed
    if (!runMigrations()) {
        qCritical() << "Failed to run database migrations";
        return false;
    }

    m_isInitialized = true;
    emit databaseInitialized();

    qInfo() << "Database initialized successfully:" << dbPath;
    return true;
}

void DatabaseManager::close()
{
    if (m_isInitialized) {
        m_database.close();
        m_isInitialized = false;
        emit databaseClosed();
        qInfo() << "Database closed";
    }
}

bool DatabaseManager::isInitialized() const
{
    return m_isInitialized;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);

    // Models table
    QString createModelsTable =
        "CREATE TABLE IF NOT EXISTS models ("
        "id TEXT PRIMARY KEY,"
        "filename TEXT NOT NULL,"
        "file_size INTEGER NOT NULL,"
        "import_date TEXT NOT NULL,"
        "thumbnail_path TEXT,"
        "mesh_stats TEXT,"  // JSON string
        "created_date TEXT DEFAULT CURRENT_TIMESTAMP,"
        "modified_date TEXT DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (!query.exec(createModelsTable)) {
        qCritical() << "Failed to create models table:" << query.lastError().text();
        return false;
    }

    // Projects table
    QString createProjectsTable =
        "CREATE TABLE IF NOT EXISTS projects ("
        "id TEXT PRIMARY KEY,"
        "name TEXT NOT NULL,"
        "description TEXT,"
        "created_date TEXT NOT NULL,"
        "metadata TEXT,"  // JSON string
        "modified_date TEXT DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (!query.exec(createProjectsTable)) {
        qCritical() << "Failed to create projects table:" << query.lastError().text();
        return false;
    }

    // Project-Model relationships table
    QString createProjectModelsTable =
        "CREATE TABLE IF NOT EXISTS project_models ("
        "project_id TEXT NOT NULL,"
        "model_id TEXT NOT NULL,"
        "added_date TEXT DEFAULT CURRENT_TIMESTAMP,"
        "PRIMARY KEY (project_id, model_id),"
        "FOREIGN KEY (project_id) REFERENCES projects(id) ON DELETE CASCADE,"
        "FOREIGN KEY (model_id) REFERENCES models(id) ON DELETE CASCADE"
        ")";

    if (!query.exec(createProjectModelsTable)) {
        qCritical() << "Failed to create project_models table:" << query.lastError().text();
        return false;
    }

    // Tags table
    QString createTagsTable =
        "CREATE TABLE IF NOT EXISTS tags ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT UNIQUE NOT NULL,"
        "category TEXT,"
        "color TEXT,"
        "created_date TEXT DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (!query.exec(createTagsTable)) {
        qCritical() << "Failed to create tags table:" << query.lastError().text();
        return false;
    }

    // Model-Tag relationships table
    QString createModelTagsTable =
        "CREATE TABLE IF NOT EXISTS model_tags ("
        "model_id TEXT NOT NULL,"
        "tag_id INTEGER NOT NULL,"
        "assigned_date TEXT DEFAULT CURRENT_TIMESTAMP,"
        "PRIMARY KEY (model_id, tag_id),"
        "FOREIGN KEY (model_id) REFERENCES models(id) ON DELETE CASCADE,"
        "FOREIGN KEY (tag_id) REFERENCES tags(id) ON DELETE CASCADE"
        ")";

    if (!query.exec(createModelTagsTable)) {
        qCritical() << "Failed to create model_tags table:" << query.lastError().text();
        return false;
    }

    // Project-Tag relationships table
    QString createProjectTagsTable =
        "CREATE TABLE IF NOT EXISTS project_tags ("
        "project_id TEXT NOT NULL,"
        "tag_id INTEGER NOT NULL,"
        "assigned_date TEXT DEFAULT CURRENT_TIMESTAMP,"
        "PRIMARY KEY (project_id, tag_id),"
        "FOREIGN KEY (project_id) REFERENCES projects(id) ON DELETE CASCADE,"
        "FOREIGN KEY (tag_id) REFERENCES tags(id) ON DELETE CASCADE"
        ")";

    if (!query.exec(createProjectTagsTable)) {
        qCritical() << "Failed to create project_tags table:" << query.lastError().text();
        return false;
    }

    // Settings table
    QString createSettingsTable =
        "CREATE TABLE IF NOT EXISTS settings ("
        "key TEXT PRIMARY KEY,"
        "value TEXT NOT NULL,"
        "created_date TEXT DEFAULT CURRENT_TIMESTAMP,"
        "modified_date TEXT DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (!query.exec(createSettingsTable)) {
        qCritical() << "Failed to create settings table:" << query.lastError().text();
        return false;
    }

    // Schema version table
    QString createSchemaTable =
        "CREATE TABLE IF NOT EXISTS schema_version ("
        "version TEXT PRIMARY KEY,"
        "applied_date TEXT DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (!query.exec(createSchemaTable)) {
        qCritical() << "Failed to create schema_version table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::createIndexes()
{
    QSqlQuery query(m_database);

    // Indexes for models table
    QStringList modelIndexes = {
        "CREATE INDEX IF NOT EXISTS idx_models_filename ON models(filename)",
        "CREATE INDEX IF NOT EXISTS idx_models_import_date ON models(import_date)",
        "CREATE INDEX IF NOT EXISTS idx_models_file_size ON models(file_size)",
        "CREATE INDEX IF NOT EXISTS idx_models_created_date ON models(created_date)"
    };

    for (const QString& indexQuery : modelIndexes) {
        if (!query.exec(indexQuery)) {
            qCritical() << "Failed to create model index:" << query.lastError().text();
            return false;
        }
    }

    // Indexes for projects table
    QStringList projectIndexes = {
        "CREATE INDEX IF NOT EXISTS idx_projects_name ON projects(name)",
        "CREATE INDEX IF NOT EXISTS idx_projects_created_date ON projects(created_date)"
    };

    for (const QString& indexQuery : projectIndexes) {
        if (!query.exec(indexQuery)) {
            qCritical() << "Failed to create project index:" << query.lastError().text();
            return false;
        }
    }

    // Indexes for tags table
    QStringList tagIndexes = {
        "CREATE INDEX IF NOT EXISTS idx_tags_name ON tags(name)",
        "CREATE INDEX IF NOT EXISTS idx_tags_category ON tags(category)"
    };

    for (const QString& indexQuery : tagIndexes) {
        if (!query.exec(indexQuery)) {
            qCritical() << "Failed to create tag index:" << query.lastError().text();
            return false;
        }
    }

    // Full-text search index for models
    QString ftsQuery = "CREATE VIRTUAL TABLE IF NOT EXISTS models_fts USING fts5("
                      "filename, content=models, content_rowid=id)";

    if (!query.exec(ftsQuery)) {
        qCritical() << "Failed to create FTS table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::runMigrations()
{
    // Check current schema version
    QSqlQuery versionQuery("SELECT version FROM schema_version ORDER BY applied_date DESC LIMIT 1", m_database);
    QString currentVersion;

    if (versionQuery.next()) {
        currentVersion = versionQuery.value(0).toString();
    }

    // If no version found, this is a new database
    if (currentVersion.isEmpty()) {
        // Insert initial schema version
        QSqlQuery insertVersion(m_database);
        insertVersion.prepare("INSERT INTO schema_version (version) VALUES (?)");
        insertVersion.addBindValue(CURRENT_SCHEMA_VERSION);

        if (!insertVersion.exec()) {
            qCritical() << "Failed to insert schema version:" << insertVersion.lastError().text();
            return false;
        }

        return true;
    }

    // Check if migration is needed
    if (currentVersion == CURRENT_SCHEMA_VERSION) {
        return true;  // No migration needed
    }

    // Run migrations from current version to target version
    return migrateFromVersion(currentVersion);
}

bool DatabaseManager::migrateFromVersion(const QString& fromVersion)
{
    // For now, we only support the initial version
    // Future versions would implement migration logic here

    qInfo() << "Migrating database from version" << fromVersion << "to" << CURRENT_SCHEMA_VERSION;

    // Update schema version
    QSqlQuery updateVersion(m_database);
    updateVersion.prepare("UPDATE schema_version SET version = ?, applied_date = CURRENT_TIMESTAMP WHERE version = ?");
    updateVersion.addBindValue(CURRENT_SCHEMA_VERSION);
    updateVersion.addBindValue(fromVersion);

    if (!updateVersion.exec()) {
        qCritical() << "Failed to update schema version:" << updateVersion.lastError().text();
        return false;
    }

    return true;
}

QString DatabaseManager::sanitizeString(const QString& input) const
{
    QString sanitized = input;
    sanitized.replace("\"", "\"\"");  // Escape double quotes
    sanitized.replace("'", "''");    // Escape single quotes
    return sanitized;
}

QVariantMap DatabaseManager::modelToVariantMap(const ModelMetadata& model) const
{
    QVariantMap map;
    map["id"] = model.id.toString();
    map["filename"] = model.filename;
    map["file_size"] = model.fileSize;
    map["import_date"] = model.importDate;
    map["tags"] = model.tags;
    map["custom_fields"] = model.customFields;
    map["thumbnail_path"] = model.thumbnailPath;
    map["mesh_stats"] = model.meshStats;
    return map;
}

ModelMetadata DatabaseManager::variantMapToModel(const QVariantMap& map) const
{
    ModelMetadata model(QUuid::fromString(map["id"].toString()));
    model.filename = map["filename"].toString();
    model.fileSize = map["file_size"].toLongLong();
    model.importDate = map["import_date"].toString();
    model.tags = map["tags"].toStringList();
    model.customFields = map["custom_fields"].toMap();
    model.thumbnailPath = map["thumbnail_path"].toString();
    model.meshStats = map["mesh_stats"].toMap();
    return model;
}

QVariantMap DatabaseManager::projectToVariantMap(const ProjectData& project) const
{
    QVariantMap map;
    map["id"] = project.id.toString();
    map["name"] = project.name;
    map["description"] = project.description;
    map["created_date"] = project.createdDate;
    map["models"] = project.modelIds;
    map["metadata"] = project.metadata;
    return map;
}

ProjectData DatabaseManager::variantMapToProject(const QVariantMap& map) const
{
    ProjectData project(QUuid::fromString(map["id"].toString()));
    project.name = map["name"].toString();
    project.description = map["description"].toString();
    project.createdDate = map["created_date"].toString();

    QStringList modelIds;
    QVariantList modelIdList = map["models"].toList();
    for (const QVariant& id : modelIdList) {
        modelIds.append(id.toString());
    }
    project.modelIds = modelIds;

    project.metadata = map["metadata"].toMap();
    return project;
}