#pragma once

#include "../core/BaseCanvas.h"
#include "../core/BaseTypes.h"
#include <QWidget>
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
#include <QJsonObject>

/**
 * @brief Settings Canvas - Comprehensive application configuration
 *
 * Centralized settings management for all application preferences including:
 * - General application settings
 * - 3D visualization preferences
 * - Mesh repair and optimization settings
 * - Performance and memory configuration
 * - CNC-specific settings
 * - UI customization options
 */
class SettingsCanvas : public BaseCanvas
{
    Q_OBJECT

public:
    explicit SettingsCanvas(QWidget* parent = nullptr);
    virtual ~SettingsCanvas() = default;

    // BaseCanvas interface implementation
    QString canvasName() const override { return "SettingsCanvas"; }
    QString canvasDescription() const override { return "Application settings and configuration"; }
    QString canvasType() const override { return "settings"; }
    QString canvasVersion() const override { return "1.0.0"; }

    void addWidget(BaseWidget* widget, DockArea area = DockArea::Center) override;
    void removeWidget(BaseWidget* widget) override;
    void removeWidget(const QString& widgetName) override;
    QList<BaseWidget*> getWidgets() const override;
    QList<BaseWidget*> getWidgetsByCategory(WidgetCategory category) const override;
    BaseWidget* getWidget(const QString& widgetName) const override;

    void saveLayout(const QString& name) override;
    void restoreLayout(const QString& name) override;
    QStringList getAvailableLayouts() const override;
    void resetLayout() override;

    void saveState(QJsonObject& state) const override;
    void restoreState(const QJsonObject& state) override;

    // SettingsCanvas specific methods
    void applySettings();
    void resetToDefaults();
    void exportSettings(const QString& filepath);
    void importSettings(const QString& filepath);

    // Settings access
    QVariant getSetting(const QString& category, const QString& key) const;
    void setSetting(const QString& category, const QString& key, const QVariant& value);

protected:
    void setupDefaultLayout();
    void createGeneralSettings();
    void createVisualizationSettings();
    void createPerformanceSettings();
    void createCNCSettings();
    void createUISettings();
    void createAdvancedSettings();

    // Settings management
    void loadSettingsFromStorage();
    void saveSettingsToStorage();
    void validateSettings();

    // UI helpers
    void addSettingRow(QFormLayout* layout, const QString& label, QWidget* widget, const QString& tooltip = QString());
    void addSectionHeader(QVBoxLayout* layout, const QString& title, const QString& description = QString());

private slots:
    // General settings
    void onThemeChanged(const QString& theme);
    void onLanguageChanged(const QString& language);
    void onAutoSaveChanged(bool enabled);
    void onBackupSettingsChanged(bool enabled);

    // Visualization settings
    void onBackgroundColorChanged();
    void onGridSettingsChanged();
    void onLightingSettingsChanged();
    void onLODSettingsChanged();

    // Performance settings
    void onMemoryLimitChanged(int limit);
    void onCacheSizeChanged(int size);
    void onParallelProcessingChanged(bool enabled);
    void onHardwareAccelerationChanged(bool enabled);

    // CNC settings
    void onDefaultUnitsChanged(const QString& units);
    void onSafetyMarginsChanged(double margin);
    void onToolLibraryPathChanged();
    void onGCodeSettingsChanged();

    // UI settings
    void onFontSettingsChanged();
    void onColorSchemeChanged();
    void onLayoutSettingsChanged();

    // Advanced settings
    void onDebugModeChanged(bool enabled);
    void onLoggingLevelChanged(const QString& level);
    void onExperimentalFeaturesChanged(bool enabled);

    // Settings actions
    void onApplySettingsClicked();
    void onResetDefaultsClicked();
    void onExportSettingsClicked();
    void onImportSettingsClicked();

private:
    // Main settings widget
    QTabWidget* m_settingsTabs;

    // Settings categories
    QWidget* m_generalWidget;
    QWidget* m_visualizationWidget;
    QWidget* m_performanceWidget;
    QWidget* m_cncWidget;
    QWidget* m_uiWidget;
    QWidget* m_advancedWidget;

    // Settings storage
    QJsonObject m_settings;
    QString m_settingsFile;

    // UI controls for each category
    // General settings
    QComboBox* m_themeCombo;
    QComboBox* m_languageCombo;
    QCheckBox* m_autoSaveCheck;
    QCheckBox* m_backupCheck;
    QSpinBox* m_autoSaveIntervalSpin;

    // Visualization settings
    QPushButton* m_backgroundColorButton;
    QComboBox* m_gridTypeCombo;
    QCheckBox* m_showAxesCheck;
    QCheckBox* m_showGridCheck;
    QCheckBox* m_hardwareAccelCheck;
    QSpinBox* m_lodDistanceSpin;

    // Performance settings
    QSpinBox* m_memoryLimitSpin;
    QSpinBox* m_cacheSizeSpin;
    QCheckBox* m_parallelProcessingCheck;
    QCheckBox* m_progressiveLoadingCheck;
    QSpinBox* m_maxFileSizeSpin;

    // CNC settings
    QComboBox* m_defaultUnitsCombo;
    QDoubleSpinBox* m_safetyMarginSpin;
    QLineEdit* m_toolLibraryPathEdit;
    QPushButton* m_toolLibraryBrowseButton;
    QCheckBox* m_autoCalculateCheck;

    // UI settings
    QPushButton* m_fontButton;
    QComboBox* m_colorSchemeCombo;
    QCheckBox* m_showTooltipsCheck;
    QCheckBox* m_compactModeCheck;

    // Advanced settings
    QCheckBox* m_debugModeCheck;
    QComboBox* m_loggingLevelCombo;
    QCheckBox* m_experimentalFeaturesCheck;
    QLineEdit* m_customPathEdit;

    // Action buttons
    QPushButton* m_applyButton;
    QPushButton* m_resetButton;
    QPushButton* m_exportButton;
    QPushButton* m_importButton;
};