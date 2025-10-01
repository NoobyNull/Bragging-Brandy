#include "CutListCanvas.h"
#include "../core/ModelService.h"
#include "../nesting/MaterialOptimizer.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QTableWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <random>
#include <algorithm>
#include <cmath>

CutListCanvas::CutListCanvas(QWidget* parent)
    : BaseCanvas(parent)
    , m_mainSplitter(nullptr)
    , m_partsTable(nullptr)
    , m_materialCombo(nullptr)
    , m_algorithmCombo(nullptr)
    , m_runOptimizationButton(nullptr)
    , m_stopOptimizationButton(nullptr)
    , m_optimizationProgress(nullptr)
    , m_nestingView(nullptr)
    , m_nestingScene(nullptr)
    , m_sheetWidth(48.0f)    // 4x8 sheet default
    , m_sheetHeight(96.0f)
    , m_sheetThickness(0.75f)
    , m_optimizationRunning(false)
    , m_currentAlgorithm("genetic")
    , m_optimizationTimer(nullptr)
    , m_materialEfficiency(0.0f)
    , m_totalWaste(0.0f)
    , m_estimatedTime(0)
    , m_estimatedCost(0.0f)
    , m_materialOptimizer(nullptr)
{
    setCanvasName("CutListCanvas");
    setupDefaultLayout();

    // Create material optimizer
    m_materialOptimizer = new MaterialOptimizer(this);

    // Connect optimizer signals
    connect(m_materialOptimizer, &MaterialOptimizer::optimizationProgress, this, &CutListCanvas::onOptimizationProgress);
    connect(m_materialOptimizer, &MaterialOptimizer::optimizationCompleted, this, &CutListCanvas::onAdvancedOptimizationCompleted);

    // Create optimization timer
    m_optimizationTimer = new QTimer(this);
    m_optimizationTimer->setInterval(100);
    connect(m_optimizationTimer, &QTimer::timeout, this, &CutListCanvas::onOptimizationStep);

    // Initialize material library
    initializeMaterialLibrary();

    // Load default layout
    restoreLayout("cutlist_default");
}

void CutListCanvas::setupDefaultLayout()
{
    // Create main horizontal splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->setChildrenCollapsible(false);

    // Create left panel (parts list and controls)
    createPartsList();
    createMaterialSelector();
    createOptimizationControls();

    // Create center panel (nesting visualization)
    createNestingView();

    // Create right panel (results and analysis)
    createResultsPanel();

    // Add panels to splitter
    m_mainSplitter->addWidget(createLeftPanel());
    m_mainSplitter->addWidget(m_nestingView);
    m_mainSplitter->addWidget(createRightPanel());

    // Set splitter proportions (25% : 50% : 25%)
    m_mainSplitter->setStretchFactor(0, 25);
    m_mainSplitter->setStretchFactor(1, 50);
    m_mainSplitter->setStretchFactor(2, 25);

    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_mainSplitter);

    setLayout(mainLayout);
}

QWidget* CutListCanvas::createLeftPanel()
{
    QWidget* leftPanel = new QWidget();
    leftPanel->setMinimumWidth(300);
    leftPanel->setMaximumWidth(400);

    QVBoxLayout* layout = new QVBoxLayout(leftPanel);

    layout->addWidget(createPartsListWidget());
    layout->addWidget(createMaterialSelectorWidget());
    layout->addWidget(createOptimizationControlsWidget());

    return leftPanel;
}

QWidget* CutListCanvas::createRightPanel()
{
    QWidget* rightPanel = new QWidget();
    rightPanel->setMinimumWidth(250);
    rightPanel->setMaximumWidth(350);

    QVBoxLayout* layout = new QVBoxLayout(rightPanel);

    layout->addWidget(createResultsPanelWidget());
    layout->addStretch();

    return rightPanel;
}

void CutListCanvas::createPartsList()
{
    m_partsTable = new QTableWidget();
    m_partsTable->setColumnCount(5);
    m_partsTable->setHorizontalHeaderLabels(QStringList() << "Part" << "Width" << "Height" << "Quantity" << "Material");
    m_partsTable->horizontalHeader()->setStretchLastSection(true);
    m_partsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_partsTable->setAlternatingRowColors(true);
    m_partsTable->setMinimumHeight(200);

    connect(m_partsTable, &QTableWidget::itemSelectionChanged, this, &CutListCanvas::onPartSelectionChanged);
}

void CutListCanvas::createMaterialSelector()
{
    // Material selection would be implemented here
    m_materialCombo = new QComboBox();
    m_materialCombo->addItem("Plywood 3/4\"", "plywood_075");
    m_materialCombo->addItem("Aluminum 1/4\"", "aluminum_025");
    m_materialCombo->addItem("Steel 1/8\"", "steel_0125");
    m_materialCombo->addItem("Acrylic 1/2\"", "acrylic_050");

    connect(m_materialCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &CutListCanvas::onMaterialSelected);
}

