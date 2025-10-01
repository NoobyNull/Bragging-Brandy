#pragma once

#include "../core/BaseCanvas.h"
#include "../core/BaseTypes.h"
#include "../tools/FeedSpeedCalculator.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTableWidget>
#include <QTreeWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QProgressBar>
#include <QTimer>

/**
 * @brief CNC Tools Canvas - Tool library and feed/speed optimization workspace
 *
 * Comprehensive CNC tool management featuring:
 * - Industry-standard .tools JSON file import/export
 * - Tool geometry and cutting parameter management
 * - Feed and speed calculation with material database
 * - Tool wear tracking and life estimation
 * - Machining simulation and optimization
 * - Cost per tool and replacement scheduling
 */
class CNCToolsCanvas : public BaseCanvas
{
    Q_OBJECT

public:
    explicit CNCToolsCanvas(QWidget* parent = nullptr);
    virtual ~CNCToolsCanvas() = default;

    // BaseCanvas interface implementation
    QString canvasName() const override { return "CNCToolsCanvas"; }
    QString canvasDescription() const override { return "CNC tool library and feed/speed optimization"; }
    QString canvasType() const override { return "cnc_tools"; }
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

    // CNCToolsCanvas specific methods
    void importToolsFile(const QString& filepath);
    void exportToolsFile(const QString& filepath);
    void addTool(const QString& toolType);
    void editTool(const QString& toolId);
    void deleteTool(const QString& toolId);

    // Feed/speed calculations
    void calculateFeedSpeed(const QString& toolId, const QString& material);
    void optimizeMachiningParameters();
    void generateMachiningReport();

    // Tool management
    void updateToolWear(const QString& toolId, float wearAmount);
    void scheduleToolReplacement(const QString& toolId);
    void trackToolUsage(const QString& toolId, float usageTime);

    // Material database
    void addMaterial(const QString& name, const QVariantMap& properties);
    void updateMaterialProperties(const QString& name, const QVariantMap& properties);
    void removeMaterial(const QString& name);

protected:
    void setupDefaultLayout();
    void createToolLibrary();
    void createMaterialDatabase();
    void createFeedSpeedCalculator();
    void createToolVisualization();
    void createWearTracking();

    // Tool library management
    void loadToolsFromDatabase();
    void saveToolsToDatabase();
    void validateToolData(const QVariantMap& toolData);

    // Feed/speed calculations
    void calculateOptimalParameters(const QString& toolId, const QString& material);
    void applySafetyMargins(float& feedRate, float& speed, float safetyFactor);

    // Tool visualization
    void renderToolGeometry(const QString& toolId);
    void renderCuttingSimulation();

private slots:
    // Tool library events
    void onToolSelectionChanged();
    void onAddToolClicked();
    void onEditToolClicked();
    void onDeleteToolClicked();
    void onImportToolsClicked();
    void onExportToolsClicked();

    // Material events
    void onMaterialSelectionChanged();
    void onAddMaterialClicked();
    void onEditMaterialClicked();

    // Feed/speed events
    void onCalculateFeedSpeedClicked();
    void onOptimizeParametersClicked();
    void onMaterialChanged(const QString& material);
    void onToolChanged(const QString& tool);

    // Wear tracking events
    void onUpdateWearClicked();
    void onResetWearClicked();
    void onToolUsageRecorded(float usageTime);

private:
    // Main UI components
    QSplitter* m_mainSplitter;
    QTreeWidget* m_toolTree;
    QTableWidget* m_toolDetailsTable;
    QComboBox* m_materialCombo;
    QComboBox* m_toolTypeCombo;

    // Feed/speed calculator
    QGroupBox* m_calculatorGroup;
    QLineEdit* m_feedRateEdit;
    QLineEdit* m_spindleSpeedEdit;
    QLineEdit* m_cuttingDepthEdit;
    QPushButton* m_calculateButton;
    QPushButton* m_optimizeButton;

    // Tool visualization
    QWidget* m_visualizationWidget;
    QTimer* m_animationTimer;

    // Wear tracking
    QProgressBar* m_toolWearBar;
    QLabel* m_toolLifeLabel;
    QPushButton* m_updateWearButton;

    // Tool data structure
    struct ToolData {
        QString id;
        QString name;
        QString type; // "endmill", "drill", "router", etc.
        QString manufacturer;

        // Geometry
        float diameter;
        float fluteLength;
        float overallLength;
        float shankDiameter;
        int fluteCount;
        float helixAngle;
        QString material;

        // Cutting parameters
        QVariantMap cuttingParameters; // Material-specific parameters

        // Usage tracking
        float totalUsageTime;
        float remainingLife;
        QDateTime lastUsed;
        QDateTime purchaseDate;

        // Cost information
        float purchaseCost;
        float replacementCost;

        ToolData() : diameter(0), fluteLength(0), overallLength(0), shankDiameter(0),
                     fluteCount(2), helixAngle(30), totalUsageTime(0), remainingLife(100),
                     purchaseCost(0), replacementCost(0) {}
    };

    // Material data structure
    struct MaterialData {
        QString name;
        QString category; // "wood", "metal", "plastic", "composite"
        QVariantMap properties; // Hardness, density, etc.

        // Machining parameters
        QVariantMap machiningParameters; // SFM, chip load, etc.

        MaterialData() {}
        MaterialData(const QString& name) : name(name) {}
    };

    QList<ToolData> m_tools;
    QList<MaterialData> m_materials;
    QString m_selectedToolId;
    QString m_selectedMaterial;

    // Calculation results
    float m_calculatedFeedRate;
    float m_calculatedSpindleSpeed;
    float m_calculatedCuttingTime;
    QString m_optimizationReport;

    // Advanced calculation system
    FeedSpeedCalculator* m_feedSpeedCalculator;
};