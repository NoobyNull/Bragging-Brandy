#include "SearchService.h"
#include "DatabaseManager.h"
#include <QStringList>
#include <QRegularExpression>
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <algorithm>

SearchService::SearchService(QObject* parent)
    : QObject(parent)
    , m_searchTimer(new QTimer(this))
    , m_hitCount(0)
    , m_missCount(0)
{
    // Configure search timer for debounced search
    m_searchTimer->setSingleShot(true);
    m_searchTimer->setInterval(100); // 100ms debounce

    connect(m_searchTimer, &QTimer::timeout, this, [this]() {
        performAsyncSearch();
    });

    // Default search options
    QVariantMap defaultOptions;
    defaultOptions["fuzzy_threshold"] = 0.6;
    defaultOptions["max_results"] = 100;
    defaultOptions["search_delay"] = 100;
    defaultOptions["enable_caching"] = true;
    defaultOptions["cache_timeout"] = 30000; // 30 seconds

    setSearchOptions(defaultOptions);

    qRegisterMetaType<QList<SearchResult>>("QList<SearchResult>");
    qRegisterMetaType<SearchFilters>("SearchFilters");
}

QList<SearchResult> SearchService::search(const QString& query,
                                         const QStringList& types,
                                         const QVariantMap& filters)
{
    qint64 startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    SearchFilters searchFilters;
    searchFilters.contentTypes = types;
    searchFilters.maxResults = getSearchOptions()["max_results"].toInt();

    // Apply additional filters if provided
    if (filters.contains("tags")) {
        searchFilters.tags = filters["tags"].toStringList();
    }

    QList<SearchResult> results = performSearch(query, searchFilters);

    qint64 elapsed = QDateTime::currentDateTime().toMSecsSinceEpoch() - startTime;

    // Update performance metrics
    m_lastSearchTime = elapsed;
    if (elapsed <= 100) {
        m_fastSearches++;
    } else {
        m_slowSearches++;
    }

    qDebug() << QString("Search completed in %1ms: '%2' -> %3 results")
                .arg(elapsed).arg(query).arg(results.count());

    return results;
}

QList<SearchResult> SearchService::searchAsync(const QString& query,
                                              const QStringList& types,
                                              const QVariantMap& filters)
{
    // Cancel any pending search
    m_searchTimer->stop();

    // Store search parameters for async execution
    m_pendingQuery = query;
    m_pendingTypes = types;
    m_pendingFilters = filters;

    // Start debounced search timer
    m_searchTimer->start();

    return QList<SearchResult>(); // Return empty list, results will come via signal
}

QStringList SearchService::getSuggestions(const QString& partialQuery, int maxSuggestions)
{
    QStringList suggestions;

    if (partialQuery.length() < 2) {
        return suggestions;
    }

    // Get recent searches for suggestions
    suggestions.append(getRecentSearches(maxSuggestions / 2));

    // Get tag suggestions
    QStringList tagSuggestions = getTagSuggestions(partialQuery, maxSuggestions / 2);
    suggestions.append(tagSuggestions);

    // Remove duplicates and limit
    suggestions.removeDuplicates();
    while (suggestions.count() > maxSuggestions) {
        suggestions.removeLast();
    }

    return suggestions;
}

QStringList SearchService::getTagSuggestions(const QString& partialTag, int maxSuggestions)
{
    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (!dbManager) {
        return QStringList();
    }

    QStringList allTags = dbManager->getAllTags();
    QStringList suggestions;

    for (const QString& tag : allTags) {
        if (tag.contains(partialTag, Qt::CaseInsensitive)) {
            suggestions.append(tag);
            if (suggestions.count() >= maxSuggestions) {
                break;
            }
        }
    }

    return suggestions;
}

void SearchService::indexModel(const ModelMetadata& model)
{
    // Index model for search
    QString searchableText = buildSearchableText(model);

    // Store in search index (simplified for now)
    m_searchIndex[model.id.toString()] = searchableText;

    // Update tag index
    for (const QString& tag : model.tags) {
        m_tagIndex[tag].append(model.id.toString());
    }

    emit itemIndexed(model.id, "model");
}

