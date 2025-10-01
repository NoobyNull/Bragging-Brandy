# Core Services API

## Overview

The Core Services API provides access to the fundamental functionality of the 3D Model Management Utility, including model management, search, tagging, and data persistence.

## ModelService

### Class: ModelService

The ModelService handles all 3D model operations including loading, saving, metadata management, and file operations.

### Methods

#### Model Loading
```cpp
// Asynchronous model loading
QFuture<ModelMetadata> loadModelAsync(const QString& filepath)

// Synchronous model loading
ModelMetadata loadModel(const QString& filepath)

// Model saving
bool saveModel(const ModelMetadata& model, const QString& filepath)
```

#### Model Management
```cpp
// Get all models
QList<ModelMetadata> getAllModels() const

// Get specific model
ModelMetadata getModel(const QUuid& id) const

// Update model metadata
bool updateModelMetadata(const ModelMetadata& model)

// Delete model
bool deleteModel(const QUuid& id)
```

#### Search and Filtering
```cpp
// Search models with query and filters
QList<ModelMetadata> searchModels(const QString& query,
                                 const QStringList& tags = QStringList(),
                                 const QVariantMap& filters = QVariantMap()) const
```

#### Batch Operations
```cpp
// Tag multiple models
bool tagModels(const QList<QUuid>& modelIds, const QStringList& tags)

// Remove tags from models
bool untagModels(const QList<QUuid>& modelIds, const QStringList& tags)

// Delete multiple models
bool deleteModels(const QList<QUuid>& modelIds)
```

#### Statistics and Information
```cpp
// Get model statistics
QVariantMap getModelStatistics() const

// Get all available tags
QStringList getAllTags() const

// Get total model count
qint64 getTotalModelsCount() const

// Get total models size
qint64 getTotalModelsSize() const
```

### Signals

```cpp
// Model events
void modelLoaded(const ModelMetadata& model)
void modelDeleted(const QUuid& id)
void modelUpdated(const ModelMetadata& model)
void modelsImported(const QList<ModelMetadata>& models)

// Progress events
void importProgress(const QString& filename, int percentage)
void exportProgress(const QString& filename, int percentage)

// Error events
void errorOccurred(const QString& operation, const QString& error, const QString& details)
```

### Usage Example

```cpp
// Initialize model service
ModelService* modelService = new ModelService(this);

// Connect to signals
connect(modelService, &ModelService::modelLoaded, this, &MyClass::onModelLoaded);
connect(modelService, &ModelService::errorOccurred, this, &MyClass::onError);

// Load a model asynchronously
QFuture<ModelMetadata> future = modelService->loadModelAsync("/path/to/model.stl");

// Get all models
QList<ModelMetadata> models = modelService->getAllModels();

// Search models
QList<ModelMetadata> results = modelService->searchModels("gear", QStringList() << "mechanical");

// Tag models
QList<QUuid> modelIds = {model1.id, model2.id};
modelService->tagModels(modelIds, QStringList() << "processed" << "cnc");
```

## SearchService

### Class: SearchService

The SearchService provides high-performance search capabilities with real-time results and intelligent ranking.

### Methods

#### Search Operations
```cpp
// Basic search
QList<SearchResult> search(const QString& query,
                          const QStringList& types = QStringList(),
                          const QVariantMap& filters = QVariantMap())

// Asynchronous search
QList<SearchResult> searchAsync(const QString& query,
                               const QStringList& types = QStringList(),
                               const QVariantMap& filters = QVariantMap())
```

#### Search Suggestions
```cpp
// Get search suggestions
QStringList getSuggestions(const QString& partialQuery, int maxSuggestions = 10)

// Get tag suggestions
QStringList getTagSuggestions(const QString& partialTag, int maxSuggestions = 10)
```

#### Index Management
```cpp
// Index model for search
void indexModel(const ModelMetadata& model)

// Index project for search
void indexProject(const ProjectData& project)

// Remove from index
void removeFromIndex(const QUuid& id)

// Rebuild search index
void rebuildIndex()
```

