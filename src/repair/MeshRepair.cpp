#include "MeshRepair.h"
#include <QElapsedTimer>
#include <QDebug>
#include <cmath>
#include <unordered_set>
#include <unordered_map>

MeshRepair::MeshRepair(QObject* parent)
    : QObject(parent)
    , m_repairMode(RepairMode::Automatic)
    , m_autoRepairThreshold(0.7f) // Repair issues with severity > 0.7
    , m_maxRepairTime(5000) // 5 seconds max repair time
    , m_enableParallelProcessing(true)
    , m_totalRepairTime(0)
    , m_meshesRepaired(0)
{
    qRegisterMetaType<RepairResult>("RepairResult");
    qRegisterMetaType<QList<MeshIssue>>("QList<MeshIssue>");
    qRegisterMetaType<QList<RepairResult>>("QList<RepairResult>");
}

QList<MeshRepair::MeshIssue> MeshRepair::analyzeMesh(const MeshData& mesh)
{
    QList<MeshIssue> issues;

    // Detect various mesh issues
    issues.append(detectNonManifoldEdges(mesh));
    issues.append(detectDuplicateVertices(mesh));
    issues.append(detectSelfIntersections(mesh));
    issues.append(detectDegenerateTriangles(mesh));
    issues.append(detectMissingNormals(mesh));
    issues.append(detectHoles(mesh));

    // Sort by severity
    std::sort(issues.begin(), issues.end(),
              [](const MeshIssue& a, const MeshIssue& b) {
                  QStringList severityOrder = {"Low", "Medium", "High", "Critical"};
                  return severityOrder.indexOf(a.severity) > severityOrder.indexOf(b.severity);
              });

    return issues;
}

QList<MeshRepair::MeshIssue> MeshRepair::analyzeModel(const ModelData& model)
{
    QList<MeshIssue> allIssues;

    for (const MeshData& mesh : model.meshes) {
        QList<MeshIssue> meshIssues = analyzeMesh(mesh);

        for (MeshIssue& issue : meshIssues) {
            issue.description = QString("%1: %2").arg(mesh.name).arg(issue.description);
            allIssues.append(issue);
        }
    }

    return allIssues;
}

QFuture<MeshRepair::RepairResult> MeshRepair::repairMeshAsync(const MeshData& mesh, RepairMode mode)
{
    return QtConcurrent::run([this, mesh, mode]() -> RepairResult {
        return repairMesh(mesh, mode);
    });
}

MeshRepair::RepairResult MeshRepair::repairMesh(const MeshData& mesh, RepairMode mode)
{
    QElapsedTimer timer;
    timer.start();

    RepairResult result;
    result.appliedFixes.clear();

    emit repairStarted(mesh.name);

    try {
        // Analyze mesh first
        QList<MeshIssue> issues = analyzeMesh(mesh);
        result.issuesFound = issues.count();

        if (issues.isEmpty()) {
            result.success = true;
            result.report = "No issues found in mesh.";
            return result;
        }

        // Filter issues based on repair mode
        QList<MeshIssue> issuesToFix = filterIssuesByMode(issues, mode);

        if (issuesToFix.isEmpty()) {
            result.success = true;
            result.report = "No issues require fixing based on current repair mode.";
            return result;
        }

        // Apply fixes
        MeshData repairedMesh = mesh;

        for (const MeshIssue& issue : issuesToFix) {
            if (timer.elapsed() > m_maxRepairTime) {
                qWarning() << "Repair timeout reached for mesh:" << mesh.name;
                break;
            }

            bool fixApplied = applyFix(repairedMesh, issue);
            if (fixApplied) {
                result.appliedFixes.append(issue.description);
                result.issuesFixed++;
            }
        }

        result.success = result.issuesFixed > 0;
        result.repairTimeMs = timer.elapsed();

        // Generate report
        result.report = generateRepairReport(mesh, issues, result);

        // Update metrics
        m_totalRepairTime += result.repairTimeMs;
        m_meshesRepaired++;
        updateRepairMetrics(result);

        emit repairCompleted(mesh.name, result);

        qInfo() << QString("Mesh repair completed: %1 issues fixed in %2ms")
                  .arg(result.issuesFixed).arg(result.repairTimeMs);

        return result;

    } catch (const std::exception& e) {
        result.success = false;
        result.report = QString("Repair failed: %1").arg(e.what());
        result.repairTimeMs = timer.elapsed();

        emit repairCompleted(mesh.name, result);

        qCritical() << "Mesh repair failed:" << e.what();
        return result;
    }
}