void CutListCanvas::createNestingView()
{
    m_nestingView = new QGraphicsView();
    m_nestingView->setRenderHint(QPainter::Antialiasing);
    m_nestingView->setDragMode(QGraphicsView::RubberBandDrag);
    m_nestingView->setMinimumSize(600, 400);

    m_nestingScene = new QGraphicsScene(m_nestingView);
    m_nestingScene->setSceneRect(0, 0, 1000, 800);
    m_nestingView->setScene(m_nestingScene);

    // Set white background
    m_nestingView->setBackgroundBrush(Qt::white);
}

void CutListCanvas::createOptimizationControls()
{
    m_algorithmCombo = new QComboBox();
    m_algorithmCombo->addItem("Genetic Algorithm", "genetic");
    m_algorithmCombo->addItem("Rectangular Packing", "rectangular");
    m_algorithmCombo->addItem("Heuristic Nesting", "heuristic");

    connect(m_algorithmCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &CutListCanvas::onAlgorithmSelected);

    m_runOptimizationButton = new QPushButton("Run Optimization");
    connect(m_runOptimizationButton, &QPushButton::clicked, this, &CutListCanvas::onRunOptimizationClicked);

    m_stopOptimizationButton = new QPushButton("Stop");
    m_stopOptimizationButton->setEnabled(false);
    connect(m_stopOptimizationButton, &QPushButton::clicked, this, &CutListCanvas::onStopOptimizationClicked);

    m_optimizationProgress = new QProgressBar();
    m_optimizationProgress->setVisible(false);
}

void CutListCanvas::createResultsPanel()
{
    m_efficiencyLabel = new QLabel("Efficiency: 0%");
    m_wasteLabel = new QLabel("Waste: 0 sq in");
    m_timeLabel = new QLabel("Time: 0 min");
    m_costLabel = new QLabel("Cost: $0.00");
}

QWidget* CutListCanvas::createPartsListWidget()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    // Parts table
    layout->addWidget(m_partsTable);

    // Parts control buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    QPushButton* addButton = new QPushButton("Add Part");
    connect(addButton, &QPushButton::clicked, this, &CutListCanvas::onAddPartClicked);
    buttonLayout->addWidget(addButton);

    QPushButton* removeButton = new QPushButton("Remove");
    connect(removeButton, &QPushButton::clicked, this, &CutListCanvas::onRemovePartClicked);
    buttonLayout->addWidget(removeButton);

    QPushButton* clearButton = new QPushButton("Clear All");
    connect(clearButton, &QPushButton::clicked, this, &CutListCanvas::onClearPartsClicked);
    buttonLayout->addWidget(clearButton);

    layout->addLayout(buttonLayout);

    return widget;
}

QWidget* CutListCanvas::createMaterialSelectorWidget()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QGroupBox* materialGroup = new QGroupBox("Material", widget);
    QVBoxLayout* materialLayout = new QVBoxLayout(materialGroup);

    materialLayout->addWidget(m_materialCombo);

    // Sheet size controls
    QHBoxLayout* sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(new QLabel("Width:"));
    QLineEdit* widthEdit = new QLineEdit(QString::number(m_sheetWidth));
    connect(widthEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        m_sheetWidth = text.toFloat();
        onSheetSizeChanged();
    });
    sizeLayout->addWidget(widthEdit);

    sizeLayout->addWidget(new QLabel("Height:"));
    QLineEdit* heightEdit = new QLineEdit(QString::number(m_sheetHeight));
    connect(heightEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        m_sheetHeight = text.toFloat();
        onSheetSizeChanged();
    });
    sizeLayout->addWidget(heightEdit);

    materialLayout->addLayout(sizeLayout);

    layout->addWidget(materialGroup);

    return widget;
}

QWidget* CutListCanvas::createOptimizationControlsWidget()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QGroupBox* optGroup = new QGroupBox("Optimization", widget);
    QVBoxLayout* optLayout = new QVBoxLayout(optGroup);

    optLayout->addWidget(m_algorithmCombo);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_runOptimizationButton);
    buttonLayout->addWidget(m_stopOptimizationButton);
    optLayout->addLayout(buttonLayout);

    optLayout->addWidget(m_optimizationProgress);

    layout->addWidget(optGroup);

    return widget;
}

QWidget* CutListCanvas::createResultsPanelWidget()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QGroupBox* resultsGroup = new QGroupBox("Results", widget);
    QVBoxLayout* resultsLayout = new QVBoxLayout(resultsGroup);

    resultsLayout->addWidget(m_efficiencyLabel);
    resultsLayout->addWidget(m_wasteLabel);
    resultsLayout->addWidget(m_timeLabel);
    resultsLayout->addWidget(m_costLabel);

    // Export buttons
    QHBoxLayout* exportLayout = new QHBoxLayout();

    QPushButton* gcodeButton = new QPushButton("Export G-Code");
    connect(gcodeButton, &QPushButton::clicked, this, &CutListCanvas::onExportGCodeClicked);
    exportLayout->addWidget(gcodeButton);

    QPushButton* dxfButton = new QPushButton("Export DXF");
    connect(dxfButton, &QPushButton::clicked, this, &CutListCanvas::onExportDXFClicked);
    exportLayout->addWidget(dxfButton);

    QPushButton* reportButton = new QPushButton("Export Report");
    connect(reportButton, &QPushButton::clicked, this, &CutListCanvas::onExportReportClicked);
    exportLayout->addWidget(reportButton);

    resultsLayout->addLayout(exportLayout);

    layout->addWidget(resultsGroup);

    return widget;
}