#### Configuration
```cpp
// Set search options
void setSearchOptions(const QVariantMap& options)

// Get search options
QVariantMap getSearchOptions() const
```

### Advanced Search Features

#### Search Filters
```cpp
struct SearchFilters {
    QStringList contentTypes;    // "model", "project"
    QStringList tags;           // Required tags (AND operation)
    QStringList excludeTags;    // Excluded tags (NOT operation)
    QStringList fileTypes;      // File extensions
    QVariantMap dateRange;      // Date filters
    QVariantMap sizeRange;      // File size filters
    QVariantMap customFilters;  // Custom field filters
    QString sortBy;             // "relevance", "name", "date", "size"
    bool sortDescending = true;
    int maxResults = 100;
};
```

#### Filtered Search
```cpp
// Search with advanced filters
QList<SearchResult> searchWithFilters(const QString& query, const SearchFilters& filters)
```

### Signals

```cpp
// Search events
void searchCompleted(const QString& query, const QList<SearchResult>& results)
void searchFailed(const QString& query, const QString& error)

// Index events
void indexRebuilt()
void itemIndexed(const QUuid& id, const QString& type)

// Suggestion events
void suggestionsReady(const QStringList& suggestions)
```

### Usage Example

```cpp
// Initialize search service
SearchService* searchService = new SearchService(this);

// Connect to signals
connect(searchService, &SearchService::searchCompleted, this, &MyClass::onSearchCompleted);

// Perform search
QList<SearchResult> results = searchService->search("gear", QStringList() << "model");

// Get suggestions
QStringList suggestions = searchService->getSuggestions("ge");

// Advanced search with filters
SearchService::SearchFilters filters;
filters.contentTypes << "model";
filters.tags << "mechanical";
filters.maxResults = 50;

QList<SearchResult> filteredResults = searchService->searchWithFilters("gear", filters);
```

## TagManager

### Class: TagManager

The TagManager handles intelligent tag management with auto-suggestion and hierarchical organization.

### Methods

#### Tag Operations
```cpp
// Get all tags
QStringList getAllTags() const

// Get tags for model
QStringList getTagsForModel(const QUuid& modelId) const

// Get tags for project
QStringList getTagsForProject(const QUuid& projectId) const
```

#### Tag Suggestions
```cpp
// Get tag suggestions based on context
QStringList suggestTags(const QString& context,
                       const QStringList& existingTags = QStringList(),
                       int maxSuggestions = 10)

// Suggest tags for model
QStringList suggestTagsForModel(const ModelMetadata& model, int maxSuggestions = 10)

// Suggest tags for project
QStringList suggestTagsForProject(const ProjectData& project, int maxSuggestions = 10)
```

#### Tag Management
```cpp
// Create new tag
bool createTag(const QString& tag, const QString& parentTag = QString())

// Rename tag
bool renameTag(const QString& oldTag, const QString& newTag)

// Delete tag
bool deleteTag(const QString& tag)

// Merge tags
bool mergeTags(const QString& sourceTag, const QString& targetTag)
```

#### Tag Relationships
```cpp
// Get child tags
QStringList getChildTags(const QString& parentTag) const

// Get parent tags
QStringList getParentTags(const QString& childTag) const

// Get related tags
QStringList getRelatedTags(const QString& tag, int maxRelated = 10) const
```

#### Batch Operations
```cpp
// Add tags to models
bool addTagsToModels(const QStringList& tags, const QList<QUuid>& modelIds)

// Remove tags from models
bool removeTagsFromModels(const QStringList& tags, const QList<QUuid>& modelIds)

// Set tags for model
bool setTagsForModel(const QUuid& modelId, const QStringList& tags)
```

### Tag Categories

```cpp
struct TagCategory {
    QString name;
    QString description;
    QStringList tags;
    QString color;
    bool isSystemCategory;
};
```

