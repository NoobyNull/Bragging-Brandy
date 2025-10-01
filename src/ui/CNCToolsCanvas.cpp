#include "CNCToolsCanvas.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QFormLayout>
#include <QTableWidget>
#include <QTreeWidget>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <cmath>

CNCToolsCanvas::CNCToolsCanvas(QWidget* parent)
    : BaseCanvas(parent)
    , m_mainSplitter(nullptr)
    , m_toolTree(nullptr)
    , m_toolDetailsTable(nullptr)
    , m_materialCombo(nullptr)
    , m_toolTypeCombo(nullptr)
    , m_calculatorGroup(nullptr)
    , m_feedRateEdit(nullptr)
    , m_spindleSpeedEdit(nullptr)
    , m_cuttingDepthEdit(nullptr)
    , m_calculateButton(nullptr)
    , m_optimizeButton(nullptr)
    , m_visualizationWidget(nullptr)
    , m_animationTimer(nullptr)
    , m_toolWearBar(nullptr)
    , m_toolLifeLabel(nullptr)
    , m_updateWearButton(nullptr)
    , m_selectedToolId("")
    , m_selectedMaterial("aluminum")
    , m_calculatedFeedRate(0.0f)
    , m_calculatedSpindleSpeed(0.0f)
    , m_calculatedCuttingTime(0.0f)
    , m_feedSpeedCalculator(nullptr)
{
    setCanvasName("CNCToolsCanvas");
    setupDefaultLayout();

    // Create feed/speed calculator
    m_feedSpeedCalculator = new FeedSpeedCalculator(this);

    // Connect calculator signals
    connect(m_feedSpeedCalculator, &FeedSpeedCalculator::calculationCompleted, this, &CNCToolsCanvas::onAdvancedCalculationCompleted);

    // Create animation timer for tool visualization
    m_animationTimer = new QTimer(this);
    m_animationTimer->setInterval(50); // 20 FPS for smooth animation
    connect(m_animationTimer, &QTimer::timeout, this, &CNCToolsCanvas::onAnimationFrame);

    // Initialize material database
    initializeMaterialDatabase();

    // Load tools from database
    loadToolsFromDatabase();

    // Load default layout
    restoreLayout("cnc_tools_default");
}

void CNCToolsCanvas::setupDefaultLayout()
{
    // Create main horizontal splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->setChildrenCollapsible(false);

    // Create left panel (tool library)
    createToolLibrary();

    // Create center panel (feed/speed calculator)
    createFeedSpeedCalculator();

    // Create right panel (tool visualization and wear tracking)
    createToolVisualization();
    createWearTracking();

    // Add panels to splitter
    m_mainSplitter->addWidget(createLeftPanel());
    m_mainSplitter->addWidget(createCenterPanel());
    m_mainSplitter->addWidget(createRightPanel());

    // Set splitter proportions (30% : 40% : 30%)
    m_mainSplitter->setStretchFactor(0, 30);
    m_mainSplitter->setStretchFactor(1, 40);
    m_mainSplitter->setStretchFactor(2, 30);

    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_mainSplitter);

    setLayout(mainLayout);
}

QWidget* CNCToolsCanvas::createLeftPanel()
{
    QWidget* leftPanel = new QWidget();
    leftPanel->setMinimumWidth(300);
    leftPanel->setMaximumWidth(400);

    QVBoxLayout* layout = new QVBoxLayout(leftPanel);

    layout->addWidget(createToolLibraryWidget());
    layout->addWidget(createMaterialDatabaseWidget());

    return leftPanel;
}

QWidget* CNCToolsCanvas::createCenterPanel()
{
    QWidget* centerPanel = new QWidget();
    centerPanel->setMinimumWidth(400);

    QVBoxLayout* layout = new QVBoxLayout(centerPanel);

    layout->addWidget(createFeedSpeedCalculatorWidget());

    return centerPanel;
}

QWidget* CNCToolsCanvas::createRightPanel()
{
    QWidget* rightPanel = new QWidget();
    rightPanel->setMinimumWidth(250);
    rightPanel->setMaximumWidth(350);

    QVBoxLayout* layout = new QVBoxLayout(rightPanel);

    layout->addWidget(createToolVisualizationWidget());
    layout->addWidget(createWearTrackingWidget());
    layout->addStretch();

    return rightPanel;
}

void CNCToolsCanvas::createToolLibrary()
{
    m_toolTree = new QTreeWidget();
    m_toolTree->setHeaderLabel("Tool Library");
    m_toolTree->setRootIsDecorated(true);

    // Create tool type categories
    QStringList toolTypes = {"End Mills", "Drills", "Routers", "Custom Tools"};

    for (const QString& type : toolTypes) {
        QTreeWidgetItem* categoryItem = new QTreeWidgetItem(m_toolTree);
        categoryItem->setText(0, type);
        categoryItem->setData(0, Qt::UserRole, "category");

        // Add sample tools for each category
        for (int i = 0; i < 3; ++i) {
            QTreeWidgetItem* toolItem = new QTreeWidgetItem(categoryItem);
            toolItem->setText(0, QString("%1 Tool %2").arg(type).arg(i + 1));
            toolItem->setData(0, Qt::UserRole, QString("tool_%1_%2").arg(type.toLower()).arg(i + 1));
        }
    }

    m_toolTree->expandAll();

    connect(m_toolTree, &QTreeWidget::itemSelectionChanged, this, &CNCToolsCanvas::onToolSelectionChanged);
}