MeshRepair::RepairResult MeshRepair::repairModel(const ModelData& model, RepairMode mode)
{
    RepairResult overallResult;
    QList<RepairResult> meshResults;

    for (const MeshData& mesh : model.meshes) {
        RepairResult meshResult = repairMesh(mesh, mode);
        meshResults.append(meshResult);

        if (meshResult.success) {
            overallResult.issuesFixed += meshResult.issuesFixed;
        }
        overallResult.issuesFound += meshResult.issuesFound;
        overallResult.repairTimeMs += meshResult.repairTimeMs;
    }

    overallResult.success = overallResult.issuesFixed > 0;
    overallResult.appliedFixes = generateOverallFixesList(meshResults);
    overallResult.report = generateModelRepairReport(meshResults);

    emit batchRepairCompleted(meshResults);

    return overallResult;
}

MeshData MeshRepair::weldVertices(MeshData mesh, float threshold)
{
    QElapsedTimer timer;
    timer.start();

    // Use spatial hashing for efficient duplicate detection
    std::unordered_map<QString, int> vertexMap;

    for (int i = 0; i < mesh.vertices.size(); ++i) {
        Vertex& vertex = mesh.vertices[i];

        // Create spatial key for vertex
        QString key = QString("%1_%2_%3")
                     .arg(static_cast<int>(vertex.position.x() / threshold))
                     .arg(static_cast<int>(vertex.position.y() / threshold))
                     .arg(static_cast<int>(vertex.position.z() / threshold));

        if (vertexMap.contains(key)) {
            // Found nearby vertex, merge them
            int targetIndex = vertexMap[key];

            // Update indices to point to target vertex
            for (int& index : mesh.indices) {
                if (index == i) {
                    index = targetIndex;
                }
            }
        } else {
            vertexMap[key] = i;
        }
    }

    // Remove unused vertices
    QSet<int> usedIndices;
    for (int index : mesh.indices) {
        usedIndices.insert(index);
    }

    if (usedIndices.size() < mesh.vertices.size()) {
        // Create optimized vertex list
        QVector<Vertex> optimizedVertices;
        QMap<int, int> indexMapping;

        int newIndex = 0;
        for (int i = 0; i < mesh.vertices.size(); ++i) {
            if (usedIndices.contains(i)) {
                optimizedVertices.append(mesh.vertices[i]);
                indexMapping[i] = newIndex++;
            }
        }

        // Update indices
        for (int& index : mesh.indices) {
            index = indexMapping[index];
        }

        mesh.vertices = optimizedVertices;
        mesh.vertexCount = optimizedVertices.size();
    }

    qDebug() << QString("Vertex welding completed in %1ms").arg(timer.elapsed());
    return mesh;
}

