#include "ProjectCanvas.h"
#include "../core/ModelService.h"
#include "../core/SearchService.h"
#include "../core/TagManager.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTreeWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QProgressBar>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

ProjectCanvas::ProjectCanvas(QWidget* parent)
    : BaseCanvas(parent)
    , m_mainSplitter(nullptr)
    , m_leftSplitter(nullptr)
    , m_rightSplitter(nullptr)
    , m_projectPanel(nullptr)
    , m_modelPanel(nullptr)
    , m_propertiesPanel(nullptr)
    , m_currentProjectId("")
    , m_currentLayoutName("default")
{
    setCanvasName("ProjectCanvas");
    setupDefaultLayout();

    // Connect to core services
    ModelService* modelService = qobject_cast<ModelService*>(parent);
    SearchService* searchService = qobject_cast<SearchService*>(parent);
    TagManager* tagManager = qobject_cast<TagManager*>(parent);

    if (modelService) {
        connect(modelService, &ModelService::modelLoaded, this, &ProjectCanvas::onModelLoaded);
        connect(modelService, &ModelService::modelDeleted, this, &ProjectCanvas::onModelDeleted);
        connect(modelService, &ModelService::modelsImported, this, &ProjectCanvas::onModelsImported);
    }

    if (searchService) {
        connect(searchService, &SearchService::searchCompleted, this, &ProjectCanvas::onSearchCompleted);
    }
}

void ProjectCanvas::setupDefaultLayout()
{
    // Create main horizontal splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->setChildrenCollapsible(false);

    // Create left panel (Project Browser)
    createProjectBrowser();

    // Create center panel (Model Grid)
    createModelGrid();

    // Create right panel (Properties)
    createPropertiesPanel();

    // Add panels to splitter
    m_mainSplitter->addWidget(m_projectPanel);
    m_mainSplitter->addWidget(m_modelPanel);
    m_mainSplitter->addWidget(m_propertiesPanel);

    // Set splitter proportions (20% : 60% : 20%)
    m_mainSplitter->setStretchFactor(0, 2);
    m_mainSplitter->setStretchFactor(1, 6);
    m_mainSplitter->setStretchFactor(2, 2);

    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Add toolbar
    createToolBar();
    mainLayout->addWidget(m_toolbar);

    // Add main splitter
    mainLayout->addWidget(m_mainSplitter);

    // Add status bar
    createStatusBar();
    mainLayout->addWidget(m_statusLabel);

    setLayout(mainLayout);

    // Load default layout
    restoreLayout("default");
}

void ProjectCanvas::createProjectBrowser()
{
    m_projectPanel = new QWidget();
    m_projectPanel->setMinimumWidth(200);
    m_projectPanel->setMaximumWidth(300);

    m_projectLayout = new QVBoxLayout(m_projectPanel);

    // Project tree
    m_projectTree = new QTreeWidget(m_projectPanel);
    m_projectTree->setHeaderLabel("Projects");
    m_projectTree->setRootIsDecorated(true);
    m_projectTree->setSelectionMode(QAbstractItemView::SingleSelection);

    // Create sample project structure
    QTreeWidgetItem* rootItem = new QTreeWidgetItem(m_projectTree);
    rootItem->setText(0, "My Projects");

    QTreeWidgetItem* defaultProject = new QTreeWidgetItem(rootItem);
    defaultProject->setText(0, "Default Project");
    defaultProject->setData(0, Qt::UserRole, "default_project_id");

    m_projectTree->expandAll();

    m_projectLayout->addWidget(m_projectTree);

    // Project buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_newProjectButton = new QPushButton("New", m_projectPanel);
    m_saveProjectButton = new QPushButton("Save", m_projectPanel);

    buttonLayout->addWidget(m_newProjectButton);
    buttonLayout->addWidget(m_saveProjectButton);

    m_projectLayout->addLayout(buttonLayout);

    // Connect signals
    connect(m_projectTree, &QTreeWidget::itemSelectionChanged, this, &ProjectCanvas::onProjectSelectionChanged);
    connect(m_projectTree, &QTreeWidget::itemDoubleClicked, this, &ProjectCanvas::onProjectDoubleClicked);
    connect(m_newProjectButton, &QPushButton::clicked, this, &ProjectCanvas::onNewProjectClicked);
    connect(m_saveProjectButton, &QPushButton::clicked, this, &ProjectCanvas::onSaveProjectClicked);
}

