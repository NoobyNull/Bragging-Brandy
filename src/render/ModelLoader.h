#pragma once

#include "../core/BaseTypes.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QFuture>

// Forward declarations
struct aiScene;
struct aiMesh;
struct aiMaterial;
struct aiNode;

namespace Assimp {
    class Importer;
}

/**
 * @brief 3D Model data structures for rendering
 */
struct Vertex {
    QVector3D position;
    QVector3D normal;
    QVector2D texCoord;
    QVector3D tangent;
    QVector3D bitangent;

    Vertex() = default;
    Vertex(const QVector3D& pos, const QVector3D& norm = QVector3D(0, 0, 1),
           const QVector2D& tex = QVector2D(0, 0))
        : position(pos), normal(norm), texCoord(tex) {}
};

struct MeshData {
    QString name;
    QString materialName;

    // Vertex data
    QVector<Vertex> vertices;
    QVector<unsigned int> indices;

    // Material properties
    QVector3D diffuseColor;
    QVector3D specularColor;
    QVector3D ambientColor;
    float shininess;
    float opacity;

    // Texture information
    QString diffuseTexture;
    QString specularTexture;
    QString normalTexture;

    // OpenGL buffers
    QOpenGLBuffer vertexBuffer;
    QOpenGLBuffer indexBuffer;
    QOpenGLVertexArrayObject vao;

    // Bounding box
    QVector3D minBounds;
    QVector3D maxBounds;

    // Statistics
    int vertexCount;
    int triangleCount;

    MeshData() : shininess(32.0f), opacity(1.0f), vertexCount(0), triangleCount(0) {}
};

struct ModelData {
    QString filename;
    QString sourcePath;
    const aiScene* scene;

    // Mesh data
    QVector<MeshData> meshes;

    // Hierarchy information
    QStringList materialNames;
    QMatrix4x4 globalTransform;

    // Model statistics
    int totalVertices;
    int totalTriangles;
    QVector3D modelBoundsMin;
    QVector3D modelBoundsMax;

    // Metadata
    QString format;
    qint64 fileSize;
    QDateTime importTime;

    ModelData() : scene(nullptr), totalVertices(0), totalTriangles(0) {}
};

/**
 * @brief 3D Model Loader using Assimp library
 *
 * Handles loading and processing of 3D models from various file formats
 * with support for large files and progressive loading.
 */
class ModelLoader : public QObject
{
    Q_OBJECT

public:
    explicit ModelLoader(QObject* parent = nullptr);
    virtual ~ModelLoader();

    // Model loading
    virtual QFuture<ModelData> loadModelAsync(const QString& filepath) = 0;
    virtual ModelData loadModel(const QString& filepath) = 0;
    virtual bool saveModel(const ModelData& model, const QString& filepath) = 0;

    // Supported formats
    virtual QStringList getSupportedFormats() const = 0;
    virtual QString detectFormat(const QString& filepath) const = 0;
    virtual bool isFormatSupported(const QString& filepath) const = 0;

    // Model processing
    virtual void processModel(ModelData& model) = 0;
    virtual void calculateTangents(MeshData& mesh) = 0;
    virtual void optimizeMesh(MeshData& mesh) = 0;

    // Material processing
    virtual void processMaterials(ModelData& model) = 0;
    virtual QString extractTexturePath(const QString& modelPath, const QString& textureName) = 0;

    // Performance monitoring
    virtual PerformanceMetrics getLoadingMetrics() const = 0;

    // Large model support
    virtual void setMaxFileSize(qint64 maxSizeBytes) = 0;
    virtual qint64 getMaxFileSize() const = 0;

signals:
    // Loading progress
    void loadingProgress(const QString& filename, int percentage, const QString& status);
    void meshProcessed(const QString& meshName, int meshIndex, int totalMeshes);
    void materialProcessed(const QString& materialName, int materialIndex, int totalMaterials);

    // Loading events
    void modelLoaded(const ModelData& model);
    void modelLoadFailed(const QString& filepath, const QString& error);

    // Performance events
    void largeModelDetected(const QString& filepath, qint64 fileSize);
    void memoryWarning(qint64 requiredMemory, qint64 availableMemory);

protected:
    // Assimp processing
    virtual void processNode(ModelData& model, aiNode* node, const aiScene* scene) = 0;
    virtual MeshData processMesh(aiMesh* mesh, const aiScene* scene) = 0;
    virtual void processMaterial(ModelData& model, aiMaterial* material, unsigned int index) = 0;

    // Helper methods
    virtual QMatrix4x4 aiMatrix4x4ToQMatrix(const aiMatrix4x4& matrix) const = 0;
    virtual QVector3D aiVector3DToQVector(const aiVector3D& vector) const = 0;
    virtual QQuaternion aiQuaternionToQQuaternion(const aiQuaternion& quaternion) const = 0;

    // Memory management
    virtual qint64 estimateMemoryUsage(const QString& filepath) const;
    virtual bool checkMemoryAvailability(qint64 requiredBytes) const;

    // File validation
    virtual bool validateModelFile(const QString& filepath) const;
    virtual QStringList getSupportedExtensions() const;

    // Assimp importer
    Assimp::Importer* m_importer;

    // Configuration
    qint64 m_maxFileSize;
    QStringList m_supportedFormats;
    bool m_flipUVs;
    bool m_triangulate;
    bool m_calculateTangents;
};