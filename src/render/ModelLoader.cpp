#include "ModelLoader.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

// Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/mesh.h>

ModelLoader::ModelLoader(QObject* parent)
    : QObject(parent)
    , m_importer(new Assimp::Importer())
    , m_maxFileSize(500 * 1024 * 1024) // 500MB default limit
    , m_flipUVs(true)
    , m_triangulate(true)
    , m_calculateTangents(true)
{
    // Initialize supported formats
    m_supportedFormats << "stl" << "obj" << "ply" << "3mf" << "fbx" << "dae" << "gltf" << "glb"
                      << "x3d" << "blend" << "3ds" << "lwo" << "lxo" << "x" << "ms3d" << "cob"
                      << "scn" << "ase" << "md5" << "smd" << "vta" << "m3" << "ac" << "bvh";

    qRegisterMetaType<ModelData>("ModelData");
    qRegisterMetaType<MeshData>("MeshData");
}

ModelLoader::~ModelLoader()
{
    if (m_importer) {
        delete m_importer;
        m_importer = nullptr;
    }
}

QFuture<ModelData> ModelLoader::loadModelAsync(const QString& filepath)
{
    return QtConcurrent::run([this, filepath]() -> ModelData {
        return loadModel(filepath);
    });
}

ModelData ModelLoader::loadModel(const QString& filepath)
{
    QFileInfo fileInfo(filepath);

    if (!fileInfo.exists()) {
        emit modelLoadFailed(filepath, "File does not exist");
        return ModelData();
    }

    if (!isFormatSupported(filepath)) {
        emit modelLoadFailed(filepath, "Unsupported file format");
        return ModelData();
    }

    qint64 fileSize = fileInfo.size();
    if (fileSize > m_maxFileSize) {
        emit largeModelDetected(filepath, fileSize);
    }

    // Check memory availability
    qint64 estimatedMemory = estimateMemoryUsage(filepath);
    if (!checkMemoryAvailability(estimatedMemory)) {
        emit memoryWarning(estimatedMemory, getAvailableMemory());
        emit modelLoadFailed(filepath, "Insufficient memory");
        return ModelData();
    }

    emit loadingProgress(filepath, 0, "Initializing importer...");

    // Configure Assimp importer
    unsigned int importFlags = 0;

    if (m_triangulate) {
        importFlags |= aiProcess_Triangulate;
    }

    if (m_flipUVs) {
        importFlags |= aiProcess_FlipUVs;
    }

    importFlags |= aiProcess_GenSmoothNormals;
    importFlags |= aiProcess_CalcTangentSpace;
    importFlags |= aiProcess_JoinIdenticalVertices;
    importFlags |= aiProcess_ImproveCacheLocality;
    importFlags |= aiProcess_LimitBoneWeights;
    importFlags |= aiProcess_RemoveRedundantMaterials;
    importFlags |= aiProcess_SplitLargeMeshes;
    importFlags |= aiProcess_GenUVCoords;
    importFlags |= aiProcess_FindDegenerates;
    importFlags |= aiProcess_FindInvalidData;
    importFlags |= aiProcess_OptimizeMeshes;

    emit loadingProgress(filepath, 10, "Loading model file...");

    // Load the model
    const aiScene* scene = m_importer->ReadFile(filepath.toStdString(), importFlags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        QString error = QString("Assimp error: %1").arg(m_importer->GetErrorString());
        emit modelLoadFailed(filepath, error);
        return ModelData();
    }

    emit loadingProgress(filepath, 30, "Processing scene...");

    // Create model data structure
    ModelData model;
    model.filename = fileInfo.fileName();
    model.sourcePath = filepath;
    model.scene = scene;
    model.format = detectFormat(filepath);
    model.fileSize = fileSize;
    model.importTime = QDateTime::currentDateTime();

    // Process the scene
    processModel(model);

    emit loadingProgress(filepath, 90, "Finalizing model...");

    // Calculate model bounds
    calculateModelBounds(model);

    // Clean up importer for next use
    m_importer->FreeScene();

    emit loadingProgress(filepath, 100, "Model loaded successfully");
    emit modelLoaded(model);

    return model;
}

