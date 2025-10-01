#pragma once

#include <QString>
#include <QJsonObject>
#include <QList>
#include <QSize>
#include <QPoint>
#include <QUuid>

// Forward declarations
class BaseWidget;
class BaseCanvas;

// Enums for dock areas and widget categories
enum class DockArea {
    Left,
    Right,
    Top,
    Bottom,
    Center,
    Floating
};

enum class WidgetCategory {
    ModelManagement,
    Visualization,
    CNC,
    Analysis,
    Settings,
    Utility
};

// Basic data structures
struct ModelMetadata {
    QUuid id;
    QString filename;
    qint64 fileSize;
    QString importDate;
    QStringList tags;
    QVariantMap customFields;
    QString thumbnailPath;
    QVariantMap meshStats;

    ModelMetadata() = default;
    ModelMetadata(const QUuid& uuid) : id(uuid) {}
};

struct ProjectData {
    QUuid id;
    QString name;
    QString description;
    QString createdDate;
    QList<QUuid> modelIds;
    QVariantMap metadata;

    ProjectData() = default;
    ProjectData(const QUuid& uuid) : id(uuid) {}
};

struct SearchResult {
    QUuid id;
    QString name;
    QString type; // "model" or "project"
    QStringList tags;
    qreal relevance;
    QString snippet;

    SearchResult() = default;
    SearchResult(const QUuid& uuid, const QString& name, const QString& type)
        : id(uuid), name(name), type(type) {}
};

struct PerformanceMetrics {
    qint64 operationTimeMs;
    qint64 memoryUsageBytes;
    qint64 cpuUsagePercent;
    QString operationType;

    PerformanceMetrics() = default;
};

struct ToolData {
    QUuid id;
    QString name;
    QString manufacturer;
    QString toolType;
    QVariantMap geometry;
    QVariantMap cuttingParameters;
    QString toolFilePath;

    ToolData() = default;
    ToolData(const QUuid& uuid) : id(uuid) {}
};