#### Category Management
```cpp
// Get tag categories
QList<TagCategory> getTagCategories() const

// Create tag category
bool createTagCategory(const TagCategory& category)

// Update tag category
bool updateTagCategory(const QString& name, const TagCategory& category)

// Delete tag category
bool deleteTagCategory(const QString& name)
```

### Signals

```cpp
// Tag events
void tagCreated(const QString& tag)
void tagDeleted(const QString& tag)
void tagRenamed(const QString& oldTag, const QString& newTag)
void tagsChanged(const QUuid& itemId, const QString& itemType)

// Suggestion events
void suggestionsReady(const QStringList& suggestions)
```

### Usage Example

```cpp
// Initialize tag manager
TagManager* tagManager = new TagManager(this);

// Connect to signals
connect(tagManager, &TagManager::tagCreated, this, &MyClass::onTagCreated);

// Get tag suggestions
QStringList suggestions = tagManager->suggestTags("gear mechanical part", QStringList(), 10);

// Create hierarchical tags
tagManager->createTag("mechanical");
tagManager->createTag("gear", "mechanical");
tagManager->createTag("spur_gear", "gear");

// Tag models
QList<QUuid> modelIds = {model1.id, model2.id};
tagManager->addTagsToModels(QStringList() << "mechanical" << "gear", modelIds);
```

## DatabaseManager

### Class: DatabaseManager

The DatabaseManager handles all database operations including model metadata, project data, tags, and application settings.

### Methods

#### Database Lifecycle
```cpp
// Initialize database
bool initialize(const QString& databasePath = QString())

// Close database
void close()

// Check if initialized
bool isInitialized() const
```

#### Model Operations
```cpp
// Insert model
bool insertModel(const ModelMetadata& model)

// Update model
bool updateModel(const ModelMetadata& model)

// Delete model
bool deleteModel(const QUuid& id)

// Get model
ModelMetadata getModel(const QUuid& id) const

// Get all models
QList<ModelMetadata> getAllModels() const

// Search models
QList<ModelMetadata> searchModels(const QString& query,
                                 const QStringList& tags = QStringList()) const
```

#### Project Operations
```cpp
// Insert project
bool insertProject(const ProjectData& project)

// Update project
bool updateProject(const ProjectData& project)

// Delete project
bool deleteProject(const QUuid& id)

// Get project
ProjectData getProject(const QUuid& id) const

// Get all projects
QList<ProjectData> getAllProjects() const
```

#### Tag Operations
```cpp
// Insert model tags
bool insertModelTags(const QUuid& modelId, const QStringList& tags)

// Insert project tags
bool insertProjectTags(const QUuid& projectId, const QStringList& tags)

// Get model tags
QStringList getModelTags(const QUuid& modelId) const

// Get project tags
QStringList getProjectTags(const QUuid& projectId) const

// Get all tags
QStringList getAllTags() const

// Get tag usage counts
QMap<QString, int> getTagUsageCounts() const
```

#### Settings Operations
```cpp
// Save setting
bool saveSetting(const QString& key, const QVariant& value)

// Get setting
QVariant getSetting(const QString& key, const QVariant& defaultValue = QVariant()) const

// Get all settings
QVariantMap getAllSettings() const

// Delete setting
bool deleteSetting(const QString& key)
```

#### Database Maintenance
```cpp
// Optimize database
bool optimize()

// Backup database
bool backup(const QString& backupPath)

// Restore database
bool restore(const QString& backupPath)

// Get database size
qint64 getDatabaseSize() const

// Get table names
QStringList getTableNames() const
```

#### Transaction Management
```cpp
// Begin transaction
bool beginTransaction()

// Commit transaction
bool commitTransaction()

// Rollback transaction
bool rollbackTransaction()
```

### Signals

```cpp
// Database events
void databaseInitialized()
void databaseClosed()
void databaseError(const QString& error, const QString& details)

// Data change events
void modelInserted(const ModelMetadata& model)
void modelUpdated(const ModelMetadata& model)
void modelDeleted(const QUuid& id)
void projectInserted(const ProjectData& project)
void projectUpdated(const ProjectData& project)
void projectDeleted(const QUuid& id)
```