void ProjectCanvas::createModelGrid()
{
    m_modelPanel = new QWidget();

    m_modelLayout = new QVBoxLayout(m_modelPanel);

    // Search and filter bar
    createSearchBar();
    m_modelLayout->addWidget(m_searchBox->parentWidget());

    // Model grid view
    m_modelGrid = new QListWidget(m_modelPanel);
    m_modelGrid->setViewMode(QListView::IconMode);
    m_modelGrid->setIconSize(QSize(128, 128));
    m_modelGrid->setGridSize(QSize(150, 150));
    m_modelGrid->setResizeMode(QListView::Adjust);
    m_modelGrid->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_modelGrid->setContextMenuPolicy(Qt::CustomContextMenu);

    // Add sample models for demonstration
    for (int i = 0; i < 10; ++i) {
        QListWidgetItem* item = new QListWidgetItem(m_modelGrid);
        item->setText(QString("Model_%1.stl").arg(i + 1));
        item->setIcon(QIcon(":/icons/model_placeholder.png"));
        item->setData(Qt::UserRole, QUuid::createUuid().toString());
    }

    m_modelLayout->addWidget(m_modelGrid);

    // Connect signals
    connect(m_modelGrid, &QListWidget::itemDoubleClicked, this, &ProjectCanvas::onModelDoubleClicked);
    connect(m_modelGrid, &QListWidget::itemSelectionChanged, this, &ProjectCanvas::onModelSelectionChanged);
    connect(m_modelGrid, &QListWidget::customContextMenuRequested, this, &ProjectCanvas::onModelContextMenu);
}

void ProjectCanvas::createSearchBar()
{
    QWidget* searchWidget = new QWidget(m_modelPanel);
    QHBoxLayout* searchLayout = new QHBoxLayout(searchWidget);

    m_searchBox = new QLineEdit(searchWidget);
    m_searchBox->setPlaceholderText("Search models...");
    m_searchBox->setClearButtonEnabled(true);

    searchLayout->addWidget(m_searchBox);

    // View mode selector
    m_viewModeCombo = new QComboBox(searchWidget);
    m_viewModeCombo->addItem("Grid View", QListView::IconMode);
    m_viewModeCombo->addItem("List View", QListView::ListMode);
    m_viewModeCombo->addItem("Detail View", 2);

    searchLayout->addWidget(m_viewModeCombo);

    // Sort mode selector
    m_sortModeCombo = new QComboBox(searchWidget);
    m_sortModeCombo->addItem("Name", "name");
    m_sortModeCombo->addItem("Date", "date");
    m_sortModeCombo->addItem("Size", "size");
    m_sortModeCombo->addItem("Type", "type");

    searchLayout->addWidget(m_sortModeCombo);

    // Thumbnail size selector
    m_thumbnailSizeCombo = new QComboBox(searchWidget);
    m_thumbnailSizeCombo->addItem("Small", 64);
    m_thumbnailSizeCombo->addItem("Medium", 128);
    m_thumbnailSizeCombo->addItem("Large", 256);

    searchLayout->addWidget(m_thumbnailSizeCombo);

    // Connect signals
    connect(m_searchBox, &QLineEdit::textChanged, this, &ProjectCanvas::onSearchTextChanged);
    connect(m_viewModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ProjectCanvas::onViewModeChanged);
    connect(m_sortModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ProjectCanvas::onSortModeChanged);
    connect(m_thumbnailSizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ProjectCanvas::onThumbnailSizeChanged);
}

void ProjectCanvas::createPropertiesPanel()
{
    m_propertiesPanel = new QWidget();
    m_propertiesPanel->setMinimumWidth(250);
    m_propertiesPanel->setMaximumWidth(350);

    m_propertiesLayout = new QVBoxLayout(m_propertiesPanel);

    // Model properties table
    m_modelProperties = new QTableWidget(m_propertiesPanel);
    m_modelProperties->setColumnCount(2);
    m_modelProperties->setHorizontalHeaderLabels(QStringList() << "Property" << "Value");
    m_modelProperties->horizontalHeader()->setStretchLastSection(true);
    m_modelProperties->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_modelProperties->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_modelProperties->setAlternatingRowColors(true);

    m_propertiesLayout->addWidget(m_modelProperties);

    // Tags section
    QLabel* tagsLabel = new QLabel("Tags", m_propertiesPanel);
    m_propertiesLayout->addWidget(tagsLabel);

    m_modelTags = new QListWidget(m_propertiesPanel);
    m_modelTags->setSelectionMode(QAbstractItemView::MultiSelection);
    m_propertiesLayout->addWidget(m_modelTags);

    // Tag buttons
    QHBoxLayout* tagButtonLayout = new QHBoxLayout();

    m_addTagButton = new QPushButton("Add Tag", m_propertiesPanel);
    m_removeTagButton = new QPushButton("Remove Tag", m_propertiesPanel);

    tagButtonLayout->addWidget(m_addTagButton);
    tagButtonLayout->addWidget(m_removeTagButton);

    m_propertiesLayout->addLayout(tagButtonLayout);

    // Connect signals
    connect(m_addTagButton, &QPushButton::clicked, this, &ProjectCanvas::onAddTagClicked);
    connect(m_removeTagButton, &QPushButton::clicked, this, &ProjectCanvas::onRemoveTagClicked);
}

