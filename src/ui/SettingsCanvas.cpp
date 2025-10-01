#include "SettingsCanvas.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QGroupBox>
#include <QFormLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

SettingsCanvas::SettingsCanvas(QWidget* parent)
    : BaseCanvas(parent)
    , m_settingsTabs(nullptr)
    , m_generalWidget(nullptr)
    , m_visualizationWidget(nullptr)
    , m_performanceWidget(nullptr)
    , m_cncWidget(nullptr)
    , m_uiWidget(nullptr)
    , m_advancedWidget(nullptr)
{
    setCanvasName("SettingsCanvas");
    setupDefaultLayout();

    // Load current settings
    loadSettingsFromStorage();

    // Load default layout
    restoreLayout("settings_default");
}

void SettingsCanvas::setupDefaultLayout()
{
    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create tabbed settings interface
    m_settingsTabs = new QTabWidget(this);
    mainLayout->addWidget(m_settingsTabs);

    // Create settings categories
    createGeneralSettings();
    createVisualizationSettings();
    createPerformanceSettings();
    createCNCSettings();
    createUISettings();
    createAdvancedSettings();

    // Add tabs
    m_settingsTabs->addTab(m_generalWidget, "General");
    m_settingsTabs->addTab(m_visualizationWidget, "3D Visualization");
    m_settingsTabs->addTab(m_performanceWidget, "Performance");
    m_settingsTabs->addTab(m_cncWidget, "CNC");
    m_settingsTabs->addTab(m_uiWidget, "User Interface");
    m_settingsTabs->addTab(m_advancedWidget, "Advanced");

    // Create action buttons
    QWidget* buttonWidget = new QWidget(this);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);

    m_applyButton = new QPushButton("Apply", buttonWidget);
    connect(m_applyButton, &QPushButton::clicked, this, &SettingsCanvas::onApplySettingsClicked);
    buttonLayout->addWidget(m_applyButton);

    m_resetButton = new QPushButton("Reset to Defaults", buttonWidget);
    connect(m_resetButton, &QPushButton::clicked, this, &SettingsCanvas::onResetDefaultsClicked);
    buttonLayout->addWidget(m_resetButton);

    buttonLayout->addStretch();

    m_exportButton = new QPushButton("Export Settings", buttonWidget);
    connect(m_exportButton, &QPushButton::clicked, this, &SettingsCanvas::onExportSettingsClicked);
    buttonLayout->addWidget(m_exportButton);

    m_importButton = new QPushButton("Import Settings", buttonWidget);
    connect(m_importButton, &QPushButton::clicked, this, &SettingsCanvas::onImportSettingsClicked);
    buttonLayout->addWidget(m_importButton);

    mainLayout->addWidget(buttonWidget);

    setLayout(mainLayout);
}

void SettingsCanvas::createGeneralSettings()
{
    m_generalWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_generalWidget);

    addSectionHeader(layout, "Application Settings",
        "Basic application configuration and preferences");

    QGroupBox* appGroup = new QGroupBox("Application", m_generalWidget);
    QFormLayout* appLayout = new QFormLayout(appGroup);

    // Theme selection
    m_themeCombo = new QComboBox(appGroup);
    m_themeCombo->addItem("Light", "light");
    m_themeCombo->addItem("Dark", "dark");
    m_themeCombo->addItem("System", "system");
    addSettingRow(appLayout, "Theme:", m_themeCombo, "Choose the application color theme");
    connect(m_themeCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &SettingsCanvas::onThemeChanged);

    // Language selection
    m_languageCombo = new QComboBox(appGroup);
    m_languageCombo->addItem("English", "en");
    m_languageCombo->addItem("Spanish", "es");
    m_languageCombo->addItem("French", "fr");
    m_languageCombo->addItem("German", "de");
    addSettingRow(appLayout, "Language:", m_languageCombo, "Select interface language");
    connect(m_languageCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &SettingsCanvas::onLanguageChanged);

    layout->addWidget(appGroup);

    // Auto-save settings
    QGroupBox* saveGroup = new QGroupBox("Auto-Save", m_generalWidget);
    QFormLayout* saveLayout = new QFormLayout(saveGroup);

    m_autoSaveCheck = new QCheckBox("Enable auto-save", saveGroup);
    addSettingRow(saveLayout, "", m_autoSaveCheck, "Automatically save projects");
    connect(m_autoSaveCheck, &QCheckBox::toggled, this, &SettingsCanvas::onAutoSaveChanged);

    m_autoSaveIntervalSpin = new QSpinBox(saveGroup);
    m_autoSaveIntervalSpin->setRange(1, 60);
    m_autoSaveIntervalSpin->setValue(5);
    m_autoSaveIntervalSpin->setSuffix(" minutes");
    addSettingRow(saveLayout, "Interval:", m_autoSaveIntervalSpin, "Auto-save interval");

    layout->addWidget(saveGroup);

    // Backup settings
    QGroupBox* backupGroup = new QGroupBox("Backup", m_generalWidget);
    QFormLayout* backupLayout = new QFormLayout(backupGroup);

    m_backupCheck = new QCheckBox("Enable automatic backups", backupGroup);
    addSettingRow(backupLayout, "", m_backupCheck, "Create backup files");
    connect(m_backupCheck, &QCheckBox::toggled, this, &SettingsCanvas::onBackupSettingsChanged);

    layout->addWidget(backupGroup);

    layout->addStretch();
}

