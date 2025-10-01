#include "TagManager.h"
#include "DatabaseManager.h"
#include <QStringList>
#include <QRegularExpression>
#include <QDateTime>
#include <QDebug>
#include <algorithm>

TagManager::TagManager(QObject* parent)
    : QObject(parent)
{
    // Initialize system tags
    m_systemTags.insert("cnc");
    m_systemTags.insert("mechanical");
    m_systemTags.insert("gear");
    m_systemTags.insert("fixture");
    m_systemTags.insert("prototype");
    m_systemTags.insert("production");
    m_systemTags.insert("aluminum");
    m_systemTags.insert("steel");
    m_systemTags.insert("plastic");
    m_systemTags.insert("wood");

    // Load tag hierarchy
    loadTagHierarchy();

    qRegisterMetaType<QStringList>("QStringList");
}

QStringList TagManager::getAllTags() const
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (dbManager) {
        return dbManager->getAllTags();
    }
    return QStringList();
}

QStringList TagManager::getTagsForModel(const QUuid& modelId) const
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (dbManager) {
        return dbManager->getModelTags(modelId);
    }
    return QStringList();
}

QStringList TagManager::getTagsForProject(const QUuid& projectId) const
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (dbManager) {
        return dbManager->getProjectTags(projectId);
    }
    return QStringList();
}

QStringList TagManager::suggestTags(const QString& context,
                                   const QStringList& existingTags,
                                   int maxSuggestions)
{
    QStringList suggestions;

    // Extract keywords from context
    QStringList keywords = extractKeywords(context);

    // Get all available tags
    QStringList allTags = getAllTags();

    // Score tags based on relevance
    QMap<QString, qreal> scoredTags;

    for (const QString& tag : allTags) {
        if (existingTags.contains(tag, Qt::CaseInsensitive)) {
            continue; // Skip already assigned tags
        }

        qreal score = calculateTagRelevance(tag, context);

        // Boost score for exact matches
        for (const QString& keyword : keywords) {
            if (tag.contains(keyword, Qt::CaseInsensitive)) {
                score += 0.5;
            }
        }

        // Boost score for popular tags
        int usageCount = getTagUsageCounts()[tag];
        score += qMin(usageCount / 10.0, 2.0); // Cap the boost

        if (score > 0.1) {
            scoredTags[tag] = score;
        }
    }

    // Sort by score and return top suggestions
    QList<QPair<QString, qreal>> sortedTags;
    for (auto it = scoredTags.begin(); it != scoredTags.end(); ++it) {
        sortedTags.append(QPair<QString, qreal>(it.key(), it.value()));
    }

    std::sort(sortedTags.begin(), sortedTags.end(),
              [](const QPair<QString, qreal>& a, const QPair<QString, qreal>& b) {
                  return a.second > b.second;
              });

    for (const QPair<QString, qreal>& pair : sortedTags) {
        if (suggestions.count() >= maxSuggestions) {
            break;
        }
        suggestions.append(pair.first);
    }

    return suggestions;
}

QStringList TagManager::suggestTagsForModel(const ModelMetadata& model, int maxSuggestions)
{
    QStringList context;

    // Build context from model properties
    context.append(model.filename);

    // Add mesh statistics as context
    for (auto it = model.meshStats.begin(); it != model.meshStats.end(); ++it) {
        context.append(it.value().toString());
    }

    // Add custom fields as context
    for (auto it = model.customFields.begin(); it != model.customFields.end(); ++it) {
        context.append(it.key() + " " + it.value().toString());
    }

    // Add existing tags as context for related suggestions
    QStringList existingTags = model.tags;

    return suggestTags(context.join(" "), existingTags, maxSuggestions);
}

QStringList TagManager::suggestTagsForProject(const ProjectData& project, int maxSuggestions)
{
    QStringList context;

    // Build context from project properties
    context.append(project.name);
    context.append(project.description);

    // Add project metadata as context
    for (auto it = project.metadata.begin(); it != project.metadata.end(); ++it) {
        context.append(it.key() + " " + it.value().toString());
    }

    return suggestTags(context.join(" "), QStringList(), maxSuggestions);
}

