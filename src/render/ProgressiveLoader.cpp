#include "ProgressiveLoader.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QDebug>

ProgressiveLoader::ProgressiveLoader(QObject* parent)
    : QObject(parent)
    , m_loadingThread(nullptr)
    , m_updateTimer(nullptr)
    , m_loadingState(LoadingState::Idle)
    , m_loadingProgress(0)
    , m_cancelRequested(false)
    , m_loadingMode(LoadingMode::Progressive)
    , m_maxMemoryUsage(2LL * 1024 * 1024 * 1024) // 2GB default
    , m_chunkSize(10000) // 10k vertices per chunk
    , m_updateInterval(100) // 100ms updates
    , m_memoryUsage(0)
    , m_qualityLevel(1.0f)
    , m_interactionLevel(0.0f)
{
    // Create loading thread
    m_loadingThread = new QThread(this);
    m_loadingThread->setObjectName("ProgressiveLoader");

    // Create update timer
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(m_updateInterval);
    connect(m_updateTimer, &QTimer::timeout, this, &ProgressiveLoader::onUpdateTimeout);

    // Move to thread
    this->moveToThread(m_loadingThread);

    // Start thread
    m_loadingThread->start();
}

ProgressiveLoader::~ProgressiveLoader()
{
    cancelLoading();

    if (m_loadingThread) {
        m_loadingThread->quit();
        m_loadingThread->wait(5000); // Wait up to 5 seconds
        delete m_loadingThread;
    }
}

void ProgressiveLoader::setLoadingMode(LoadingMode mode)
{
    QMetaObject::invokeMethod(this, "setLoadingModeImpl", Qt::QueuedConnection,
                             Q_ARG(LoadingMode, mode));
}

void ProgressiveLoader::setLoadingModeImpl(LoadingMode mode)
{
    if (m_loadingState != LoadingState::Idle && m_loadingState != LoadingState::Completed) {
        qWarning() << "Cannot change loading mode during active loading";
        return;
    }

    m_loadingMode = mode;
}

void ProgressiveLoader::setMaxMemoryUsage(qint64 maxBytes)
{
    QMetaObject::invokeMethod(this, "setMaxMemoryUsageImpl", Qt::QueuedConnection,
                             Q_ARG(qint64, maxBytes));
}

void ProgressiveLoader::setMaxMemoryUsageImpl(qint64 maxBytes)
{
    m_maxMemoryUsage = maxBytes;

    // If current usage exceeds new limit, optimize memory
    if (m_memoryUsage > m_maxMemoryUsage) {
        optimizeMemory();
    }
}

void ProgressiveLoader::setChunkSize(int chunkSize)
{
    QMetaObject::invokeMethod(this, "setChunkSizeImpl", Qt::QueuedConnection,
                             Q_ARG(int, chunkSize));
}

void ProgressiveLoader::setChunkSizeImpl(int chunkSize)
{
    m_chunkSize = qMax(1000, chunkSize); // Minimum 1k vertices per chunk
}

void ProgressiveLoader::setUpdateInterval(int milliseconds)
{
    QMetaObject::invokeMethod(this, "setUpdateIntervalImpl", Qt::QueuedConnection,
                             Q_ARG(int, milliseconds));
}

void ProgressiveLoader::setUpdateIntervalImpl(int milliseconds)
{
    m_updateInterval = qMax(50, milliseconds); // Minimum 50ms
    m_updateTimer->setInterval(m_updateInterval);
}

QFuture<ModelData> ProgressiveLoader::loadProgressiveAsync(const QString& filepath)
{
    return QtConcurrent::run([this, filepath]() -> ModelData {
        QMetaObject::invokeMethod(this, "startProgressiveLoading", Qt::QueuedConnection,
                                 Q_ARG(QString, filepath));
        return ModelData(); // Will be updated via signals
    });
}

void ProgressiveLoader::cancelLoading()
{
    QMetaObject::invokeMethod(this, "cancelLoadingImpl", Qt::QueuedConnection);
}