### Usage Example

```cpp
// Initialize database manager
DatabaseManager* dbManager = new DatabaseManager(this);

// Connect to signals
connect(dbManager, &DatabaseManager::databaseError, this, &MyClass::onDatabaseError);

// Initialize database
if (!dbManager->initialize("/path/to/models.db")) {
    qCritical() << "Failed to initialize database";
    return;
}

// Insert model
ModelMetadata model = createTestModel();
if (dbManager->insertModel(model)) {
    qInfo() << "Model inserted successfully";
}

// Search models
QList<ModelMetadata> results = dbManager->searchModels("gear", QStringList() << "mechanical");

// Save setting
dbManager->saveSetting("default_material", "aluminum");

// Get setting
QString material = dbManager->getSetting("default_material", "steel").toString();
```

## Data Types

### ModelMetadata
```cpp
struct ModelMetadata {
    QUuid id;
    QString filename;
    qint64 fileSize;
    QString importDate;
    QStringList tags;
    QVariantMap customFields;
    QString thumbnailPath;
    QVariantMap meshStats;
};
```

### ProjectData
```cpp
struct ProjectData {
    QUuid id;
    QString name;
    QString description;
    QString createdDate;
    QList<QUuid> modelIds;
    QVariantMap metadata;
};
```

### SearchResult
```cpp
struct SearchResult {
    QUuid id;
    QString name;
    QString type; // "model" or "project"
    QStringList tags;
    qreal relevance;
    QString snippet;
};
```

## Best Practices

### Error Handling
```cpp
// Always check return values
if (!modelService->loadModel(filepath).id.isNull()) {
    // Success
} else {
    // Handle error
}

// Connect to error signals
connect(modelService, &ModelService::errorOccurred, this, &MyClass::handleError);
```

### Performance Optimization
```cpp
// Use asynchronous operations for large models
QFuture<ModelMetadata> future = modelService->loadModelAsync(largeFile);
connect(&future, &QFutureWatcher::finished, this, &MyClass::onModelLoaded);

// Use search filters for better performance
SearchService::SearchFilters filters;
filters.maxResults = 50;
QList<SearchResult> results = searchService->searchWithFilters(query, filters);
```

### Memory Management
```cpp
// Models are loaded on demand
// Use progressive loading for large models
// Clear caches periodically
cacheManager->clearExpiredCache();
```

### Thread Safety
```cpp
// All services are thread-safe
// Use Qt::QueuedConnection for signal-slot connections across threads
connect(service, &Service::operationCompleted, this, &MyClass::onCompleted, Qt::QueuedConnection);
```

## Integration Examples

### Complete Workflow
```cpp
void MyClass::processModelWorkflow(const QString& filepath) {
    // 1. Load model
    QFuture<ModelMetadata> loadFuture = modelService->loadModelAsync(filepath);

    // 2. When loaded, analyze and tag
    connect(&loadFuture, &QFutureWatcher::finished, this, [this, loadFuture]() {
        ModelMetadata model = loadFuture.result();

        // 3. Generate thumbnail
        thumbnailGenerator->generateThumbnailAsync(model.id.toString());

        // 4. Suggest and apply tags
        QStringList suggestedTags = tagManager->suggestTagsForModel(model, 5);
        modelService->tagModels(QList<QUuid>() << model.id, suggestedTags);

        // 5. Index for search
        searchService->indexModel(model);
    });
}
```

### Batch Processing
```cpp
void MyClass::batchProcessModels(const QStringList& filepaths) {
    for (const QString& filepath : filepaths) {
        // Import model
        QFuture<ModelMetadata> future = modelService->importModelAsync(filepath);

        connect(&future, &QFutureWatcher::finished, this, [this, future]() {
            ModelMetadata model = future.result();

            // Process model
            processSingleModel(model);
        });
    }
}
```

---

*For more advanced usage and integration examples, check the [Advanced API Documentation](../api/advanced.md).*