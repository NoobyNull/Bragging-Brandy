#pragma once

#include "BaseTypes.h"
#include "BaseWidget.h"
#include <QWidget>
#include <QTabWidget>
#include <QJsonObject>
#include <QList>

/**
 * @brief Base class for all canvas types in the application
 *
 * Canvases are specialized workspaces that contain and manage widgets.
 * Each canvas type (Project, Design, Cut List, etc.) inherits from BaseCanvas.
 */
class BaseCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit BaseCanvas(QWidget* parent = nullptr);
    virtual ~BaseCanvas() = default;

    // Canvas identification
    virtual QString canvasName() const = 0;
    virtual QString canvasDescription() const { return QString(); }
    virtual QString canvasType() const = 0;
    virtual QString canvasVersion() const { return "1.0.0"; }

    // Widget management
    virtual void addWidget(BaseWidget* widget, DockArea area = DockArea::Center) = 0;
    virtual void removeWidget(BaseWidget* widget) = 0;
    virtual void removeWidget(const QString& widgetName) = 0;
    virtual QList<BaseWidget*> getWidgets() const = 0;
    virtual QList<BaseWidget*> getWidgetsByCategory(WidgetCategory category) const = 0;
    virtual BaseWidget* getWidget(const QString& widgetName) const = 0;

    // Layout management
    virtual void saveLayout(const QString& name) = 0;
    virtual void restoreLayout(const QString& name) = 0;
    virtual QStringList getAvailableLayouts() const = 0;
    virtual void resetLayout() = 0;

    // Canvas state
    virtual void saveState(QJsonObject& state) const = 0;
    virtual void restoreState(const QJsonObject& state) = 0;

    // Canvas lifecycle
    virtual void onShow() {}  // Called when canvas becomes active
    virtual void onHide() {}  // Called when canvas becomes inactive
    virtual void refresh() {} // Refresh all widgets in canvas

    // Data sharing between widgets
    virtual QVariant getSharedData(const QString& key) const;
    virtual void setSharedData(const QString& key, const QVariant& value);
    virtual void clearSharedData(const QString& key);

    // Performance monitoring
    virtual PerformanceMetrics getCanvasMetrics() const;
    virtual void updateWidgetMetrics(const QString& widgetName, const PerformanceMetrics& metrics);

signals:
    // Canvas events
    void canvasShown();
    void canvasHidden();
    void widgetAdded(BaseWidget* widget);
    void widgetRemoved(BaseWidget* widget);
    void layoutChanged();
    void sharedDataChanged(const QString& key, const QVariant& value);

    // Error events
    void errorOccurred(const QString& error, const QString& details);

protected:
    // Helper methods for layout management
    void saveWidgetStates(QJsonObject& state) const;
    void restoreWidgetStates(const QJsonObject& state);
    void updateCanvasMetrics();

    // Canvas properties
    QString m_canvasId;
    QVariantMap m_sharedData;
    PerformanceMetrics m_canvasMetrics;
    qint64 m_lastUpdateTime;

    // Widget tracking
    QList<BaseWidget*> m_widgets;
    QMap<QString, BaseWidget*> m_widgetMap;
};