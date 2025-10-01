#include "MainWindow.h"
#include "ProjectCanvas.h"
#include "DesignCanvas.h"
#include "CutListCanvas.h"
#include "CNCToolsCanvas.h"
#include "SettingsCanvas.h"
#include "../core/DatabaseManager.h"
#include "../core/ModelService.h"
#include "../core/SearchService.h"
#include "../core/TagManager.h"
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QKeySequence>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_canvasTabs(nullptr)
    , m_menuBar(nullptr)
    , m_mainToolBar(nullptr)
    , m_canvasToolBar(nullptr)
    , m_statusBar(nullptr)
    , m_systemTray(nullptr)
    , m_currentCanvasName("ProjectCanvas")
    , m_isInitialized(false)
    , m_isShuttingDown(false)
    , m_lastUpdateTime(0)
    , m_frameCount(0)
{
    setWindowTitle("3D Model Management Utility");
    setMinimumSize(1200, 800);

    // Initialize settings
    QString settingsDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(settingsDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_settingsFile = settingsDir + "/settings.ini";
    m_settings = new QSettings(m_settingsFile, QSettings::IniFormat, this);

    // Setup UI components
    setupMenuBar();
    setupToolBars();
    setupStatusBar();
    setupCanvasTabs();
    setupSystemTray();

    // Load window state and settings
    loadApplicationSettings();

    // Start performance monitoring
    startTimer(1000); // Update every second
}

MainWindow::~MainWindow()
{
    // Cleanup handled by Qt parent-child system
}

void MainWindow::setupMenuBar()
{
    m_menuBar = menuBar();

    // File menu
    QMenu* fileMenu = m_menuBar->addMenu("&File");

    QAction* newProjectAction = new QAction("&New Project", this);
    newProjectAction->setShortcut(QKeySequence::New);
    newProjectAction->setStatusTip("Create a new project");
    connect(newProjectAction, &QAction::triggered, this, &MainWindow::onNewProject);
    fileMenu->addAction(newProjectAction);

    QAction* openProjectAction = new QAction("&Open Project", this);
    openProjectAction->setShortcut(QKeySequence::Open);
    openProjectAction->setStatusTip("Open an existing project");
    connect(openProjectAction, &QAction::triggered, this, &MainWindow::onOpenProject);
    fileMenu->addAction(openProjectAction);

    QAction* saveProjectAction = new QAction("&Save Project", this);
    saveProjectAction->setShortcut(QKeySequence::Save);
    saveProjectAction->setStatusTip("Save the current project");
    connect(saveProjectAction, &QAction::triggered, this, &MainWindow::onSaveProject);
    fileMenu->addAction(saveProjectAction);

    fileMenu->addSeparator();

    QAction* importModelsAction = new QAction("&Import Models", this);
    importModelsAction->setShortcut(QKeySequence("Ctrl+I"));
    importModelsAction->setStatusTip("Import 3D models");
    connect(importModelsAction, &QAction::triggered, this, &MainWindow::onImportModels);
    fileMenu->addAction(importModelsAction);

    QAction* exportProjectAction = new QAction("&Export Project", this);
    exportProjectAction->setShortcut(QKeySequence("Ctrl+E"));
    exportProjectAction->setStatusTip("Export the current project");
    connect(exportProjectAction, &QAction::triggered, this, &MainWindow::onExportProject);
    fileMenu->addAction(exportProjectAction);

    fileMenu->addSeparator();

    QAction* exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip("Exit the application");
    connect(exitAction, &QAction::triggered, this, &MainWindow::onExit);
    fileMenu->addAction(exitAction);

    // Edit menu
    QMenu* editMenu = m_menuBar->addMenu("&Edit");

    QAction* undoAction = new QAction("&Undo", this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setEnabled(false); // Will be enabled when undo is available
    connect(undoAction, &QAction::triggered, this, &MainWindow::onUndo);
    editMenu->addAction(undoAction);

    QAction* redoAction = new QAction("&Redo", this);
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setEnabled(false); // Will be enabled when redo is available
    connect(redoAction, &QAction::triggered, this, &MainWindow::onRedo);
    editMenu->addAction(redoAction);

    editMenu->addSeparator();

    QAction* cutAction = new QAction("Cu&t", this);
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, this, &MainWindow::onCut);
    editMenu->addAction(cutAction);

    QAction* copyAction = new QAction("&Copy", this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &MainWindow::onCopy);
    editMenu->addAction(copyAction);

    QAction* pasteAction = new QAction("&Paste", this);
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::onPaste);
    editMenu->addAction(pasteAction);

    QAction* deleteAction = new QAction("&Delete", this);
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::onDelete);
    editMenu->addAction(deleteAction);

    editMenu->addSeparator();

    QAction* selectAllAction = new QAction("Select &All", this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAction, &QAction::triggered, this, &MainWindow::onSelectAll);
    editMenu->addAction(selectAllAction);

    // View menu
    QMenu* viewMenu = m_menuBar->addMenu("&View");

    QAction* toggleFullScreenAction = new QAction("&Full Screen", this);
    toggleFullScreenAction->setShortcut(QKeySequence::FullScreen);
    toggleFullScreenAction->setCheckable(true);
    connect(toggleFullScreenAction, &QAction::triggered, this, &MainWindow::onToggleFullScreen);
    viewMenu->addAction(toggleFullScreenAction);

    QAction* toggleMenuBarAction = new QAction("&Menu Bar", this);
    toggleMenuBarAction->setCheckable(true);
    toggleMenuBarAction->setChecked(true);
    connect(toggleMenuBarAction, &QAction::triggered, this, &MainWindow::onToggleMenuBar);
    viewMenu->addAction(toggleMenuBarAction);

    QAction* toggleToolBarsAction = new QAction("&Toolbars", this);
    toggleToolBarsAction->setCheckable(true);
    toggleToolBarsAction->setChecked(true);
    connect(toggleToolBarsAction, &QAction::triggered, this, &MainWindow::onToggleToolBars);
    viewMenu->addAction(toggleToolBarsAction);

    QAction* toggleStatusBarAction = new QAction("&Status Bar", this);
    toggleStatusBarAction->setCheckable(true);
    toggleStatusBarAction->setChecked(true);
    connect(toggleStatusBarAction, &QAction::triggered, this, &MainWindow::onToggleStatusBar);
    viewMenu->addAction(toggleStatusBarAction);

    viewMenu->addSeparator();

    QAction* resetLayoutAction = new QAction("&Reset Layout", this);
    resetLayoutAction->setShortcut(QKeySequence("Ctrl+Shift+R"));
    connect(resetLayoutAction, &QAction::triggered, this, &MainWindow::onResetLayout);
    viewMenu->addAction(resetLayoutAction);

    // Canvas menu
    QMenu* canvasMenu = m_menuBar->addMenu("&Canvas");

    QAction* newCanvasAction = new QAction("&New Canvas", this);
    newCanvasAction->setShortcut(QKeySequence("Ctrl+T"));
    connect(newCanvasAction, &QAction::triggered, this, &MainWindow::onNewCanvas);
    canvasMenu->addAction(newCanvasAction);

    QAction* closeCanvasAction = new QAction("&Close Canvas", this);
    closeCanvasAction->setShortcut(QKeySequence("Ctrl+W"));
    connect(closeCanvasAction, &QAction::triggered, this, &MainWindow::onCloseCanvas);
    canvasMenu->addAction(closeCanvasAction);

    canvasMenu->addSeparator();

    QAction* renameCanvasAction = new QAction("&Rename Canvas", this);
    renameCanvasAction->setShortcut(QKeySequence("F2"));
    connect(renameCanvasAction, &QAction::triggered, this, &MainWindow::onRenameCanvas);
    canvasMenu->addAction(renameCanvasAction);

    QAction* duplicateCanvasAction = new QAction("&Duplicate Canvas", this);
    duplicateCanvasAction->setShortcut(QKeySequence("Ctrl+D"));
    connect(duplicateCanvasAction, &QAction::triggered, this, &MainWindow::onDuplicateCanvas);
    canvasMenu->addAction(duplicateCanvasAction);

    // Tools menu
    QMenu* toolsMenu = m_menuBar->addMenu("&Tools");

    QAction* optionsAction = new QAction("&Options", this);
    optionsAction->setShortcut(QKeySequence("Ctrl+,"));
    connect(optionsAction, &QAction::triggered, this, &MainWindow::onOptions);
    toolsMenu->addAction(optionsAction);

    QAction* checkUpdatesAction = new QAction("Check for &Updates", this);
    connect(checkUpdatesAction, &QAction::triggered, this, &MainWindow::onCheckForUpdates);
    toolsMenu->addAction(checkUpdatesAction);

    // Window menu
    QMenu* windowMenu = m_menuBar->addMenu("&Window");

    QAction* nextCanvasAction = new QAction("&Next Canvas", this);
    nextCanvasAction->setShortcut(QKeySequence("Ctrl+Tab"));
    connect(nextCanvasAction, &QAction::triggered, this, &MainWindow::onNextCanvas);
    windowMenu->addAction(nextCanvasAction);

    QAction* prevCanvasAction = new QAction("&Previous Canvas", this);
    prevCanvasAction->setShortcut(QKeySequence("Ctrl+Shift+Tab"));
    connect(prevCanvasAction, &QAction::triggered, this, &MainWindow::onPreviousCanvas);
    windowMenu->addAction(prevCanvasAction);

    // Help menu
    QMenu* helpMenu = m_menuBar->addMenu("&Help");

    QAction* userGuideAction = new QAction("&User Guide", this);
    userGuideAction->setShortcut(QKeySequence::HelpContents);
    connect(userGuideAction, &QAction::triggered, this, &MainWindow::onUserGuide);
    helpMenu->addAction(userGuideAction);

    QAction* aboutAction = new QAction("&About", this);
    aboutAction->setShortcut(QKeySequence("F1"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupToolBars()
{
    // Main toolbar
    m_mainToolBar = addToolBar("Main");
    m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // File operations
    QAction* newAction = new QAction("New", this);
    newAction->setToolTip("New Project");
    m_mainToolBar->addAction(newAction);

    QAction* openAction = new QAction("Open", this);
    openAction->setToolTip("Open Project");
    m_mainToolBar->addAction(openAction);

    QAction* saveAction = new QAction("Save", this);
    saveAction->setToolTip("Save Project");
    m_mainToolBar->addAction(saveAction);

    m_mainToolBar->addSeparator();

    // Import/Export
    QAction* importAction = new QAction("Import", this);
    importAction->setToolTip("Import Models");
    m_mainToolBar->addAction(importAction);

    QAction* exportAction = new QAction("Export", this);
    exportAction->setToolTip("Export Project");
    m_mainToolBar->addAction(exportAction);

    // Canvas toolbar
    m_canvasToolBar = addToolBar("Canvas");
    m_canvasToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QAction* newCanvasAction = new QAction("New Canvas", this);
    newCanvasAction->setToolTip("Create New Canvas");
    m_canvasToolBar->addAction(newCanvasAction);

    QAction* closeCanvasAction = new QAction("Close Canvas", this);
    closeCanvasAction->setToolTip("Close Current Canvas");
    m_canvasToolBar->addAction(closeCanvasAction);
}

void MainWindow::setupStatusBar()
{
    m_statusBar = statusBar();

    // Performance info label
    QLabel* perfLabel = new QLabel("Ready", this);
    m_statusBar->addWidget(perfLabel);

    m_statusBar->addPermanentWidget(perfLabel);

    // Canvas info label
    QLabel* canvasLabel = new QLabel("Project Canvas", this);
    m_statusBar->addPermanentWidget(canvasLabel);

    // Memory usage
    QLabel* memoryLabel = new QLabel("0 MB", this);
    m_statusBar->addPermanentWidget(memoryLabel);
}

void MainWindow::setupCanvasTabs()
{
    // Create central widget with tabbed interface
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    // Create tab widget for canvases
    m_canvasTabs = new QTabWidget(centralWidget);
    m_canvasTabs->setTabsClosable(true);
    m_canvasTabs->setMovable(true);
    m_canvasTabs->setDocumentMode(true);

    connect(m_canvasTabs, &QTabWidget::currentChanged, this, &MainWindow::onCanvasTabChanged);
    connect(m_canvasTabs, &QTabWidget::tabCloseRequested, this, &MainWindow::onCanvasCloseRequested);

    layout->addWidget(m_canvasTabs);

    // Create default Project Canvas
    createDefaultCanvas();
}

void MainWindow::setupSystemTray()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    m_systemTray = new QSystemTrayIcon(this);
    m_systemTray->setIcon(QIcon(":/icons/app_icon.png"));

    QMenu* trayMenu = new QMenu(this);

    QAction* showAction = new QAction("Show", this);
    connect(showAction, &QAction::triggered, this, &MainWindow::onShowWindow);
    trayMenu->addAction(showAction);

    QAction* minimizeAction = new QAction("Minimize to Tray", this);
    connect(minimizeAction, &QAction::triggered, this, &MainWindow::onMinimizeToTray);
    trayMenu->addAction(minimizeAction);

    trayMenu->addSeparator();

    QAction* quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, this, &MainWindow::onExit);
    trayMenu->addAction(quitAction);

    m_systemTray->setContextMenu(trayMenu);
    m_systemTray->setToolTip("3D Model Management Utility");

    connect(m_systemTray, &QSystemTrayIcon::activated, this, &MainWindow::onSystemTrayActivated);

    m_systemTray->show();
}

