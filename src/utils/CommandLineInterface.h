#pragma once

#include "../core/BaseTypes.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QCommandLineParser>
#include <QCommandLineOption>

/**
 * @brief Command Line Interface for 3D Model Management Utility
 *
 * Provides comprehensive command-line support for all operations including:
 * - Database management (reset, backup, restore)
 * - Settings management (reset, import, export)
 * - Model operations (import, export, repair)
 * - Batch processing and automation
 * - Headless operation support
 */
class CommandLineInterface : public QObject
{
    Q_OBJECT

public:
    explicit CommandLineInterface(QObject* parent = nullptr);
    virtual ~CommandLineInterface() = default;

    // CLI configuration
    struct CLIOptions {
        // Database options
        bool resetDatabase;
        bool backupDatabase;
        bool restoreDatabase;
        QString backupPath;
        QString restorePath;

        // Settings options
        bool resetSettings;
        bool importSettings;
        bool exportSettings;
        QString settingsPath;

        // Model operations
        bool importModels;
        bool exportModels;
        bool repairModels;
        bool generateThumbnails;
        QStringList importPaths;
        QString exportPath;
        QString exportFormat;
        QString repairMode;

        // GPU and performance
        QString gpuAdapter;
        bool enableHardwareAcceleration;
        int maxMemoryUsage;

        // CNC operations
        bool generateGCode;
        bool optimizeNesting;
        QString outputDirectory;

        // Logging and debug
        bool verboseLogging;
        bool debugMode;
        QString logLevel;
        QString logFile;

        // Batch operations
        bool batchMode;
        QString batchScript;

        CLIOptions() : resetDatabase(false), backupDatabase(false), restoreDatabase(false),
                      resetSettings(false), importSettings(false), exportSettings(false),
                      importModels(false), exportModels(false), repairModels(false), generateThumbnails(false),
                      enableHardwareAcceleration(true), maxMemoryUsage(2048),
                      generateGCode(false), optimizeNesting(false),
                      verboseLogging(false), debugMode(false), logLevel("info"), batchMode(false) {}
    };

    // Main CLI processing
    virtual bool processCommandLine(const QCoreApplication& app) = 0;
    virtual CLIOptions parseArguments(const QStringList& arguments) = 0;

    // Command execution
    virtual bool executeDatabaseReset() = 0;
    virtual bool executeDatabaseBackup(const QString& path) = 0;
    virtual bool executeDatabaseRestore(const QString& path) = 0;

    virtual bool executeSettingsReset() = 0;
    virtual bool executeSettingsImport(const QString& path) = 0;
    virtual bool executeSettingsExport(const QString& path) = 0;

    virtual bool executeModelImport(const QStringList& paths) = 0;
    virtual bool executeModelExport(const QString& path, const QString& format) = 0;
    virtual bool executeModelRepair(const QString& mode) = 0;
    virtual bool executeThumbnailGeneration() = 0;

    virtual bool executeGCodeGeneration(const QString& outputDir) = 0;
    virtual bool executeNestingOptimization() = 0;

    // Batch processing
    virtual bool executeBatchScript(const QString& scriptPath) = 0;
    virtual bool executeBatchImport(const QStringList& paths) = 0;
    virtual bool executeBatchExport(const QString& format, const QString& outputDir) = 0;

    // Configuration and setup
    virtual void configureGPU(const QString& adapter) = 0;
    virtual void configureMemory(int maxUsageMB) = 0;
    virtual void configureLogging(const QString& level, const QString& file) = 0;

    // Help and information
    virtual void showHelp() = 0;
    virtual void showVersion() = 0;
    virtual void showSystemInfo() = 0;

    // Validation
    virtual bool validateOptions(const CLIOptions& options) = 0;
    virtual QStringList getValidationErrors(const CLIOptions& options) = 0;

signals:
    // Operation progress
    void operationProgress(const QString& operation, int percentage);
    void operationCompleted(const QString& operation, bool success);
    void operationFailed(const QString& operation, const QString& error);

    // Batch processing
    void batchOperationStarted(const QString& script);
    void batchOperationProgress(int current, int total);
    void batchOperationCompleted();

protected:
    // Parser setup
    virtual void setupCommandLineParser(QCommandLineParser& parser) = 0;
    virtual void addDatabaseOptions(QCommandLineParser& parser) = 0;
    virtual void addSettingsOptions(QCommandLineParser& parser) = 0;
    virtual void addModelOptions(QCommandLineParser& parser) = 0;
    virtual void addCNCOptions(QCommandLineParser& parser) = 0;
    virtual void addPerformanceOptions(QCommandLineParser& parser) = 0;

    // Option processing
    virtual CLIOptions extractOptions(const QCommandLineParser& parser) = 0;
    virtual bool validatePaths(const QStringList& paths) = 0;
    virtual bool validateFormats(const QStringList& formats) = 0;

    // Execution helpers
    virtual bool executeWithProgress(const std::function<bool()>& operation,
                                   const QString& operationName) = 0;

    virtual void logOperation(const QString& operation, const QString& details) = 0;
    virtual void logError(const QString& operation, const QString& error) = 0;

    // File operations
    virtual bool ensureDirectoryExists(const QString& path) = 0;
    virtual QString generateTimestampedFilename(const QString& baseName, const QString& extension) = 0;

    // Configuration
    CLIOptions m_options;
    QCommandLineParser m_parser;
};