void ProjectCanvas::createToolBar()
{
    m_toolbar = new QWidget(this);
    m_toolbarLayout = new QHBoxLayout(m_toolbar);

    m_importButton = new QPushButton("Import", m_toolbar);
    m_deleteButton = new QPushButton("Delete", m_toolbar);
    m_tagButton = new QPushButton("Tag", m_toolbar);

    m_toolbarLayout->addWidget(m_importButton);
    m_toolbarLayout->addWidget(m_deleteButton);
    m_toolbarLayout->addWidget(m_tagButton);
    m_toolbarLayout->addStretch();

    // Connect signals
    connect(m_importButton, &QPushButton::clicked, this, &ProjectCanvas::onImportButtonClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &ProjectCanvas::onDeleteButtonClicked);
    connect(m_tagButton, &QPushButton::clicked, this, &ProjectCanvas::onTagButtonClicked);
}

void ProjectCanvas::createStatusBar()
{
    m_statusLabel = new QLabel("Ready", this);
    m_statusLabel->setMinimumHeight(20);
    m_statusLabel->setStyleSheet("padding: 2px; border-top: 1px solid #ccc;");

    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumHeight(15);
}

// BaseCanvas interface implementation
void ProjectCanvas::addWidget(BaseWidget* widget, DockArea area)
{
    if (!widget) {
        return;
    }

    QString widgetName = widget->widgetName();
    m_registeredWidgets[widgetName] = widget;

    // For now, add to the appropriate panel based on area
    switch (area) {
        case DockArea::Left:
            m_projectLayout->addWidget(widget);
            break;
        case DockArea::Center:
            m_modelLayout->addWidget(widget);
            break;
        case DockArea::Right:
            m_propertiesLayout->addWidget(widget);
            break;
        default:
            m_modelLayout->addWidget(widget);
            break;
    }

    emit widgetAdded(widget);
}

void ProjectCanvas::removeWidget(BaseWidget* widget)
{
    if (!widget) {
        return;
    }

    QString widgetName = widget->widgetName();
    if (m_registeredWidgets.contains(widgetName)) {
        m_registeredWidgets.remove(widgetName);
        widget->setParent(nullptr);
        emit widgetRemoved(widget);
    }
}

void ProjectCanvas::removeWidget(const QString& widgetName)
{
    if (m_registeredWidgets.contains(widgetName)) {
        BaseWidget* widget = m_registeredWidgets[widgetName];
        removeWidget(widget);
    }
}

QList<BaseWidget*> ProjectCanvas::getWidgets() const
{
    return m_registeredWidgets.values();
}

QList<BaseWidget*> ProjectCanvas::getWidgetsByCategory(WidgetCategory category) const
{
    QList<BaseWidget*> widgets;

    for (BaseWidget* widget : m_registeredWidgets.values()) {
        if (widget->widgetCategory() == category) {
            widgets.append(widget);
        }
    }

    return widgets;
}

BaseWidget* ProjectCanvas::getWidget(const QString& widgetName) const
{
    return m_registeredWidgets.value(widgetName, nullptr);
}

void ProjectCanvas::saveLayout(const QString& name)
{
    QJsonObject state;

    // Save splitter states
    saveSplitterState(state);

    // Save widget states
    QJsonObject widgetsState;
    for (BaseWidget* widget : m_registeredWidgets.values()) {
        QJsonObject widgetState;
        widget->saveState(widgetState);
        widgetsState[widget->widgetName()] = widgetState;
    }
    state["widgets"] = widgetsState;

    // Save canvas state
    saveState(state);

    // In a full implementation, this would save to persistent storage
    qDebug() << "Layout saved:" << name;
}