void MainWindow::createDefaultCanvas()
{
    // Create the default Project Canvas
    ProjectCanvas* projectCanvas = new ProjectCanvas(this);

    // Add to canvas registry
    m_canvases[projectCanvas->canvasName()] = projectCanvas;

    // Add to tab widget
    int tabIndex = m_canvasTabs->addTab(projectCanvas, projectCanvas->canvasName());
    m_canvasTabs->setCurrentIndex(tabIndex);

    connect(projectCanvas, &BaseCanvas::errorOccurred, this, [this](const QString& error, const QString& details) {
        statusBar()->showMessage(QString("Canvas Error: %1").arg(error), 5000);
    });
}

bool MainWindow::initialize()
{
    if (m_isInitialized) {
        return true;
    }

    // Initialize core services
    DatabaseManager* dbManager = new DatabaseManager(this);
    ModelService* modelService = new ModelService(this);
    SearchService* searchService = new SearchService(this);
    TagManager* tagManager = new TagManager(this);

    // Initialize database
    QString databasePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/models.db";
    if (!dbManager->initialize(databasePath)) {
        QMessageBox::critical(this, "Initialization Error",
            "Failed to initialize database. The application will now exit.");
        return false;
    }

    // Rebuild search index
    searchService->rebuildIndex();

    m_isInitialized = true;
    statusBar()->showMessage("Ready", 2000);

    return true;
}