void CNCToolsCanvas::createMaterialDatabase()
{
    m_materialCombo = new QComboBox();
    m_materialCombo->addItem("Aluminum 6061", "aluminum_6061");
    m_materialCombo->addItem("Steel 1018", "steel_1018");
    m_materialCombo->addItem("Plywood (Birch)", "plywood_birch");
    m_materialCombo->addItem("Acrylic", "acrylic");
    m_materialCombo->addItem("Brass", "brass");

    connect(m_materialCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &CNCToolsCanvas::onMaterialSelectionChanged);
}

void CNCToolsCanvas::createFeedSpeedCalculator()
{
    m_calculatorGroup = new QGroupBox("Feed/Speed Calculator");

    QFormLayout* formLayout = new QFormLayout(m_calculatorGroup);

    // Tool selection
    m_toolTypeCombo = new QComboBox();
    m_toolTypeCombo->addItem("End Mill", "endmill");
    m_toolTypeCombo->addItem("Drill", "drill");
    m_toolTypeCombo->addItem("Router", "router");
    formLayout->addRow("Tool Type:", m_toolTypeCombo);

    // Material selection
    formLayout->addRow("Material:", m_materialCombo);

    // Cutting parameters
    m_feedRateEdit = new QLineEdit("10.0");
    m_feedRateEdit->setSuffix(" IPM");
    formLayout->addRow("Feed Rate:", m_feedRateEdit);

    m_spindleSpeedEdit = new QLineEdit("12000");
    m_spindleSpeedEdit->setSuffix(" RPM");
    formLayout->addRow("Spindle Speed:", m_spindleSpeedEdit);

    m_cuttingDepthEdit = new QLineEdit("0.125");
    m_cuttingDepthEdit->setSuffix(" in");
    formLayout->addRow("Cut Depth:", m_cuttingDepthEdit);

    // Calculator buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_calculateButton = new QPushButton("Calculate");
    connect(m_calculateButton, &QPushButton::clicked, this, &CNCToolsCanvas::onCalculateFeedSpeedClicked);
    buttonLayout->addWidget(m_calculateButton);

    m_optimizeButton = new QPushButton("Optimize");
    connect(m_optimizeButton, &QPushButton::clicked, this, &CNCToolsCanvas::onOptimizeParametersClicked);
    buttonLayout->addWidget(m_optimizeButton);

    formLayout->addRow(buttonLayout);

    // Results display
    QLabel* resultsLabel = new QLabel("Results will appear here");
    resultsLabel->setObjectName("resultsLabel");
    resultsLabel->setWordWrap(true);
    resultsLabel->setStyleSheet("padding: 10px; background-color: #f0f0f0; border: 1px solid #ccc;");
    formLayout->addRow(resultsLabel);

    connect(m_toolTypeCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &CNCToolsCanvas::onToolChanged);
    connect(m_materialCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &CNCToolsCanvas::onMaterialChanged);
}

void CNCToolsCanvas::createToolVisualization()
{
    m_visualizationWidget = new QWidget();
    m_visualizationWidget->setMinimumHeight(200);
    m_visualizationWidget->setStyleSheet("background-color: #1a1a1a; border: 1px solid #333;");

    QVBoxLayout* layout = new QVBoxLayout(m_visualizationWidget);

    QLabel* visLabel = new QLabel("Tool Visualization");
    visLabel->setAlignment(Qt::AlignCenter);
    visLabel->setStyleSheet("color: white; font-weight: bold;");
    layout->addWidget(visLabel);

    layout->addStretch();
}

void CNCToolsCanvas::createWearTracking()
{
    QGroupBox* wearGroup = new QGroupBox("Tool Wear Tracking");

    QVBoxLayout* layout = new QVBoxLayout(wearGroup);

    m_toolWearBar = new QProgressBar();
    m_toolWearBar->setRange(0, 100);
    m_toolWearBar->setValue(25);
    m_toolWearBar->setFormat("Wear: %p%");
    layout->addWidget(m_toolWearBar);

    m_toolLifeLabel = new QLabel("Estimated Life Remaining: 75%");
    layout->addWidget(m_toolLifeLabel);

    QHBoxLayout* wearButtonLayout = new QHBoxLayout();

    m_updateWearButton = new QPushButton("Update Wear");
    connect(m_updateWearButton, &QPushButton::clicked, this, &CNCToolsCanvas::onUpdateWearClicked);
    wearButtonLayout->addWidget(m_updateWearButton);

    QPushButton* resetWearButton = new QPushButton("Reset");
    connect(resetWearButton, &QPushButton::clicked, this, &CNCToolsCanvas::onResetWearClicked);
    wearButtonLayout->addWidget(resetWearButton);

    layout->addLayout(wearButtonLayout);
}