void ProjectCanvas::restoreLayout(const QString& name)
{
    // In a full implementation, this would load from persistent storage
    qDebug() << "Layout restored:" << name;
    m_currentLayoutName = name;
}

QStringList ProjectCanvas::getAvailableLayouts() const
{
    return QStringList() << "default" << "compact" << "detailed";
}

void ProjectCanvas::resetLayout()
{
    restoreLayout("default");
}

void ProjectCanvas::saveState(QJsonObject& state) const
{
    state["current_project"] = m_currentProjectId;
    state["search_query"] = m_currentSearchQuery;
    state["tag_filters"] = QJsonArray::fromStringList(m_currentTagFilters);
    state["layout_name"] = m_currentLayoutName;
}

void ProjectCanvas::restoreState(const QJsonObject& state)
{
    m_currentProjectId = state["current_project"].toString();
    m_currentSearchQuery = state["search_query"].toString();

    QJsonArray tagFilters = state["tag_filters"].toArray();
    for (const QJsonValue& value : tagFilters) {
        m_currentTagFilters.append(value.toString());
    }

    m_currentLayoutName = state["layout_name"].toString("default");
}

// ProjectCanvas specific methods
void ProjectCanvas::importModels(const QStringList& filePaths)
{
    ModelService* modelService = qobject_cast<ModelService*>(parent());
    if (modelService) {
        modelService->importModels(filePaths);
    }
}

void ProjectCanvas::createProject(const QString& name, const QString& description)
{
    QString projectName = name;
    if (projectName.isEmpty()) {
        projectName = QInputDialog::getText(this, "New Project",
            "Enter project name:", QLineEdit::Normal, "New Project");
    }

    if (!projectName.isEmpty()) {
        // Create new project in tree
        QTreeWidgetItem* projectItem = new QTreeWidgetItem(m_projectTree);
        projectItem->setText(0, projectName);
        projectItem->setData(0, Qt::UserRole, QUuid::createUuid().toString());

        m_projectTree->expandItem(projectItem);
        m_projectTree->setCurrentItem(projectItem);

        m_currentProjectId = projectItem->data(0, Qt::UserRole).toString();
    }
}

void ProjectCanvas::openProject(const QString& projectId)
{
    m_currentProjectId = projectId;

    // Update UI to show project contents
    // This would load models for the selected project
    qDebug() << "Opening project:" << projectId;
}

void ProjectCanvas::saveProject()
{
    if (m_currentProjectId.isEmpty()) {
        return;
    }

    // Save current project state
    qDebug() << "Saving project:" << m_currentProjectId;
}

void ProjectCanvas::selectModels(const QList<QUuid>& modelIds)
{
    m_selectedModelIds = modelIds;
    m_modelGrid->clearSelection();

    for (int i = 0; i < m_modelGrid->count(); ++i) {
        QListWidgetItem* item = m_modelGrid->item(i);
        QString modelId = item->data(Qt::UserRole).toString();

        if (modelIds.contains(QUuid(modelId))) {
            item->setSelected(true);
        }
    }
}

void ProjectCanvas::tagSelectedModels(const QStringList& tags)
{
    if (m_selectedModelIds.isEmpty() || tags.isEmpty()) {
        return;
    }

    TagManager* tagManager = qobject_cast<TagManager*>(parent());
    if (tagManager) {
        tagManager->addTagsToModels(tags, m_selectedModelIds);
    }
}

void ProjectCanvas::deleteSelectedModels()
{
    if (m_selectedModelIds.isEmpty()) {
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Delete Models",
        QString("Delete %1 selected models?").arg(m_selectedModelIds.count()),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        ModelService* modelService = qobject_cast<ModelService*>(parent());
        if (modelService) {
            modelService->deleteModels(m_selectedModelIds);
        }
    }
}

void ProjectCanvas::searchModels(const QString& query)
{
    m_currentSearchQuery = query;

    SearchService* searchService = qobject_cast<SearchService*>(parent());
    if (searchService) {
        searchService->searchAsync(query, QStringList() << "model");
    }
}

void ProjectCanvas::filterByTags(const QStringList& tags)
{
    m_currentTagFilters = tags;

    // Update model grid to show only models with specified tags
    for (int i = 0; i < m_modelGrid->count(); ++i) {
        QListWidgetItem* item = m_modelGrid->item(i);
        // Filter logic would go here
    }
}