void SettingsCanvas::createVisualizationSettings()
{
    m_visualizationWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_visualizationWidget);

    addSectionHeader(layout, "3D Visualization Settings",
        "Configure 3D rendering and visualization options");

    // Background settings
    QGroupBox* backgroundGroup = new QGroupBox("Background", m_visualizationWidget);
    QFormLayout* backgroundLayout = new QFormLayout(backgroundGroup);

    m_backgroundColorButton = new QPushButton("Choose Color", backgroundGroup);
    addSettingRow(backgroundLayout, "Background Color:", m_backgroundColorButton, "Set 3D view background color");
    connect(m_backgroundColorButton, &QPushButton::clicked, this, &SettingsCanvas::onBackgroundColorChanged);

    layout->addWidget(backgroundGroup);

    // Grid and axes settings
    QGroupBox* displayGroup = new QGroupBox("Display Options", m_visualizationWidget);
    QFormLayout* displayLayout = new QFormLayout(displayGroup);

    m_showGridCheck = new QCheckBox("Show grid", displayGroup);
    addSettingRow(displayLayout, "", m_showGridCheck, "Display ground grid");
    connect(m_showGridCheck, &QCheckBox::toggled, this, &SettingsCanvas::onGridSettingsChanged);

    m_showAxesCheck = new QCheckBox("Show axes", displayGroup);
    addSettingRow(displayLayout, "", m_showAxesCheck, "Display coordinate axes");
    connect(m_showAxesCheck, &QCheckBox::toggled, this, &SettingsCanvas::onGridSettingsChanged);

    layout->addWidget(displayGroup);

    // LOD settings
    QGroupBox* lodGroup = new QGroupBox("Level of Detail", m_visualizationWidget);
    QFormLayout* lodLayout = new QFormLayout(lodGroup);

    m_lodDistanceSpin = new QSpinBox(lodGroup);
    m_lodDistanceSpin->setRange(10, 1000);
    m_lodDistanceSpin->setValue(100);
    m_lodDistanceSpin->setSuffix(" units");
    addSettingRow(lodLayout, "LOD Distance:", m_lodDistanceSpin, "Distance for LOD switching");
    connect(m_lodDistanceSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsCanvas::onLODSettingsChanged);

    layout->addWidget(lodGroup);

    layout->addStretch();
}