bool TagManager::createTag(const QString& tag, const QString& parentTag)
{
    if (!isValidTag(tag)) {
        return false;
    }

    QString sanitizedTag = sanitizeTag(tag);

    if (parentTag.isEmpty()) {
        // Create root tag
        m_tagHierarchy[sanitizedTag] = QStringList();
    } else {
        // Create child tag
        QString sanitizedParent = sanitizeTag(parentTag);

        if (!m_tagHierarchy.contains(sanitizedParent)) {
            m_tagHierarchy[sanitizedParent] = QStringList();
        }

        QStringList children = m_tagHierarchy[sanitizedParent];
        if (!children.contains(sanitizedTag)) {
            children.append(sanitizedTag);
            m_tagHierarchy[sanitizedParent] = children;
        }

        m_childToParent[sanitizedTag] = sanitizedParent;
    }

    saveTagHierarchy();
    emit tagCreated(sanitizedTag);

    return true;
}

bool TagManager::renameTag(const QString& oldTag, const QString& newTag)
{
    if (!isValidTag(newTag)) {
        return false;
    }

    QString sanitizedOld = sanitizeTag(oldTag);
    QString sanitizedNew = sanitizeTag(newTag);

    if (sanitizedOld == sanitizedNew) {
        return true; // No change needed
    }

    // Update hierarchy
    if (m_tagHierarchy.contains(sanitizedOld)) {
        QStringList children = m_tagHierarchy[sanitizedOld];
        m_tagHierarchy.remove(sanitizedOld);
        m_tagHierarchy[sanitizedNew] = children;

        // Update child references
        for (const QString& child : children) {
            m_childToParent[child] = sanitizedNew;
        }
    }

    // Update child-to-parent mapping
    if (m_childToParent.contains(sanitizedOld)) {
        QString parent = m_childToParent[sanitizedOld];
        m_childToParent.remove(sanitizedOld);
        m_childToParent[sanitizedNew] = parent;

        // Update parent's children list
        QStringList children = m_tagHierarchy[parent];
        children.removeAll(sanitizedOld);
        children.append(sanitizedNew);
        m_tagHierarchy[parent] = children;
    }

    saveTagHierarchy();
    emit tagRenamed(sanitizedOld, sanitizedNew);

    return true;
}

bool TagManager::deleteTag(const QString& tag)
{
    QString sanitizedTag = sanitizeTag(tag);

    // Remove from hierarchy
    if (m_tagHierarchy.contains(sanitizedTag)) {
        QStringList children = m_tagHierarchy[sanitizedTag];

        // Re-parent children to grandparent or root
        for (const QString& child : children) {
            QString parent = m_childToParent[child];
            if (parent == sanitizedTag) {
                if (m_childToParent.contains(sanitizedTag)) {
                    // Move to grandparent
                    m_childToParent[child] = m_childToParent[sanitizedTag];
                } else {
                    // Move to root
                    m_childToParent.remove(child);
                }
            }
        }

        m_tagHierarchy.remove(sanitizedTag);
    }

    // Remove from child-to-parent mapping
    m_childToParent.remove(sanitizedTag);

    saveTagHierarchy();
    emit tagDeleted(sanitizedTag);

    return true;
}

bool TagManager::mergeTags(const QString& sourceTag, const QString& targetTag)
{
    QString sanitizedSource = sanitizeTag(sourceTag);
    QString sanitizedTarget = sanitizeTag(targetTag);

    if (sanitizedSource == sanitizedTarget) {
        return true; // No merge needed
    }

    // This would require updating all references in the database
    // For now, just update the hierarchy
    qWarning() << "Tag merging not fully implemented";

    return renameTag(sanitizedSource, sanitizedTarget);
}

QStringList TagManager::getChildTags(const QString& parentTag) const
{
    QString sanitizedParent = sanitizeTag(parentTag);
    return m_tagHierarchy.value(sanitizedParent);
}

QStringList TagManager::getParentTags(const QString& childTag) const
{
    QStringList parents;

    QString currentTag = sanitizeTag(childTag);
    while (m_childToParent.contains(currentTag)) {
        currentTag = m_childToParent[currentTag];
        parents.append(currentTag);
    }

    return parents;
}

QStringList TagManager::getRelatedTags(const QString& tag, int maxRelated) const
{
    QStringList related;

    QString sanitizedTag = sanitizeTag(tag);

    // Get tags that are often used together
    QMap<QString, int> coOccurrence;

    // This would require analyzing usage patterns from the database
    // For now, return child and parent tags
    related.append(getChildTags(sanitizedTag));
    QStringList parents = getParentTags(sanitizedTag);
    related.append(parents);

    // Remove duplicates and limit
    related.removeDuplicates();
    while (related.count() > maxRelated) {
        related.removeLast();
    }

    return related;
}

bool TagManager::isValidTag(const QString& tag) const
{
    if (tag.isEmpty() || tag.length() > 50) {
        return false;
    }

    // Check for invalid characters
    QRegularExpression invalidChars("[<>:\"|?*\\\\]");
    if (invalidChars.match(tag).hasMatch()) {
        return false;
    }

    return true;
}