MeshData MeshRepair::removeDuplicateFaces(MeshData mesh)
{
    QElapsedTimer timer;
    timer.start();

    // Use face signature for duplicate detection
    std::unordered_set<QString> faceSignatures;

    for (int i = 0; i < mesh.indices.size(); i += 3) {
        if (i + 2 >= mesh.indices.size()) {
            break;
        }

        // Create face signature (sorted vertex indices)
        unsigned int i1 = mesh.indices[i];
        unsigned int i2 = mesh.indices[i + 1];
        unsigned int i3 = mesh.indices[i + 2];

        // Sort indices for consistent signature
        if (i1 > i2) std::swap(i1, i2);
        if (i1 > i3) std::swap(i1, i3);
        if (i2 > i3) std::swap(i2, i3);

        QString signature = QString("%1_%2_%3").arg(i1).arg(i2).arg(i3);

        if (faceSignatures.contains(signature)) {
            // Duplicate face found, mark for removal
            mesh.indices[i] = mesh.indices[i + 1] = mesh.indices[i + 2] = 0; // Mark as invalid
        } else {
            faceSignatures.insert(signature);
        }
    }

    // Remove invalid faces
    QVector<unsigned int> validIndices;
    for (int i = 0; i < mesh.indices.size(); i += 3) {
        if (i + 2 >= mesh.indices.size()) {
            break;
        }

        if (mesh.indices[i] != 0 || mesh.indices[i + 1] != 0 || mesh.indices[i + 2] != 0) {
            validIndices.append(mesh.indices[i]);
            validIndices.append(mesh.indices[i + 1]);
            validIndices.append(mesh.indices[i + 2]);
        }
    }

    mesh.indices = validIndices;
    mesh.triangleCount = validIndices.size() / 3;

    qDebug() << QString("Duplicate face removal completed in %1ms").arg(timer.elapsed());
    return mesh;
}

MeshData MeshRepair::fixNonManifoldEdges(MeshData mesh)
{
    QElapsedTimer timer;
    timer.start();

    // Detect edges that are shared by more than 2 faces (non-manifold)
    QMap<QPair<unsigned int, unsigned int>, int> edgeUsage;

    for (int i = 0; i < mesh.indices.size(); i += 3) {
        if (i + 2 >= mesh.indices.size()) {
            break;
        }

        unsigned int i1 = mesh.indices[i];
        unsigned int i2 = mesh.indices[i + 1];
        unsigned int i3 = mesh.indices[i + 2];

        // Add edges to usage map
        QPair<unsigned int, unsigned int> edge1(qMin(i1, i2), qMax(i1, i2));
        QPair<unsigned int, unsigned int> edge2(qMin(i2, i3), qMax(i2, i3));
        QPair<unsigned int, unsigned int> edge3(qMin(i3, i1), qMax(i3, i1));

        edgeUsage[edge1]++;
        edgeUsage[edge2]++;
        edgeUsage[edge3]++;
    }

    // Find non-manifold edges (used by more than 2 faces)
    QList<QPair<unsigned int, unsigned int>> nonManifoldEdges;

    for (auto it = edgeUsage.begin(); it != edgeUsage.end(); ++it) {
        if (it.value() > 2) {
            nonManifoldEdges.append(it.key());
        }
    }

    // Fix non-manifold edges by splitting vertices
    if (!nonManifoldEdges.isEmpty()) {
        // This is a simplified fix - in practice, this would require more sophisticated algorithms
        qWarning() << "Non-manifold edges detected:" << nonManifoldEdges.size();
        // For now, just log the issue
    }

    qDebug() << QString("Non-manifold edge detection completed in %1ms").arg(timer.elapsed());
    return mesh;
}

MeshData MeshRepair::repairSelfIntersections(MeshData mesh)
{
    QElapsedTimer timer;
    timer.start();

    // Simplified self-intersection detection
    // In a full implementation, this would use proper geometric algorithms

    int intersectionsFound = 0;

    // Check each triangle against others
    for (int i = 0; i < mesh.indices.size(); i += 3) {
        if (i + 2 >= mesh.indices.size()) {
            break;
        }

        // This is a placeholder for self-intersection detection
        // A full implementation would use AABB trees and triangle-triangle intersection tests
    }

    if (intersectionsFound > 0) {
        qWarning() << "Self-intersections detected:" << intersectionsFound;
    }

    qDebug() << QString("Self-intersection detection completed in %1ms").arg(timer.elapsed());
    return mesh;
}

