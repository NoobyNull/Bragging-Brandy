#pragma once

#include "BaseTypes.h"
#include <QWidget>
#include <QJsonObject>

/**
 * @brief Base class for all widgets in the application
 *
 * All widgets must inherit from BaseWidget to ensure consistent
 * behavior for state management, docking, and inter-widget communication.
 */
class BaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BaseWidget(QWidget* parent = nullptr);
    virtual ~BaseWidget() = default;

    // Widget identification and categorization
    virtual QString widgetName() const = 0;
    virtual QString widgetDescription() const { return QString(); }
    virtual WidgetCategory widgetCategory() const = 0;
    virtual QString widgetVersion() const { return "1.0.0"; }

    // State management
    virtual void saveState(QJsonObject& state) const = 0;
    virtual void restoreState(const QJsonObject& state) = 0;

    // Layout and docking
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

    // Widget lifecycle
    virtual void onShow() {}  // Called when widget becomes visible
    virtual void onHide() {}  // Called when widget becomes hidden
    virtual void onFocus() {} // Called when widget gains focus

    // Data refresh and updates
    virtual void refresh() {} // Refresh widget data
    virtual void updatePerformanceMetrics(const PerformanceMetrics& metrics) {}

    // Error handling
    virtual void handleError(const QString& error, const QString& details = QString()) {}

signals:
    // Widget events
    void widgetShown();
    void widgetHidden();
    void widgetFocused();
    void stateChanged();
    void errorOccurred(const QString& error, const QString& details);

    // Data events
    void dataRefreshRequested();
    void performanceMetricsUpdated(const PerformanceMetrics& metrics);

protected:
    // Helper methods for state management
    void saveGeometryToState(QJsonObject& state) const;
    void restoreGeometryFromState(const QJsonObject& state);
    void saveSplitterState(QJsonObject& state, const QString& key) const;
    void restoreSplitterState(const QJsonObject& state, const QString& key);

    // Widget properties
    QString m_widgetId;
    bool m_isInitialized;
    qint64 m_lastRefreshTime;
};