QStringList TagManager::validateTags(const QStringList& tags) const
{
    QStringList validTags;

    for (const QString& tag : tags) {
        if (isValidTag(tag)) {
            validTags.append(sanitizeTag(tag));
        }
    }

    return validTags;
}

QString TagManager::sanitizeTag(const QString& tag) const
{
    QString sanitized = tag.trimmed().toLower();

    // Replace spaces with underscores
    sanitized.replace(' ', "_");

    // Remove invalid characters
    QRegularExpression invalidChars("[^a-zA-Z0-9_]");
    sanitized.replace(invalidChars, "");

    return sanitized;
}

QMap<QString, int> TagManager::getTagUsageCounts() const
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (dbManager) {
        return dbManager->getTagUsageCounts();
    }
    return QMap<QString, int>();
}

QStringList TagManager::getPopularTags(int maxTags) const
{
    QMap<QString, int> usageCounts = getTagUsageCounts();

    // Sort by usage count
    QList<QPair<QString, int>> sortedTags;
    for (auto it = usageCounts.begin(); it != usageCounts.end(); ++it) {
        sortedTags.append(QPair<QString, int>(it.key(), it.value()));
    }

    std::sort(sortedTags.begin(), sortedTags.end(),
              [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
                  return a.second > b.second;
              });

    QStringList popular;
    for (const QPair<QString, int>& pair : sortedTags) {
        if (popular.count() >= maxTags) {
            break;
        }
        popular.append(pair.first);
    }

    return popular;
}

QStringList TagManager::getRecentTags(int maxTags) const
{
    // This would require tracking recently used tags
    // For now, return popular tags as a substitute
    return getPopularTags(maxTags);
}

bool TagManager::addTagsToModels(const QStringList& tags, const QList<QUuid>& modelIds)
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (!dbManager) {
        return false;
    }

    QStringList sanitizedTags = validateTags(tags);

    for (const QUuid& modelId : modelIds) {
        QStringList currentTags = dbManager->getModelTags(modelId);
        QStringList newTags = currentTags + sanitizedTags;
        newTags.removeDuplicates();

        dbManager->insertModelTags(modelId, newTags);
    }

    emit tagsChanged(QUuid(), "models"); // Broadcast signal
    return true;
}

bool TagManager::removeTagsFromModels(const QStringList& tags, const QList<QUuid>& modelIds)
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (!dbManager) {
        return false;
    }

    QStringList tagsToRemove = validateTags(tags);

    for (const QUuid& modelId : modelIds) {
        QStringList currentTags = dbManager->getModelTags(modelId);

        for (const QString& tag : tagsToRemove) {
            currentTags.removeAll(tag);
        }

        dbManager->insertModelTags(modelId, currentTags);
    }

    emit tagsChanged(QUuid(), "models"); // Broadcast signal
    return true;
}

bool TagManager::setTagsForModel(const QUuid& modelId, const QStringList& tags)
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (!dbManager) {
        return false;
    }

    QStringList sanitizedTags = validateTags(tags);
    return dbManager->insertModelTags(modelId, sanitizedTags);
}

QList<TagManager::TagCategory> TagManager::getTagCategories() const
{
    QList<TagCategory> categories;

    // Define default categories
    categories.append({
        "material",
        "Material types and properties",
        QStringList{"aluminum", "steel", "plastic", "wood", "brass", "copper"},
        "#3498db",
        true
    });

    categories.append({
        "mechanical",
        "Mechanical components and systems",
        QStringList{"gear", "bearing", "shaft", "fastener", "spring", "valve"},
        "#e74c3c",
        true
    });

    categories.append({
        "manufacturing",
        "Manufacturing and CNC related",
        QStringList{"cnc", "fixture", "tooling", "prototype", "production"},
        "#2ecc71",
        true
    });

    categories.append({
        "design",
        "Design and modeling",
        QStringList{"concept", "assembly", "part", "sketch", "drawing"},
        "#f39c12",
        true
    });

    return categories;
}

bool TagManager::createTagCategory(const TagCategory& category)
{
    // For now, categories are static
    // In a full implementation, this would be stored in the database
    qWarning() << "Custom tag categories not yet implemented";
    return false;
}

bool TagManager::updateTagCategory(const QString& name, const TagCategory& category)
{
    // For now, categories are static
    qWarning() << "Tag category updates not yet implemented";
    return false;
}

bool TagManager::deleteTagCategory(const QString& name)
{
    // For now, categories are static
    qWarning() << "Tag category deletion not yet implemented";
    return false;
}