void ProgressiveLoader::cancelLoadingImpl()
{
    if (m_loadingState == LoadingState::Idle || m_loadingState == LoadingState::Completed) {
        return;
    }

    m_cancelRequested = true;
    m_loadingState = LoadingState::Cancelled;

    emit loadingStateChanged(m_loadingState);
    emit loadingCancelled();

    qInfo() << "Progressive loading cancelled";
}

ProgressiveLoader::LoadingState ProgressiveLoader::getLoadingState() const
{
    return m_loadingState;
}

int ProgressiveLoader::getLoadingProgress() const
{
    return m_loadingProgress;
}

ModelData ProgressiveLoader::getCurrentModel() const
{
    QMutexLocker locker(&m_loadingMutex);
    return m_currentModel;
}

bool ProgressiveLoader::canInteract() const
{
    return m_interactionLevel > 0.5f && m_loadingState != LoadingState::Idle;
}

void ProgressiveLoader::setInteractionLevel(float level)
{
    QMetaObject::invokeMethod(this, "setInteractionLevelImpl", Qt::QueuedConnection,
                             Q_ARG(float, level));
}

void ProgressiveLoader::setInteractionLevelImpl(float level)
{
    m_interactionLevel = qBound(0.0f, level, 1.0f);

    // Adjust loading parameters based on interaction level
    if (m_interactionLevel > 0.7f) {
        // High interaction: prioritize responsiveness
        m_updateInterval = 50;
        m_chunkSize = 5000;
    } else if (m_interactionLevel > 0.3f) {
        // Medium interaction: balance speed and responsiveness
        m_updateInterval = 100;
        m_chunkSize = 10000;
    } else {
        // Low interaction: prioritize speed
        m_updateInterval = 200;
        m_chunkSize = 20000;
    }

    m_updateTimer->setInterval(m_updateInterval);
}

qint64 ProgressiveLoader::getMemoryUsage() const
{
    return m_memoryUsage;
}

void ProgressiveLoader::optimizeMemory()
{
    QMetaObject::invokeMethod(this, "optimizeMemoryImpl", Qt::QueuedConnection);
}

void ProgressiveLoader::optimizeMemoryImpl()
{
    if (m_loadingState == LoadingState::Idle) {
        return;
    }

    // Deallocate unused mesh chunks
    deallocateUnusedChunks();

    // Reduce quality if memory usage is still high
    if (m_memoryUsage > m_maxMemoryUsage * 0.9f) {
        float qualityReduction = (m_memoryUsage - m_maxMemoryUsage * 0.9f) / (m_maxMemoryUsage * 0.1f);
        qualityReduction = qMin(qualityReduction, 0.5f); // Don't reduce below 50%

        setQualityLevel(m_qualityLevel * (1.0f - qualityReduction));
    }

    qint64 freedMemory = m_memoryUsage - getMemoryUsage();
    if (freedMemory > 0) {
        qInfo() << "Memory optimized, freed:" << freedMemory << "bytes";
    }
}

void ProgressiveLoader::freeUnusedMemory()
{
    QMetaObject::invokeMethod(this, "freeUnusedMemoryImpl", Qt::QueuedConnection);
}

void ProgressiveLoader::freeUnusedMemoryImpl()
{
    // Force garbage collection of unused data
    QCoreApplication::processEvents();

    // Clear any temporary buffers
    m_memoryUsage = getMemoryUsage();

    qInfo() << "Unused memory freed, current usage:" << m_memoryUsage << "bytes";
}

void ProgressiveLoader::setQualityLevel(float level)
{
    QMetaObject::invokeMethod(this, "setQualityLevelImpl", Qt::QueuedConnection,
                             Q_ARG(float, level));
}