// Event handlers
void ProjectCanvas::onSearchTextChanged(const QString& text)
{
    if (text.length() >= 2) {
        searchModels(text);
    } else if (text.isEmpty()) {
        // Show all models
        for (int i = 0; i < m_modelGrid->count(); ++i) {
            m_modelGrid->item(i)->setHidden(false);
        }
    }
}

void ProjectCanvas::onModelDoubleClicked(QListWidgetItem* item)
{
    QString modelId = item->data(Qt::UserRole).toString();

    // Open model in 3D viewer
    // This would switch to Design Canvas or open 3D viewer widget
    qDebug() << "Opening model in 3D viewer:" << modelId;
}

void ProjectCanvas::onModelSelectionChanged()
{
    m_selectedModelIds.clear();

    for (QListWidgetItem* item : m_modelGrid->selectedItems()) {
        QString modelId = item->data(Qt::UserRole).toString();
        m_selectedModelIds.append(QUuid(modelId));
    }

    // Update properties panel
    if (m_selectedModelIds.count() == 1) {
        updatePropertiesPanel(m_selectedModelIds.first());
    } else {
        clearPropertiesPanel();
    }

    // Update toolbar button states
    bool hasSelection = !m_selectedModelIds.isEmpty();
    m_deleteButton->setEnabled(hasSelection);
    m_tagButton->setEnabled(hasSelection);
}

void ProjectCanvas::onImportButtonClicked()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(this,
        "Import 3D Models",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "3D Models (*.stl *.obj *.ply *.3mf);;All Files (*)");

    if (!filePaths.isEmpty()) {
        importModels(filePaths);
    }
}

void ProjectCanvas::onDeleteButtonClicked()
{
    deleteSelectedModels();
}

void ProjectCanvas::onTagButtonClicked()
{
    if (m_selectedModelIds.isEmpty()) {
        return;
    }

    QStringList currentTags;
    if (m_selectedModelIds.count() == 1) {
        // Get tags for single model
        TagManager* tagManager = qobject_cast<TagManager*>(parent());
        if (tagManager) {
            currentTags = tagManager->getTagsForModel(m_selectedModelIds.first());
        }
    }

    // Show tag dialog
    QStringList availableTags = getAllAvailableTags();
    // Tag selection dialog would go here

    qDebug() << "Tag dialog for models:" << m_selectedModelIds;
}

void ProjectCanvas::onProjectSelectionChanged()
{
    QTreeWidgetItem* currentItem = m_projectTree->currentItem();
    if (currentItem && currentItem->data(0, Qt::UserRole).isValid()) {
        QString projectId = currentItem->data(0, Qt::UserRole).toString();
        openProject(projectId);
    }
}

void ProjectCanvas::onNewProjectClicked()
{
    createProject();
}

void ProjectCanvas::onSaveProjectClicked()
{
    saveProject();
}

void ProjectCanvas::onViewModeChanged(int index)
{
    int viewMode = m_viewModeCombo->itemData(index).toInt();

    if (viewMode == QListView::IconMode) {
        m_modelGrid->setViewMode(QListView::IconMode);
    } else if (viewMode == QListView::ListMode) {
        m_modelGrid->setViewMode(QListView::ListMode);
    } else {
        // Detail view would require a different widget
        qDebug() << "Detail view not implemented";
    }
}

void ProjectCanvas::onSortModeChanged(int index)
{
    QString sortMode = m_sortModeCombo->itemData(index).toString();

    // Sort models based on selected criteria
    Qt::SortOrder order = Qt::AscendingOrder;

    if (sortMode == "name") {
        m_modelGrid->sortItems(Qt::AscendingOrder);
    } else if (sortMode == "date") {
        // Would need to sort by date
        qDebug() << "Date sorting not implemented";
    } else if (sortMode == "size") {
        // Would need to sort by file size
        qDebug() << "Size sorting not implemented";
    }

    qDebug() << "Sorting by:" << sortMode;
}

void ProjectCanvas::onThumbnailSizeChanged(int size)
{
    int iconSize = m_thumbnailSizeCombo->itemData(m_thumbnailSizeCombo->currentIndex()).toInt();
    m_modelGrid->setIconSize(QSize(iconSize, iconSize));

    // Adjust grid size
    int gridSize = iconSize + 50; // Add padding
    m_modelGrid->setGridSize(QSize(gridSize, gridSize));
}