void SettingsCanvas::createPerformanceSettings()
{
    m_performanceWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_performanceWidget);

    addSectionHeader(layout, "Performance Settings",
        "Configure performance and memory usage options");

    // Memory settings
    QGroupBox* memoryGroup = new QGroupBox("Memory", m_performanceWidget);
    QFormLayout* memoryLayout = new QFormLayout(memoryGroup);

    m_memoryLimitSpin = new QSpinBox(memoryGroup);
    m_memoryLimitSpin->setRange(512, 8192);
    m_memoryLimitSpin->setValue(2048);
    m_memoryLimitSpin->setSuffix(" MB");
    addSettingRow(memoryLayout, "Memory Limit:", m_memoryLimitSpin, "Maximum memory usage");
    connect(m_memoryLimitSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsCanvas::onMemoryLimitChanged);

    layout->addWidget(memoryGroup);

    // Cache settings
    QGroupBox* cacheGroup = new QGroupBox("Cache", m_performanceWidget);
    QFormLayout* cacheLayout = new QFormLayout(cacheGroup);

    m_cacheSizeSpin = new QSpinBox(cacheGroup);
    m_cacheSizeSpin->setRange(100, 5000);
    m_cacheSizeSpin->setValue(1000);
    m_cacheSizeSpin->setSuffix(" MB");
    addSettingRow(cacheLayout, "Cache Size:", m_cacheSizeSpin, "Thumbnail and model cache size");
    connect(m_cacheSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsCanvas::onCacheSizeChanged);

    layout->addWidget(cacheGroup);

    // Processing settings
    QGroupBox* processingGroup = new QGroupBox("Processing", m_performanceWidget);
    QFormLayout* processingLayout = new QFormLayout(processingGroup);

    m_parallelProcessingCheck = new QCheckBox("Enable parallel processing", processingGroup);
    addSettingRow(processingLayout, "", m_parallelProcessingCheck, "Use multiple CPU cores");
    connect(m_parallelProcessingCheck, &QCheckBox::toggled, this, &SettingsCanvas::onParallelProcessingChanged);

    m_progressiveLoadingCheck = new QCheckBox("Enable progressive loading", processingGroup);
    m_progressiveLoadingCheck->setChecked(true);
    addSettingRow(processingLayout, "", m_progressiveLoadingCheck, "Load large models progressively");

    layout->addWidget(processingGroup);

    // File size limits
    QGroupBox* fileGroup = new QGroupBox("File Limits", m_performanceWidget);
    QFormLayout* fileLayout = new QFormLayout(fileGroup);

    m_maxFileSizeSpin = new QSpinBox(fileGroup);
    m_maxFileSizeSpin->setRange(100, 2000);
    m_maxFileSizeSpin->setValue(500);
    m_maxFileSizeSpin->setSuffix(" MB");
    addSettingRow(fileLayout, "Max File Size:", m_maxFileSizeSpin, "Maximum supported file size");
    connect(m_maxFileSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsCanvas::onMemoryLimitChanged);

    layout->addWidget(fileGroup);

    layout->addStretch();
}

void SettingsCanvas::createCNCSettings()
{
    m_cncWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_cncWidget);

    addSectionHeader(layout, "CNC Settings",
        "Configure CNC-specific options and preferences");

    // Units and measurements
    QGroupBox* unitsGroup = new QGroupBox("Units", m_cncWidget);
    QFormLayout* unitsLayout = new QFormLayout(unitsGroup);

    m_defaultUnitsCombo = new QComboBox(unitsGroup);
    m_defaultUnitsCombo->addItem("Inches", "inches");
    m_defaultUnitsCombo->addItem("Millimeters", "mm");
    addSettingRow(unitsLayout, "Default Units:", m_defaultUnitsCombo, "Default measurement units");
    connect(m_defaultUnitsCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &SettingsCanvas::onDefaultUnitsChanged);

    layout->addWidget(unitsGroup);

    // Safety settings
    QGroupBox* safetyGroup = new QGroupBox("Safety", m_cncWidget);
    QFormLayout* safetyLayout = new QFormLayout(safetyGroup);

    m_safetyMarginSpin = new QDoubleSpinBox(safetyGroup);
    m_safetyMarginSpin->setRange(0.1, 0.5);
    m_safetyMarginSpin->setValue(0.2);
    m_safetyMarginSpin->setSingleStep(0.05);
    m_safetyMarginSpin->setSuffix(" (20%)");
    addSettingRow(safetyLayout, "Safety Margin:", m_safetyMarginSpin, "Safety margin for feed/speed calculations");
    connect(m_safetyMarginSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &SettingsCanvas::onSafetyMarginsChanged);

    layout->addWidget(safetyGroup);

    // Tool library settings
    QGroupBox* toolGroup = new QGroupBox("Tool Library", m_cncWidget);
    QFormLayout* toolLayout = new QFormLayout(toolGroup);

    m_toolLibraryPathEdit = new QLineEdit(toolGroup);
    m_toolLibraryPathEdit->setText(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/tools");
    addSettingRow(toolLayout, "Library Path:", m_toolLibraryPathEdit, "Default tool library location");

    m_toolLibraryBrowseButton = new QPushButton("Browse", toolGroup);
    connect(m_toolLibraryBrowseButton, &QPushButton::clicked, this, &SettingsCanvas::onToolLibraryPathChanged);
    toolLayout->addRow("", m_toolLibraryBrowseButton);

    layout->addWidget(toolGroup);

    // Calculation settings
    QGroupBox* calcGroup = new QGroupBox("Calculations", m_cncWidget);
    QFormLayout* calcLayout = new QFormLayout(calcGroup);

    m_autoCalculateCheck = new QCheckBox("Auto-calculate feed/speed", calcGroup);
    m_autoCalculateCheck->setChecked(true);
    addSettingRow(calcLayout, "", m_autoCalculateCheck, "Automatically calculate optimal parameters");
    connect(m_autoCalculateCheck, &QCheckBox::toggled, this, &SettingsCanvas::onGCodeSettingsChanged);

    layout->addWidget(calcGroup);

    layout->addStretch();
}