QWidget* CNCToolsCanvas::createToolLibraryWidget()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    // Tool tree
    layout->addWidget(m_toolTree);

    // Tool management buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    QPushButton* addButton = new QPushButton("Add Tool");
    connect(addButton, &QPushButton::clicked, this, &CNCToolsCanvas::onAddToolClicked);
    buttonLayout->addWidget(addButton);

    QPushButton* editButton = new QPushButton("Edit");
    connect(editButton, &QPushButton::clicked, this, &CNCToolsCanvas::onEditToolClicked);
    buttonLayout->addWidget(editButton);

    QPushButton* deleteButton = new QPushButton("Delete");
    connect(deleteButton, &QPushButton::clicked, this, &CNCToolsCanvas::onDeleteToolClicked);
    buttonLayout->addWidget(deleteButton);

    layout->addLayout(buttonLayout);

    // Import/Export buttons
    QHBoxLayout* ioLayout = new QHBoxLayout();

    QPushButton* importButton = new QPushButton("Import");
    connect(importButton, &QPushButton::clicked, this, &CNCToolsCanvas::onImportToolsClicked);
    ioLayout->addWidget(importButton);

    QPushButton* exportButton = new QPushButton("Export");
    connect(exportButton, &QPushButton::clicked, this, &CNCToolsCanvas::onExportToolsClicked);
    ioLayout->addWidget(exportButton);

    layout->addLayout(ioLayout);

    return widget;
}

QWidget* CNCToolsCanvas::createMaterialDatabaseWidget()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QGroupBox* materialGroup = new QGroupBox("Material Database");
    QVBoxLayout* materialLayout = new QVBoxLayout(materialGroup);

    materialLayout->addWidget(m_materialCombo);

    QHBoxLayout* matButtonLayout = new QHBoxLayout();

    QPushButton* addMatButton = new QPushButton("Add");
    connect(addMatButton, &QPushButton::clicked, this, &CNCToolsCanvas::onAddMaterialClicked);
    matButtonLayout->addWidget(addMatButton);

    QPushButton* editMatButton = new QPushButton("Edit");
    connect(editMatButton, &QPushButton::clicked, this, &CNCToolsCanvas::onEditMaterialClicked);
    matButtonLayout->addWidget(editMatButton);

    materialLayout->addLayout(matButtonLayout);
    layout->addWidget(materialGroup);

    return widget;
}

QWidget* CNCToolsCanvas::createFeedSpeedCalculatorWidget()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    layout->addWidget(m_calculatorGroup);

    return widget;
}

QWidget* CNCToolsCanvas::createToolVisualizationWidget()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    layout->addWidget(m_visualizationWidget);

    return widget;
}

QWidget* CNCToolsCanvas::createWearTrackingWidget()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);

    layout->addWidget(createWearTracking());

    return widget;
}

// BaseCanvas interface implementation
void CNCToolsCanvas::addWidget(BaseWidget* widget, DockArea area)
{
    // Widget addition implementation
}

void CNCToolsCanvas::removeWidget(BaseWidget* widget)
{
    // Widget removal implementation
}

void CNCToolsCanvas::removeWidget(const QString& widgetName)
{
    // Widget removal by name implementation
}

QList<BaseWidget*> CNCToolsCanvas::getWidgets() const
{
    return QList<BaseWidget*>();
}

QList<BaseWidget*> CNCToolsCanvas::getWidgetsByCategory(WidgetCategory category) const
{
    return QList<BaseWidget*>();
}

BaseWidget* CNCToolsCanvas::getWidget(const QString& widgetName) const
{
    return nullptr;
}

void CNCToolsCanvas::saveLayout(const QString& name)
{
    // Layout saving implementation
}

void CNCToolsCanvas::restoreLayout(const QString& name)
{
    // Layout restoration implementation
}

QStringList CNCToolsCanvas::getAvailableLayouts() const
{
    return QStringList() << "cnc_tools_default" << "cnc_tools_compact" << "cnc_tools_detailed";
}

void CNCToolsCanvas::resetLayout()
{
    restoreLayout("cnc_tools_default");
}

void CNCToolsCanvas::saveState(QJsonObject& state) const
{
    state["selected_tool"] = m_selectedToolId;
    state["selected_material"] = m_selectedMaterial;
    state["feed_rate"] = m_feedRateEdit->text().toDouble();
    state["spindle_speed"] = m_spindleSpeedEdit->text().toDouble();
    state["cutting_depth"] = m_cuttingDepthEdit->text().toDouble();
}

void CNCToolsCanvas::restoreState(const QJsonObject& state)
{
    m_selectedToolId = state["selected_tool"].toString();
    m_selectedMaterial = state["selected_material"].toString();

    if (m_feedRateEdit) {
        m_feedRateEdit->setText(QString::number(state["feed_rate"].toDouble(), 'f', 2));
    }
    if (m_spindleSpeedEdit) {
        m_spindleSpeedEdit->setText(QString::number(state["spindle_speed"].toInt()));
    }
    if (m_cuttingDepthEdit) {
        m_cuttingDepthEdit->setText(QString::number(state["cutting_depth"].toDouble(), 'f', 3));
    }
}