bool ModelLoader::saveModel(const ModelData& model, const QString& filepath)
{
    // For now, this is a placeholder
    // In a full implementation, this would export the model using Assimp
    qWarning() << "Model saving not yet implemented";
    return false;
}

QStringList ModelLoader::getSupportedFormats() const
{
    return m_supportedFormats;
}

QString ModelLoader::detectFormat(const QString& filepath) const
{
    QFileInfo fileInfo(filepath);
    QString extension = fileInfo.suffix().toLower();

    // Map common extensions to format names
    if (extension == "stl") return "STL";
    if (extension == "obj") return "Wavefront OBJ";
    if (extension == "ply") return "Stanford PLY";
    if (extension == "3mf") return "3D Manufacturing Format";
    if (extension == "fbx") return "FBX";
    if (extension == "dae") return "COLLADA";
    if (extension == "gltf" || extension == "glb") return "glTF";
    if (extension == "x3d") return "X3D";
    if (extension == "blend") return "Blender";
    if (extension == "3ds") return "3D Studio Max";

    return extension.toUpper();
}

bool ModelLoader::isFormatSupported(const QString& filepath) const
{
    QFileInfo fileInfo(filepath);
    QString extension = fileInfo.suffix().toLower();
    return m_supportedFormats.contains(extension);
}

void ModelLoader::processModel(ModelData& model)
{
    const aiScene* scene = model.scene;

    // Process materials first
    model.materialNames.reserve(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        processMaterial(model, scene->mMaterials[i], i);
    }

    // Process meshes
    model.meshes.reserve(scene->mNumMeshes);
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        MeshData mesh = processMesh(scene->mMeshes[i], scene);
        model.meshes.append(mesh);

        model.totalVertices += mesh.vertexCount;
        model.totalTriangles += mesh.triangleCount;

        emit meshProcessed(mesh.name, i, scene->mNumMeshes);
    }

    // Process node hierarchy
    if (scene->mRootNode) {
        processNode(model, scene->mRootNode, scene);
    }
}

void ModelLoader::processNode(ModelData& model, aiNode* node, const aiScene* scene)
{
    // Process all meshes in this node
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        unsigned int meshIndex = node->mMeshes[i];
        if (meshIndex < model.meshes.size()) {
            MeshData& mesh = model.meshes[meshIndex];

            // Apply node transformation
            aiMatrix4x4 transform = node->mTransformation;
            QMatrix4x4 qtTransform = aiMatrix4x4ToQMatrix(transform);

            // Transform mesh vertices
            for (Vertex& vertex : mesh.vertices) {
                QVector4D pos(vertex.position, 1.0f);
                pos = qtTransform * pos;
                vertex.position = pos.toVector3D();
            }

            // Update mesh bounds
            updateMeshBounds(mesh);
        }
    }

    // Process child nodes
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(model, node->mChildren[i], scene);
    }
}