void SettingsCanvas::createUISettings()
{
    m_uiWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_uiWidget);

    addSectionHeader(layout, "User Interface Settings",
        "Customize the appearance and behavior of the interface");

    // Font settings
    QGroupBox* fontGroup = new QGroupBox("Fonts", m_uiWidget);
    QFormLayout* fontLayout = new QFormLayout(fontGroup);

    m_fontButton = new QPushButton("Choose Application Font", fontGroup);
    addSettingRow(fontLayout, "Font:", m_fontButton, "Select application font");
    connect(m_fontButton, &QPushButton::clicked, this, &SettingsCanvas::onFontSettingsChanged);

    layout->addWidget(fontGroup);

    // Color scheme
    QGroupBox* colorGroup = new QGroupBox("Colors", m_uiWidget);
    QFormLayout* colorLayout = new QFormLayout(colorGroup);

    m_colorSchemeCombo = new QComboBox(colorGroup);
    m_colorSchemeCombo->addItem("Default", "default");
    m_colorSchemeCombo->addItem("Blue", "blue");
    m_colorSchemeCombo->addItem("Green", "green");
    m_colorSchemeCombo->addItem("Orange", "orange");
    addSettingRow(colorLayout, "Color Scheme:", m_colorSchemeCombo, "Select color scheme");
    connect(m_colorSchemeCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &SettingsCanvas::onColorSchemeChanged);

    layout->addWidget(colorGroup);

    // Interface options
    QGroupBox* interfaceGroup = new QGroupBox("Interface", m_uiWidget);
    QFormLayout* interfaceLayout = new QFormLayout(interfaceGroup);

    m_showTooltipsCheck = new QCheckBox("Show tooltips", interfaceGroup);
    m_showTooltipsCheck->setChecked(true);
    addSettingRow(interfaceLayout, "", m_showTooltipsCheck, "Display helpful tooltips");
    connect(m_showTooltipsCheck, &QCheckBox::toggled, this, &SettingsCanvas::onLayoutSettingsChanged);

    m_compactModeCheck = new QCheckBox("Compact mode", interfaceGroup);
    addSettingRow(interfaceLayout, "", m_compactModeCheck, "Use compact interface layout");
    connect(m_compactModeCheck, &QCheckBox::toggled, this, &SettingsCanvas::onLayoutSettingsChanged);

    layout->addWidget(interfaceGroup);

    layout->addStretch();
}

void SettingsCanvas::createAdvancedSettings()
{
    m_advancedWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_advancedWidget);

    addSectionHeader(layout, "Advanced Settings",
        "Advanced configuration options for power users");

    // Debug settings
    QGroupBox* debugGroup = new QGroupBox("Debug", m_advancedWidget);
    QFormLayout* debugLayout = new QFormLayout(debugGroup);

    m_debugModeCheck = new QCheckBox("Enable debug mode", debugGroup);
    addSettingRow(debugLayout, "", m_debugModeCheck, "Enable debug logging and diagnostics");
    connect(m_debugModeCheck, &QCheckBox::toggled, this, &SettingsCanvas::onDebugModeChanged);

    m_loggingLevelCombo = new QComboBox(debugGroup);
    m_loggingLevelCombo->addItem("Error", "error");
    m_loggingLevelCombo->addItem("Warning", "warning");
    m_loggingLevelCombo->addItem("Info", "info");
    m_loggingLevelCombo->addItem("Debug", "debug");
    addSettingRow(debugLayout, "Logging Level:", m_loggingLevelCombo, "Set logging verbosity");
    connect(m_loggingLevelCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &SettingsCanvas::onLoggingLevelChanged);

    layout->addWidget(debugGroup);

    // Experimental features
    QGroupBox* experimentalGroup = new QGroupBox("Experimental", m_advancedWidget);
    QFormLayout* experimentalLayout = new QFormLayout(experimentalGroup);

    m_experimentalFeaturesCheck = new QCheckBox("Enable experimental features", experimentalGroup);
    addSettingRow(experimentalLayout, "", m_experimentalFeaturesCheck, "Enable experimental/unstable features");
    connect(m_experimentalFeaturesCheck, &QCheckBox::toggled, this, &SettingsCanvas::onExperimentalFeaturesChanged);

    layout->addWidget(experimentalGroup);

    // Custom paths
    QGroupBox* pathsGroup = new QGroupBox("Custom Paths", m_advancedWidget);
    QFormLayout* pathsLayout = new QFormLayout(pathsGroup);

    m_customPathEdit = new QLineEdit(pathsGroup);
    m_customPathEdit->setPlaceholderText("Custom configuration path");
    addSettingRow(pathsLayout, "Config Path:", m_customPathEdit, "Custom configuration file location");

    layout->addWidget(pathsGroup);

    layout->addStretch();
}