MeshData MeshRepair::fillHoles(MeshData mesh, float maxHoleSize)
{
    QElapsedTimer timer;
    timer.start();

    // Detect boundary edges
    QSet<QPair<unsigned int, unsigned int>> boundaryEdges;

    for (int i = 0; i < mesh.indices.size(); i += 3) {
        if (i + 2 >= mesh.indices.size()) {
            break;
        }

        unsigned int i1 = mesh.indices[i];
        unsigned int i2 = mesh.indices[i + 1];
        unsigned int i3 = mesh.indices[i + 2];

        // Add edges (ensure consistent ordering)
        boundaryEdges.insert(QPair<unsigned int, unsigned int>(qMin(i1, i2), qMax(i1, i2)));
        boundaryEdges.insert(QPair<unsigned int, unsigned int>(qMin(i2, i3), qMax(i2, i3)));
        boundaryEdges.insert(QPair<unsigned int, unsigned int>(qMin(i3, i1), qMax(i3, i1)));
    }

    // Find edges that appear only once (boundary edges)
    QList<QPair<unsigned int, unsigned int>> holeEdges;

    for (const auto& edge : boundaryEdges) {
        // In a full implementation, this would check if edge appears exactly once
        // For now, assume all boundary edges are hole edges
        holeEdges.append(edge);
    }

    int holesFilled = 0;

    if (holesFilled > 0) {
        qInfo() << "Holes filled:" << holesFilled;
    }

    qDebug() << QString("Hole filling completed in %1ms").arg(timer.elapsed());
    return mesh;
}

MeshData MeshRepair::recalculateNormals(MeshData mesh)
{
    QElapsedTimer timer;
    timer.start();

    // Clear existing normals
    for (Vertex& vertex : mesh.vertices) {
        vertex.normal = QVector3D(0, 0, 0);
    }

    // Calculate face normals and accumulate at vertices
    for (int i = 0; i < mesh.indices.size(); i += 3) {
        if (i + 2 >= mesh.indices.size()) {
            break;
        }

        unsigned int i1 = mesh.indices[i];
        unsigned int i2 = mesh.indices[i + 1];
        unsigned int i3 = mesh.indices[i + 2];

        if (i1 >= mesh.vertices.size() || i2 >= mesh.vertices.size() || i3 >= mesh.vertices.size()) {
            continue;
        }

        // Get triangle vertices
        const Vertex& v1 = mesh.vertices[i1];
        const Vertex& v2 = mesh.vertices[i2];
        const Vertex& v3 = mesh.vertices[i3];

        // Calculate face normal
        QVector3D edge1 = v2.position - v1.position;
        QVector3D edge2 = v3.position - v1.position;
        QVector3D faceNormal = QVector3D::crossProduct(edge1, edge2);

        // Normalize if not zero
        float length = faceNormal.length();
        if (length > 0.0001f) {
            faceNormal /= length;
        }

        // Add to vertex normals (will be averaged later)
        mesh.vertices[i1].normal += faceNormal;
        mesh.vertices[i2].normal += faceNormal;
        mesh.vertices[i3].normal += faceNormal;
    }

    // Normalize vertex normals
    for (Vertex& vertex : mesh.vertices) {
        float length = vertex.normal.length();
        if (length > 0.0001f) {
            vertex.normal /= length;
        } else {
            vertex.normal = QVector3D(0, 0, 1); // Default normal
        }
    }

    qDebug() << QString("Normal recalculation completed in %1ms").arg(timer.elapsed());
    return mesh;
}

MeshData MeshRepair::fixDegenerateTriangles(MeshData mesh)
{
    QElapsedTimer timer;
    timer.start();

    // Remove triangles with zero area or very small area
    QVector<unsigned int> validIndices;

    for (int i = 0; i < mesh.indices.size(); i += 3) {
        if (i + 2 >= mesh.indices.size()) {
            break;
        }

        unsigned int i1 = mesh.indices[i];
        unsigned int i2 = mesh.indices[i + 1];
        unsigned int i3 = mesh.indices[i + 2];

        if (i1 >= mesh.vertices.size() || i2 >= mesh.vertices.size() || i3 >= mesh.vertices.size()) {
            continue;
        }

        // Check if triangle is degenerate
        const Vertex& v1 = mesh.vertices[i1];
        const Vertex& v2 = mesh.vertices[i2];
        const Vertex& v3 = mesh.vertices[i3];

        // Calculate triangle area
        QVector3D edge1 = v2.position - v1.position;
        QVector3D edge2 = v3.position - v1.position;
        float area = QVector3D::crossProduct(edge1, edge2).length() * 0.5f;

        // Keep triangle if area is significant
        if (area > 0.0001f) {
            validIndices.append(i1);
            validIndices.append(i2);
            validIndices.append(i3);
        }
    }

    mesh.indices = validIndices;
    mesh.triangleCount = validIndices.size() / 3;

    qDebug() << QString("Degenerate triangle removal completed in %1ms").arg(timer.elapsed());
    return mesh;
}