// BaseCanvas interface implementation
void CutListCanvas::addWidget(BaseWidget* widget, DockArea area)
{
    // Widget addition implementation
}

void CutListCanvas::removeWidget(BaseWidget* widget)
{
    // Widget removal implementation
}

void CutListCanvas::removeWidget(const QString& widgetName)
{
    // Widget removal by name implementation
}

QList<BaseWidget*> CutListCanvas::getWidgets() const
{
    return QList<BaseWidget*>();
}

QList<BaseWidget*> CutListCanvas::getWidgetsByCategory(WidgetCategory category) const
{
    return QList<BaseWidget*>();
}

BaseWidget* CutListCanvas::getWidget(const QString& widgetName) const
{
    return nullptr;
}

void CutListCanvas::saveLayout(const QString& name)
{
    // Layout saving implementation
}

void CutListCanvas::restoreLayout(const QString& name)
{
    // Layout restoration implementation
}

QStringList CutListCanvas::getAvailableLayouts() const
{
    return QStringList() << "cutlist_default" << "cutlist_compact" << "cutlist_detailed";
}

void CutListCanvas::resetLayout()
{
    restoreLayout("cutlist_default");
}

void CutListCanvas::saveState(QJsonObject& state) const
{
    state["current_material"] = m_currentMaterial;
    state["sheet_width"] = m_sheetWidth;
    state["sheet_height"] = m_sheetHeight;
    state["sheet_thickness"] = m_sheetThickness;
    state["current_algorithm"] = m_currentAlgorithm;

    // Save parts list
    QJsonArray partsArray;
    for (const NestingPart& part : m_partsList) {
        QJsonObject partObj;
        partObj["model_id"] = part.modelId;
        partObj["name"] = part.name;
        partObj["width"] = part.width;
        partObj["height"] = part.height;
        partObj["quantity"] = part.quantity;
        partObj["can_rotate"] = part.canRotate;
        partObj["material"] = part.material;
        partsArray.append(partObj);
    }
    state["parts_list"] = partsArray;
}

void CutListCanvas::restoreState(const QJsonObject& state)
{
    m_currentMaterial = state["current_material"].toString();
    m_sheetWidth = state["sheet_width"].toDouble();
    m_sheetHeight = state["sheet_height"].toDouble();
    m_sheetThickness = state["sheet_thickness"].toDouble();
    m_currentAlgorithm = state["current_algorithm"].toString();

    // Restore parts list
    m_partsList.clear();
    QJsonArray partsArray = state["parts_list"].toArray();
    for (const QJsonValue& value : partsArray) {
        QJsonObject partObj = value.toObject();
        NestingPart part;
        part.modelId = partObj["model_id"].toString();
        part.name = partObj["name"].toString();
        part.width = partObj["width"].toDouble();
        part.height = partObj["height"].toDouble();
        part.quantity = partObj["quantity"].toInt();
        part.canRotate = partObj["can_rotate"].toBool();
        part.material = partObj["material"].toString();
        m_partsList.append(part);
    }

    updatePartsTable();
    updateNestingVisualization();
}

// CutListCanvas specific methods
void CutListCanvas::addPartToNesting(const QString& modelId, int quantity)
{
    ModelService* modelService = qobject_cast<ModelService*>(parent());
    if (!modelService) {
        return;
    }

    ModelMetadata metadata = modelService->getModel(QUuid(modelId));
    if (metadata.id.isNull()) {
        return;
    }

    // Calculate part dimensions from model bounds
    QVariantMap meshStats = metadata.meshStats;
    float width = meshStats["bounds"].toMap()["x"].toDouble();
    float height = meshStats["bounds"].toMap()["y"].toDouble();

    NestingPart part;
    part.modelId = modelId;
    part.name = QFileInfo(metadata.filename).baseName();
    part.width = width;
    part.height = height;
    part.quantity = quantity;
    part.canRotate = true;
    part.material = m_currentMaterial;

    m_partsList.append(part);
    updatePartsTable();
}

void CutListCanvas::removePartFromNesting(const QString& modelId)
{
    for (int i = 0; i < m_partsList.size(); ++i) {
        if (m_partsList[i].modelId == modelId) {
            m_partsList.removeAt(i);
            break;
        }
    }
    updatePartsTable();
}

void CutListCanvas::clearAllParts()
{
    m_partsList.clear();
    updatePartsTable();
}

void CutListCanvas::setMaterial(const QString& materialName)
{
    m_currentMaterial = materialName;

    // Update material properties
    if (materialName.contains("plywood")) {
        m_sheetThickness = 0.75f;
    } else if (materialName.contains("aluminum")) {
        m_sheetThickness = 0.25f;
    } else if (materialName.contains("steel")) {
        m_sheetThickness = 0.125f;
    } else if (materialName.contains("acrylic")) {
        m_sheetThickness = 0.5f;
    }

    updateMaterialDisplay();
}

