#pragma once

#include "../core/BaseTypes.h"
#include "../core/BaseCanvas.h"
#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QSettings>

/**
 * @brief Main application window with tabbed canvas system
 *
 * This is the central UI component that manages all canvases and provides
 * the main interface for the 3D Model Management Utility.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow() = default;

    // Canvas management
    void addCanvas(BaseCanvas* canvas);
    void removeCanvas(const QString& canvasName);
    BaseCanvas* getCurrentCanvas() const;
    BaseCanvas* getCanvas(const QString& canvasName) const;
    QStringList getCanvasNames() const;

    // Window management
    void saveWindowState();
    void restoreWindowState();
    void resetLayout();

    // Application lifecycle
    bool initialize();
    void shutdown();

    // Command line integration
    void processCommandLineArguments();

protected:
    // Event handlers
    virtual void closeEvent(QCloseEvent* event) override;
    virtual void changeEvent(QEvent* event) override;
    virtual bool eventFilter(QObject* obj, QEvent* event) override;

    // UI setup
    void setupMenuBar();
    void setupToolBars();
    void setupStatusBar();
    void setupSystemTray();
    void setupCanvasTabs();

    // Canvas switching
    void switchToCanvas(const QString& canvasName);
    void updateCanvasTab(BaseCanvas* canvas);

    // Settings and state
    void loadApplicationSettings();
    void saveApplicationSettings();

private slots:
    // File menu actions
    void onNewProject();
    void onOpenProject();
    void onSaveProject();
    void onImportModels();
    void onExportProject();
    void onExit();

    // Edit menu actions
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    void onSelectAll();

    // View menu actions
    void onToggleFullScreen();
    void onToggleMenuBar();
    void onToggleToolBars();
    void onToggleStatusBar();
    void onResetLayout();

    // Canvas menu actions
    void onNewCanvas();
    void onCloseCanvas();
    void onRenameCanvas();
    void onDuplicateCanvas();

    // Tools menu actions
    void onOptions();
    void onCheckForUpdates();

    // Window menu actions
    void onNextCanvas();
    void onPreviousCanvas();
    void onCanvasList();

    // Help menu actions
    void onUserGuide();
    void onAbout();

    // System tray actions
    void onSystemTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowWindow();
    void onMinimizeToTray();

    // Canvas events
    void onCanvasTabChanged(int index);
    void onCanvasCloseRequested(int index);

private:
    // UI components
    QTabWidget* m_canvasTabs;
    QMenuBar* m_menuBar;
    QToolBar* m_mainToolBar;
    QToolBar* m_canvasToolBar;
    QStatusBar* m_statusBar;
    QSystemTrayIcon* m_systemTray;

    // Canvas management
    QMap<QString, BaseCanvas*> m_canvases;
    QString m_currentCanvasName;

    // Application state
    QSettings* m_settings;
    QString m_settingsFile;
    bool m_isInitialized;
    bool m_isShuttingDown;

    // Performance monitoring
    qint64 m_lastUpdateTime;
    int m_frameCount;

    // UI state
    QString m_currentCanvasName;
};