void ProgressiveLoader::setQualityLevelImpl(float level)
{
    float oldLevel = m_qualityLevel;
    m_qualityLevel = qBound(0.1f, level, 1.0f);

    if (qAbs(m_qualityLevel - oldLevel) > 0.01f) {
        emit qualityChanged(m_qualityLevel);

        // Adjust loading parameters based on quality
        if (m_qualityLevel > 0.8f) {
            // High quality: smaller chunks for better detail
            m_chunkSize = qMax(1000, m_chunkSize / 2);
        } else if (m_qualityLevel < 0.4f) {
            // Low quality: larger chunks for speed
            m_chunkSize = qMin(50000, m_chunkSize * 2);
        }
    }
}

float ProgressiveLoader::getQualityLevel() const
{
    return m_qualityLevel;
}

void ProgressiveLoader::startProgressiveLoading(const QString& filepath)
{
    if (m_loadingState != LoadingState::Idle) {
        qWarning() << "Progressive loading already in progress";
        return;
    }

    m_cancelRequested = false;
    m_loadingState = LoadingState::Initializing;
    m_loadingProgress = 0;
    m_currentFilepath = filepath;

    emit loadingStateChanged(m_loadingState);
    emit loadingProgress(0, "Initializing...");

    // Start update timer
    m_updateTimer->start();

    qInfo() << "Starting progressive loading of:" << filepath;

    // Load model in stages
    QTimer::singleShot(100, this, [this, filepath]() {
        loadModelInStages(filepath);
    });
}

void ProgressiveLoader::loadModelInStages(const QString& filepath)
{
    QElapsedTimer totalTimer;
    totalTimer.start();

    try {
        // Stage 1: Initialize and load basic structure
        m_loadingState = LoadingState::LoadingCoarse;
        emit loadingStateChanged(m_loadingState);
        emit loadingProgress(10, "Loading coarse model...");

        if (m_cancelRequested) {
            finalizeLoading();
            return;
        }

        // Load basic model structure (vertices only, no indices)
        loadCoarseModel(filepath);

        if (m_cancelRequested) {
            finalizeLoading();
            return;
        }

        // Stage 2: Load medium detail
        m_loadingState = LoadingState::LoadingMedium;
        emit loadingStateChanged(m_loadingState);
        emit loadingProgress(40, "Loading medium detail...");

        loadMediumDetail();

        if (m_cancelRequested) {
            finalizeLoading();
            return;
        }

        // Stage 3: Load fine detail
        m_loadingState = LoadingState::LoadingFine;
        emit loadingStateChanged(m_loadingState);
        emit loadingProgress(70, "Loading fine detail...");

        loadFineDetail();

        if (m_cancelRequested) {
            finalizeLoading();
            return;
        }

        // Stage 4: Optimize and finalize
        m_loadingState = LoadingState::Optimizing;
        emit loadingStateChanged(m_loadingState);
        emit loadingProgress(90, "Optimizing model...");

        optimizeLoadedModel();

        // Final stage
        m_loadingState = LoadingState::Completed;
        m_loadingProgress = 100;
        emit loadingStateChanged(m_loadingState);
        emit loadingProgress(100, "Loading completed");

        qint64 elapsed = totalTimer.elapsed();
        qInfo() << QString("Progressive loading completed in %1ms").arg(elapsed);

    } catch (const std::exception& e) {
        m_loadingState = LoadingState::Failed;
        emit loadingStateChanged(m_loadingState);
        emit loadingFailed(QString("Loading error: %1").arg(e.what()));

        qCritical() << "Progressive loading failed:" << e.what();
    }

    finalizeLoading();
}