// CNCToolsCanvas specific methods
void CNCToolsCanvas::importToolsFile(const QString& filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Import Failed", "Cannot read file: " + filepath);
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        QMessageBox::critical(this, "Import Failed", "Invalid JSON file: " + filepath);
        return;
    }

    QJsonArray toolsArray = doc.array();
    int importedCount = 0;

    for (const QJsonValue& value : toolsArray) {
        QJsonObject toolObj = value.toObject();

        ToolData tool;
        tool.id = toolObj["id"].toString();
        tool.name = toolObj["name"].toString();
        tool.type = toolObj["type"].toString();
        tool.manufacturer = toolObj["manufacturer"].toString();
        tool.diameter = toolObj["diameter"].toDouble();
        tool.fluteLength = toolObj["flute_length"].toDouble();
        tool.overallLength = toolObj["overall_length"].toDouble();
        tool.shankDiameter = toolObj["shank_diameter"].toDouble();
        tool.fluteCount = toolObj["flute_count"].toInt();
        tool.helixAngle = toolObj["helix_angle"].toDouble();
        tool.material = toolObj["material"].toString();

        // Import cutting parameters
        if (toolObj.contains("cutting_parameters")) {
            tool.cuttingParameters = toolObj["cutting_parameters"].toObject().toVariantMap();
        }

        m_tools.append(tool);
        importedCount++;
    }

    file.close();

    // Update UI
    updateToolTree();
    statusBar()->showMessage(QString("Imported %1 tools from: %2").arg(importedCount).arg(filepath));
}

void CNCToolsCanvas::exportToolsFile(const QString& filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Export Failed", "Cannot write to file: " + filepath);
        return;
    }

    QJsonArray toolsArray;

    for (const ToolData& tool : m_tools) {
        QJsonObject toolObj;
        toolObj["id"] = tool.id;
        toolObj["name"] = tool.name;
        toolObj["type"] = tool.type;
        toolObj["manufacturer"] = tool.manufacturer;
        toolObj["diameter"] = tool.diameter;
        toolObj["flute_length"] = tool.fluteLength;
        toolObj["overall_length"] = tool.overallLength;
        toolObj["shank_diameter"] = tool.shankDiameter;
        toolObj["flute_count"] = tool.fluteCount;
        toolObj["helix_angle"] = tool.helixAngle;
        toolObj["material"] = tool.material;
        toolObj["cutting_parameters"] = QJsonObject::fromVariantMap(tool.cuttingParameters);

        toolsArray.append(toolObj);
    }

    QJsonDocument doc(toolsArray);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    statusBar()->showMessage(QString("Exported %1 tools to: %2").arg(m_tools.size()).arg(filepath));
}

void CNCToolsCanvas::addTool(const QString& toolType)
{
    ToolData tool;
    tool.id = QUuid::createUuid().toString();
    tool.name = QString("New %1").arg(toolType);
    tool.type = toolType.toLower();

    // Set default values based on tool type
    if (toolType == "End Mill") {
        tool.diameter = 0.25f;
        tool.fluteLength = 0.75f;
        tool.overallLength = 2.0f;
        tool.shankDiameter = 0.25f;
        tool.fluteCount = 2;
        tool.helixAngle = 30.0f;
        tool.material = "Carbide";
    } else if (toolType == "Drill") {
        tool.diameter = 0.125f;
        tool.fluteLength = 1.0f;
        tool.overallLength = 2.5f;
        tool.shankDiameter = 0.125f;
        tool.fluteCount = 2;
        tool.helixAngle = 30.0f;
        tool.material = "HSS";
    }

    m_tools.append(tool);
    updateToolTree();

    // Edit the newly added tool
    editTool(tool.id);
}

void CNCToolsCanvas::editTool(const QString& toolId)
{
    // Find tool
    ToolData* tool = nullptr;
    for (ToolData& t : m_tools) {
        if (t.id == toolId) {
            tool = &t;
            break;
        }
    }

    if (!tool) {
        return;
    }

    // Create edit dialog
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Tool");
    dialog.resize(400, 300);

    QFormLayout* form = new QFormLayout(&dialog);

    QLineEdit* nameEdit = new QLineEdit(tool->name);
    form->addRow("Name:", nameEdit);

    QLineEdit* manufacturerEdit = new QLineEdit(tool->manufacturer);
    form->addRow("Manufacturer:", manufacturerEdit);

    QDoubleSpinBox* diameterSpin = new QDoubleSpinBox();
    diameterSpin->setRange(0.001, 10.0);
    diameterSpin->setValue(tool->diameter);
    diameterSpin->setSuffix(" in");
    form->addRow("Diameter:", diameterSpin);

    QDoubleSpinBox* fluteLengthSpin = new QDoubleSpinBox();
    fluteLengthSpin->setRange(0.001, 20.0);
    fluteLengthSpin->setValue(tool->fluteLength);
    fluteLengthSpin->setSuffix(" in");
    form->addRow("Flute Length:", fluteLengthSpin);

    QSpinBox* fluteCountSpin = new QSpinBox();
    fluteCountSpin->setRange(1, 8);
    fluteCountSpin->setValue(tool->fluteCount);
    form->addRow("Flute Count:", fluteCountSpin);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    QPushButton* okButton = new QPushButton("OK");
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    buttonLayout->addWidget(okButton);

    QPushButton* cancelButton = new QPushButton("Cancel");
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);

    form->addRow(buttonLayout);

    if (dialog.exec() == QDialog::Accepted) {
        // Update tool data
        tool->name = nameEdit->text();
        tool->manufacturer = manufacturerEdit->text();
        tool->diameter = diameterSpin->value();
        tool->fluteLength = fluteLengthSpin->value();
        tool->fluteCount = fluteCountSpin->value();

        updateToolTree();
        saveToolsToDatabase();
    }
}