void SearchService::indexProject(const ProjectData& project)
{
    // Index project for search
    QString searchableText = buildSearchableText(project);

    // Store in search index
    m_searchIndex[project.id.toString()] = searchableText;

    // Update tag index
    // Note: Would need to get project tags from database

    emit itemIndexed(project.id, "project");
}

void SearchService::removeFromIndex(const QUuid& id)
{
    QString idStr = id.toString();
    m_searchIndex.remove(idStr);

    // Remove from tag index
    QMutableMapIterator<QString, QStringList> it(m_tagIndex);
    while (it.hasNext()) {
        it.next();
        it.value().removeAll(idStr);
        if (it.value().isEmpty()) {
            it.remove();
        }
    }
}

void SearchService::rebuildIndex()
{
    qInfo() << "Rebuilding search index...";

    m_searchIndex.clear();
    m_tagIndex.clear();

    DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
    if (!dbManager) {
        qWarning() << "Cannot rebuild index: no database manager";
        return;
    }

    // Index all models
    QList<ModelMetadata> models = dbManager->getAllModels();
    for (const ModelMetadata& model : models) {
        indexModel(model);
    }

    // Index all projects
    QList<ProjectData> projects = dbManager->getAllProjects();
    for (const ProjectData& project : projects) {
        indexProject(project);
    }

    emit indexRebuilt();
    qInfo() << QString("Search index rebuilt: %1 models, %2 projects")
              .arg(models.count()).arg(projects.count());
}

void SearchService::setSearchOptions(const QVariantMap& options)
{
    m_searchOptions = options;

    // Update timer interval if specified
    if (options.contains("search_delay")) {
        m_searchTimer->setInterval(options["search_delay"].toInt());
    }
}

QVariantMap SearchService::getSearchOptions() const
{
    return m_searchOptions;
}

PerformanceMetrics SearchService::getSearchMetrics() const
{
    PerformanceMetrics metrics;
    metrics.operationType = "SearchService";
    metrics.operationTimeMs = m_lastSearchTime;
    metrics.memoryUsageBytes = calculateMemoryUsage();

    QVariantMap details;
    details["total_searches"] = m_hitCount + m_missCount;
    details["cache_hits"] = m_hitCount;
    details["cache_misses"] = m_missCount;
    details["fast_searches"] = m_fastSearches;
    details["slow_searches"] = m_slowSearches;
    details["index_size"] = m_searchIndex.size();

    return metrics;
}

void SearchService::clearSearchCache()
{
    m_searchCache.clear();
    m_hitCount = 0;
    m_missCount = 0;
}

QList<SearchResult> SearchService::searchWithFilters(const QString& query, const SearchFilters& filters)
{
    return performSearch(query, filters);
}

