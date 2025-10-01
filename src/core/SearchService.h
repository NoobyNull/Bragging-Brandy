#pragma once

#include "BaseTypes.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include <QFuture>
#include <QTimer>

/**
 * @brief High-performance search service for model and project discovery
 *
 * Provides real-time search capabilities with fuzzy matching, faceted search,
 * and intelligent ranking. Performance target: ≤100ms for most queries.
 */
class SearchService : public QObject
{
    Q_OBJECT

public:
    explicit SearchService(QObject* parent = nullptr);
    virtual ~SearchService() = default;

    // Search operations
    virtual QList<SearchResult> search(const QString& query,
                                     const QStringList& types = QStringList(),
                                     const QVariantMap& filters = QVariantMap()) = 0;

    virtual QList<SearchResult> searchAsync(const QString& query,
                                          const QStringList& types = QStringList(),
                                          const QVariantMap& filters = QVariantMap()) = 0;

    // Search suggestions and autocomplete
    virtual QStringList getSuggestions(const QString& partialQuery,
                                     int maxSuggestions = 10) = 0;

    virtual QStringList getTagSuggestions(const QString& partialTag,
                                         int maxSuggestions = 10) = 0;

    virtual QStringList getRecentSearches(int maxSearches = 10) = 0;

    // Index management
    virtual void indexModel(const ModelMetadata& model) = 0;
    virtual void indexProject(const ProjectData& project) = 0;
    virtual void removeFromIndex(const QUuid& id) = 0;
    virtual void rebuildIndex() = 0;

    // Search configuration
    virtual void setSearchOptions(const QVariantMap& options) = 0;
    virtual QVariantMap getSearchOptions() const = 0;

    // Performance monitoring
    virtual PerformanceMetrics getSearchMetrics() const = 0;
    virtual void clearSearchCache() = 0;

    // Advanced search features
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

    virtual QList<SearchResult> searchWithFilters(const QString& query,
                                                 const SearchFilters& filters) = 0;

signals:
    // Search events
    void searchCompleted(const QString& query, const QList<SearchResult>& results);
    void searchFailed(const QString& query, const QString& error);
    void indexProgress(int percentage, const QString& status);
    void suggestionsReady(const QStringList& suggestions);

    // Index events
    void indexRebuilt();
    void itemIndexed(const QUuid& id, const QString& type);

protected:
    // Search implementation helpers
    virtual QList<SearchResult> performSearch(const QString& query,
                                             const SearchFilters& filters) = 0;
    virtual qreal calculateRelevance(const QString& query, const SearchResult& result) = 0;
    virtual qreal calculateRelevance(const QString& query, const QString& searchableText, const QStringList& searchTerms) = 0;
    virtual QStringList extractSearchTerms(const QString& query) = 0;

    // Additional helper methods
    virtual void performAsyncSearch() = 0;
    virtual QString buildSearchableText(const ModelMetadata& model) = 0;
    virtual QString buildSearchableText(const ProjectData& project) = 0;

    // Fuzzy matching
    virtual qreal fuzzyMatch(const QString& pattern, const QString& text) = 0;
    virtual QStringList getFuzzyMatches(const QString& pattern,
                                      const QStringList& candidates,
                                      qreal threshold = 0.6) = 0;

    // Performance optimization
    QTimer* m_searchTimer;
    QStringList m_recentQueries;
    QVariantMap m_searchCache;

    // Search data
    QMap<QString, QString> m_searchIndex;  // id -> searchable text
    QMap<QString, QStringList> m_tagIndex; // tag -> list of ids

    // Search parameters for async operations
    QString m_pendingQuery;
    QStringList m_pendingTypes;
    QVariantMap m_pendingFilters;

    // Search options
    QVariantMap m_searchOptions;

    // Performance tracking
    qint64 m_lastSearchTime;
    int m_fastSearches;
    int m_slowSearches;
    mutable int m_hitCount;
    mutable int m_missCount;
};