void CNCToolsCanvas::deleteTool(const QString& toolId)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Delete Tool",
        "Are you sure you want to delete this tool?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        for (int i = 0; i < m_tools.size(); ++i) {
            if (m_tools[i].id == toolId) {
                m_tools.removeAt(i);
                break;
            }
        }

        updateToolTree();
        saveToolsToDatabase();
    }
}

void CNCToolsCanvas::calculateFeedSpeed(const QString& toolId, const QString& material)
{
    // Use advanced FeedSpeedCalculator if available
    if (m_feedSpeedCalculator) {
        QVariantMap operationParams;
        operationParams["depth_of_cut"] = 0.125f;
        operationParams["width_of_cut"] = 0.25f;

        m_feedSpeedCalculator->calculateOptimalParameters(toolId, material, operationParams);
    } else {
        // Fallback to basic calculation
        basicFeedSpeedCalculation(toolId, material);
    }
}

void CNCToolsCanvas::basicFeedSpeedCalculation(const QString& toolId, const QString& material)
{
    // Get tool data
    ToolData* tool = nullptr;
    for (ToolData& t : m_tools) {
        if (t.id == toolId) {
            tool = &t;
            break;
        }
    }

    if (!tool) {
        return;
    }

    // Get material data
    MaterialData* mat = nullptr;
    for (MaterialData& m : m_materials) {
        if (m.name == material) {
            mat = &m;
            break;
        }
    }

    if (!mat) {
        return;
    }

    // Get cutting parameters for this material
    QVariantMap toolParams = tool->cuttingParameters.value(material).toMap();
    QVariantMap materialParams = mat->machiningParameters;

    // Calculate feed rate and spindle speed
    float diameter = tool->diameter;
    float sfm = materialParams.value("sfm", 100.0f).toFloat(); // Surface feet per minute
    float chipload = toolParams.value("chipload", 0.001f).toFloat(); // Chip load per tooth

    // Calculate spindle speed (RPM)
    m_calculatedSpindleSpeed = (sfm * 3.82f) / diameter;

    // Calculate feed rate (IPM)
    m_calculatedFeedRate = m_calculatedSpindleSpeed * chipload * tool->fluteCount;

    // Apply safety margins
    applySafetyMargins(m_calculatedFeedRate, m_calculatedSpindleSpeed, 0.8f);

    // Update UI
    m_feedRateEdit->setText(QString::number(m_calculatedFeedRate, 'f', 2));
    m_spindleSpeedEdit->setText(QString::number(static_cast<int>(m_calculatedSpindleSpeed)));

    // Update results display
    updateCalculationResults();
}

void CNCToolsCanvas::optimizeMachiningParameters()
{
    // Advanced optimization algorithm
    float bestEfficiency = 0.0f;
    float bestFeedRate = m_calculatedFeedRate;
    float bestSpindleSpeed = m_calculatedSpindleSpeed;

    // Try different parameter combinations
    for (float feedFactor = 0.5f; feedFactor <= 1.5f; feedFactor += 0.1f) {
        for (float speedFactor = 0.7f; speedFactor <= 1.3f; speedFactor += 0.1f) {
            float testFeed = m_calculatedFeedRate * feedFactor;
            float testSpeed = m_calculatedSpindleSpeed * speedFactor;

            // Calculate efficiency metric (simplified)
            float efficiency = calculateMachiningEfficiency(testFeed, testSpeed);

            if (efficiency > bestEfficiency) {
                bestEfficiency = efficiency;
                bestFeedRate = testFeed;
                bestSpindleSpeed = testSpeed;
            }
        }
    }

    m_calculatedFeedRate = bestFeedRate;
    m_calculatedSpindleSpeed = bestSpindleSpeed;

    // Update UI
    m_feedRateEdit->setText(QString::number(m_calculatedFeedRate, 'f', 2));
    m_spindleSpeedEdit->setText(QString::number(static_cast<int>(m_calculatedSpindleSpeed)));

    updateCalculationResults();
}

void CNCToolsCanvas::generateMachiningReport()
{
    QString report = "MACHINING PARAMETERS REPORT\n";
    report += "==========================\n\n";

    report += QString("Tool: %1\n").arg(m_selectedToolId);
    report += QString("Material: %1\n").arg(m_selectedMaterial);
    report += QString("Feed Rate: %1 IPM\n").arg(m_calculatedFeedRate, 0, 'f', 2);
    report += QString("Spindle Speed: %1 RPM\n").arg(static_cast<int>(m_calculatedSpindleSpeed));
    report += QString("Cutting Time: %1 minutes\n").arg(m_calculatedCuttingTime, 0, 'f', 1);

    // Show report in message box for now
    QMessageBox::information(this, "Machining Report", report);
}

void CNCToolsCanvas::updateToolWear(const QString& toolId, float wearAmount)
{
    for (ToolData& tool : m_tools) {
        if (tool.id == toolId) {
            tool.totalUsageTime += wearAmount;
            tool.remainingLife = qMax(0.0f, tool.remainingLife - wearAmount);

            // Update wear display
            if (toolId == m_selectedToolId) {
                m_toolWearBar->setValue(static_cast<int>((1.0f - tool.remainingLife / 100.0f) * 100.0f));
                m_toolLifeLabel->setText(QString("Estimated Life Remaining: %1%").arg(tool.remainingLife, 0, 'f', 1));
            }

            break;
        }
    }

    saveToolsToDatabase();
}