QList<SearchResult> SearchService::performSearch(const QString& query, const SearchFilters& filters)
{
    QList<SearchResult> results;

    if (query.trimmed().isEmpty()) {
        return results;
    }

    QStringList searchTerms = extractSearchTerms(query);

    // Search in index
    QMap<QString, qreal> scoredResults;

    for (auto it = m_searchIndex.begin(); it != m_searchIndex.end(); ++it) {
        QString id = it.key();
        QString searchableText = it.value();

        qreal score = calculateRelevance(query, searchableText, searchTerms);

        if (score > 0.1) { // Minimum relevance threshold
            scoredResults[id] = score;
        }
    }

    // Filter by content types if specified
    if (!filters.contentTypes.isEmpty()) {
        QMutableMapIterator<QString, qreal> it(scoredResults);
        while (it.hasNext()) {
            it.next();
            QString id = it.key();

            // Determine content type (simplified)
            QString contentType = determineContentType(id);

            if (!filters.contentTypes.contains(contentType)) {
                it.remove();
            }
        }
    }

    // Filter by tags if specified
    if (!filters.tags.isEmpty()) {
        QMutableMapIterator<QString, qreal> it(scoredResults);
        while (it.hasNext()) {
            it.next();
            QString id = it.key();

            QStringList itemTags = getItemTags(id);
            bool hasAllTags = true;

            for (const QString& requiredTag : filters.tags) {
                if (!itemTags.contains(requiredTag, Qt::CaseInsensitive)) {
                    hasAllTags = false;
                    break;
                }
            }

            if (!hasAllTags) {
                it.remove();
            }
        }
    }

    // Sort by relevance score
    QList<QPair<QString, qreal>> sortedResults;
    for (auto it = scoredResults.begin(); it != scoredResults.end(); ++it) {
        sortedResults.append(QPair<QString, qreal>(it.key(), it.value()));
    }

    std::sort(sortedResults.begin(), sortedResults.end(),
              [](const QPair<QString, qreal>& a, const QPair<QString, qreal>& b) {
                  return a.second > b.second;
              });

    // Convert to SearchResult objects
    int resultCount = 0;
    for (const QPair<QString, qreal>& pair : sortedResults) {
        if (resultCount >= filters.maxResults) {
            break;
        }

        SearchResult result = createSearchResult(pair.first, query);
        if (result.id.isNull()) {
            continue; // Skip invalid results
        }

        result.relevance = pair.second;
        results.append(result);
        resultCount++;
    }

    return results;
}

qreal SearchService::calculateRelevance(const QString& query, const SearchResult& result)
{
    QString searchableText = m_searchIndex[result.id.toString()];
    QStringList searchTerms = extractSearchTerms(query);

    return calculateRelevance(query, searchableText, searchTerms);
}

qreal SearchService::calculateRelevance(const QString& query, const QString& searchableText, const QStringList& searchTerms)
{
    qreal totalScore = 0.0;
    qreal bestScore = 0.0;

    for (const QString& term : searchTerms) {
        qreal termScore = fuzzyMatch(term, searchableText);
        totalScore += termScore;
        bestScore = qMax(bestScore, termScore);
    }

    // Boost exact matches
    if (searchableText.contains(query, Qt::CaseInsensitive)) {
        totalScore *= 2.0;
    }

    return totalScore / searchTerms.count();
}

QStringList SearchService::extractSearchTerms(const QString& query)
{
    // Split query into terms and clean them
    QStringList terms = query.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    // Remove common stop words
    QStringList stopWords = {"the", "a", "an", "and", "or", "but", "in", "on", "at", "to", "for", "of", "with", "by"};
    QMutableStringListIterator it(terms);
    while (it.hasNext()) {
        QString term = it.next().toLower();
        if (stopWords.contains(term) || term.length() < 2) {
            it.remove();
        }
    }

    return terms;
}

qreal SearchService::fuzzyMatch(const QString& pattern, const QString& text)
{
    if (pattern.isEmpty()) {
        return 0.0;
    }

    if (text.contains(pattern, Qt::CaseInsensitive)) {
        return 1.0; // Exact match gets highest score
    }

    // Simple fuzzy matching based on character overlap
    QString p = pattern.toLower();
    QString t = text.toLower();

    int matches = 0;
    int pIndex = 0;

    for (int tIndex = 0; tIndex < t.length() && pIndex < p.length(); ++tIndex) {
        if (t[tIndex] == p[pIndex]) {
            matches++;
            pIndex++;
        }
    }

    qreal score = static_cast<qreal>(matches) / p.length();

    // Boost score for matches at word boundaries
    if (t.contains(" " + p + " ") || t.startsWith(p + " ") || t.endsWith(" " + p)) {
        score *= 1.5;
    }

    return qMin(score, 1.0);
}

QStringList SearchService::getFuzzyMatches(const QString& pattern, const QStringList& candidates, qreal threshold)
{
    QStringList matches;

    for (const QString& candidate : candidates) {
        qreal score = fuzzyMatch(pattern, candidate);
        if (score >= threshold) {
            matches.append(candidate);
        }
    }

    return matches;
}