MeshData ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene)
{
    MeshData meshData;
    meshData.name = mesh->mName.C_Str();
    if (meshData.name.isEmpty()) {
        meshData.name = QString("Mesh_%1").arg(mesh->mName.C_Str());
    }

    meshData.vertexCount = mesh->mNumVertices;
    meshData.triangleCount = mesh->mNumFaces;

    // Process vertices
    meshData.vertices.reserve(mesh->mNumVertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;

        // Position
        if (mesh->HasPositions()) {
            aiVector3D pos = mesh->mVertices[i];
            vertex.position = aiVector3DToQVector(pos);
        }

        // Normal
        if (mesh->HasNormals()) {
            aiVector3D normal = mesh->mNormals[i];
            vertex.normal = aiVector3DToQVector(normal);
        }

        // Texture coordinates
        if (mesh->HasTextureCoords(0)) {
            aiVector3D texCoord = mesh->mTextureCoords[0][i];
            vertex.texCoord = QVector2D(texCoord.x, texCoord.y);
        }

        // Tangent and bitangent
        if (mesh->HasTangentsAndBitangents()) {
            aiVector3D tangent = mesh->mTangents[i];
            aiVector3D bitangent = mesh->mBitangents[i];
            vertex.tangent = aiVector3DToQVector(tangent);
            vertex.bitangent = aiVector3DToQVector(bitangent);
        }

        meshData.vertices.append(vertex);
    }

    // Process indices
    meshData.indices.reserve(mesh->mNumFaces * 3);

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];

        if (face.mNumIndices == 3) { // Triangle
            meshData.indices.append(face.mIndices[0]);
            meshData.indices.append(face.mIndices[1]);
            meshData.indices.append(face.mIndices[2]);
        }
    }

    // Get material name
    if (mesh->mMaterialIndex >= 0 && mesh->mMaterialIndex < scene->mNumMaterials) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString materialName;
        if (material->Get(AI_MATKEY_NAME, materialName) == AI_SUCCESS) {
            meshData.materialName = materialName.C_Str();
        }
    }

    // Calculate initial bounds
    calculateMeshBounds(meshData);

    return meshData;
}

void ModelLoader::processMaterial(ModelData& model, aiMaterial* material, unsigned int index)
{
    aiString name;
    if (material->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
        model.materialNames.append(name.C_Str());
    } else {
        model.materialNames.append(QString("Material_%1").arg(index));
    }

    // Process material properties
    aiColor3D diffuseColor(1.0f, 1.0f, 1.0f);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);

    aiColor3D specularColor(0.0f, 0.0f, 0.0f);
    material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);

    aiColor3D ambientColor(0.0f, 0.0f, 0.0f);
    material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);

    float shininess = 32.0f;
    material->Get(AI_MATKEY_SHININESS, shininess);

    float opacity = 1.0f;
    material->Get(AI_MATKEY_OPACITY, opacity);

    // Store material properties in the corresponding mesh
    if (index < model.meshes.size()) {
        MeshData& mesh = model.meshes[index];
        mesh.diffuseColor = aiVector3DToQVector(diffuseColor);
        mesh.specularColor = aiVector3DToQVector(specularColor);
        mesh.ambientColor = aiVector3DToQVector(ambientColor);
        mesh.shininess = shininess;
        mesh.opacity = opacity;
    }

    // Process textures
    processMaterialTextures(model, material, index);

    emit materialProcessed(model.materialNames[index], index, model.materialNames.size());
}

void ModelLoader::processMaterialTextures(ModelData& model, aiMaterial* material, unsigned int meshIndex)
{
    if (meshIndex >= model.meshes.size()) {
        return;
    }

    MeshData& mesh = model.meshes[meshIndex];

    // Process different texture types
    QStringList textureTypes = {
        aiTextureType_DIFFUSE,
        aiTextureType_SPECULAR,
        aiTextureType_NORMALS,
        aiTextureType_HEIGHT
    };

    QStringList textureNames = {
        "diffuseTexture",
        "specularTexture",
        "normalTexture",
        "heightTexture"
    };

    for (int i = 0; i < textureTypes.size() && i < textureNames.size(); ++i) {
        aiTextureType textureType = static_cast<aiTextureType>(i);

        for (unsigned int j = 0; j < material->GetTextureCount(textureType); ++j) {
            aiString texturePath;
            if (material->GetTexture(textureType, j, &texturePath) == AI_SUCCESS) {
                QString textureFile = extractTexturePath(model.sourcePath, texturePath.C_Str());

                if (!textureFile.isEmpty()) {
                    mesh.setProperty(textureNames[i].toUtf8(), textureFile);
                }
            }
        }
    }
}

