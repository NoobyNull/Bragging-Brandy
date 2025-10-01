#pragma once

#include "../core/BaseCanvas.h"
#include "../core/BaseTypes.h"
#include "../nesting/MaterialOptimizer.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTableWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QProgressBar>
#include <QTimer>

/**
 * @brief Cut List Canvas - Material optimization and nesting workspace
 *
 * Specialized canvas for CNC material optimization featuring:
 * - 2D nesting algorithms for efficient material usage
 * - Genetic algorithm optimization
 * - Visual nesting preview with drag-and-drop
 * - Material library management
 * - Cost estimation and waste calculation
 * - G-code generation for cutting operations
 */
class CutListCanvas : public BaseCanvas
{
    Q_OBJECT

public:
    explicit CutListCanvas(QWidget* parent = nullptr);
    virtual ~CutListCanvas() = default;

    // BaseCanvas interface implementation
    QString canvasName() const override { return "CutListCanvas"; }
    QString canvasDescription() const override { return "Material optimization and nesting workspace"; }
    QString canvasType() const override { return "cutlist"; }
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

    // CutListCanvas specific methods
    void addPartToNesting(const QString& modelId, int quantity = 1);
    void removePartFromNesting(const QString& modelId);
    void clearAllParts();

    // Material management
    void setMaterial(const QString& materialName);
    void setSheetSize(float width, float height);
    void setSheetThickness(float thickness);

    // Nesting algorithms
    void runNestingOptimization();
    void runGeneticOptimization();
    void runRectangularNesting();
    void stopOptimization();

    // Results and export
    void exportGCode(const QString& filepath);
    void exportDXF(const QString& filepath);
    void exportCuttingReport(const QString& filepath);

    // Analysis
    float getMaterialEfficiency() const;
    float getTotalWaste() const;
    qint64 getEstimatedCuttingTime() const;
    float getEstimatedCost() const;

protected:
    void setupDefaultLayout();
    void createPartsList();
    void createMaterialSelector();
    void createNestingView();
    void createOptimizationControls();
    void createResultsPanel();

    // Nesting visualization
    void updateNestingVisualization();
    void drawSheet(const QString& sheetId);
    void drawPart(const QString& partId, const QPointF& position, float rotation);
    void drawCuttingPath();

    // Optimization algorithms
    void geneticAlgorithmOptimization();
    void rectangularBinPacking();
    void heuristicNesting();

private slots:
    // Parts management
    void onPartSelectionChanged();
    void onAddPartClicked();
    void onRemovePartClicked();
    void onClearPartsClicked();

    // Material selection
    void onMaterialSelected(const QString& material);
    void onSheetSizeChanged();
    void onThicknessChanged();

    // Optimization
    void onOptimizationStarted();
    void onOptimizationProgress(int percentage);
    void onOptimizationCompleted();

    // Algorithm selection
    void onAlgorithmSelected(const QString& algorithm);
    void onRunOptimizationClicked();
    void onStopOptimizationClicked();

    // Export
    void onExportGCodeClicked();
    void onExportDXFClicked();
    void onExportReportClicked();

private:
    // Main UI components
    QSplitter* m_mainSplitter;
    QTableWidget* m_partsTable;
    QComboBox* m_materialCombo;
    QComboBox* m_algorithmCombo;
    QPushButton* m_runOptimizationButton;
    QPushButton* m_stopOptimizationButton;
    QProgressBar* m_optimizationProgress;

    // Nesting visualization
    QGraphicsView* m_nestingView;
    QGraphicsScene* m_nestingScene;
    QList<QGraphicsItem*> m_sheetItems;
    QList<QGraphicsItem*> m_partItems;

    // Results display
    QLabel* m_efficiencyLabel;
    QLabel* m_wasteLabel;
    QLabel* m_timeLabel;
    QLabel* m_costLabel;

    // Material properties
    QString m_currentMaterial;
    float m_sheetWidth;
    float m_sheetHeight;
    float m_sheetThickness;

    // Parts data
    struct NestingPart {
        QString modelId;
        QString name;
        float width;
        float height;
        int quantity;
        bool canRotate;
        QString material;
    };

    QList<NestingPart> m_partsList;

    // Optimization state
    bool m_optimizationRunning;
    QString m_currentAlgorithm;
    QTimer* m_optimizationTimer;

    // Results
    float m_materialEfficiency;
    float m_totalWaste;
    qint64 m_estimatedTime;
    float m_estimatedCost;

    // Nesting solution
    struct NestingSolution {
        QString sheetId;
        QList<QPointF> partPositions;
        QList<float> partRotations;
        float efficiency;
        float waste;
    };

    QList<NestingSolution> m_nestingSolutions;

    // Advanced optimization
    MaterialOptimizer* m_materialOptimizer;
};