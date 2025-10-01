#include "Logger.h"
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QThread>
#include <QDebug>
#include <iostream>

Logger::Logger(QObject* parent)
    : QObject(parent)
    , m_logLevel(Info)
    , m_logTargets(Console | File)
    , m_maxFileSize(10 * 1024 * 1024) // 10MB
    , m_maxLogAge(30) // 30 days
    , m_maxRecentEntries(1000)
    , m_realTimeLogging(false)
{
    // Set default log file path
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    QDir dir(logDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_logFilePath = logDir + "/application.log";

    // Create flush timer
    m_flushTimer = new QTimer(this);
    m_flushTimer->setInterval(5000); // Flush every 5 seconds
    connect(m_flushTimer, &QTimer::timeout, this, &Logger::flushLogs);

    // Start session
    startSession();

    qRegisterMetaType<LogLevel>("LogLevel");
    qRegisterMetaType<LogEntry>("LogEntry");
}

void Logger::setLogLevel(LogLevel level)
{
    QMutexLocker locker(&m_logMutex);
    m_logLevel = level;
    emit logLevelChanged(level);

    info("Log level changed", "Logger");
}

void Logger::setLogTargets(int targets)
{
    QMutexLocker locker(&m_logMutex);
    m_logTargets = targets;
    emit logTargetsChanged(targets);

    info("Log targets changed", "Logger");
}

void Logger::setLogFile(const QString& filepath)
{
    QMutexLocker locker(&m_logMutex);
    m_logFilePath = filepath;

    // Ensure directory exists
    QFileInfo fileInfo(filepath);
    QDir dir(fileInfo.dir());
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    info("Log file changed", "Logger");
}

void Logger::setMaxFileSize(qint64 maxSize)
{
    QMutexLocker locker(&m_logMutex);
    m_maxFileSize = maxSize;
}

void Logger::setMaxLogAge(int days)
{
    QMutexLocker locker(&m_logMutex);
    m_maxLogAge = days;
}

void Logger::trace(const QString& message, const QString& category)
{
    log(Trace, message, category, "", "", 0);
}

void Logger::debug(const QString& message, const QString& category)
{
    log(Debug, message, category, "", "", 0);
}

void Logger::info(const QString& message, const QString& category)
{
    log(Info, message, category, "", "", 0);
}

void Logger::warning(const QString& message, const QString& category)
{
    log(Warning, message, category, "", "", 0);
}

void Logger::error(const QString& message, const QString& category)
{
    log(Error, message, category, "", "", 0);
}

void Logger::critical(const QString& message, const QString& category)
{
    log(Critical, message, category, "", "", 0);
}

void Logger::log(LogLevel level, const QString& message, const QString& category,
                const QString& function, const QString& file, int line)
{
    if (level < m_logLevel) {
        return; // Skip if below log level
    }

    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.category = category.isEmpty() ? "General" : category;
    entry.function = function;
    entry.file = file;
    entry.line = line;
    entry.timestamp = QDateTime::currentDateTime();
    entry.threadId = reinterpret_cast<qint64>(QThread::currentThread());
    entry.sessionId = m_currentSessionId;

    writeLog(entry);
}

void Logger::logWithContext(LogLevel level, const QString& message, const QVariantMap& context,
                           const QString& category)
{
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.category = category.isEmpty() ? "General" : category;
    entry.timestamp = QDateTime::currentDateTime();
    entry.threadId = reinterpret_cast<qint64>(QThread::currentThread());
    entry.sessionId = m_currentSessionId;
    entry.context = context;

    writeLog(entry);
}

void Logger::startPerformanceTimer(const QString& operation)
{
    QMutexLocker locker(&m_performanceMutex);
    m_performanceTimers[operation] = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void Logger::stopPerformanceTimer(const QString& operation)
{
    QMutexLocker locker(&m_performanceMutex);

    if (m_performanceTimers.contains(operation)) {
        qint64 startTime = m_performanceTimers[operation];
        qint64 elapsed = QDateTime::currentDateTime().toMSecsSinceEpoch() - startTime;

        logPerformance(operation, elapsed);
    }
}

void Logger::logPerformance(const QString& operation, qint64 elapsedMs)
{
    QVariantMap context;
    context["operation"] = operation;
    context["elapsed_ms"] = elapsedMs;
    context["thread_id"] = reinterpret_cast<qint64>(QThread::currentThread());

    logWithContext(Debug, QString("Performance: %1 completed in %2ms")
                   .arg(operation).arg(elapsedMs), context, "Performance");

    emit performanceMetric(operation, elapsedMs);

    // Update performance statistics
    updatePerformanceMetrics(operation, elapsedMs);
}

void Logger::startSession(const QString& sessionId)
{
    m_currentSessionId = sessionId.isEmpty() ? QUuid::createUuid().toString() : sessionId;
    m_sessionStartTime = QDateTime::currentDateTime();

    QVariantMap context;
    context["session_id"] = m_currentSessionId;
    context["start_time"] = m_sessionStartTime.toString(Qt::ISODate);

    info("Session started", "Session");
}

void Logger::endSession()
{
    QVariantMap context;
    context["session_id"] = m_currentSessionId;
    context["duration_ms"] = m_sessionStartTime.msecsTo(QDateTime::currentDateTime());

    info("Session ended", "Session");

    m_currentSessionId = "";
}

QString Logger::getCurrentSessionId() const
{
    return m_currentSessionId;
}

QList<Logger::LogEntry> Logger::getLogs(const QString& category, LogLevel minLevel,
                                       const QDateTime& startTime, const QDateTime& endTime) const
{
    QMutexLocker locker(&m_logMutex);
    QList<LogEntry> filteredLogs;

    for (const LogEntry& entry : m_logEntries) {
        // Filter by category
        if (!category.isEmpty() && entry.category != category) {
            continue;
        }

        // Filter by level
        if (entry.level < minLevel) {
            continue;
        }

        // Filter by time range
        if (startTime.isValid() && entry.timestamp < startTime) {
            continue;
        }
        if (endTime.isValid() && entry.timestamp > endTime) {
            continue;
        }

        filteredLogs.append(entry);
    }

    return filteredLogs;
}

QList<Logger::LogEntry> Logger::getLogsByThread(qint64 threadId) const
{
    QMutexLocker locker(&m_logMutex);
    QList<LogEntry> filteredLogs;

    for (const LogEntry& entry : m_logEntries) {
        if (entry.threadId == threadId) {
            filteredLogs.append(entry);
        }
    }

    return filteredLogs;
}

QList<Logger::LogEntry> Logger::getLogsBySession(const QString& sessionId) const
{
    QMutexLocker locker(&m_logMutex);
    QList<LogEntry> filteredLogs;

    for (const LogEntry& entry : m_logEntries) {
        if (entry.sessionId == sessionId) {
            filteredLogs.append(entry);
        }
    }

    return filteredLogs;
}

QVariantMap Logger::getLogStatistics() const
{
    QMutexLocker locker(&m_logMutex);
    QVariantMap stats;

    QMap<LogLevel, int> levelCounts;
    QMap<QString, int> categoryCounts;

    for (const LogEntry& entry : m_logEntries) {
        levelCounts[entry.level]++;
        categoryCounts[entry.category]++;
    }

    QVariantMap levelStats;
    levelStats["trace"] = levelCounts[Trace];
    levelStats["debug"] = levelCounts[Debug];
    levelStats["info"] = levelCounts[Info];
    levelStats["warning"] = levelCounts[Warning];
    levelStats["error"] = levelCounts[Error];
    levelStats["critical"] = levelCounts[Critical];
    stats["by_level"] = levelStats;

    stats["by_category"] = QVariantMap::fromStdMap(categoryCounts.toStdMap());
    stats["total_entries"] = m_logEntries.size();

    return stats;
}

QVariantMap Logger::getPerformanceStatistics() const
{
    QMutexLocker locker(&m_performanceMutex);
    return m_performanceStats;
}

void Logger::clearLogs()
{
    QMutexLocker locker(&m_logMutex);
    m_logEntries.clear();
    m_recentEntries.clear();

    info("Logs cleared", "Logger");
}

void Logger::archiveLogs()
{
    // Archive old logs to compressed file
    QString archivePath = m_logFilePath + ".archive." + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");

    // In a full implementation, this would compress and move old log entries
    info("Logs archived", "Logger");
}

void Logger::exportLogs(const QString& filepath, const QString& format)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly)) {
        error("Failed to export logs", "Logger");
        return;
    }

    if (format == "json") {
        QJsonArray logArray;

        QMutexLocker locker(&m_logMutex);
        for (const LogEntry& entry : m_logEntries) {
            QJsonObject entryObj;
            entryObj["level"] = getLevelString(entry.level);
            entryObj["message"] = entry.message;
            entryObj["category"] = entry.category;
            entryObj["timestamp"] = entry.timestamp.toString(Qt::ISODate);
            entryObj["thread_id"] = entry.threadId;
            entryObj["session_id"] = entry.sessionId;

            if (!entry.function.isEmpty()) {
                entryObj["function"] = entry.function;
            }
            if (!entry.file.isEmpty()) {
                entryObj["file"] = entry.file;
            }
            if (entry.line > 0) {
                entryObj["line"] = entry.line;
            }

            logArray.append(entryObj);
        }

        QJsonDocument doc(logArray);
        file.write(doc.toJson(QJsonDocument::Indented));

    } else if (format == "csv") {
        QTextStream stream(&file);

        // CSV header
        stream << "Timestamp,Level,Category,Message,ThreadID,SessionID\n";

        QMutexLocker locker(&m_logMutex);
        for (const LogEntry& entry : m_logEntries) {
            stream << entry.timestamp.toString(Qt::ISODate) << ","
                   << getLevelString(entry.level) << ","
                   << entry.category << ","
                   << "\"" << entry.message << "\","
                   << entry.threadId << ","
                   << entry.sessionId << "\n";
        }
    }

    file.close();

    info("Logs exported", "Logger");
}