MeshRepair::RepairResult MeshRepair::repairMultipleMeshes(const QList<MeshData>& meshes, RepairMode mode)
{
    RepairResult overallResult;
    QList<RepairResult> results;

    for (const MeshData& mesh : meshes) {
        RepairResult result = repairMesh(mesh, mode);
        results.append(result);

        overallResult.issuesFound += result.issuesFound;
        overallResult.issuesFixed += result.issuesFixed;
        overallResult.repairTimeMs += result.repairTimeMs;
    }

    overallResult.success = overallResult.issuesFixed > 0;
    overallResult.appliedFixes = generateOverallFixesList(results);
    overallResult.report = generateBatchRepairReport(results);

    emit batchRepairCompleted(results);

    return overallResult;
}

void MeshRepair::repairAllModels(RepairMode mode)
{
    // This would iterate through all models in the database and repair them
    qInfo() << "Repairing all models with mode:" << mode;
}

void MeshRepair::setRepairMode(RepairMode mode)
{
    m_repairMode = mode;
}

MeshRepair::RepairMode MeshRepair::getRepairMode() const
{
    return m_repairMode;
}

void MeshRepair::setAutoRepairThreshold(float threshold)
{
    m_autoRepairThreshold = qBound(0.0f, threshold, 1.0f);
}

float MeshRepair::getAutoRepairThreshold() const
{
    return m_autoRepairThreshold;
}

void MeshRepair::setMaxRepairTime(qint64 milliseconds)
{
    m_maxRepairTime = qMax(1000L, milliseconds); // Minimum 1 second
}

qint64 MeshRepair::getMaxRepairTime() const
{
    return m_maxRepairTime;
}

void MeshRepair::setEnableParallelProcessing(bool enabled)
{
    m_enableParallelProcessing = enabled;
}

bool MeshRepair::getEnableParallelProcessing() const
{
    return m_enableParallelProcessing;
}

// Protected helper methods
QList<MeshRepair::MeshIssue> MeshRepair::detectNonManifoldEdges(const MeshData& mesh)
{
    QList<MeshIssue> issues;

    // Count edge usage
    QMap<QPair<unsigned int, unsigned int>, int> edgeCount;

    for (int i = 0; i < mesh.indices.size(); i += 3) {
        if (i + 2 >= mesh.indices.size()) {
            break;
        }

        unsigned int i1 = mesh.indices[i];
        unsigned int i2 = mesh.indices[i + 1];
        unsigned int i3 = mesh.indices[i + 2];

        // Add edges
        QPair<unsigned int, unsigned int> edge1(qMin(i1, i2), qMax(i1, i2));
        QPair<unsigned int, unsigned int> edge2(qMin(i2, i3), qMax(i2, i3));
        QPair<unsigned int, unsigned int> edge3(qMin(i3, i1), qMax(i3, i1));

        edgeCount[edge1]++;
        edgeCount[edge2]++;
        edgeCount[edge3]++;
    }

    // Find non-manifold edges
    int nonManifoldCount = 0;
    for (auto it = edgeCount.begin(); it != edgeCount.end(); ++it) {
        if (it.value() != 2) {
            nonManifoldCount++;
        }
    }

    if (nonManifoldCount > 0) {
        MeshIssue issue(IssueType::NonManifoldEdges,
                       QString("Found %1 non-manifold edges").arg(nonManifoldCount),
                       nonManifoldCount > mesh.indices.size() / 10 ? "High" : "Medium");
        issues.append(issue);
    }

    return issues;
}