void CutListCanvas::setSheetSize(float width, float height)
{
    m_sheetWidth = width;
    m_sheetHeight = height;
    updateNestingVisualization();
}

void CutListCanvas::setSheetThickness(float thickness)
{
    m_sheetThickness = thickness;
    updateMaterialDisplay();
}

void CutListCanvas::runNestingOptimization()
{
    if (m_partsList.isEmpty()) {
        QMessageBox::warning(this, "No Parts", "Please add parts to the nesting list first.");
        return;
    }

    m_optimizationRunning = true;
    m_runOptimizationButton->setEnabled(false);
    m_stopOptimizationButton->setEnabled(true);
    m_optimizationProgress->setVisible(true);
    m_optimizationProgress->setValue(0);

    emit onOptimizationStarted();

    // Use advanced MaterialOptimizer
    if (m_materialOptimizer) {
        // Convert parts to optimizer format
        QList<MaterialOptimizer::Part> optimizerParts;
        for (const NestingPart& part : m_partsList) {
            MaterialOptimizer::Part optPart;
            optPart.id = part.modelId;
            optPart.name = part.name;
            optPart.dimensions = QSizeF(part.width, part.height);
            optPart.quantity = part.quantity;
            optPart.canRotate = part.canRotate;
            optPart.material = part.material;
            optimizerParts.append(optPart);
        }

        // Create sheet
        QList<MaterialOptimizer::Sheet> sheets;
        MaterialOptimizer::Sheet sheet;
        sheet.id = "main_sheet";
        sheet.name = "Main Sheet";
        sheet.dimensions = QSizeF(m_sheetWidth, m_sheetHeight);
        sheet.cost = 50.0f; // Default cost
        sheet.material = m_currentMaterial;
        sheets.append(sheet);

        // Configure optimization
        MaterialOptimizer::OptimizationConfig config;
        config.algorithm = m_currentAlgorithm;
        config.populationSize = 100;
        config.maxGenerations = 200;
        config.mutationRate = 0.1f;
        config.crossoverRate = 0.8f;

        // Start async optimization
        QFuture<MaterialOptimizer::OptimizationResult> future = m_materialOptimizer->optimizeNestingAsync(optimizerParts, sheets, config);

        // Connect to completion signal
        connect(&future, &QFutureWatcherBase::finished, this, [this, future]() {
            onAdvancedOptimizationCompleted(future.result());
        });
    } else {
        // Fallback to basic algorithms
        if (m_currentAlgorithm == "genetic") {
            runGeneticOptimization();
        } else if (m_currentAlgorithm == "rectangular") {
            runRectangularNesting();
        } else {
            heuristicNesting();
        }
    }
}

void CutListCanvas::runGeneticOptimization()
{
    // Genetic algorithm implementation
    m_optimizationTimer->start();

    // Initialize population
    initializeGeneticPopulation();

    // Run evolution
    for (int generation = 0; generation < 100; ++generation) {
        if (!m_optimizationRunning) {
            break;
        }

        // Selection, crossover, mutation
        geneticSelection();
        geneticCrossover();
        geneticMutation();

        // Evaluate fitness
        evaluatePopulation();

        // Update progress
        int progress = (generation * 100) / 100;
        m_optimizationProgress->setValue(progress);

        // Process events to keep UI responsive
        QCoreApplication::processEvents();
    }

    m_optimizationTimer->stop();
    finalizeOptimization();
}

void CutListCanvas::runRectangularNesting()
{
    // Rectangular bin packing algorithm
    rectangularBinPacking();

    m_optimizationProgress->setValue(100);
    finalizeOptimization();
}

void CutListCanvas::stopOptimization()
{
    m_optimizationRunning = false;
    m_runOptimizationButton->setEnabled(true);
    m_stopOptimizationButton->setEnabled(false);
    m_optimizationProgress->setVisible(false);

    if (m_optimizationTimer->isActive()) {
        m_optimizationTimer->stop();
    }
}

void CutListCanvas::exportGCode(const QString& filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Failed", "Cannot write to file: " + filepath);
        return;
    }

    QTextStream stream(&file);

    // Generate G-code header
    stream << "%\n";
    stream << "O1000 (CUT LIST PROGRAM)\n";
    stream << QString(" (Material: %1)\n").arg(m_currentMaterial);
    stream << QString(" (Sheet: %1x%2x%3)\n").arg(m_sheetWidth).arg(m_sheetHeight).arg(m_sheetThickness);
    stream << QString(" (Efficiency: %1%)\n").arg(m_materialEfficiency, 0, 'f', 1);
    stream << "\n";

    // Generate cutting paths
    for (const NestingSolution& solution : m_nestingSolutions) {
        stream << QString(" (Sheet %1)\n").arg(solution.sheetId);

        for (int i = 0; i < solution.partPositions.size(); ++i) {
            QPointF pos = solution.partPositions[i];
            float rotation = solution.partRotations[i];

            stream << QString(" (Part at X%1 Y%2 R%3)\n")
                      .arg(pos.x()).arg(pos.y()).arg(rotation);
            // Add cutting path G-code here
        }
    }

    stream << "M30\n";
    stream << "%\n";

    file.close();

    statusBar()->showMessage(QString("G-Code exported to: %1").arg(filepath));
}