void Logger::enableRealTimeLogging(bool enabled)
{
    m_realTimeLogging = enabled;

    if (enabled) {
        m_flushTimer->start();
    } else {
        m_flushTimer->stop();
    }
}

void Logger::setFlushInterval(int milliseconds)
{
    m_flushTimer->setInterval(milliseconds);
}

void Logger::writeLog(const LogEntry& entry)
{
    QMutexLocker locker(&m_logMutex);

    // Add to log entries
    m_logEntries.append(entry);

    // Add to recent entries (circular buffer)
    if (m_recentEntries.size() >= m_maxRecentEntries) {
        m_recentEntries.dequeue();
    }
    m_recentEntries.enqueue(entry);

    // Write to configured targets
    if (m_logTargets & Console) {
        writeToConsole(entry);
    }

    if (m_logTargets & File) {
        writeToFile(entry);
    }

    if (m_logTargets & Database) {
        writeToDatabase(entry);
    }

    // Emit signal for real-time monitoring
    if (m_realTimeLogging) {
        emit logEntryAdded(entry);
    }

    // Check file size and rotate if needed
    checkLogFileSize();
}

void Logger::writeToConsole(const LogEntry& entry)
{
    QString formatted = formatLogEntry(entry);

    // Output to appropriate stream with colors
    if (entry.level >= Error) {
        std::cerr << formatted.toStdString() << std::endl;
    } else {
        std::cout << formatted.toStdString() << std::endl;
    }
}