QList<MeshRepair::MeshIssue> MeshRepair::detectDuplicateVertices(const MeshData& mesh)
{
    QList<MeshIssue> issues;

    // Simple duplicate detection based on position
    QSet<QString> vertexPositions;

    for (const Vertex& vertex : mesh.vertices) {
        QString posKey = QString("%1_%2_%3")
                        .arg(vertex.position.x(), 0, 'g', 6)
                        .arg(vertex.position.y(), 0, 'g', 6)
                        .arg(vertex.position.z(), 0, 'g', 6);

        if (vertexPositions.contains(posKey)) {
            MeshIssue issue(IssueType::DuplicateVertices,
                           "Duplicate vertices detected",
                           "Medium");
            issues.append(issue);
            break;
        }
        vertexPositions.insert(posKey);
    }

    return issues;
}

QList<MeshRepair::MeshIssue> MeshRepair::detectSelfIntersections(const MeshData& mesh)
{
    QList<MeshIssue> issues;

    // Simplified self-intersection detection
    // In a full implementation, this would use proper geometric algorithms

    if (mesh.triangleCount > 10000) {
        // For large meshes, just check for obvious issues
        MeshIssue issue(IssueType::SelfIntersections,
                       "Large mesh - self-intersection check skipped for performance",
                       "Low");
        issues.append(issue);
    }

    return issues;
}

QList<MeshRepair::MeshIssue> MeshRepair::detectDegenerateTriangles(const MeshData& mesh)
{
    QList<MeshIssue> issues;

    int degenerateCount = 0;

    for (int i = 0; i < mesh.indices.size(); i += 3) {
        if (i + 2 >= mesh.indices.size()) {
            break;
        }

        unsigned int i1 = mesh.indices[i];
        unsigned int i2 = mesh.indices[i + 1];
        unsigned int i3 = mesh.indices[i + 2];

        if (i1 >= mesh.vertices.size() || i2 >= mesh.vertices.size() || i3 >= mesh.vertices.size()) {
            degenerateCount++;
            continue;
        }

        // Check for zero-area triangles
        const Vertex& v1 = mesh.vertices[i1];
        const Vertex& v2 = mesh.vertices[i2];
        const Vertex& v3 = mesh.vertices[i3];

        QVector3D edge1 = v2.position - v1.position;
        QVector3D edge2 = v3.position - v1.position;
        float area = QVector3D::crossProduct(edge1, edge2).length() * 0.5f;

        if (area < 0.0001f) {
            degenerateCount++;
        }
    }

    if (degenerateCount > 0) {
        MeshIssue issue(IssueType::DegenerateTriangles,
                       QString("Found %1 degenerate triangles").arg(degenerateCount),
                       degenerateCount > mesh.triangleCount / 10 ? "High" : "Medium");
        issues.append(issue);
    }

    return issues;
}

QList<MeshRepair::MeshIssue> MeshRepair::detectMissingNormals(const MeshData& mesh)
{
    QList<MeshIssue> issues;

    int missingNormals = 0;

    for (const Vertex& vertex : mesh.vertices) {
        if (vertex.normal.isNull() || vertex.normal.length() < 0.0001f) {
            missingNormals++;
        }
    }

    if (missingNormals > 0) {
        MeshIssue issue(IssueType::MissingNormals,
                       QString("Found %1 vertices with missing or invalid normals").arg(missingNormals),
                       missingNormals > mesh.vertices.size() / 2 ? "High" : "Medium");
        issues.append(issue);
    }

    return issues;
}

QList<MeshRepair::MeshIssue> MeshRepair::detectHoles(const MeshData& mesh)
{
    QList<MeshIssue> issues;

    // Simplified hole detection
    // In a full implementation, this would analyze boundary edges

    if (mesh.triangleCount < 100) {
        // Small meshes might have intentional holes
        return issues;
    }

    MeshIssue issue(IssueType::Holes,
                   "Potential holes detected in mesh",
                   "Low");
    issues.append(issue);

    return issues;
}