void SettingsCanvas::addSettingRow(QFormLayout* layout, const QString& label, QWidget* widget, const QString& tooltip)
{
    if (!label.isEmpty()) {
        layout->addRow(label, widget);
    } else {
        layout->addRow(widget);
    }

    if (!tooltip.isEmpty()) {
        widget->setToolTip(tooltip);
    }
}

void SettingsCanvas::addSectionHeader(QVBoxLayout* layout, const QString& title, const QString& description)
{
    QLabel* titleLabel = new QLabel(title);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(titleFont.pointSize() + 2);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    if (!description.isEmpty()) {
        QLabel* descLabel = new QLabel(description);
        descLabel->setStyleSheet("color: #666; font-style: italic;");
        layout->addWidget(descLabel);
    }

    layout->addSpacing(10);
}

// BaseCanvas interface implementation
void SettingsCanvas::addWidget(BaseWidget* widget, DockArea area)
{
    // Settings canvas doesn't support additional widgets
}

void SettingsCanvas::removeWidget(BaseWidget* widget)
{
    // Settings canvas doesn't support widget removal
}

void SettingsCanvas::removeWidget(const QString& widgetName)
{
    // Settings canvas doesn't support widget removal by name
}

QList<BaseWidget*> SettingsCanvas::getWidgets() const
{
    return QList<BaseWidget*>();
}

QList<BaseWidget*> SettingsCanvas::getWidgetsByCategory(WidgetCategory category) const
{
    return QList<BaseWidget*>();
}

BaseWidget* SettingsCanvas::getWidget(const QString& widgetName) const
{
    return nullptr;
}

void SettingsCanvas::saveLayout(const QString& name)
{
    // Layout saving implementation
}

void SettingsCanvas::restoreLayout(const QString& name)
{
    // Layout restoration implementation
}

QStringList SettingsCanvas::getAvailableLayouts() const
{
    return QStringList() << "settings_default" << "settings_compact" << "settings_detailed";
}

void SettingsCanvas::resetLayout()
{
    restoreLayout("settings_default");
}

void SettingsCanvas::saveState(QJsonObject& state) const
{
    // Save current settings to state
    state["current_tab"] = m_settingsTabs->currentIndex();
}

void SettingsCanvas::restoreState(const QJsonObject& state)
{
    int currentTab = state["current_tab"].toInt();
    if (currentTab >= 0 && currentTab < m_settingsTabs->count()) {
        m_settingsTabs->setCurrentIndex(currentTab);
    }
}

// SettingsCanvas specific methods
void SettingsCanvas::applySettings()
{
    // Apply all current settings
    validateSettings();

    // Save to storage
    saveSettingsToStorage();

    // Notify other components to reload settings
    emit settingsChanged();

    statusBar()->showMessage("Settings applied successfully", 3000);
}