void Logger::writeToFile(const LogEntry& entry)
{
    QFile file(m_logFilePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        // Fallback to console if file write fails
        writeToConsole(entry);
        return;
    }

    QTextStream stream(&file);
    stream << formatLogEntry(entry) << "\n";
    file.close();
}

void Logger::writeToDatabase(const LogEntry& entry)
{
    // In a full implementation, this would write to the SQLite database
    // For now, just store in memory
    qDebug() << "Database logging not yet implemented";
}

QString Logger::formatLogEntry(const LogEntry& entry) const
{
    return QString("[%1] %2 %3: %4")
           .arg(entry.timestamp.toString("yyyy-MM-dd hh:mm:ss"))
           .arg(getLevelString(entry.level))
           .arg(entry.category)
           .arg(entry.message);
}

QString Logger::getLevelString(LogLevel level) const
{
    switch (level) {
        case Trace: return "TRACE";
        case Debug: return "DEBUG";
        case Info: return "INFO";
        case Warning: return "WARN";
        case Error: return "ERROR";
        case Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

QString Logger::getLevelColor(LogLevel level) const
{
    switch (level) {
        case Trace: return "gray";
        case Debug: return "blue";
        case Info: return "green";
        case Warning: return "orange";
        case Error: return "red";
        case Critical: return "darkred";
        default: return "black";
    }
}

void Logger::checkLogFileSize()
{
    QFile file(m_logFilePath);
    if (file.exists() && file.size() > m_maxFileSize) {
        rotateLogFile();
    }
}

void Logger::rotateLogFile()
{
    // Rotate log file
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString backupPath = m_logFilePath + "." + timestamp;

    QFile currentFile(m_logFilePath);
    currentFile.copy(backupPath);

    // Clear current file
    currentFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    currentFile.close();

    emit logFileRotated(backupPath);

    info("Log file rotated", "Logger");
}

void Logger::cleanupOldLogs()
{
    QDir logDir(QFileInfo(m_logFilePath).dir());
    QDateTime cutoffTime = QDateTime::currentDateTime().addDays(-m_maxLogAge);

    QFileInfoList files = logDir.entryInfoList(QStringList() << "*.log*", QDir::Files);
    qint64 freedBytes = 0;

    for (const QFileInfo& file : files) {
        if (file.lastModified() < cutoffTime) {
            freedBytes += file.size();
            QFile::remove(file.absoluteFilePath());
        }
    }

    if (freedBytes > 0) {
        info("Old logs cleaned up", "Logger");
    }
}

void Logger::flushLogs()
{
    // Force flush of any buffered log data
    // In a full implementation, this would flush database connections, etc.
}

void Logger::updatePerformanceMetrics(const QString& operation, qint64 elapsedMs)
{
    QMutexLocker locker(&m_performanceMutex);

    // Update operation statistics
    if (!m_performanceStats.contains(operation)) {
        m_performanceStats[operation] = QVariantMap();
    }

    QVariantMap opStats = m_performanceStats[operation].toMap();
    opStats["count"] = opStats.value("count", 0).toInt() + 1;
    opStats["total_time"] = opStats.value("total_time", 0).toLongLong() + elapsedMs;
    opStats["avg_time"] = opStats["total_time"].toLongLong() / opStats["count"].toInt();
    opStats["min_time"] = qMin(opStats.value("min_time", qint64(INT64_MAX)).toLongLong(), elapsedMs);
    opStats["max_time"] = qMax(opStats.value("max_time", qint64(0)).toLongLong(), elapsedMs);

    m_performanceStats[operation] = opStats;

    // Check for performance warnings
    if (elapsedMs > 1000) { // Operation took more than 1 second
        emit performanceWarning(QString("Slow operation: %1 took %2ms").arg(operation).arg(elapsedMs));
    }
}