void ProgressiveLoader::loadCoarseModel(const QString& filepath)
{
    // Load basic model structure using ModelLoader
    ModelLoader loader;
    ModelData model = loader.loadModel(filepath);

    if (model.meshes.isEmpty()) {
        throw std::runtime_error("No meshes found in model");
    }

    QMutexLocker locker(&m_loadingMutex);

    // Store basic model data
    m_currentModel = model;

    // For coarse loading, only keep every Nth vertex
    int decimationFactor = qMax(1, m_chunkSize / 1000);

    for (MeshData& mesh : m_currentModel.meshes) {
        if (mesh.vertices.size() > 10000) {
            // Decimate vertices for coarse view
            QVector<Vertex> coarseVertices;
            for (int i = 0; i < mesh.vertices.size(); i += decimationFactor) {
                coarseVertices.append(mesh.vertices[i]);
            }
            mesh.vertices = coarseVertices;
            mesh.vertexCount = coarseVertices.size();

            // Update indices accordingly
            mesh.indices.clear();
            for (int i = 0; i < mesh.vertexCount - 2; i += 3) {
                mesh.indices.append(i);
                mesh.indices.append(i + 1);
                mesh.indices.append(i + 2);
            }
            mesh.triangleCount = mesh.indices.size() / 3;
        }
    }

    m_memoryUsage = calculateModelMemoryUsage(m_currentModel);

    emit modelUpdated(m_currentModel);
    emit memoryUsageChanged(m_memoryUsage);
}

void ProgressiveLoader::loadMediumDetail()
{
    QMutexLocker locker(&m_loadingMutex);

    if (m_currentModel.meshes.isEmpty()) {
        return;
    }

    // Add more vertices and improve mesh quality
    int refinementFactor = 2;

    for (MeshData& mesh : m_currentModel.meshes) {
        if (mesh.vertices.size() < 50000) { // Don't refine very large meshes yet
            // Add intermediate vertices for smoother appearance
            refineMeshVertices(mesh, refinementFactor);
        }
    }

    m_memoryUsage = calculateModelMemoryUsage(m_currentModel);

    emit modelUpdated(m_currentModel);
    emit memoryUsageChanged(m_memoryUsage);
}

void ProgressiveLoader::loadFineDetail()
{
    QMutexLocker locker(&m_loadingMutex);

    if (m_currentModel.meshes.isEmpty()) {
        return;
    }

    // Load full detail for visible meshes
    for (MeshData& mesh : m_currentModel.meshes) {
        // Load full vertex and index data
        loadFullMeshDetail(mesh);
    }

    m_memoryUsage = calculateModelMemoryUsage(m_currentModel);

    emit modelUpdated(m_currentModel);
    emit memoryUsageChanged(m_memoryUsage);
}

void ProgressiveLoader::optimizeLoadedModel()
{
    QMutexLocker locker(&m_loadingMutex);

    // Optimize meshes for rendering
    for (MeshData& mesh : m_currentModel.meshes) {
        // Generate normals if missing
        if (mesh.vertices.size() > 0 && mesh.vertices[0].normal.isNull()) {
            generateMeshNormals(mesh);
        }

        // Calculate tangents for normal mapping
        if (m_qualityLevel > 0.7f) {
            calculateTangents(mesh);
        }

        // Optimize vertex cache
        optimizeVertexCache(mesh);
    }

    // Update memory usage
    m_memoryUsage = calculateModelMemoryUsage(m_currentModel);

    emit modelUpdated(m_currentModel);
    emit memoryUsageChanged(m_memoryUsage);
}

void ProgressiveLoader::finalizeLoading()
{
    m_updateTimer->stop();

    if (m_loadingState == LoadingState::Completed) {
        qInfo() << "Progressive loading finalized successfully";
    } else if (m_loadingState == LoadingState::Failed) {
        qWarning() << "Progressive loading finalized with errors";
    } else if (m_loadingState == LoadingState::Cancelled) {
        qInfo() << "Progressive loading cancelled by user";
    }
}