void SettingsCanvas::resetToDefaults()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Reset Settings",
        "Reset all settings to defaults? This cannot be undone.",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Reset all controls to default values
        m_themeCombo->setCurrentIndex(0);
        m_languageCombo->setCurrentIndex(0);
        m_autoSaveCheck->setChecked(false);
        m_backupCheck->setChecked(false);
        m_autoSaveIntervalSpin->setValue(5);

        m_backgroundColorButton->setText("Choose Color");
        m_showGridCheck->setChecked(true);
        m_showAxesCheck->setChecked(true);
        m_lodDistanceSpin->setValue(100);

        m_memoryLimitSpin->setValue(2048);
        m_cacheSizeSpin->setValue(1000);
        m_parallelProcessingCheck->setChecked(true);
        m_progressiveLoadingCheck->setChecked(true);
        m_maxFileSizeSpin->setValue(500);

        m_defaultUnitsCombo->setCurrentIndex(0);
        m_safetyMarginSpin->setValue(0.2);
        m_toolLibraryPathEdit->setText(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/tools");
        m_autoCalculateCheck->setChecked(true);

        m_colorSchemeCombo->setCurrentIndex(0);
        m_showTooltipsCheck->setChecked(true);
        m_compactModeCheck->setChecked(false);

        m_debugModeCheck->setChecked(false);
        m_loggingLevelCombo->setCurrentIndex(0);
        m_experimentalFeaturesCheck->setChecked(false);

        // Apply defaults
        applySettings();
    }
}

void SettingsCanvas::exportSettings(const QString& filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Export Failed", "Cannot write to file: " + filepath);
        return;
    }

    // Collect current settings
    QJsonObject settings;

    // General settings
    QJsonObject general;
    general["theme"] = m_themeCombo->currentData().toString();
    general["language"] = m_languageCombo->currentData().toString();
    general["auto_save"] = m_autoSaveCheck->isChecked();
    general["auto_save_interval"] = m_autoSaveIntervalSpin->value();
    general["backup_enabled"] = m_backupCheck->isChecked();
    settings["general"] = general;

    // Visualization settings
    QJsonObject visualization;
    visualization["show_grid"] = m_showGridCheck->isChecked();
    visualization["show_axes"] = m_showAxesCheck->isChecked();
    visualization["lod_distance"] = m_lodDistanceSpin->value();
    settings["visualization"] = visualization;

    // Performance settings
    QJsonObject performance;
    performance["memory_limit"] = m_memoryLimitSpin->value();
    performance["cache_size"] = m_cacheSizeSpin->value();
    performance["parallel_processing"] = m_parallelProcessingCheck->isChecked();
    performance["progressive_loading"] = m_progressiveLoadingCheck->isChecked();
    performance["max_file_size"] = m_maxFileSizeSpin->value();
    settings["performance"] = performance;

    // CNC settings
    QJsonObject cnc;
    cnc["default_units"] = m_defaultUnitsCombo->currentData().toString();
    cnc["safety_margin"] = m_safetyMarginSpin->value();
    cnc["tool_library_path"] = m_toolLibraryPathEdit->text();
    cnc["auto_calculate"] = m_autoCalculateCheck->isChecked();
    settings["cnc"] = cnc;

    // UI settings
    QJsonObject ui;
    ui["color_scheme"] = m_colorSchemeCombo->currentData().toString();
    ui["show_tooltips"] = m_showTooltipsCheck->isChecked();
    ui["compact_mode"] = m_compactModeCheck->isChecked();
    settings["ui"] = ui;

    // Advanced settings
    QJsonObject advanced;
    advanced["debug_mode"] = m_debugModeCheck->isChecked();
    advanced["logging_level"] = m_loggingLevelCombo->currentData().toString();
    advanced["experimental_features"] = m_experimentalFeaturesCheck->isChecked();
    settings["advanced"] = advanced;

    QJsonDocument doc(settings);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    statusBar()->showMessage(QString("Settings exported to: %1").arg(filepath));
}