void MainWindow::shutdown()
{
    if (m_isShuttingDown) {
        return;
    }

    m_isShuttingDown = true;

    // Save window state and settings
    saveWindowState();
    saveApplicationSettings();

    // Close database connections
    // This is handled automatically by Qt's parent-child system
}

void MainWindow::addCanvas(BaseCanvas* canvas)
{
    if (!canvas) {
        return;
    }

    QString canvasName = canvas->canvasName();
    m_canvases[canvasName] = canvas;

    // Add to tab widget
    int tabIndex = m_canvasTabs->addTab(canvas, canvasName);
    m_canvasTabs->setCurrentIndex(tabIndex);

    connect(canvas, &BaseCanvas::errorOccurred, this, [this](const QString& error, const QString& details) {
        statusBar()->showMessage(QString("Canvas Error: %1").arg(error), 5000);
    });
}

void MainWindow::removeCanvas(const QString& canvasName)
{
    if (m_canvases.contains(canvasName)) {
        BaseCanvas* canvas = m_canvases[canvasName];
        m_canvasTabs->removeTab(m_canvasTabs->indexOf(canvas));
        m_canvases.remove(canvasName);
        canvas->deleteLater();
    }
}

BaseCanvas* MainWindow::getCurrentCanvas() const
{
    return getCanvas(m_currentCanvasName);
}