void CutListCanvas::exportDXF(const QString& filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Failed", "Cannot write to file: " + filepath);
        return;
    }

    QTextStream stream(&file);

    // DXF header
    stream << "0\nSECTION\n2\nHEADER\n0\nENDSEC\n";
    stream << "0\nSECTION\n2\nENTITIES\n";

    // Export sheet and parts as DXF entities
    for (const NestingSolution& solution : m_nestingSolutions) {
        // Export sheet boundary
        stream << "0\nLWPOLYLINE\n8\nSHEET\n";
        stream << QString("10\n%1\n20\n%2\n").arg(0).arg(0);
        stream << QString("10\n%1\n20\n%2\n").arg(m_sheetWidth).arg(0);
        stream << QString("10\n%1\n20\n%2\n").arg(m_sheetWidth).arg(m_sheetHeight);
        stream << QString("10\n%1\n20\n%2\n").arg(0).arg(m_sheetHeight);
        stream << "0\n";

        // Export parts
        for (int i = 0; i < solution.partPositions.size(); ++i) {
            QPointF pos = solution.partPositions[i];
            float rotation = solution.partRotations[i];

            // Get part dimensions
            if (i < m_partsList.size()) {
                const NestingPart& part = m_partsList[i];
                float w = part.width;
                float h = part.height;

                // Export part rectangle
                stream << "0\nLWPOLYLINE\n8\nPART\n";
                stream << QString("10\n%1\n20\n%2\n").arg(pos.x()).arg(pos.y());
                stream << QString("10\n%1\n20\n%2\n").arg(pos.x() + w).arg(pos.y());
                stream << QString("10\n%1\n20\n%2\n").arg(pos.x() + w).arg(pos.y() + h);
                stream << QString("10\n%1\n20\n%2\n").arg(pos.x()).arg(pos.y() + h);
                stream << "0\n";
            }
        }
    }

    stream << "0\nENDSEC\n0\nEOF\n";
    file.close();

    statusBar()->showMessage(QString("DXF exported to: %1").arg(filepath));
}

void CutListCanvas::exportCuttingReport(const QString& filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Failed", "Cannot write to file: " + filepath);
        return;
    }

    QTextStream stream(&file);

    // Generate cutting report
    stream << "CUTTING REPORT\n";
    stream << "==============\n\n";

    stream << QString("Material: %1\n").arg(m_currentMaterial);
    stream << QString("Sheet Size: %1\" x %2\"\n").arg(m_sheetWidth).arg(m_sheetHeight);
    stream << QString("Thickness: %1\"\n\n").arg(m_sheetThickness);

    stream << QString("Material Efficiency: %1%\n").arg(m_materialEfficiency, 0, 'f', 1);
    stream << QString("Total Waste: %1 sq in\n").arg(m_totalWaste, 0, 'f', 1);
    stream << QString("Estimated Time: %1 minutes\n").arg(m_estimatedTime / 60);
    stream << QString("Estimated Cost: $%1\n\n").arg(m_estimatedCost, 0, 'f', 2);

    stream << "PARTS LIST:\n";
    stream << "-----------\n";

    for (const NestingPart& part : m_partsList) {
        stream << QString("  %1: %2\" x %3\" x %4 (Qty: %5)\n")
                  .arg(part.name)
                  .arg(part.width, 0, 'f', 2)
                  .arg(part.height, 0, 'f', 2)
                  .arg(m_sheetThickness, 0, 'f', 2)
                  .arg(part.quantity);
    }

    file.close();

    statusBar()->showMessage(QString("Report exported to: %1").arg(filepath));
}

float CutListCanvas::getMaterialEfficiency() const
{
    return m_materialEfficiency;
}

float CutListCanvas::getTotalWaste() const
{
    return m_totalWaste;
}

qint64 CutListCanvas::getEstimatedCuttingTime() const
{
    return m_estimatedTime;
}

float CutListCanvas::getEstimatedCost() const
{
    return m_estimatedCost;
}

// Private helper methods
void CutListCanvas::updatePartsTable()
{
    m_partsTable->setRowCount(0);

    for (const NestingPart& part : m_partsList) {
        int row = m_partsTable->rowCount();
        m_partsTable->insertRow(row);

        m_partsTable->setItem(row, 0, new QTableWidgetItem(part.name));
        m_partsTable->setItem(row, 1, new QTableWidgetItem(QString::number(part.width, 'f', 2)));
        m_partsTable->setItem(row, 2, new QTableWidgetItem(QString::number(part.height, 'f', 2)));
        m_partsTable->setItem(row, 3, new QTableWidgetItem(QString::number(part.quantity)));
        m_partsTable->setItem(row, 4, new QTableWidgetItem(part.material));
    }
}