void SettingsCanvas::importSettings(const QString& filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Import Failed", "Cannot read file: " + filepath);
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        QMessageBox::critical(this, "Import Failed", "Invalid settings file: " + filepath);
        return;
    }

    QJsonObject settings = doc.object();

    // Import general settings
    if (settings.contains("general")) {
        QJsonObject general = settings["general"].toObject();

        setComboBoxByData(m_themeCombo, general["theme"].toString());
        setComboBoxByData(m_languageCombo, general["language"].toString());
        m_autoSaveCheck->setChecked(general["auto_save"].toBool());
        m_autoSaveIntervalSpin->setValue(general["auto_save_interval"].toInt());
        m_backupCheck->setChecked(general["backup_enabled"].toBool());
    }

    // Import visualization settings
    if (settings.contains("visualization")) {
        QJsonObject visualization = settings["visualization"].toObject();

        m_showGridCheck->setChecked(visualization["show_grid"].toBool());
        m_showAxesCheck->setChecked(visualization["show_axes"].toBool());
        m_lodDistanceSpin->setValue(visualization["lod_distance"].toInt());
    }

    // Import performance settings
    if (settings.contains("performance")) {
        QJsonObject performance = settings["performance"].toObject();

        m_memoryLimitSpin->setValue(performance["memory_limit"].toInt());
        m_cacheSizeSpin->setValue(performance["cache_size"].toInt());
        m_parallelProcessingCheck->setChecked(performance["parallel_processing"].toBool());
        m_progressiveLoadingCheck->setChecked(performance["progressive_loading"].toBool());
        m_maxFileSizeSpin->setValue(performance["max_file_size"].toInt());
    }

    // Import CNC settings
    if (settings.contains("cnc")) {
        QJsonObject cnc = settings["cnc"].toObject();

        setComboBoxByData(m_defaultUnitsCombo, cnc["default_units"].toString());
        m_safetyMarginSpin->setValue(cnc["safety_margin"].toDouble());
        m_toolLibraryPathEdit->setText(cnc["tool_library_path"].toString());
        m_autoCalculateCheck->setChecked(cnc["auto_calculate"].toBool());
    }

    // Import UI settings
    if (settings.contains("ui")) {
        QJsonObject ui = settings["ui"].toObject();

        setComboBoxByData(m_colorSchemeCombo, ui["color_scheme"].toString());
        m_showTooltipsCheck->setChecked(ui["show_tooltips"].toBool());
        m_compactModeCheck->setChecked(ui["compact_mode"].toBool());
    }

    // Import advanced settings
    if (settings.contains("advanced")) {
        QJsonObject advanced = settings["advanced"].toObject();

        m_debugModeCheck->setChecked(advanced["debug_mode"].toBool());
        setComboBoxByData(m_loggingLevelCombo, advanced["logging_level"].toString());
        m_experimentalFeaturesCheck->setChecked(advanced["experimental_features"].toBool());
    }

    file.close();

    // Apply imported settings
    applySettings();

    statusBar()->showMessage(QString("Settings imported from: %1").arg(filepath));
}

QVariant SettingsCanvas::getSetting(const QString& category, const QString& key) const
{
    if (m_settings.contains(category)) {
        QJsonObject catSettings = m_settings[category].toObject();
        return catSettings[key].toVariant();
    }
    return QVariant();
}

void SettingsCanvas::setSetting(const QString& category, const QString& key, const QVariant& value)
{
    if (!m_settings.contains(category)) {
        m_settings[category] = QJsonObject();
    }

    QJsonObject catSettings = m_settings[category].toObject();
    catSettings[key] = QJsonValue::fromVariant(value);
    m_settings[category] = catSettings;
}

void SettingsCanvas::loadSettingsFromStorage()
{
    // Load settings from storage (simplified)
    // In a full implementation, this would load from the database or file

    // Set default values
    m_themeCombo->setCurrentIndex(0);
    m_languageCombo->setCurrentIndex(0);
    m_autoSaveCheck->setChecked(false);
    m_backupCheck->setChecked(false);
    m_memoryLimitSpin->setValue(2048);
    m_cacheSizeSpin->setValue(1000);
    m_parallelProcessingCheck->setChecked(true);
    m_progressiveLoadingCheck->setChecked(true);
    m_maxFileSizeSpin->setValue(500);
    m_defaultUnitsCombo->setCurrentIndex(0);
    m_safetyMarginSpin->setValue(0.2);
    m_showTooltipsCheck->setChecked(true);
    m_compactModeCheck->setChecked(false);
    m_debugModeCheck->setChecked(false);
    m_loggingLevelCombo->setCurrentIndex(0);
    m_experimentalFeaturesCheck->setChecked(false);
}

void SettingsCanvas::saveSettingsToStorage()
{
    // Save settings to storage (simplified)
    // In a full implementation, this would save to the database or file
    qDebug() << "Settings saved to storage";
}

void SettingsCanvas::validateSettings()
{
    // Validate current settings
    if (m_memoryLimitSpin->value() < 512) {
        QMessageBox::warning(this, "Invalid Setting",
            "Memory limit must be at least 512 MB");
        m_memoryLimitSpin->setValue(512);
    }

    if (m_cacheSizeSpin->value() < 100) {
        QMessageBox::warning(this, "Invalid Setting",
            "Cache size must be at least 100 MB");
        m_cacheSizeSpin->setValue(100);
    }
}

void SettingsCanvas::setComboBoxByData(QComboBox* combo, const QString& data)
{
    for (int i = 0; i < combo->count(); ++i) {
        if (combo->itemData(i).toString() == data) {
            combo->setCurrentIndex(i);
            break;
        }
    }
}