BaseCanvas* MainWindow::getCanvas(const QString& canvasName) const
{
    return m_canvases.value(canvasName, nullptr);
}

QStringList MainWindow::getCanvasNames() const
{
    return m_canvases.keys();
}

void MainWindow::switchToCanvas(const QString& canvasName)
{
    if (m_canvases.contains(canvasName)) {
        BaseCanvas* canvas = m_canvases[canvasName];
        int tabIndex = m_canvasTabs->indexOf(canvas);
        if (tabIndex != -1) {
            m_canvasTabs->setCurrentIndex(tabIndex);
        }
    }
}

void MainWindow::processCommandLineArguments()
{
    // This would process command line arguments passed to the application
    // For now, just a placeholder
    qDebug() << "Processing command line arguments";
}

void MainWindow::loadApplicationSettings()
{
    // Load window geometry and state
    if (m_settings->contains("geometry")) {
        restoreGeometry(m_settings->value("geometry").toByteArray());
    }

    if (m_settings->contains("windowState")) {
        restoreState(m_settings->value("windowState").toByteArray());
    }

    // Load other application settings
    // This would include theme, toolbar visibility, etc.
}

void MainWindow::saveApplicationSettings()
{
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("windowState", saveState());

    m_settings->sync();
}

void MainWindow::saveWindowState()
{
    // Save current layout and canvas states
    // This would save the current canvas layout and widget positions
}