void CutListCanvas::updateNestingVisualization()
{
    m_nestingScene->clear();
    m_sheetItems.clear();
    m_partItems.clear();

    // Draw sheets
    for (const NestingSolution& solution : m_nestingSolutions) {
        drawSheet(solution.sheetId);
    }

    // Draw parts
    for (int i = 0; i < m_nestingSolutions.size() && i < m_partsList.size(); ++i) {
        const NestingSolution& solution = m_nestingSolutions[i];
        const NestingPart& part = m_partsList[i];

        for (int j = 0; j < solution.partPositions.size(); ++j) {
            QPointF pos = solution.partPositions[j];
            float rotation = solution.partRotations[j];
            drawPart(part.name, pos, rotation);
        }
    }

    // Draw cutting paths if available
    drawCuttingPath();
}

void CutListCanvas::drawSheet(const QString& sheetId)
{
    QGraphicsRectItem* sheet = new QGraphicsRectItem(0, 0, m_sheetWidth, m_sheetHeight);
    sheet->setPen(QPen(Qt::black, 2));
    sheet->setBrush(QBrush(QColor(240, 240, 240)));
    m_nestingScene->addItem(sheet);
    m_sheetItems.append(sheet);

    // Add sheet label
    QGraphicsTextItem* label = new QGraphicsTextItem(sheetId);
    QFont font = label->font();
    font.setPointSize(10);
    label->setFont(font);
    label->setPos(10, 10);
    m_nestingScene->addItem(label);
}

void CutListCanvas::drawPart(const QString& partId, const QPointF& position, float rotation)
{
    // Find part in parts list
    for (const NestingPart& part : m_partsList) {
        if (part.name == partId) {
            QGraphicsRectItem* partItem = new QGraphicsRectItem(position.x(), position.y(),
                                                               part.width, part.height);
            partItem->setPen(QPen(Qt::blue, 1));
            partItem->setBrush(QBrush(QColor(100, 150, 255, 100)));
            partItem->setRotation(rotation);
            m_nestingScene->addItem(partItem);
            m_partItems.append(partItem);

            // Add part label
            QGraphicsTextItem* label = new QGraphicsTextItem(part.name);
            QFont font = label->font();
            font.setPointSize(8);
            label->setFont(font);
            label->setPos(position.x() + 5, position.y() + 5);
            m_nestingScene->addItem(label);

            break;
        }
    }
}

void CutListCanvas::drawCuttingPath()
{
    // Draw cutting paths between parts
    // This would generate the actual cutting sequence
}

void CutListCanvas::initializeMaterialLibrary()
{
    // Initialize material properties
    // In a full implementation, this would load from a database
}

void CutListCanvas::updateMaterialDisplay()
{
    // Update material-related UI elements
}

void CutListCanvas::initializeGeneticPopulation()
{
    // Initialize genetic algorithm population
    m_nestingSolutions.clear();

    // Create initial random solutions
    for (int i = 0; i < 50; ++i) { // Population size
        NestingSolution solution;
        solution.sheetId = QString("Sheet_%1").arg(i + 1);
        solution.efficiency = 0.0f;
        solution.waste = m_sheetWidth * m_sheetHeight;

        // Generate random part positions
        for (const NestingPart& part : m_partsList) {
            QPointF randomPos(
                QRandomGenerator::global()->bounded(m_sheetWidth - part.width),
                QRandomGenerator::global()->bounded(m_sheetHeight - part.height)
            );
            solution.partPositions.append(randomPos);
            solution.partRotations.append(QRandomGenerator::global()->bounded(360.0f));
        }

        m_nestingSolutions.append(solution);
    }
}

void CutListCanvas::geneticSelection()
{
    // Select best solutions for reproduction
    std::sort(m_nestingSolutions.begin(), m_nestingSolutions.end(),
              [](const NestingSolution& a, const NestingSolution& b) {
                  return a.efficiency > b.efficiency;
              });

    // Keep only top 50% of solutions
    int keepCount = m_nestingSolutions.size() / 2;
    m_nestingSolutions = m_nestingSolutions.mid(0, keepCount);
}

void CutListCanvas::geneticCrossover()
{
    // Create new solutions through crossover
    QList<NestingSolution> newSolutions;

    while (newSolutions.size() < 50) {
        int parent1Index = QRandomGenerator::global()->bounded(m_nestingSolutions.size());
        int parent2Index = QRandomGenerator::global()->bounded(m_nestingSolutions.size());

        NestingSolution child = crossoverSolutions(m_nestingSolutions[parent1Index],
                                                  m_nestingSolutions[parent2Index]);
        newSolutions.append(child);
    }

    m_nestingSolutions.append(newSolutions);
}

void CutListCanvas::geneticMutation()
{
    // Apply random mutations to solutions
    for (NestingSolution& solution : m_nestingSolutions) {
        if (QRandomGenerator::global()->bounded(1.0f) < 0.1f) { // 10% mutation rate
            // Randomly mutate a part position
            int partIndex = QRandomGenerator::global()->bounded(solution.partPositions.size());
            QPointF& pos = solution.partPositions[partIndex];

            pos.setX(qMax(0.0f, qMin(m_sheetWidth - 10.0f, pos.x() + QRandomGenerator::global()->bounded(-20.0f, 20.0f))));
            pos.setY(qMax(0.0f, qMin(m_sheetHeight - 10.0f, pos.y() + QRandomGenerator::global()->bounded(-20.0f, 20.0f))));
        }
    }
}