void CNCToolsCanvas::scheduleToolReplacement(const QString& toolId)
{
    // Find tool and schedule replacement
    for (ToolData& tool : m_tools) {
        if (tool.id == toolId) {
            // Calculate replacement date based on usage
            QDateTime replacementDate = QDateTime::currentDateTime().addDays(30); // Simplified

            qInfo() << QString("Tool %1 scheduled for replacement on %2")
                      .arg(tool.name).arg(replacementDate.toString());

            break;
        }
    }
}

void CNCToolsCanvas::trackToolUsage(const QString& toolId, float usageTime)
{
    updateToolWear(toolId, usageTime);

    // Check if tool needs replacement
    for (const ToolData& tool : m_tools) {
        if (tool.id == toolId && tool.remainingLife < 10.0f) {
            scheduleToolReplacement(toolId);
        }
    }
}

void CNCToolsCanvas::addMaterial(const QString& name, const QVariantMap& properties)
{
    MaterialData material(name);
    material.category = properties.value("category", "metal").toString();
    material.properties = properties;

    // Set default machining parameters
    QVariantMap machiningParams;
    machiningParams["sfm"] = properties.value("sfm", 100.0f);
    machiningParams["chipload"] = properties.value("chipload", 0.001f);
    machiningParams["hardness"] = properties.value("hardness", 100.0f);
    material.machiningParameters = machiningParams;

    m_materials.append(material);

    // Update material combo box
    m_materialCombo->addItem(name, name.toLower().replace(" ", "_"));
}

void CNCToolsCanvas::updateMaterialProperties(const QString& name, const QVariantMap& properties)
{
    for (MaterialData& material : m_materials) {
        if (material.name == name) {
            material.properties = properties;
            break;
        }
    }
}

void CNCToolsCanvas::removeMaterial(const QString& name)
{
    for (int i = 0; i < m_materials.size(); ++i) {
        if (m_materials[i].name == name) {
            m_materials.removeAt(i);
            break;
        }
    }

    // Update material combo box
    int index = m_materialCombo->findText(name);
    if (index >= 0) {
        m_materialCombo->removeItem(index);
    }
}

// Protected methods
void CNCToolsCanvas::loadToolsFromDatabase()
{
    // Load tools from database
    // For now, create sample tools
    createSampleTools();
    updateToolTree();
}

void CNCToolsCanvas::saveToolsToDatabase()
{
    // Save tools to database
    qDebug() << "Saving tools to database";
}

void CNCToolsCanvas::validateToolData(const QVariantMap& toolData)
{
    // Validate tool data before saving
    QStringList requiredFields = {"name", "type", "diameter"};

    for (const QString& field : requiredFields) {
        if (!toolData.contains(field)) {
            throw std::runtime_error(QString("Missing required field: %1").arg(field).toStdString());
        }
    }
}

void CNCToolsCanvas::calculateOptimalParameters(const QString& toolId, const QString& material)
{
    // Advanced parameter optimization
    calculateFeedSpeed(toolId, material);

    // Apply material-specific optimizations
    if (material.contains("aluminum")) {
        // Aluminum can handle higher speeds
        m_calculatedSpindleSpeed *= 1.2f;
        m_calculatedFeedRate *= 1.1f;
    } else if (material.contains("steel")) {
        // Steel needs more conservative parameters
        m_calculatedSpindleSpeed *= 0.8f;
        m_calculatedFeedRate *= 0.9f;
    }

    updateCalculationResults();
}

void CNCToolsCanvas::applySafetyMargins(float& feedRate, float& speed, float safetyFactor)
{
    feedRate *= safetyFactor;
    speed *= safetyFactor;
}

void CNCToolsCanvas::renderToolGeometry(const QString& toolId)
{
    // Render 3D tool geometry in visualization widget
    // This would use OpenGL for 3D tool rendering
}

void CNCToolsCanvas::renderCuttingSimulation()
{
    // Render cutting simulation animation
    // This would show material removal simulation
}

void CNCToolsCanvas::initializeMaterialDatabase()
{
    // Initialize with common CNC materials
    QVariantMap aluminumProps;
    aluminumProps["category"] = "metal";
    aluminumProps["sfm"] = 800.0f;
    aluminumProps["chipload"] = 0.002f;
    aluminumProps["hardness"] = 95.0f;
    addMaterial("Aluminum 6061", aluminumProps);

    QVariantMap steelProps;
    steelProps["category"] = "metal";
    steelProps["sfm"] = 200.0f;
    steelProps["chipload"] = 0.001f;
    steelProps["hardness"] = 150.0f;
    addMaterial("Steel 1018", steelProps);

    QVariantMap plywoodProps;
    plywoodProps["category"] = "wood";
    plywoodProps["sfm"] = 1000.0f;
    plywoodProps["chipload"] = 0.005f;
    plywoodProps["hardness"] = 50.0f;
    addMaterial("Plywood (Birch)", plywoodProps);

    QVariantMap acrylicProps;
    acrylicProps["category"] = "plastic";
    acrylicProps["sfm"] = 400.0f;
    acrylicProps["chipload"] = 0.003f;
    acrylicProps["hardness"] = 75.0f;
    addMaterial("Acrylic", acrylicProps);
}