void ProgressiveLoader::onUpdateTimeout()
{
    if (m_loadingState == LoadingState::Idle || m_loadingState == LoadingState::Completed) {
        return;
    }

    // Update memory usage
    qint64 currentMemory = getMemoryUsage();
    if (currentMemory != m_memoryUsage) {
        m_memoryUsage = currentMemory;
        emit memoryUsageChanged(m_memoryUsage);
    }

    // Check memory limits
    if (m_memoryUsage > m_maxMemoryUsage) {
        emit memoryWarning(m_memoryUsage, m_maxMemoryUsage);
        optimizeMemory();
    }

    // Update progress based on current state
    switch (m_loadingState) {
        case LoadingState::LoadingCoarse:
            m_loadingProgress = qMin(30, m_loadingProgress + 1);
            break;
        case LoadingState::LoadingMedium:
            m_loadingProgress = qMin(60, m_loadingProgress + 1);
            break;
        case LoadingState::LoadingFine:
            m_loadingProgress = qMin(85, m_loadingProgress + 1);
            break;
        case LoadingState::Optimizing:
            m_loadingProgress = qMin(95, m_loadingProgress + 1);
            break;
        default:
            break;
    }

    emit loadingProgress(m_loadingProgress, getLoadingStateString());
}

QString ProgressiveLoader::getLoadingStateString() const
{
    switch (m_loadingState) {
        case LoadingState::Idle: return "Idle";
        case LoadingState::Initializing: return "Initializing";
        case LoadingState::LoadingCoarse: return "Loading coarse model";
        case LoadingState::LoadingMedium: return "Loading medium detail";
        case LoadingState::LoadingFine: return "Loading fine detail";
        case LoadingState::Optimizing: return "Optimizing";
        case LoadingState::Completed: return "Completed";
        case LoadingState::Failed: return "Failed";
        case LoadingState::Cancelled: return "Cancelled";
        default: return "Unknown";
    }
}

void ProgressiveLoader::refineMeshVertices(MeshData& mesh, int refinementFactor)
{
    if (refinementFactor <= 1) {
        return;
    }

    // Simple vertex refinement - add vertices between existing ones
    QVector<Vertex> refinedVertices;
    refinedVertices.reserve(mesh.vertices.size() * refinementFactor);

    for (int i = 0; i < mesh.vertices.size() - 1; ++i) {
        Vertex v1 = mesh.vertices[i];
        Vertex v2 = mesh.vertices[i + 1];

        refinedVertices.append(v1);

        // Add intermediate vertices
        for (int j = 1; j < refinementFactor; ++j) {
            float t = static_cast<float>(j) / refinementFactor;
            Vertex intermediate;
            intermediate.position = v1.position * (1.0f - t) + v2.position * t;
            intermediate.normal = v1.normal * (1.0f - t) + v2.normal * t;
            intermediate.texCoord = v1.texCoord * (1.0f - t) + v2.texCoord * t;
            refinedVertices.append(intermediate);
        }
    }

    // Add the last vertex
    refinedVertices.append(mesh.vertices.last());

    mesh.vertices = refinedVertices;
    mesh.vertexCount = refinedVertices.size();
}

void ProgressiveLoader::loadFullMeshDetail(MeshData& mesh)
{
    // In a full implementation, this would load the complete mesh data
    // For now, just ensure we have all vertices and indices
    if (mesh.indices.isEmpty() && mesh.vertices.size() >= 3) {
        // Generate basic triangle indices
        for (int i = 0; i < mesh.vertices.size() - 2; i += 3) {
            mesh.indices.append(i);
            mesh.indices.append(i + 1);
            mesh.indices.append(i + 2);
        }
        mesh.triangleCount = mesh.indices.size() / 3;
    }
}

void ProgressiveLoader::generateMeshNormals(MeshData& mesh)
{
    // Calculate face normals and vertex normals
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
        QVector3D faceNormal = QVector3D::crossProduct(edge1, edge2).normalized();

        // Add to vertex normals (will be averaged later)
        mesh.vertices[i1].normal += faceNormal;
        mesh.vertices[i2].normal += faceNormal;
        mesh.vertices[i3].normal += faceNormal;
    }

    // Normalize vertex normals
    for (Vertex& vertex : mesh.vertices) {
        if (!vertex.normal.isNull()) {
            vertex.normal.normalize();
        }
    }
}