// Event handlers
void SettingsCanvas::onThemeChanged(const QString& theme)
{
    // Apply theme change immediately
    setSetting("general", "theme", theme);
}

void SettingsCanvas::onLanguageChanged(const QString& language)
{
    setSetting("general", "language", language);
}

void SettingsCanvas::onAutoSaveChanged(bool enabled)
{
    setSetting("general", "auto_save", enabled);
    m_autoSaveIntervalSpin->setEnabled(enabled);
}

void SettingsCanvas::onBackupSettingsChanged(bool enabled)
{
    setSetting("general", "backup_enabled", enabled);
}

void SettingsCanvas::onBackgroundColorChanged()
{
    QColor color = QColorDialog::getColor(Qt::darkGray, this, "Choose Background Color");
    if (color.isValid()) {
        setSetting("visualization", "background_color", color.name());
        m_backgroundColorButton->setText(color.name());
    }
}

void SettingsCanvas::onGridSettingsChanged()
{
    setSetting("visualization", "show_grid", m_showGridCheck->isChecked());
    setSetting("visualization", "show_axes", m_showAxesCheck->isChecked());
}

void SettingsCanvas::onLightingSettingsChanged()
{
    // Handle lighting settings changes
}

void SettingsCanvas::onLODSettingsChanged()
{
    setSetting("visualization", "lod_distance", m_lodDistanceSpin->value());
}

void SettingsCanvas::onMemoryLimitChanged(int limit)
{
    setSetting("performance", "memory_limit", limit);
}

void SettingsCanvas::onCacheSizeChanged(int size)
{
    setSetting("performance", "cache_size", size);
}

void SettingsCanvas::onParallelProcessingChanged(bool enabled)
{
    setSetting("performance", "parallel_processing", enabled);
}

void SettingsCanvas::onHardwareAccelerationChanged(bool enabled)
{
    setSetting("performance", "hardware_acceleration", enabled);
}

void SettingsCanvas::onDefaultUnitsChanged(const QString& units)
{
    setSetting("cnc", "default_units", units);
}

void SettingsCanvas::onSafetyMarginsChanged(double margin)
{
    setSetting("cnc", "safety_margin", margin);
}

void SettingsCanvas::onToolLibraryPathChanged()
{
    QString path = QFileDialog::getExistingDirectory(this, "Select Tool Library Path",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

    if (!path.isEmpty()) {
        m_toolLibraryPathEdit->setText(path);
        setSetting("cnc", "tool_library_path", path);
    }
}

void SettingsCanvas::onGCodeSettingsChanged()
{
    setSetting("cnc", "auto_calculate", m_autoCalculateCheck->isChecked());
}

void SettingsCanvas::onFontSettingsChanged()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QApplication::font(), this, "Choose Application Font");

    if (ok) {
        setSetting("ui", "font_family", font.family());
        setSetting("ui", "font_size", font.pointSize());
    }
}

void SettingsCanvas::onColorSchemeChanged()
{
    setSetting("ui", "color_scheme", m_colorSchemeCombo->currentData().toString());
}

void SettingsCanvas::onLayoutSettingsChanged()
{
    setSetting("ui", "show_tooltips", m_showTooltipsCheck->isChecked());
    setSetting("ui", "compact_mode", m_compactModeCheck->isChecked());
}

void SettingsCanvas::onDebugModeChanged(bool enabled)
{
    setSetting("advanced", "debug_mode", enabled);
}

void SettingsCanvas::onLoggingLevelChanged(const QString& level)
{
    setSetting("advanced", "logging_level", level);
}

void SettingsCanvas::onExperimentalFeaturesChanged(bool enabled)
{
    setSetting("advanced", "experimental_features", enabled);
}

void SettingsCanvas::onApplySettingsClicked()
{
    applySettings();
}

void SettingsCanvas::onResetDefaultsClicked()
{
    resetToDefaults();
}

void SettingsCanvas::onExportSettingsClicked()
{
    QString filepath = QFileDialog::getSaveFileName(this,
        "Export Settings",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Settings files (*.json)");

    if (!filepath.isEmpty()) {
        exportSettings(filepath);
    }
}

void SettingsCanvas::onImportSettingsClicked()
{
    QString filepath = QFileDialog::getOpenFileName(this,
        "Import Settings",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Settings files (*.json)");

    if (!filepath.isEmpty()) {
        importSettings(filepath);
    }
}