void ModelLoader::calculateTangents(MeshData& mesh)
{
    if (mesh.vertices.isEmpty()) {
        return;
    }

    // Calculate tangents and bitangents for normal mapping
    for (int i = 0; i < mesh.vertices.size(); ++i) {
        Vertex& vertex = mesh.vertices[i];

        // Simplified tangent calculation
        // In a full implementation, this would use proper tangent space calculation
        if (vertex.texCoord.x() > 0.5f) {
            vertex.tangent = QVector3D(1.0f, 0.0f, 0.0f);
        } else {
            vertex.tangent = QVector3D(-1.0f, 0.0f, 0.0f);
        }

        // Calculate bitangent as cross product of normal and tangent
        vertex.bitangent = QVector3D::crossProduct(vertex.normal, vertex.tangent);
    }
}

void ModelLoader::optimizeMesh(MeshData& mesh)
{
    // Remove duplicate vertices
    QMap<QString, int> vertexMap;

    for (int i = 0; i < mesh.vertices.size(); ++i) {
        Vertex& vertex = mesh.vertices[i];
        QString key = QString("%1_%2_%3_%4_%5_%6")
                     .arg(vertex.position.x()).arg(vertex.position.y()).arg(vertex.position.z())
                     .arg(vertex.normal.x()).arg(vertex.normal.y()).arg(vertex.normal.z());

        if (vertexMap.contains(key)) {
            // Duplicate vertex found, reuse index
            int originalIndex = vertexMap[key];
            // Update indices to point to original vertex
            for (int& index : mesh.indices) {
                if (index == i) {
                    index = originalIndex;
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
}

void ModelLoader::processMaterials(ModelData& model)
{
    // Process materials for all meshes
    for (int i = 0; i < model.meshes.size(); ++i) {
        MeshData& mesh = model.meshes[i];

        if (i < model.scene->mNumMaterials) {
            processMaterial(model, model.scene->mMaterials[i], i);
        }
    }
}

QString ModelLoader::extractTexturePath(const QString& modelPath, const QString& textureName)
{
    QFileInfo modelInfo(modelPath);
    QDir modelDir = modelInfo.dir();

    // Try to find texture file in various locations
    QStringList searchPaths = {
        modelDir.absolutePath(),
        modelDir.absolutePath() + "/textures",
        modelDir.absolutePath() + "/../textures",
        modelDir.absolutePath() + "/materials"
    };

    for (const QString& searchPath : searchPaths) {
        QDir dir(searchPath);
        QStringList filters = {textureName + ".*", "*" + textureName + ".*"};

        for (const QString& filter : filters) {
            QStringList files = dir.entryList(QStringList() << filter, QDir::Files);
            if (!files.isEmpty()) {
                return dir.absoluteFilePath(files.first());
            }
        }
    }

    return QString(); // Texture not found
}

PerformanceMetrics ModelLoader::getLoadingMetrics() const
{
    PerformanceMetrics metrics;
    metrics.operationType = "ModelLoader";
    metrics.operationTimeMs = 0; // Would be calculated from actual operations
    metrics.memoryUsageBytes = 0; // Would be tracked during loading

    return metrics;
}

void ModelLoader::setMaxFileSize(qint64 maxSizeBytes)
{
    m_maxFileSize = maxSizeBytes;
}

qint64 ModelLoader::getMaxFileSize() const
{
    return m_maxFileSize;
}

// Protected helper methods
QMatrix4x4 ModelLoader::aiMatrix4x4ToQMatrix(const aiMatrix4x4& matrix) const
{
    QMatrix4x4 qtMatrix;

    qtMatrix(0, 0) = matrix.a1; qtMatrix(0, 1) = matrix.a2; qtMatrix(0, 2) = matrix.a3; qtMatrix(0, 3) = matrix.a4;
    qtMatrix(1, 0) = matrix.b1; qtMatrix(1, 1) = matrix.b2; qtMatrix(1, 2) = matrix.b3; qtMatrix(1, 3) = matrix.b4;
    qtMatrix(2, 0) = matrix.c1; qtMatrix(2, 1) = matrix.c2; qtMatrix(2, 2) = matrix.c3; qtMatrix(2, 3) = matrix.c4;
    qtMatrix(3, 0) = matrix.d1; qtMatrix(3, 1) = matrix.d2; qtMatrix(3, 2) = matrix.d3; qtMatrix(3, 3) = matrix.d4;

    return qtMatrix;
}

QVector3D ModelLoader::aiVector3DToQVector(const aiVector3D& vector) const
{
    return QVector3D(vector.x, vector.y, vector.z);
}

QQuaternion ModelLoader::aiQuaternionToQQuaternion(const aiQuaternion& quaternion) const
{
    return QQuaternion(quaternion.w, quaternion.x, quaternion.y, quaternion.z);
}

qint64 ModelLoader::estimateMemoryUsage(const QString& filepath) const
{
    QFileInfo fileInfo(filepath);
    qint64 fileSize = fileInfo.size();

    // Estimate memory usage: typically 3-5x file size for processed mesh data
    return fileSize * 4;
}

bool ModelLoader::checkMemoryAvailability(qint64 requiredBytes) const
{
    // Get available memory (simplified)
    // In a full implementation, this would check system memory
    return requiredBytes < (1024 * 1024 * 1024); // 1GB limit for now
}

qint64 ModelLoader::getAvailableMemory() const
{
    // Simplified memory check
    // In a full implementation, this would query system memory
    return 1024 * 1024 * 1024; // 1GB
}

bool ModelLoader::validateModelFile(const QString& filepath) const
{
    QFile file(filepath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // Read first few bytes to validate file format
    QByteArray header = file.read(1024);

    // Check for common 3D file signatures
    if (header.startsWith("solid") || header.contains("facet normal")) {
        return true; // STL file
    }

    if (header.startsWith("#") || header.contains("v ") || header.contains("vt ") || header.contains("vn ")) {
        return true; // OBJ file
    }

    if (header.startsWith("ply") || header.contains("element vertex")) {
        return true; // PLY file
    }

    // For other formats, rely on extension check
    return isFormatSupported(filepath);
}

QStringList ModelLoader::getSupportedExtensions() const
{
    return m_supportedFormats;
}

void ModelLoader::calculateMeshBounds(MeshData& mesh)
{
    if (mesh.vertices.isEmpty()) {
        return;
    }

    mesh.minBounds = mesh.vertices[0].position;
    mesh.maxBounds = mesh.vertices[0].position;

    for (const Vertex& vertex : mesh.vertices) {
        mesh.minBounds.setX(qMin(mesh.minBounds.x(), vertex.position.x()));
        mesh.minBounds.setY(qMin(mesh.minBounds.y(), vertex.position.y()));
        mesh.minBounds.setZ(qMin(mesh.minBounds.z(), vertex.position.z()));

        mesh.maxBounds.setX(qMax(mesh.maxBounds.x(), vertex.position.x()));
        mesh.maxBounds.setY(qMax(mesh.maxBounds.y(), vertex.position.y()));
        mesh.maxBounds.setZ(qMax(mesh.maxBounds.z(), vertex.position.z()));
    }
}

void ModelLoader::updateMeshBounds(MeshData& mesh)
{
    // Recalculate bounds after transformations
    calculateMeshBounds(mesh);
}

void ModelLoader::calculateModelBounds(ModelData& model)
{
    if (model.meshes.isEmpty()) {
        return;
    }

    model.modelBoundsMin = model.meshes[0].minBounds;
    model.modelBoundsMax = model.meshes[0].maxBounds;

    for (const MeshData& mesh : model.meshes) {
        model.modelBoundsMin.setX(qMin(model.modelBoundsMin.x(), mesh.minBounds.x()));
        model.modelBoundsMin.setY(qMin(model.modelBoundsMin.y(), mesh.minBounds.y()));
        model.modelBoundsMin.setZ(qMin(model.modelBoundsMin.z(), mesh.minBounds.z()));

        model.modelBoundsMax.setX(qMax(model.modelBoundsMax.x(), mesh.maxBounds.x()));
        model.modelBoundsMax.setY(qMax(model.modelBoundsMax.y(), mesh.maxBounds.y()));
        model.modelBoundsMax.setZ(qMax(model.modelBoundsMax.z(), mesh.maxBounds.z()));
    }
}