void CNCToolsCanvas::createSampleTools()
{
    // Create sample tools for demonstration
    ToolData endmill1;
    endmill1.id = "endmill_1";
    endmill1.name = "1/4\" End Mill";
    endmill1.type = "endmill";
    endmill1.manufacturer = "Generic";
    endmill1.diameter = 0.25f;
    endmill1.fluteLength = 0.75f;
    endmill1.overallLength = 2.0f;
    endmill1.shankDiameter = 0.25f;
    endmill1.fluteCount = 2;
    endmill1.helixAngle = 30.0f;
    endmill1.material = "Carbide";

    QVariantMap endmillParams;
    endmillParams["aluminum_6061_chipload"] = 0.002f;
    endmillParams["steel_1018_chipload"] = 0.001f;
    endmillParams["plywood_birch_chipload"] = 0.005f;
    endmill1.cuttingParameters = endmillParams;

    m_tools.append(endmill1);

    ToolData drill1;
    drill1.id = "drill_1";
    drill1.name = "1/8\" Drill";
    drill1.type = "drill";
    drill1.manufacturer = "Generic";
    drill1.diameter = 0.125f;
    drill1.fluteLength = 1.0f;
    drill1.overallLength = 2.5f;
    drill1.shankDiameter = 0.125f;
    drill1.fluteCount = 2;
    drill1.helixAngle = 30.0f;
    drill1.material = "HSS";

    m_tools.append(drill1);
}

void CNCToolsCanvas::updateToolTree()
{
    m_toolTree->clear();

    // Group tools by type
    QMap<QString, QList<ToolData>> toolsByType;

    for (const ToolData& tool : m_tools) {
        toolsByType[tool.type].append(tool);
    }

    for (auto it = toolsByType.begin(); it != toolsByType.end(); ++it) {
        QString typeName = it.key();
        typeName[0] = typeName[0].toUpper(); // Capitalize

        QTreeWidgetItem* categoryItem = new QTreeWidgetItem(m_toolTree);
        categoryItem->setText(0, typeName);
        categoryItem->setData(0, Qt::UserRole, "category");

        for (const ToolData& tool : it.value()) {
            QTreeWidgetItem* toolItem = new QTreeWidgetItem(categoryItem);
            toolItem->setText(0, tool.name);
            toolItem->setData(0, Qt::UserRole, tool.id);
        }
    }

    m_toolTree->expandAll();
}

void CNCToolsCanvas::updateCalculationResults()
{
    QString results = QString("CALCULATION RESULTS:\n\n");
    results += QString("Feed Rate: %1 IPM\n").arg(m_calculatedFeedRate, 0, 'f', 2);
    results += QString("Spindle Speed: %1 RPM\n").arg(static_cast<int>(m_calculatedSpindleSpeed));
    results += QString("Material Removal Rate: %1 cu in/min\n")
               .arg(m_calculatedFeedRate * m_cuttingDepthEdit->text().toDouble(), 0, 'f', 2);

    QLabel* resultsLabel = m_calculatorGroup->findChild<QLabel*>("resultsLabel");
    if (resultsLabel) {
        resultsLabel->setText(results);
    }
}

float CNCToolsCanvas::calculateMachiningEfficiency(float feedRate, float spindleSpeed)
{
    // Simplified efficiency calculation
    // In practice, this would consider material properties, tool life, surface finish, etc.
    return (feedRate * spindleSpeed) / 1000000.0f;
}

// Event handlers
void CNCToolsCanvas::onToolSelectionChanged()
{
    QTreeWidgetItem* currentItem = m_toolTree->currentItem();
    if (!currentItem || currentItem->data(0, Qt::UserRole).toString() == "category") {
        return;
    }

    m_selectedToolId = currentItem->data(0, Qt::UserRole).toString();

    // Update tool details display
    for (const ToolData& tool : m_tools) {
        if (tool.id == m_selectedToolId) {
            // Update wear display
            m_toolWearBar->setValue(static_cast<int>((1.0f - tool.remainingLife / 100.0f) * 100.0f));
            m_toolLifeLabel->setText(QString("Estimated Life Remaining: %1%").arg(tool.remainingLife, 0, 'f', 1));

            // Start tool visualization animation
            if (!m_animationTimer->isActive()) {
                m_animationTimer->start();
            }

            break;
        }
    }
}

void CNCToolsCanvas::onAddToolClicked()
{
    QStringList toolTypes = {"End Mill", "Drill", "Router", "Custom"};
    QString toolType = QInputDialog::getItem(this, "Add Tool", "Select tool type:", toolTypes, 0, false);

    if (!toolType.isEmpty()) {
        addTool(toolType);
    }
}

void CNCToolsCanvas::onEditToolClicked()
{
    if (!m_selectedToolId.isEmpty()) {
        editTool(m_selectedToolId);
    }
}

void CNCToolsCanvas::onDeleteToolClicked()
{
    if (!m_selectedToolId.isEmpty()) {
        deleteTool(m_selectedToolId);
    }
}