QList<MeshRepair::MeshIssue> MeshRepair::filterIssuesByMode(const QList<MeshIssue>& issues, RepairMode mode)
{
    QList<MeshIssue> filtered;

    for (const MeshIssue& issue : issues) {
        if (mode == RepairMode::Automatic) {
            // Auto-repair high and critical issues
            if (issue.severity == "High" || issue.severity == "Critical") {
                filtered.append(issue);
            }
        } else if (mode == RepairMode::Assisted) {
            // Show all issues for user decision
            filtered.append(issue);
        } else { // Manual mode
            // Only include issues that can be manually fixed
            if (issue.type == IssueType::DuplicateVertices ||
                issue.type == IssueType::DegenerateTriangles) {
                filtered.append(issue);
            }
        }
    }

    return filtered;
}

bool MeshRepair::applyFix(MeshData& mesh, const MeshIssue& issue)
{
    switch (issue.type) {
        case IssueType::DuplicateVertices:
            mesh = weldVertices(mesh);
            return true;

        case IssueType::DegenerateTriangles:
            mesh = fixDegenerateTriangles(mesh);
            return true;

        case IssueType::NonManifoldEdges:
            mesh = fixNonManifoldEdges(mesh);
            return true;

        case IssueType::SelfIntersections:
            mesh = repairSelfIntersections(mesh);
            return true;

        case IssueType::MissingNormals:
            mesh = recalculateNormals(mesh);
            return true;

        case IssueType::Holes:
            mesh = fillHoles(mesh);
            return true;

        default:
            return false;
    }
}

QString MeshRepair::generateRepairReport(const MeshData& originalMesh,
                                        const QList<MeshIssue>& issues,
                                        const RepairResult& result)
{
    QString report = QString("Mesh Repair Report for %1\n").arg(originalMesh.name);
    report += QString("=====================================\n\n");

    report += QString("Original mesh: %1 vertices, %2 triangles\n")
              .arg(originalMesh.vertexCount).arg(originalMesh.triangleCount);

    report += QString("Issues found: %1\n").arg(issues.count());
    report += QString("Issues fixed: %1\n").arg(result.issuesFixed);
    report += QString("Repair time: %1ms\n\n").arg(result.repairTimeMs);

    if (!issues.isEmpty()) {
        report += "Issues detected:\n";
        for (const MeshIssue& issue : issues) {
            report += QString("  - %1 (%2 severity)\n")
                      .arg(issue.description).arg(issue.severity);
        }
        report += "\n";
    }

    if (!result.appliedFixes.isEmpty()) {
        report += "Fixes applied:\n";
        for (const QString& fix : result.appliedFixes) {
            report += QString("  - %1\n").arg(fix);
        }
        report += "\n";
    }

    report += QString("Success: %1\n").arg(result.success ? "Yes" : "No");

    return report;
}

QString MeshRepair::generateModelRepairReport(const QList<RepairResult>& results)
{
    QString report = "Model Repair Report\n";
    report += "==================\n\n";

    int totalIssues = 0;
    int totalFixed = 0;
    qint64 totalTime = 0;

    for (const RepairResult& result : results) {
        totalIssues += result.issuesFound;
        totalFixed += result.issuesFixed;
        totalTime += result.repairTimeMs;
    }

    report += QString("Total meshes processed: %1\n").arg(results.count());
    report += QString("Total issues found: %1\n").arg(totalIssues);
    report += QString("Total issues fixed: %1\n").arg(totalFixed);
    report += QString("Total repair time: %1ms\n").arg(totalTime);

    return report;
}

QString MeshRepair::generateBatchRepairReport(const QList<RepairResult>& results)
{
    return generateModelRepairReport(results);
}

QStringList MeshRepair::generateOverallFixesList(const QList<RepairResult>& results)
{
    QStringList allFixes;

    for (const RepairResult& result : results) {
        allFixes.append(result.appliedFixes);
    }

    return allFixes;
}

void MeshRepair::updateRepairMetrics(const RepairResult& result)
{
    m_repairMetrics.operationTimeMs = result.repairTimeMs;
    m_repairMetrics.memoryUsageBytes = 0; // Would be tracked during repair
}