void SearchService::performAsyncSearch()
{
    QList<SearchResult> results = search(m_pendingQuery, m_pendingTypes, m_pendingFilters);

    emit searchCompleted(m_pendingQuery, results);
}

QString SearchService::buildSearchableText(const ModelMetadata& model)
{
    QStringList parts;
    parts.append(model.filename);
    parts.append(model.tags.join(" "));

    // Add custom fields
    for (auto it = model.customFields.begin(); it != model.customFields.end(); ++it) {
        parts.append(it.value().toString());
    }

    return parts.join(" ").toLower();
}

QString SearchService::buildSearchableText(const ProjectData& project)
{
    QStringList parts;
    parts.append(project.name);
    parts.append(project.description);

    // Add model names from project (would need to fetch from database)

    return parts.join(" ").toLower();
}

QString SearchService::determineContentType(const QString& id)
{
    // Check if it's a model or project ID
    // For now, assume UUID format indicates model
    return QUuid(id).isNull() ? "unknown" : "model";
}

QStringList SearchService::getItemTags(const QString& id)
{
    return m_tagIndex.keys(); // Simplified - would need proper lookup
}

SearchResult SearchService::createSearchResult(const QString& id, const QString& query)
{
    SearchResult result;

    // Determine if it's a model or project
    QString contentType = determineContentType(id);

    if (contentType == "model") {
        // Fetch model data from database
        DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
        if (dbManager) {
            ModelMetadata model = dbManager->getModel(QUuid(id));
            if (!model.id.isNull()) {
                result.id = model.id;
                result.name = model.filename;
                result.type = "model";
                result.tags = model.tags;

                // Generate snippet
                QString searchableText = m_searchIndex[id];
                result.snippet = generateSnippet(searchableText, query);
            }
        }
    } else if (contentType == "project") {
        // Fetch project data from database
        DatabaseManager* dbManager = qobject_cast<DatabaseManager*>(parent());
        if (dbManager) {
            ProjectData project = dbManager->getProject(QUuid(id));
            if (!project.id.isNull()) {
                result.id = project.id;
                result.name = project.name;
                result.type = "project";

                // Generate snippet
                QString searchableText = m_searchIndex[id];
                result.snippet = generateSnippet(searchableText, query);
            }
        }
    }

    return result;
}

QString SearchService::generateSnippet(const QString& text, const QString& query)
{
    // Find query terms in text and extract surrounding context
    QStringList terms = extractSearchTerms(query);
    QString lowerText = text.toLower();

    for (const QString& term : terms) {
        int index = lowerText.indexOf(term);
        if (index != -1) {
            int start = qMax(0, index - 30);
            int end = qMin(text.length(), index + term.length() + 30);
            QString snippet = text.mid(start, end - start);

            if (start > 0) snippet = "..." + snippet;
            if (end < text.length()) snippet = snippet + "...";

            return snippet;
        }
    }

    return text.left(100) + (text.length() > 100 ? "..." : "");
}

QStringList SearchService::getRecentSearches(int maxSearches)
{
    QStringList recent;
    for (int i = 0; i < qMin(maxSearches, m_recentQueries.count()); ++i) {
        recent.append(m_recentQueries[i]);
    }
    return recent;
}

qint64 SearchService::calculateMemoryUsage() const
{
    qint64 usage = 0;

    // Estimate memory usage of search index
    for (auto it = m_searchIndex.begin(); it != m_searchIndex.end(); ++it) {
        usage += it.key().size() * 2; // QString storage
        usage += it.value().size() * 2;
    }

    // Estimate memory usage of tag index
    for (auto it = m_tagIndex.begin(); it != m_tagIndex.end(); ++it) {
        usage += it.key().size() * 2;
        usage += it.value().join("").size() * 2;
    }

    return usage;
}