void CNCToolsCanvas::onImportToolsClicked()
{
    QString filepath = QFileDialog::getOpenFileName(this,
        "Import Tools",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Tools files (*.tools.json)");

    if (!filepath.isEmpty()) {
        importToolsFile(filepath);
    }
}

void CNCToolsCanvas::onExportToolsClicked()
{
    QString filepath = QFileDialog::getSaveFileName(this,
        "Export Tools",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Tools files (*.tools.json)");

    if (!filepath.isEmpty()) {
        exportToolsFile(filepath);
    }
}

void CNCToolsCanvas::onMaterialSelectionChanged()
{
    m_selectedMaterial = m_materialCombo->currentData().toString();

    // Recalculate if we have a selected tool
    if (!m_selectedToolId.isEmpty()) {
        calculateFeedSpeed(m_selectedToolId, m_selectedMaterial);
    }
}

void CNCToolsCanvas::onAddMaterialClicked()
{
    QString name = QInputDialog::getText(this, "Add Material", "Material name:");
    if (!name.isEmpty()) {
        QVariantMap properties;
        properties["category"] = "custom";
        properties["sfm"] = 300.0f;
        properties["chipload"] = 0.002f;
        properties["hardness"] = 100.0f;

        addMaterial(name, properties);
    }
}

void CNCToolsCanvas::onEditMaterialClicked()
{
    QString currentMaterial = m_materialCombo->currentText();
    if (!currentMaterial.isEmpty()) {
        // Edit material properties
        qDebug() << "Edit material:" << currentMaterial;
    }
}

void CNCToolsCanvas::onCalculateFeedSpeedClicked()
{
    if (!m_selectedToolId.isEmpty()) {
        calculateFeedSpeed(m_selectedToolId, m_selectedMaterial);
    }
}

void CNCToolsCanvas::onOptimizeParametersClicked()
{
    if (!m_selectedToolId.isEmpty()) {
        optimizeMachiningParameters();
    }
}

void CNCToolsCanvas::onMaterialChanged(const QString& material)
{
    m_selectedMaterial = m_materialCombo->currentData().toString();

    if (!m_selectedToolId.isEmpty()) {
        calculateFeedSpeed(m_selectedToolId, m_selectedMaterial);
    }
}

void CNCToolsCanvas::onToolChanged(const QString& tool)
{
    // Handle tool type change
    qDebug() << "Tool type changed to:" << tool;
}

void CNCToolsCanvas::onUpdateWearClicked()
{
    if (!m_selectedToolId.isEmpty()) {
        float wearAmount = QInputDialog::getDouble(this, "Update Wear",
                                                 "Wear amount (hours):", 1.0, 0.1, 100.0, 1);

        if (wearAmount > 0) {
            updateToolWear(m_selectedToolId, wearAmount);
        }
    }
}

void CNCToolsCanvas::onResetWearClicked()
{
    if (!m_selectedToolId.isEmpty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
            "Reset Wear",
            "Reset tool wear to 100%?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            for (ToolData& tool : m_tools) {
                if (tool.id == m_selectedToolId) {
                    tool.totalUsageTime = 0.0f;
                    tool.remainingLife = 100.0f;

                    m_toolWearBar->setValue(0);
                    m_toolLifeLabel->setText("Estimated Life Remaining: 100%");
                    break;
                }
            }

            saveToolsToDatabase();
        }
    }
}

void CNCToolsCanvas::onToolUsageRecorded(float usageTime)
{
    if (!m_selectedToolId.isEmpty()) {
        trackToolUsage(m_selectedToolId, usageTime);
    }
}

void CNCToolsCanvas::onAnimationFrame()
{
    // Update tool visualization animation
    if (!m_selectedToolId.isEmpty()) {
        renderToolGeometry(m_selectedToolId);
    }
}

void CNCToolsCanvas::onAdvancedCalculationCompleted(const FeedSpeedCalculator::CalculationResult& result)
{
    // Update UI with advanced calculation results
    m_calculatedFeedRate = result.optimal.feedRate;
    m_calculatedSpindleSpeed = result.optimal.spindleSpeed;

    // Update feed rate display
    m_feedRateEdit->setText(QString::number(result.optimal.feedRate, 'f', 2));

    // Update spindle speed display
    m_spindleSpeedEdit->setText(QString::number(static_cast<int>(result.optimal.spindleSpeed)));

    // Update results display with advanced information
    QString results = QString("ADVANCED CALCULATION RESULTS:\n\n");
    results += QString("Optimal Feed Rate: %1 IPM\n").arg(result.optimal.feedRate, 0, 'f', 2);
    results += QString("Optimal Spindle Speed: %1 RPM\n").arg(static_cast<int>(result.optimal.spindleSpeed));
    results += QString("Material Removal Rate: %1 cu in/min\n").arg(result.optimal.materialRemovalRate, 0, 'f', 2);
    results += QString("Cutting Force: %1 lbs\n").arg(result.optimal.cuttingForce, 0, 'f', 1);
    results += QString("Required Power: %1 HP\n").arg(result.optimal.spindlePower, 0, 'f', 1);
    results += QString("\nEfficiency: %1%\n").arg(result.efficiency, 0, 'f', 1);
    results += QString("Safety Margin: %1%\n").arg(result.safetyMargin * 100.0f, 0, 'f', 1);

    if (!result.warnings.isEmpty()) {
        results += QString("\nWarnings:\n%1").arg(result.warnings);
    }

    if (!result.recommendations.isEmpty()) {
        results += QString("\nRecommendations:\n%1").arg(result.recommendations);
    }

    QLabel* resultsLabel = m_calculatorGroup->findChild<QLabel*>("resultsLabel");
    if (resultsLabel) {
        resultsLabel->setText(results);
    }

    statusBar()->showMessage(QString("Advanced calculation completed - Efficiency: %1%").arg(result.efficiency, 0, 'f', 1));
}