// Helper methods
void ProjectCanvas::updatePropertiesPanel(const QUuid& modelId)
{
    // Clear existing properties
    m_modelProperties->setRowCount(0);

    // Get model metadata
    ModelService* modelService = qobject_cast<ModelService*>(parent());
    if (modelService) {
        ModelMetadata model = modelService->getModel(modelId);

        // Add properties
        int row = 0;
        m_modelProperties->insertRow(row);
        m_modelProperties->setItem(row, 0, new QTableWidgetItem("Name"));
        m_modelProperties->setItem(row, 1, new QTableWidgetItem(model.filename));
        row++;

        m_modelProperties->insertRow(row);
        m_modelProperties->setItem(row, 0, new QTableWidgetItem("Size"));
        m_modelProperties->setItem(row, 1, new QTableWidgetItem(QString("%1 bytes").arg(model.fileSize)));
        row++;

        m_modelProperties->insertRow(row);
        m_modelProperties->setItem(row, 0, new QTableWidgetItem("Import Date"));
        m_modelProperties->setItem(row, 1, new QTableWidgetItem(model.importDate));
        row++;

        // Add mesh statistics
        for (auto it = model.meshStats.begin(); it != model.meshStats.end(); ++it) {
            m_modelProperties->insertRow(row);
            m_modelProperties->setItem(row, 0, new QTableWidgetItem(it.key()));
            m_modelProperties->setItem(row, 1, new QTableWidgetItem(it.value().toString()));
            row++;
        }

        // Update tags
        m_modelTags->clear();
        for (const QString& tag : model.tags) {
            m_modelTags->addItem(tag);
        }
    }
}

void ProjectCanvas::clearPropertiesPanel()
{
    m_modelProperties->setRowCount(0);
    m_modelTags->clear();
}

QStringList ProjectCanvas::getAllAvailableTags() const
{
    TagManager* tagManager = qobject_cast<TagManager*>(parent());
    if (tagManager) {
        return tagManager->getAllTags();
    }
    return QStringList();
}

void ProjectCanvas::saveSplitterState(QJsonObject& state) const
{
    if (m_mainSplitter) {
        QJsonArray splitterSizes;
        for (int size : m_mainSplitter->sizes()) {
            splitterSizes.append(size);
        }
        state["main_splitter_sizes"] = splitterSizes;
    }
}

void ProjectCanvas::restoreSplitterState(const QJsonObject& state)
{
    if (m_mainSplitter && state.contains("main_splitter_sizes")) {
        QJsonArray splitterSizes = state["main_splitter_sizes"].toArray();
        QList<int> sizes;

        for (const QJsonValue& value : splitterSizes) {
            sizes.append(value.toInt());
        }

        if (sizes.count() == m_mainSplitter->count()) {
            m_mainSplitter->setSizes(sizes);
        }
    }
}

// Event handlers for service signals
void ProjectCanvas::onModelLoaded(const ModelMetadata& model)
{
    // Add model to grid
    QListWidgetItem* item = new QListWidgetItem(m_modelGrid);
    item->setText(model.filename);
    item->setIcon(QIcon(":/icons/model_placeholder.png"));
    item->setData(Qt::UserRole, model.id.toString());

    m_statusLabel->setText(QString("Loaded: %1").arg(model.filename));
}

void ProjectCanvas::onModelDeleted(const QUuid& id)
{
    // Remove model from grid
    for (int i = 0; i < m_modelGrid->count(); ++i) {
        QListWidgetItem* item = m_modelGrid->item(i);
        if (item->data(Qt::UserRole).toString() == id.toString()) {
            delete item;
            break;
        }
    }

    m_statusLabel->setText(QString("Deleted model: %1").arg(id.toString()));
}

void ProjectCanvas::onModelsImported(const QList<ModelMetadata>& models)
{
    for (const ModelMetadata& model : models) {
        onModelLoaded(model);
    }

    m_statusLabel->setText(QString("Imported %1 models").arg(models.count()));
}

void ProjectCanvas::onSearchCompleted(const QString& query, const QList<SearchResult>& results)
{
    // Update model grid with search results
    m_modelGrid->clear();

    for (const SearchResult& result : results) {
        QListWidgetItem* item = new QListWidgetItem(m_modelGrid);
        item->setText(result.name);
        item->setIcon(QIcon(":/icons/model_placeholder.png"));
        item->setData(Qt::UserRole, result.id.toString());
    }

    m_statusLabel->setText(QString("Search: %1 results for '%2'").arg(results.count()).arg(query));
}