void CutListCanvas::evaluatePopulation()
{
    for (NestingSolution& solution : m_nestingSolutions) {
        // Calculate efficiency for this solution
        float totalPartArea = 0.0f;

        for (int i = 0; i < m_partsList.size() && i < solution.partPositions.size(); ++i) {
            const NestingPart& part = m_partsList[i];
            totalPartArea += part.width * part.height * part.quantity;
        }

        solution.efficiency = (totalPartArea / (m_sheetWidth * m_sheetHeight)) * 100.0f;
        solution.waste = m_sheetWidth * m_sheetHeight - totalPartArea;
    }
}

CutListCanvas::NestingSolution CutListCanvas::crossoverSolutions(const NestingSolution& parent1, const NestingSolution& parent2)
{
    NestingSolution child;
    child.sheetId = QString("Child_%1_%2").arg(parent1.sheetId).arg(parent2.sheetId);

    // Simple crossover: take positions from parent1, rotations from parent2
    child.partPositions = parent1.partPositions;
    child.partRotations = parent2.partRotations;

    return child;
}

void CutListCanvas::rectangularBinPacking()
{
    // Simple rectangular bin packing algorithm
    m_nestingSolutions.clear();

    NestingSolution solution;
    solution.sheetId = "Sheet_1";
    solution.partPositions.clear();
    solution.partRotations.clear();

    QPointF currentPos(0, 0);
    float maxHeightInRow = 0.0f;

    for (const NestingPart& part : m_partsList) {
        // Check if part fits in current row
        if (currentPos.x() + part.width > m_sheetWidth) {
            // Move to next row
            currentPos.setX(0);
            currentPos.setY(currentPos.y() + maxHeightInRow);
            maxHeightInRow = 0.0f;
        }

        // Check if part fits on sheet
        if (currentPos.x() + part.width > m_sheetWidth ||
            currentPos.y() + part.height > m_sheetHeight) {
            // Start new sheet
            if (!solution.partPositions.isEmpty()) {
                m_nestingSolutions.append(solution);

                solution.partPositions.clear();
                solution.partRotations.clear();
                solution.sheetId = QString("Sheet_%1").arg(m_nestingSolutions.size() + 1);
            }

            currentPos = QPointF(0, 0);
            maxHeightInRow = 0.0f;
        }

        // Place part
        solution.partPositions.append(currentPos);
        solution.partRotations.append(0.0f); // No rotation for rectangular packing

        currentPos.setX(currentPos.x() + part.width);
        maxHeightInRow = qMax(maxHeightInRow, part.height);
    }

    if (!solution.partPositions.isEmpty()) {
        m_nestingSolutions.append(solution);
    }

    // Calculate efficiency
    calculateOverallEfficiency();
}

void CutListCanvas::heuristicNesting()
{
    // Simple heuristic nesting algorithm
    rectangularBinPacking(); // Use rectangular packing as heuristic
}

void CutListCanvas::finalizeOptimization()
{
    m_optimizationRunning = false;
    m_runOptimizationButton->setEnabled(true);
    m_stopOptimizationButton->setEnabled(false);
    m_optimizationProgress->setVisible(false);

    // Update visualization with best solution
    if (!m_nestingSolutions.isEmpty()) {
        updateNestingVisualization();
        calculateOverallEfficiency();
        updateResultsDisplay();
    }

    emit onOptimizationCompleted();
}

void CutListCanvas::calculateOverallEfficiency()
{
    if (m_nestingSolutions.isEmpty()) {
        return;
    }

    float totalSheetArea = m_sheetWidth * m_sheetHeight * m_nestingSolutions.size();
    float totalPartArea = 0.0f;

    for (const NestingPart& part : m_partsList) {
        totalPartArea += part.width * part.height * part.quantity;
    }

    m_materialEfficiency = (totalPartArea / totalSheetArea) * 100.0f;
    m_totalWaste = totalSheetArea - totalPartArea;

    // Estimate cutting time (simplified)
    m_estimatedTime = static_cast<qint64>(totalPartArea * 0.1); // 0.1 seconds per square inch

    // Estimate cost (simplified)
    m_estimatedCost = totalSheetArea * 0.05f; // $0.05 per square inch
}

void CutListCanvas::updateResultsDisplay()
{
    m_efficiencyLabel->setText(QString("Efficiency: %1%").arg(m_materialEfficiency, 0, 'f', 1));
    m_wasteLabel->setText(QString("Waste: %1 sq in").arg(m_totalWaste, 0, 'f', 1));
    m_timeLabel->setText(QString("Time: %1 min").arg(m_estimatedTime / 60));
    m_costLabel->setText(QString("Cost: $%1").arg(m_estimatedCost, 0, 'f', 2));
}

// Event handlers
void CutListCanvas::onPartSelectionChanged()
{
    // Handle part selection changes
}