void ProgressiveLoader::calculateTangents(MeshData& mesh)
{
    // Simplified tangent calculation
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

        Vertex& v1 = mesh.vertices[i1];
        Vertex& v2 = mesh.vertices[i2];
        Vertex& v3 = mesh.vertices[i3];

        // Calculate tangent using UV coordinates
        QVector2D deltaUV1 = v2.texCoord - v1.texCoord;
        QVector2D deltaUV2 = v3.texCoord - v1.texCoord;

        float denominator = (deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x());
        if (qAbs(denominator) > 0.0001f) {
            float f = 1.0f / denominator;
            QVector3D tangent = (v2.position - v1.position) * deltaUV2.y() * f -
                               (v3.position - v1.position) * deltaUV1.y() * f;

            if (!tangent.isNull()) {
                tangent.normalize();
                v1.tangent = tangent;
                v2.tangent = tangent;
                v3.tangent = tangent;
            }
        }
    }
}

void ProgressiveLoader::optimizeVertexCache(MeshData& mesh)
{
    // Simple vertex cache optimization
    // In a full implementation, this would use sophisticated algorithms
    // For now, just ensure vertices are in a reasonable order

    if (mesh.indices.size() < 1000) {
        return; // Skip optimization for small meshes
    }

    // This is a placeholder for vertex cache optimization
    // A full implementation would reorder indices for better GPU cache performance
    qDebug() << "Vertex cache optimization would be applied here";
}

qint64 ProgressiveLoader::calculateModelMemoryUsage(const ModelData& model)
{
    qint64 totalUsage = 0;

    for (const MeshData& mesh : model.meshes) {
        // Estimate memory for vertices (position, normal, texcoord, tangent, bitangent)
        totalUsage += mesh.vertices.size() * 5 * sizeof(QVector3D); // 5 vectors per vertex
        totalUsage += mesh.indices.size() * sizeof(unsigned int);

        // Estimate OpenGL buffer overhead
        totalUsage += mesh.vertices.size() * 64; // Approximate per-vertex overhead
        totalUsage += 1024 * 64; // VAO and buffer overhead per mesh
    }

    return totalUsage;
}

void ProgressiveLoader::allocateMeshChunk(MeshData& mesh, int startIndex, int chunkSize)
{
    // Allocate memory for a chunk of mesh data
    int endIndex = qMin(startIndex + chunkSize, mesh.vertices.size());

    // Ensure vectors have enough capacity
    if (mesh.vertices.capacity() < mesh.vertices.size()) {
        mesh.vertices.reserve(mesh.vertices.size() * 1.2); // 20% extra capacity
    }

    // In a full implementation, this would manage memory-mapped files or streaming
    qDebug() << QString("Allocating mesh chunk: %1 to %2").arg(startIndex).arg(endIndex);
}

void ProgressiveLoader::deallocateUnusedChunks()
{
    // Deallocate mesh chunks that are not currently needed
    // This would be based on camera position and LOD requirements

    qint64 freedMemory = 0;

    // Placeholder implementation
    QCoreApplication::processEvents(); // Allow other threads to run

    if (freedMemory > 0) {
        qInfo() << "Deallocated unused chunks, freed:" << freedMemory << "bytes";
    }
}

void ProgressiveLoader::adaptQualityToViewport(const QRect& viewport)
{
    // Adjust quality based on viewport size
    float viewportArea = viewport.width() * viewport.height();
    float qualityMultiplier = qMin(viewportArea / (1920.0f * 1080.0f), 1.0f);

    setQualityLevel(m_qualityLevel * qualityMultiplier);
}

void ProgressiveLoader::adaptQualityToMemory(qint64 availableMemory)
{
    // Reduce quality if memory is running low
    if (availableMemory < 100 * 1024 * 1024) { // Less than 100MB
        setQualityLevel(m_qualityLevel * 0.7f);
    } else if (availableMemory < 500 * 1024 * 1024) { // Less than 500MB
        setQualityLevel(m_qualityLevel * 0.85f);
    }
}