#pragma once

#include "../core/BaseTypes.h"
#include "../render/ModelLoader.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QFuture>
#include <QTimer>

/**
 * @brief Mesh repair and optimization pipeline
 *
 * Automatically detects and repairs common mesh issues including:
 * - Non-manifold edges
 * - Duplicate vertices
 * - Self-intersections
 * - Degenerate triangles
 * - Holes and gaps
 * - Normal vector issues
 *
 * Performance target: ≤5 seconds for complex models
 */
class MeshRepair : public QObject
{
    Q_OBJECT

public:
    explicit MeshRepair(QObject* parent = nullptr);
    virtual ~MeshRepair() = default;

    // Repair modes
    enum RepairMode {
        Automatic,    // Automatically detect and fix issues
        Assisted,     // Show issues and let user decide
        Manual        // Manual repair tools only
    };

    // Issue types
    enum IssueType {
        NonManifoldEdges,
        DuplicateVertices,
        SelfIntersections,
        DegenerateTriangles,
        MissingNormals,
        Holes,
        InvertedNormals,
        ZeroAreaFaces,
        OverlappingFaces
    };

    struct MeshIssue {
        IssueType type;
        QString description;
        QString severity; // "Low", "Medium", "High", "Critical"
        int affectedVertices;
        int affectedFaces;
        QVector3D location; // Approximate location of issue
        QString suggestion; // Suggested fix

        MeshIssue() : affectedVertices(0), affectedFaces(0) {}
        MeshIssue(IssueType t, const QString& desc, const QString& sev = "Medium")
            : type(t), description(desc), severity(sev), affectedVertices(0), affectedFaces(0) {}
    };

    struct RepairResult {
        bool success;
        QStringList appliedFixes;
        int issuesFound;
        int issuesFixed;
        qint64 repairTimeMs;
        QString report; // Detailed repair report

        RepairResult() : success(false), issuesFound(0), issuesFixed(0), repairTimeMs(0) {}
    };

    // Mesh analysis
    virtual QList<MeshIssue> analyzeMesh(const MeshData& mesh) = 0;
    virtual QList<MeshIssue> analyzeModel(const ModelData& model) = 0;

    // Repair operations
    virtual QFuture<RepairResult> repairMeshAsync(const MeshData& mesh, RepairMode mode = Automatic) = 0;
    virtual RepairResult repairMesh(const MeshData& mesh, RepairMode mode = Automatic) = 0;
    virtual RepairResult repairModel(const ModelData& model, RepairMode mode = Automatic) = 0;

    // Individual repair functions
    virtual MeshData weldVertices(MeshData mesh, float threshold = 0.001f) = 0;
    virtual MeshData removeDuplicateFaces(MeshData mesh) = 0;
    virtual MeshData fixNonManifoldEdges(MeshData mesh) = 0;
    virtual MeshData repairSelfIntersections(MeshData mesh) = 0;
    virtual MeshData fillHoles(MeshData mesh, float maxHoleSize = 100.0f) = 0;
    virtual MeshData recalculateNormals(MeshData mesh) = 0;
    virtual MeshData fixDegenerateTriangles(MeshData mesh) = 0;

    // Batch operations
    virtual RepairResult repairMultipleMeshes(const QList<MeshData>& meshes, RepairMode mode = Automatic) = 0;
    virtual void repairAllModels(RepairMode mode = Automatic) = 0;

    // Configuration
    virtual void setRepairMode(RepairMode mode) = 0;
    virtual RepairMode getRepairMode() const = 0;
    virtual void setAutoRepairThreshold(float threshold) = 0;
    virtual float getAutoRepairThreshold() const = 0;

    // Performance settings
    virtual void setMaxRepairTime(qint64 milliseconds) = 0;
    virtual qint64 getMaxRepairTime() const = 0;
    virtual void setEnableParallelProcessing(bool enabled) = 0;
    virtual bool getEnableParallelProcessing() const = 0;

signals:
    // Repair progress
    void repairProgress(const QString& meshName, int percentage, const QString& stage);
    void repairStarted(const QString& meshName);
    void repairCompleted(const QString& meshName, const RepairResult& result);

    // Issue detection
    void issuesFound(const QString& meshName, const QList<MeshIssue>& issues);
    void criticalIssuesFound(const QString& meshName, const QList<MeshIssue>& issues);

    // Batch operations
    void batchRepairProgress(int completed, int total);
    void batchRepairCompleted(const QList<RepairResult>& results);

protected:
    // Issue detection algorithms
    virtual QList<MeshIssue> detectNonManifoldEdges(const MeshData& mesh) = 0;
    virtual QList<MeshIssue> detectDuplicateVertices(const MeshData& mesh) = 0;
    virtual QList<MeshIssue> detectSelfIntersections(const MeshData& mesh) = 0;
    virtual QList<MeshIssue> detectDegenerateTriangles(const MeshData& mesh) = 0;
    virtual QList<MeshIssue> detectMissingNormals(const MeshData& mesh) = 0;
    virtual QList<MeshIssue> detectHoles(const MeshData& mesh) = 0;

    // Repair algorithms
    virtual bool weldVerticesImpl(MeshData& mesh, float threshold) = 0;
    virtual bool removeDuplicateFacesImpl(MeshData& mesh) = 0;
    virtual bool fixNonManifoldEdgesImpl(MeshData& mesh) = 0;
    virtual bool repairSelfIntersectionsImpl(MeshData& mesh) = 0;
    virtual bool fillHolesImpl(MeshData& mesh, float maxHoleSize) = 0;

    // Utility functions
    virtual float calculateMeshVolume(const MeshData& mesh) = 0;
    virtual QVector3D calculateMeshCentroid(const MeshData& mesh) = 0;
    virtual bool validateMeshTopology(const MeshData& mesh) = 0;

    // Performance monitoring
    virtual void updateRepairMetrics(const RepairResult& result) = 0;

    // Configuration
    RepairMode m_repairMode;
    float m_autoRepairThreshold;
    qint64 m_maxRepairTime;
    bool m_enableParallelProcessing;

    // Performance tracking
    PerformanceMetrics m_repairMetrics;
    qint64 m_totalRepairTime;
    int m_meshesRepaired;
};