void CutListCanvas::onAddPartClicked()
{
    // Show dialog to add parts from model library
    QStringList availableModels;

    ModelService* modelService = qobject_cast<ModelService*>(parent());
    if (modelService) {
        QList<ModelMetadata> models = modelService->getAllModels();
        for (const ModelMetadata& model : models) {
            availableModels.append(QString("%1 (%2)")
                                   .arg(QFileInfo(model.filename).baseName())
                                   .arg(model.id.toString()));
        }
    }

    if (availableModels.isEmpty()) {
        QMessageBox::information(this, "No Models", "No models available. Please import models first.");
        return;
    }

    QString selectedModel = QInputDialog::getItem(this, "Add Part",
                                                 "Select model:", availableModels, 0, false);

    if (!selectedModel.isEmpty()) {
        // Extract model ID from selection
        QString modelId = selectedModel.split(" (").last().split(")").first();

        int quantity = QInputDialog::getInt(this, "Part Quantity",
                                          "Enter quantity:", 1, 1, 1000, 1);

        if (quantity > 0) {
            addPartToNesting(modelId, quantity);
        }
    }
}

void CutListCanvas::onRemovePartClicked()
{
    int currentRow = m_partsTable->currentRow();
    if (currentRow >= 0 && currentRow < m_partsList.size()) {
        m_partsList.removeAt(currentRow);
        updatePartsTable();
    }
}

void CutListCanvas::onClearPartsClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Clear Parts",
        "Remove all parts from the nesting list?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        clearAllParts();
    }
}

void CutListCanvas::onMaterialSelected(const QString& material)
{
    setMaterial(material);
}

void CutListCanvas::onSheetSizeChanged()
{
    updateNestingVisualization();
}

void CutListCanvas::onThicknessChanged()
{
    updateMaterialDisplay();
}

void CutListCanvas::onAlgorithmSelected(const QString& algorithm)
{
    m_currentAlgorithm = m_algorithmCombo->itemData(m_algorithmCombo->currentIndex()).toString();
}

void CutListCanvas::onRunOptimizationClicked()
{
    runNestingOptimization();
}

void CutListCanvas::onStopOptimizationClicked()
{
    stopOptimization();
}

void CutListCanvas::onExportGCodeClicked()
{
    QString filepath = QFileDialog::getSaveFileName(this,
        "Export G-Code",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "G-Code files (*.nc *.gcode)");

    if (!filepath.isEmpty()) {
        exportGCode(filepath);
    }
}

void CutListCanvas::onExportDXFClicked()
{
    QString filepath = QFileDialog::getSaveFileName(this,
        "Export DXF",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "DXF files (*.dxf)");

    if (!filepath.isEmpty()) {
        exportDXF(filepath);
    }
}

void CutListCanvas::onExportReportClicked()
{
    QString filepath = QFileDialog::getSaveFileName(this,
        "Export Report",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Text files (*.txt)");

    if (!filepath.isEmpty()) {
        exportCuttingReport(filepath);
    }
}

void CutListCanvas::onOptimizationStarted()
{
    statusBar()->showMessage("Optimization started...");
}

void CutListCanvas::onOptimizationProgress(int percentage)
{
    m_optimizationProgress->setValue(percentage);
}

void CutListCanvas::onOptimizationCompleted()
{
    statusBar()->showMessage(QString("Optimization completed - Efficiency: %1%").arg(m_materialEfficiency, 0, 'f', 1));
}

void CutListCanvas::onAdvancedOptimizationCompleted(const MaterialOptimizer::OptimizationResult& result)
{
    m_optimizationRunning = false;
    m_runOptimizationButton->setEnabled(true);
    m_stopOptimizationButton->setEnabled(false);
    m_optimizationProgress->setVisible(false);

    // Convert optimizer result to canvas format
    m_nestingSolutions.clear();

    for (int i = 0; i < result.usedSheets.size(); ++i) {
        NestingSolution solution;
        solution.sheetId = result.usedSheets[i].id;
        solution.efficiency = result.totalEfficiency;
        solution.waste = result.totalCost; // Simplified mapping

        // Convert positions
        if (i < result.partPositions.size()) {
            solution.partPositions = result.partPositions[i];
        }

        if (i < result.partRotations.size()) {
            solution.partRotations = result.partRotations[i];
        }

        m_nestingSolutions.append(solution);
    }

    // Update display values
    m_materialEfficiency = result.totalEfficiency;
    m_totalWaste = result.totalCost; // Simplified mapping
    m_estimatedTime = result.optimizationTimeMs / 1000; // Convert to seconds
    m_estimatedCost = result.totalCost;

    // Update visualization
    updateNestingVisualization();
    updateResultsDisplay();

    statusBar()->showMessage(QString("Advanced optimization completed - Efficiency: %1% in %2ms")
                             .arg(result.totalEfficiency, 0, 'f', 1)
                             .arg(result.optimizationTimeMs));
}

void CutListCanvas::onOptimizationStep()
{
    // Handle optimization step updates
    static int stepCount = 0;
    stepCount++;

    // Update progress periodically
    if (stepCount % 10 == 0) {
        int progress = qMin(90, stepCount / 2); // Simulate progress
        m_optimizationProgress->setValue(progress);
    }
}