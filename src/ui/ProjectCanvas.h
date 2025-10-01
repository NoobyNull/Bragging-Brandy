#pragma once

#include "../core/BaseCanvas.h"
#include "../core/BaseTypes.h"
#include <QWidget>
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
#include <QStatusBar>

/**
 * @brief Project Canvas - Main workspace for 3D model management
 *
 * This is the primary canvas that users interact with for:
 * - Model import and organization
 * - Project management
 * - Basic model visualization
 * - Tag management and search
 */
class ProjectCanvas : public BaseCanvas
{
    Q_OBJECT

public:
    explicit ProjectCanvas(QWidget* parent = nullptr);
    virtual ~ProjectCanvas() = default;

    // BaseCanvas interface implementation
    QString canvasName() const override { return "ProjectCanvas"; }
    QString canvasDescription() const override { return "Main workspace for 3D model management"; }
    QString canvasType() const override { return "project"; }
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

    // ProjectCanvas specific methods
    void importModels(const QStringList& filePaths);
    void createProject(const QString& name, const QString& description = QString());
    void openProject(const QString& projectId);
    void saveProject();

    // Model operations
    void selectModels(const QList<QUuid>& modelIds);
    void tagSelectedModels(const QStringList& tags);
    void deleteSelectedModels();

    // Search operations
    void searchModels(const QString& query);
    void filterByTags(const QStringList& tags);

protected:
    void setupDefaultLayout();
    void createProjectBrowser();
    void createModelGrid();
    void createSearchBar();
    void createStatusBar();
    void createToolBar();

    // Layout management
    void saveSplitterState(QJsonObject& state) const;
    void restoreSplitterState(const QJsonObject& state);

private slots:
    // Search functionality
    void onSearchTextChanged(const QString& text);
    void onSearchEnterPressed();

    // Model operations
    void onModelDoubleClicked(QListWidgetItem* item);
    void onModelSelectionChanged();
    void onImportButtonClicked();
    void onDeleteButtonClicked();
    void onTagButtonClicked();

    // Project operations
    void onProjectSelectionChanged();
    void onNewProjectClicked();
    void onSaveProjectClicked();

    // View operations
    void onViewModeChanged(int index);
    void onSortModeChanged(int index);
    void onThumbnailSizeChanged(int size);

private:
    // Main layout components
    QSplitter* m_mainSplitter;
    QSplitter* m_leftSplitter;
    QSplitter* m_rightSplitter;

    // Project browser (left panel)
    QWidget* m_projectPanel;
    QVBoxLayout* m_projectLayout;
    QTreeWidget* m_projectTree;
    QPushButton* m_newProjectButton;
    QPushButton* m_saveProjectButton;

    // Model grid (center panel)
    QWidget* m_modelPanel;
    QVBoxLayout* m_modelLayout;
    QLineEdit* m_searchBox;
    QListWidget* m_modelGrid;
    QComboBox* m_viewModeCombo;
    QComboBox* m_sortModeCombo;
    QComboBox* m_thumbnailSizeCombo;

    // Properties panel (right panel)
    QWidget* m_propertiesPanel;
    QVBoxLayout* m_propertiesLayout;
    QTableWidget* m_modelProperties;
    QListWidget* m_modelTags;
    QPushButton* m_addTagButton;
    QPushButton* m_removeTagButton;

    // Toolbar and status
    QWidget* m_toolbar;
    QHBoxLayout* m_toolbarLayout;
    QPushButton* m_importButton;
    QPushButton* m_deleteButton;
    QPushButton* m_tagButton;
    QLabel* m_statusLabel;
    QProgressBar* m_progressBar;

    // State management
    QString m_currentProjectId;
    QList<QUuid> m_selectedModelIds;
    QString m_currentSearchQuery;
    QStringList m_currentTagFilters;
    QString m_currentLayoutName;

    // Widget registry
    QMap<QString, BaseWidget*> m_registeredWidgets;
};