PerformanceMetrics TagManager::getTagMetrics() const
{
    PerformanceMetrics metrics;
    metrics.operationType = "TagManager";
    metrics.operationTimeMs = 0; // Would be calculated from actual operations
    metrics.memoryUsageBytes = calculateMemoryUsage();

    return metrics;
}

QStringList TagManager::extractKeywords(const QString& text) const
{
    QStringList keywords;

    // Extract words from text
    QRegularExpression wordRegex("\\b\\w+\\b");
    QRegularExpressionMatchIterator it = wordRegex.globalMatch(text.toLower());

    while (it.hasNext()) {
        QString word = it.next().captured();
        if (word.length() >= 3) { // Only consider words with 3+ characters
            keywords.append(word);
        }
    }

    // Remove duplicates
    keywords.removeDuplicates();

    return keywords;
}

qreal TagManager::calculateTagRelevance(const QString& tag, const QString& context) const
{
    qreal score = 0.0;

    QString lowerTag = tag.toLower();
    QString lowerContext = context.toLower();

    // Exact match gets highest score
    if (lowerContext.contains(lowerTag)) {
        score = 1.0;
    } else {
        // Fuzzy matching based on character overlap
        QStringList tagChars = lowerTag.split("");
        QStringList contextWords = lowerContext.split(QRegularExpression("\\s+"));

        for (const QString& word : contextWords) {
            int matches = 0;
            for (const QString& char_ : tagChars) {
                if (char_.length() == 1 && word.contains(char_)) {
                    matches++;
                }
            }

            if (matches > 0) {
                qreal wordScore = static_cast<qreal>(matches) / tagChars.length();
                score = qMax(score, wordScore);
            }
        }
    }

    return score;
}

QStringList TagManager::analyzeModelProperties(const ModelMetadata& model) const
{
    QStringList suggestedTags;

    // Analyze filename for clues
    QString filename = model.filename.toLower();

    // Material detection
    if (filename.contains("aluminum") || filename.contains("alum")) {
        suggestedTags.append("aluminum");
    }
    if (filename.contains("steel") || filename.contains("stl")) {
        suggestedTags.append("steel");
    }
    if (filename.contains("plastic") || filename.contains("abs") || filename.contains("pla")) {
        suggestedTags.append("plastic");
    }

    // Component type detection
    if (filename.contains("gear") || filename.contains("cog")) {
        suggestedTags.append("gear");
    }
    if (filename.contains("fixture") || filename.contains("jig")) {
        suggestedTags.append("fixture");
    }
    if (filename.contains("prototype") || filename.contains("proto")) {
        suggestedTags.append("prototype");
    }

    // CNC related
    if (filename.contains("cnc") || filename.contains("machin") || filename.contains("mill")) {
        suggestedTags.append("cnc");
    }

    return suggestedTags;
}

QStringList TagManager::analyzeProjectContent(const ProjectData& project) const
{
    QStringList suggestedTags;

    QString name = project.name.toLower();
    QString description = project.description.toLower();

    // Analyze project name and description
    QStringList textParts;
    textParts.append(name);
    textParts.append(description);

    for (const QString& part : textParts) {
        if (part.contains("assembly") || part.contains("assy")) {
            suggestedTags.append("assembly");
        }
        if (part.contains("fixture") || part.contains("jig")) {
            suggestedTags.append("fixture");
        }
        if (part.contains("prototype") || part.contains("proto")) {
            suggestedTags.append("prototype");
        }
        if (part.contains("production") || part.contains("prod")) {
            suggestedTags.append("production");
        }
    }

    return suggestedTags;
}

void TagManager::loadTagHierarchy()
{
    // Load tag hierarchy from database or file
    // For now, initialize with empty hierarchy
    m_tagHierarchy.clear();
    m_childToParent.clear();
}

void TagManager::saveTagHierarchy()
{
    // Save tag hierarchy to database or file
    // For now, just a placeholder
    qDebug() << "Tag hierarchy saved";
}

qint64 TagManager::calculateMemoryUsage() const
{
    qint64 usage = 0;

    // Estimate memory usage of tag hierarchy
    for (auto it = m_tagHierarchy.begin(); it != m_tagHierarchy.end(); ++it) {
        usage += it.key().size() * 2;
        for (const QString& child : it.value()) {
            usage += child.size() * 2;
        }
    }

    // Estimate memory usage of child-to-parent mapping
    for (auto it = m_childToParent.begin(); it != m_childToParent.end(); ++it) {
        usage += it.key().size() * 2;
        usage += it.value().size() * 2;
    }

    return usage;
}