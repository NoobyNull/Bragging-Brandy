#pragma once

#include "../core/BaseTypes.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>
#include <QMutex>
#include <QQueue>

/**
 * @brief Comprehensive logging and performance monitoring system
 *
 * Provides structured logging with multiple output targets, performance monitoring,
 * and debugging capabilities as specified in the constitution.
 */
class Logger : public QObject
{
    Q_OBJECT

public:
    explicit Logger(QObject* parent = nullptr);
    virtual ~Logger() = default;

    // Log levels
    enum LogLevel {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warning = 3,
        Error = 4,
        Critical = 5
    };

    // Log targets
    enum LogTarget {
        Console = 0x01,
        File = 0x02,
        Database = 0x04,
        Network = 0x08,
        All = Console | File | Database | Network
    };

    // Log entry structure
    struct LogEntry {
        LogLevel level;
        QString message;
        QString category;
        QString function;
        QString file;
        int line;
        QDateTime timestamp;
        QVariantMap context;
        qint64 threadId;
        QString sessionId;

        LogEntry() : level(Info), line(0), threadId(0) {}
        LogEntry(LogLevel lvl, const QString& msg, const QString& cat = QString())
            : level(lvl), message(msg), category(cat), line(0), threadId(0) {}
    };

    // Configuration
    void setLogLevel(LogLevel level);
    void setLogTargets(int targets);
    void setLogFile(const QString& filepath);
    void setMaxFileSize(qint64 maxSize);
    void setMaxLogAge(int days);

    // Logging functions
    void trace(const QString& message, const QString& category = QString());
    void debug(const QString& message, const QString& category = QString());
    void info(const QString& message, const QString& category = QString());
    void warning(const QString& message, const QString& category = QString());
    void error(const QString& message, const QString& category = QString());
    void critical(const QString& message, const QString& category = QString());

    // Advanced logging
    void log(LogLevel level, const QString& message, const QString& category = QString(),
             const QString& function = QString(), const QString& file = QString(), int line = 0);

    void logWithContext(LogLevel level, const QString& message, const QVariantMap& context,
                       const QString& category = QString());

    // Performance monitoring
    void startPerformanceTimer(const QString& operation);
    void stopPerformanceTimer(const QString& operation);
    void logPerformance(const QString& operation, qint64 elapsedMs);

    // Session management
    void startSession(const QString& sessionId = QString());
    void endSession();
    QString getCurrentSessionId() const;

    // Log filtering and querying
    QList<LogEntry> getLogs(const QString& category = QString(),
                           LogLevel minLevel = Trace,
                           const QDateTime& startTime = QDateTime(),
                           const QDateTime& endTime = QDateTime()) const;

    QList<LogEntry> getLogsByThread(qint64 threadId) const;
    QList<LogEntry> getLogsBySession(const QString& sessionId) const;

    // Log statistics
    QVariantMap getLogStatistics() const;
    QVariantMap getPerformanceStatistics() const;

    // Log management
    void clearLogs();
    void archiveLogs();
    void exportLogs(const QString& filepath, const QString& format = "json");

    // Real-time monitoring
    void enableRealTimeLogging(bool enabled);
    void setFlushInterval(int milliseconds);

    // Log flushing
    void flushLogs();

signals:
    // Log events
    void logEntryAdded(const LogEntry& entry);
    void logLevelChanged(LogLevel newLevel);
    void logTargetsChanged(int newTargets);

    // Performance events
    void performanceMetric(const QString& operation, qint64 elapsedMs);
    void performanceWarning(const QString& warning);

    // System events
    void logFileRotated(const QString& newFile);
    void logArchiveCreated(const QString& archivePath);

protected:
    // Core logging implementation
    void writeLog(const LogEntry& entry);
    void writeToConsole(const LogEntry& entry);
    void writeToFile(const LogEntry& entry);
    void writeToDatabase(const LogEntry& entry);

    // Log formatting
    QString formatLogEntry(const LogEntry& entry) const;
    QString getLevelString(LogLevel level) const;
    QString getLevelColor(LogLevel level) const;

    // File management
    void checkLogFileSize();
    void rotateLogFile();
    void cleanupOldLogs();

    // Performance tracking
    void updatePerformanceMetrics(const QString& operation, qint64 elapsedMs);

    // Thread-safe operations
    QMutex m_logMutex;
    QMutex m_performanceMutex;

    // Configuration
    LogLevel m_logLevel;
    int m_logTargets;
    QString m_logFilePath;
    qint64 m_maxFileSize;
    int m_maxLogAge;

    // Log storage
    QList<LogEntry> m_logEntries;
    QQueue<LogEntry> m_recentEntries;
    int m_maxRecentEntries;

    // Performance tracking
    QMap<QString, qint64> m_performanceTimers;
    QVariantMap m_performanceStats;
    QTimer* m_performanceTimer;

    // Session management
    QString m_currentSessionId;
    QDateTime m_sessionStartTime;

    // Real-time logging
    QTimer* m_flushTimer;
    bool m_realTimeLogging;
};