void MainWindow::restoreWindowState()
{
    // Restore canvas layout and widget positions
    // This would restore the saved canvas layout
}

void MainWindow::resetLayout()
{
    // Reset all canvases to default layout
    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Reset Layout",
        "This will reset all canvas layouts to default. Continue?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Reset layout logic would go here
        statusBar()->showMessage("Layout reset to default", 2000);
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_isShuttingDown) {
        event->accept();
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Exit Application",
        "Are you sure you want to exit?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        shutdown();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::changeEvent(QEvent* event)
{
    QMainWindow::changeEvent(event);

    if (event->type() == QEvent::WindowStateChange) {
        // Update system tray visibility based on window state
        if (m_systemTray && windowState() == Qt::WindowMinimized) {
            // Window was minimized, could hide to tray if configured
        }
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::timerEvent(QTimerEvent* event)
{
    QMainWindow::timerEvent(event);

    // Update performance metrics every second
    m_frameCount++;

    if (m_frameCount % 60 == 0) { // Update every minute
        updatePerformanceDisplay();
    }
}

void MainWindow::updatePerformanceDisplay()
{
    // Update status bar with current performance metrics
    if (m_statusBar) {
        // This would display current FPS, memory usage, etc.
        // For now, just update the frame count
        m_frameCount = 0;
    }
}

// Menu action implementations
void MainWindow::onNewProject() { qDebug() << "New Project"; }
void MainWindow::onOpenProject() { qDebug() << "Open Project"; }
void MainWindow::onSaveProject() { qDebug() << "Save Project"; }
void MainWindow::onImportModels() { qDebug() << "Import Models"; }
void MainWindow::onExportProject() { qDebug() << "Export Project"; }
void MainWindow::onExit() { close(); }

void MainWindow::onUndo() { qDebug() << "Undo"; }
void MainWindow::onRedo() { qDebug() << "Redo"; }
void MainWindow::onCut() { qDebug() << "Cut"; }
void MainWindow::onCopy() { qDebug() << "Copy"; }
void MainWindow::onPaste() { qDebug() << "Paste"; }
void MainWindow::onDelete() { qDebug() << "Delete"; }
void MainWindow::onSelectAll() { qDebug() << "Select All"; }

void MainWindow::onToggleFullScreen() { setWindowState(windowState() ^ Qt::WindowFullScreen); }
void MainWindow::onToggleMenuBar() { m_menuBar->setVisible(!m_menuBar->isVisible()); }
void MainWindow::onToggleToolBars() {
    m_mainToolBar->setVisible(!m_mainToolBar->isVisible());
    m_canvasToolBar->setVisible(!m_canvasToolBar->isVisible());
}
void MainWindow::onToggleStatusBar() { m_statusBar->setVisible(!m_statusBar->isVisible()); }
void MainWindow::onResetLayout() { resetLayout(); }

void MainWindow::onNewCanvas()
{
    QMenu canvasMenu(this);

    QAction* projectCanvasAction = new QAction("Project Canvas", this);
    connect(projectCanvasAction, &QAction::triggered, this, &MainWindow::onNewProjectCanvas);
    canvasMenu.addAction(projectCanvasAction);

    QAction* designCanvasAction = new QAction("Design Canvas", this);
    connect(designCanvasAction, &QAction::triggered, this, &MainWindow::onNewDesignCanvas);
    canvasMenu.addAction(designCanvasAction);

    QAction* cutListCanvasAction = new QAction("Cut List Canvas", this);
    connect(cutListCanvasAction, &QAction::triggered, this, &MainWindow::onNewCutListCanvas);
    canvasMenu.addAction(cutListCanvasAction);

    QAction* cncToolsCanvasAction = new QAction("CNC Tools Canvas", this);
    connect(cncToolsCanvasAction, &QAction::triggered, this, &MainWindow::onNewCNCToolsCanvas);
    canvasMenu.addAction(cncToolsCanvasAction);

    QAction* settingsCanvasAction = new QAction("Settings Canvas", this);
    connect(settingsCanvasAction, &QAction::triggered, this, &MainWindow::onNewSettingsCanvas);
    canvasMenu.addAction(settingsCanvasAction);

    // Show menu at cursor position
    canvasMenu.exec(QCursor::pos());
}
void MainWindow::onCloseCanvas() { qDebug() << "Close Canvas"; }
void MainWindow::onRenameCanvas() { qDebug() << "Rename Canvas"; }
void MainWindow::onDuplicateCanvas() { qDebug() << "Duplicate Canvas"; }

void MainWindow::onNewProjectCanvas()
{
    static int projectCanvasCount = 1;

    QString canvasName = QString("Project Canvas %1").arg(projectCanvasCount++);
    ProjectCanvas* projectCanvas = new ProjectCanvas(this);

    // Add to canvas registry
    m_canvases[canvasName] = projectCanvas;

    // Add to tab widget
    int tabIndex = m_canvasTabs->addTab(projectCanvas, canvasName);
    m_canvasTabs->setCurrentIndex(tabIndex);

    connect(projectCanvas, &BaseCanvas::errorOccurred, this, [this](const QString& error, const QString& details) {
        statusBar()->showMessage(QString("Canvas Error: %1").arg(error), 5000);
    });

    statusBar()->showMessage(QString("Created: %1").arg(canvasName));
}

void MainWindow::onNewDesignCanvas()
{
    static int designCanvasCount = 1;

    QString canvasName = QString("Design Canvas %1").arg(designCanvasCount++);
    DesignCanvas* designCanvas = new DesignCanvas(this);

    // Add to canvas registry
    m_canvases[canvasName] = designCanvas;

    // Add to tab widget
    int tabIndex = m_canvasTabs->addTab(designCanvas, canvasName);
    m_canvasTabs->setCurrentIndex(tabIndex);

    connect(designCanvas, &BaseCanvas::errorOccurred, this, [this](const QString& error, const QString& details) {
        statusBar()->showMessage(QString("Canvas Error: %1").arg(error), 5000);
    });

    statusBar()->showMessage(QString("Created: %1").arg(canvasName));
}

void MainWindow::onNewCutListCanvas()
{
    static int cutListCanvasCount = 1;

    QString canvasName = QString("Cut List Canvas %1").arg(cutListCanvasCount++);
    CutListCanvas* cutListCanvas = new CutListCanvas(this);

    // Add to canvas registry
    m_canvases[canvasName] = cutListCanvas;

    // Add to tab widget
    int tabIndex = m_canvasTabs->addTab(cutListCanvas, canvasName);
    m_canvasTabs->setCurrentIndex(tabIndex);

    connect(cutListCanvas, &BaseCanvas::errorOccurred, this, [this](const QString& error, const QString& details) {
        statusBar()->showMessage(QString("Canvas Error: %1").arg(error), 5000);
    });

    statusBar()->showMessage(QString("Created: %1").arg(canvasName));
}

void MainWindow::onNewCNCToolsCanvas()
{
    static int cncToolsCanvasCount = 1;

    QString canvasName = QString("CNC Tools Canvas %1").arg(cncToolsCanvasCount++);
    CNCToolsCanvas* cncToolsCanvas = new CNCToolsCanvas(this);

    // Add to canvas registry
    m_canvases[canvasName] = cncToolsCanvas;

    // Add to tab widget
    int tabIndex = m_canvasTabs->addTab(cncToolsCanvas, canvasName);
    m_canvasTabs->setCurrentIndex(tabIndex);

    connect(cncToolsCanvas, &BaseCanvas::errorOccurred, this, [this](const QString& error, const QString& details) {
        statusBar()->showMessage(QString("Canvas Error: %1").arg(error), 5000);
    });

    statusBar()->showMessage(QString("Created: %1").arg(canvasName));
}

void MainWindow::onNewSettingsCanvas()
{
    static int settingsCanvasCount = 1;

    QString canvasName = QString("Settings Canvas %1").arg(settingsCanvasCount++);
    SettingsCanvas* settingsCanvas = new SettingsCanvas(this);

    // Add to canvas registry
    m_canvases[canvasName] = settingsCanvas;

    // Add to tab widget
    int tabIndex = m_canvasTabs->addTab(settingsCanvas, canvasName);
    m_canvasTabs->setCurrentIndex(tabIndex);

    connect(settingsCanvas, &BaseCanvas::errorOccurred, this, [this](const QString& error, const QString& details) {
        statusBar()->showMessage(QString("Canvas Error: %1").arg(error), 5000);
    });

    statusBar()->showMessage(QString("Created: %1").arg(canvasName));
}

void MainWindow::onOptions() { qDebug() << "Options"; }
void MainWindow::onCheckForUpdates() { qDebug() << "Check for Updates"; }

void MainWindow::onNextCanvas() {
    int currentIndex = m_canvasTabs->currentIndex();
    int nextIndex = (currentIndex + 1) % m_canvasTabs->count();
    m_canvasTabs->setCurrentIndex(nextIndex);
}

void MainWindow::onPreviousCanvas() {
    int currentIndex = m_canvasTabs->currentIndex();
    int prevIndex = (currentIndex - 1 + m_canvasTabs->count()) % m_canvasTabs->count();
    m_canvasTabs->setCurrentIndex(prevIndex);
}

void MainWindow::onCanvasList() { qDebug() << "Canvas List"; }

void MainWindow::onUserGuide() { qDebug() << "User Guide"; }
void MainWindow::onAbout() {
    QMessageBox::about(this, "About 3D Model Management Utility",
        "<h3>3D Model Management Utility</h3>"
        "<p>Version 1.0.0</p>"
        "<p>A fluid, intuitive 3D model management system with CNC integration.</p>"
        "<p>Built with Qt6 and OpenGL for cross-platform performance.</p>");
}

void MainWindow::onSystemTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::DoubleClick:
            onShowWindow();
            break;
        default:
            break;
    }
}

void MainWindow::onShowWindow()
{
    show();
    showNormal(); // Restore from minimized state
    raise();
    activateWindow();
}

void MainWindow::onMinimizeToTray()
{
    hide();
    if (m_systemTray) {
        m_systemTray->showMessage("3D Model Manager",
            "Application minimized to tray",
            QSystemTrayIcon::Information, 2000);
    }
}

void MainWindow::onCanvasTabChanged(int index)
{
    if (index >= 0 && index < m_canvasTabs->count()) {
        QWidget* tabWidget = m_canvasTabs->widget(index);
        BaseCanvas* canvas = qobject_cast<BaseCanvas*>(tabWidget);

        if (canvas) {
            m_currentCanvasName = canvas->canvasName();

            // Update status bar
            if (m_statusBar) {
                QLabel* canvasLabel = m_statusBar->findChild<QLabel*>("canvasLabel");
                if (canvasLabel) {
                    canvasLabel->setText(m_currentCanvasName);
                }
            }

            // Notify canvas that it became active
            canvas->onShow();
        }
    }
}

void MainWindow::onCanvasCloseRequested(int index)
{
    if (index > 0) { // Don't allow closing the last canvas
        QWidget* tabWidget = m_canvasTabs->widget(index);
        QString canvasName = m_canvasTabs->tabText(index);

        QMessageBox::StandardButton reply = QMessageBox::question(this,
            "Close Canvas",
            QString("Close canvas '%1'?").arg(canvasName),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            removeCanvas(canvasName);
        }
    }
}