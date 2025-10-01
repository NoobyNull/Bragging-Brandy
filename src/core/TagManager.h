#pragma once

#include "BaseTypes.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QSet>

/**
 * @brief Service for intelligent tag management and auto-suggestion
 *
 * Handles tag creation, validation, auto-suggestion, and hierarchical
 * tag relationships. Performance target: ≤100ms for tag suggestions.
 */
class TagManager : public QObject
{
    Q_OBJECT

public:
    explicit TagManager(QObject* parent = nullptr);
    virtual ~TagManager() = default;

    // Tag operations
    virtual QStringList getAllTags() const = 0;
    virtual QStringList getTagsForModel(const QUuid& modelId) const = 0;
    virtual QStringList getTagsForProject(const QUuid& projectId) const = 0;

    // Tag suggestions and auto-tagging
    virtual QStringList suggestTags(const QString& context,
                                  const QStringList& existingTags = QStringList(),
                                  int maxSuggestions = 10) = 0;

    virtual QStringList suggestTagsForModel(const ModelMetadata& model,
                                          int maxSuggestions = 10) = 0;

    virtual QStringList suggestTagsForProject(const ProjectData& project,
                                            int maxSuggestions = 10) = 0;

    // Tag management
    virtual bool createTag(const QString& tag, const QString& parentTag = QString()) = 0;
    virtual bool renameTag(const QString& oldTag, const QString& newTag) = 0;
    virtual bool deleteTag(const QString& tag) = 0;
    virtual bool mergeTags(const QString& sourceTag, const QString& targetTag) = 0;

    // Tag relationships
    virtual QStringList getChildTags(const QString& parentTag) const = 0;
    virtual QStringList getParentTags(const QString& childTag) const = 0;
    virtual QStringList getRelatedTags(const QString& tag, int maxRelated = 10) const = 0;

    // Tag validation
    virtual bool isValidTag(const QString& tag) const = 0;
    virtual QStringList validateTags(const QStringList& tags) const = 0;
    virtual QString sanitizeTag(const QString& tag) const = 0;

    // Tag statistics
    virtual QMap<QString, int> getTagUsageCounts() const = 0;
    virtual QStringList getPopularTags(int maxTags = 20) const = 0;
    virtual QStringList getRecentTags(int maxTags = 20) const = 0;

    // Batch operations
    virtual bool addTagsToModels(const QStringList& tags,
                               const QList<QUuid>& modelIds) = 0;

    virtual bool removeTagsFromModels(const QStringList& tags,
                                    const QList<QUuid>& modelIds) = 0;

    virtual bool setTagsForModel(const QUuid& modelId, const QStringList& tags) = 0;

    // Tag categories and organization
    struct TagCategory {
        QString name;
        QString description;
        QStringList tags;
        QString color;
        bool isSystemCategory;
    };

    virtual QList<TagCategory> getTagCategories() const = 0;
    virtual bool createTagCategory(const TagCategory& category) = 0;
    virtual bool updateTagCategory(const QString& name, const TagCategory& category) = 0;
    virtual bool deleteTagCategory(const QString& name) = 0;

    // Performance monitoring
    virtual PerformanceMetrics getTagMetrics() const = 0;

signals:
    // Tag events
    void tagCreated(const QString& tag);
    void tagDeleted(const QString& tag);
    void tagRenamed(const QString& oldTag, const QString& newTag);
    void tagsChanged(const QUuid& itemId, const QString& itemType);

    // Suggestion events
    void suggestionsReady(const QStringList& suggestions);

protected:
    // Tag analysis helpers
    virtual QStringList extractKeywords(const QString& text) const = 0;
    virtual qreal calculateTagRelevance(const QString& tag,
                                      const QString& context) const = 0;

    // Auto-tagging logic
    virtual QStringList analyzeModelProperties(const ModelMetadata& model) = 0;
    virtual QStringList analyzeProjectContent(const ProjectData& project) = 0;

    // Tag storage
    virtual void loadTagHierarchy() = 0;
    virtual void saveTagHierarchy() = 0;

    // Tag hierarchy data
    QMap<QString, QStringList> m_tagHierarchy;  // parent -> children
    QMap<QString, QString> m_childToParent;     // child -> parent
    QSet<QString> m_systemTags;
};