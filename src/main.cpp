#include "ui/MainWindow.h"
#include "core/DatabaseManager.h"
#include "core/ModelService.h"
#include "core/SearchService.h"
#include "core/TagManager.h"
#include "utils/Logger.h"
#include "utils/CommandLineInterface.h"
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStyleFactory>
#include <QTranslator>
#include <QLocale>
#include <QIcon>
#include <QMessageBox>
#include <QDebug>

/**
 * @brief Main application entry point
 *
 * Initializes the 3D Model Management Utility with proper command line
 * argument parsing, database initialization, and service setup.
 */
int main(int argc, char *argv[])
{
    // Enable high DPI scaling
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    // Create application instance
    QApplication app(argc, argv);
    app.setApplicationName("3D Model Management Utility");
    app.setApplicationVersion("1.0.0");
    app.setApplicationDisplayName("3D Model Manager");
    app.setOrganizationName("3D Model Management Team");
    app.setOrganizationDomain("3dmodelmanager.org");

    // Set application icon (will be loaded from resources)
    app.setWindowIcon(QIcon(":/icons/app_icon.png"));

    // Set application attributes for better performance and compatibility
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
    app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    // Parse command line arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("A fluid, intuitive 3D model management system");
    parser.addHelpOption();
    parser.addVersionOption();

    // Add command line options as specified in the constitution
    QCommandLineOption resetDatabaseOption("reset-database",
        QCoreApplication::translate("main", "Reset the project database"));
    parser.addOption(resetDatabaseOption);

    QCommandLineOption resetSettingsOption("reset-settings",
        QCoreApplication::translate("main", "Restore default settings"));
    parser.addOption(resetSettingsOption);

    QCommandLineOption importOption("import",
        QCoreApplication::translate("main", "Import project or model file"),
        QCoreApplication::translate("main", "path"));
    parser.addOption(importOption);

    QCommandLineOption exportOption("export",
        QCoreApplication::translate("main", "Export current project"),
        QCoreApplication::translate("main", "path"));
    parser.addOption(exportOption);

    QCommandLineOption gpuOption("gpu",
        QCoreApplication::translate("main", "Specify GPU adapter (auto|cpu|dgpu|adapterId)"),
        QCoreApplication::translate("main", "adapter"));
    parser.addOption(gpuOption);

    QCommandLineOption repairOption("repair",
        QCoreApplication::translate("main", "Set repair behavior (auto|assist|off)"),
        QCoreApplication::translate("main", "mode"));
    parser.addOption(repairOption);

    QCommandLineOption themeOption("theme",
        QCoreApplication::translate("main", "Set UI theme (light|dark|system)"),
        QCoreApplication::translate("main", "style"));
    parser.addOption(themeOption);

    QCommandLineOption debugOption("debug",
        QCoreApplication::translate("main", "Enable debug logging"));
    parser.addOption(debugOption);

    QCommandLineOption verboseOption("verbose",
        QCoreApplication::translate("main", "Enable verbose logging"));
    parser.addOption(verboseOption);

    // Parse the command line
    parser.process(app);

    // Initialize logging system early
    Logger* logger = new Logger(&app);

    // Configure logging based on command line arguments
    if (parser.isSet("debug")) {
        logger->setLogLevel(Logger::Debug);
        logger->info("Debug logging enabled", "Application");
    } else {
        logger->setLogLevel(Logger::Info);
    }

    // Enable real-time logging if requested
    if (parser.isSet("verbose")) {
        logger->enableRealTimeLogging(true);
        logger->info("Real-time logging enabled", "Application");
    }

    logger->info("3D Model Management Utility starting", "Application");

    // Set theme based on command line or system preference
    QString theme = parser.value(themeOption);
    if (theme.isEmpty()) {
        // Detect system theme preference
        QPalette palette = QApplication::palette();
        bool isDark = palette.color(QPalette::Window).lightness() < 128;
        theme = isDark ? "dark" : "light";
    }

    // Apply theme
    if (theme == "dark") {
        QApplication::setStyle(QStyleFactory::create("Fusion"));
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::black);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        QApplication::setPalette(darkPalette);
    }

    // Initialize core services (logger already created above)
    CommandLineInterface* cli = new CommandLineInterface(&app);
    DatabaseManager* databaseManager = new DatabaseManager(&app);
    ModelService* modelService = new ModelService(&app);
    SearchService* searchService = new SearchService(&app);
    TagManager* tagManager = new TagManager(&app);

    // Connect logger to services for comprehensive error tracking and performance monitoring
    QObject::connect(databaseManager, &DatabaseManager::databaseError, logger,
        [logger](const QString& error, const QString& details) {
            logger->error(QString("Database error: %1 - %2").arg(error).arg(details), "Database");
        });

    QObject::connect(modelService, &ModelService::errorOccurred, logger,
        [logger](const QString& operation, const QString& error, const QString& details) {
            logger->error(QString("Model service error in %1: %2 - %3").arg(operation).arg(error).arg(details), "ModelService");
        });

    // Connect performance monitoring
    QObject::connect(modelService, &ModelService::modelLoaded, logger,
        [logger](const ModelMetadata& model) {
            logger->info(QString("Model loaded: %1 (%2 bytes)").arg(model.filename).arg(model.fileSize), "ModelService");
        });

    QObject::connect(searchService, &SearchService::searchCompleted, logger,
        [logger](const QString& query, const QList<SearchResult>& results) {
            logger->debug(QString("Search completed: '%1' -> %2 results").arg(query).arg(results.count()), "SearchService");
        });

    // Connect tag manager events
    QObject::connect(tagManager, &TagManager::tagCreated, logger,
        [logger](const QString& tag) {
            logger->info(QString("Tag created: %1").arg(tag), "TagManager");
        });

    // Connect canvas error events (window not created yet, will be connected after creation)

    // Process command line arguments
    if (!cli->processCommandLine(app)) {
        logger->error("Command line processing failed", "CLI");
        return 1;
    }

    // Check if this is a command-line only operation
    CommandLineInterface::CLIOptions options = cli->parseArguments(app.arguments());
    bool hasGUIOperations = !options.importModels && !options.exportModels &&
                           !options.repairModels && !options.generateThumbnails &&
                           !options.generateGCode && !options.optimizeNesting;

    if (options.batchMode || !hasGUIOperations) {
        // Command-line only operation, exit after completion
        logger->info("Command-line operation completed, exiting", "Application");
        return 0;
    }

    // Initialize database
    QString databasePath = getDatabasePath();
    if (parser.isSet(resetDatabaseOption)) {
        // Reset database if requested
        QFile::remove(databasePath);
    }

    if (!databaseManager->initialize(databasePath)) {
        QMessageBox::critical(nullptr, "Database Error",
            QString("Failed to initialize database: %1").arg(databaseManager->lastError()));
        return 1;
    }

    // Create and show main window
    MainWindow window;
    window.resize(1600, 900);  // Default size

    // Connect canvas error events now that window exists
    QObject::connect(&window, &MainWindow::errorOccurred, logger,
        [logger](const QString& error, const QString& details) {
            logger->error(QString("Canvas error: %1 - %2").arg(error).arg(details), "Canvas");
        });

    // Process command line arguments in main window
    window.processCommandLineArguments();

    // Show window and start event loop
    window.show();

    int result = app.exec();

    // Cleanup
    window.shutdown();
    databaseManager->close();

    return result;
}

/**
 * @brief Get the appropriate database path for the current platform
 */
QString getDatabasePath()
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // Create data directory if it doesn't exist
    QDir dir(dataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    return dataDir + "/models.db";
}

/**
 * @brief Get the appropriate cache directory for the current platform
 */
QString getCacheDirectory()
{
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    // Create cache directory if it doesn't exist
    QDir dir(cacheDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    return cacheDir;
}

/**
 * @brief Get the appropriate thumbnails directory
 */
QString getThumbnailsDirectory()
{
    QString thumbsDir = getCacheDirectory() + "/thumbnails";

    // Create thumbnails directory if it doesn't exist
    QDir dir(thumbsDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    return thumbsDir;
}