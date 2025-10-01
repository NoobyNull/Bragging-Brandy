#include "ThumbnailGenerator.h"
#include "../core/ModelService.h"
#include "../core/CacheManager.h"
#include <QPixmap>
#include <QPainter>
#include <QBrush>
#include <QLinearGradient>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QThreadPool>
#include <QDebug>

ThumbnailGenerator::ThumbnailGenerator(QObject* parent)
    : QObject(parent)
    , m_maxConcurrentGenerations(2) // Default to 2 concurrent generations
    , m_generationPriority(QThread::LowPriority)
    , m_totalGenerationTime(0)
    , m_generationCount(0)
{
    // Initialize default configuration
    m_defaultConfig.size = QSize(256, 256);
    m_defaultConfig.backgroundType = "gradient";
    m_defaultConfig.backgroundColor = QColor(64, 64, 64);
    m_defaultConfig.lightDirection = QVector3D(1, 1, 1);
    m_defaultConfig.lightIntensity = 1.0f;
    m_defaultConfig.useShadows = true;
    m_defaultConfig.useWireframe = false;
    m_defaultConfig.wireframeOpacity = 0.3f;
    m_defaultConfig.outputFormat = "PNG";
    m_defaultConfig.quality = 90;

    // Set default cache directory
    setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/thumbnails");

    // Create batch processing timer
    m_batchTimer = new QTimer(this);
    m_batchTimer->setSingleShot(true);
    m_batchTimer->setInterval(1000); // Process batch every second
    connect(m_batchTimer, &QTimer::timeout, this, &ThumbnailGenerator::processGenerationQueue);

    qRegisterMetaType<ThumbnailConfig>("ThumbnailConfig");
    qRegisterMetaType<QList<ThumbnailConfig>>("QList<ThumbnailConfig>");
}

QFuture<QPixmap> ThumbnailGenerator::generateThumbnailAsync(const QString& modelId, const ThumbnailConfig& config)
{
    return QtConcurrent::run([this, modelId, config]() -> QPixmap {
        return generateThumbnail(modelId, config);
    });
}

QPixmap ThumbnailGenerator::generateThumbnail(const QString& modelId, const ThumbnailConfig& config)
{
    QElapsedTimer timer;
    timer.start();

    try {
        // Check if thumbnail already exists
        if (thumbnailExists(modelId, config.size)) {
            QPixmap existing = loadThumbnail(modelId, config.size);
            if (!existing.isNull()) {
                return existing;
            }
        }

        // Load model data
        ModelService* modelService = qobject_cast<ModelService*>(parent());
        if (!modelService) {
            return createPlaceholderThumbnail("No Model Service", config);
        }

        ModelMetadata metadata = modelService->getModel(QUuid(modelId));
        if (metadata.id.isNull()) {
            return createPlaceholderThumbnail("Model Not Found", config);
        }

        // Load model using ModelLoader
        ModelLoader loader;
        ModelData model = loader.loadModel(modelService->getModelFilePath(metadata.id));

        if (model.meshes.isEmpty()) {
            return createPlaceholderThumbnail(metadata.filename, config);
        }

        // Render thumbnail
        QPixmap thumbnail = renderModelThumbnail(model, config);

        // Apply post-processing
        thumbnail = applyPostProcessing(thumbnail, config);

        // Save to cache
        QString cachePath = getThumbnailPath(modelId, config.size);
        ensureCacheDirectory();

        if (thumbnail.save(cachePath, config.outputFormat.toUtf8(), config.quality)) {
            emit thumbnailGenerated(modelId, cachePath);
        }

        // Update performance metrics
        qint64 elapsed = timer.elapsed();
        m_totalGenerationTime += elapsed;
        m_generationCount++;

        m_generationMetrics.operationTimeMs = elapsed;
        m_generationMetrics.memoryUsageBytes = thumbnail.width() * thumbnail.height() * 4; // RGBA

        qDebug() << QString("Thumbnail generated for %1 in %2ms").arg(modelId).arg(elapsed);

        return thumbnail;

    } catch (const std::exception& e) {
        qCritical() << "Thumbnail generation failed:" << e.what();
        emit thumbnailGenerationFailed(modelId, QString("Exception: %1").arg(e.what()));
        return createPlaceholderThumbnail("Generation Failed", config);
    }
}

bool ThumbnailGenerator::generateThumbnailToFile(const QString& modelId, const QString& outputPath, const ThumbnailConfig& config)
{
    QPixmap thumbnail = generateThumbnail(modelId, config);
    return thumbnail.save(outputPath, config.outputFormat.toUtf8(), config.quality);
}

void ThumbnailGenerator::generateThumbnailsForModels(const QStringList& modelIds, const ThumbnailConfig& config)
{
    for (const QString& modelId : modelIds) {
        // Add to generation queue if not already generating
        if (!m_generatingModels.contains(modelId)) {
            m_generationQueue.enqueue(modelId);
            m_generatingModels.insert(modelId);
        }
    }

    emit generationQueueChanged(m_generationQueue.size());

    // Start batch processing if not already running
    if (!m_batchTimer->isActive()) {
        m_batchTimer->start();
    }
}

void ThumbnailGenerator::regenerateAllThumbnails()
{
    ModelService* modelService = qobject_cast<ModelService*>(parent());
    if (!modelService) {
        return;
    }

    QList<ModelMetadata> models = modelService->getAllModels();
    QStringList modelIds;

    for (const ModelMetadata& model : models) {
        modelIds.append(model.id.toString());
    }

    generateThumbnailsForModels(modelIds, m_defaultConfig);
}

void ThumbnailGenerator::regenerateThumbnailsForQuery(const QString& query)
{
    ModelService* modelService = qobject_cast<ModelService*>(parent());
    if (!modelService) {
        return;
    }

    QList<ModelMetadata> models = modelService->searchModels(query);
    QStringList modelIds;

    for (const ModelMetadata& model : models) {
        modelIds.append(model.id.toString());
    }

    generateThumbnailsForModels(modelIds, m_defaultConfig);
}

QString ThumbnailGenerator::getThumbnailPath(const QString& modelId, const QSize& size) const
{
    QString filename = generateThumbnailFilename(modelId, size);
    return getThumbnailCachePath() + "/" + filename;
}

bool ThumbnailGenerator::thumbnailExists(const QString& modelId, const QSize& size) const
{
    QString path = getThumbnailPath(modelId, size);
    return QFile::exists(path);
}

QPixmap ThumbnailGenerator::loadThumbnail(const QString& modelId, const QSize& size)
{
    QString path = getThumbnailPath(modelId, size);

    QPixmap thumbnail(path);
    if (thumbnail.isNull()) {
        return QPixmap(); // Return null pixmap
    }

    // Resize if needed
    if (thumbnail.size() != size) {
        thumbnail = thumbnail.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    return thumbnail;
}

void ThumbnailGenerator::deleteThumbnail(const QString& modelId, const QSize& size)
{
    QString path = getThumbnailPath(modelId, size);
    QFile::remove(path);
}

void ThumbnailGenerator::setDefaultConfig(const ThumbnailConfig& config)
{
    m_defaultConfig = config;
}

ThumbnailGenerator::ThumbnailConfig ThumbnailGenerator::getDefaultConfig() const
{
    return m_defaultConfig;
}

void ThumbnailGenerator::setCacheDirectory(const QString& directory)
{
    m_cacheDirectory = directory;
    ensureCacheDirectory();
}

QString ThumbnailGenerator::getCacheDirectory() const
{
    return m_cacheDirectory;
}

void ThumbnailGenerator::setMaxConcurrentGenerations(int max)
{
    m_maxConcurrentGenerations = qMax(1, max);
}

int ThumbnailGenerator::getMaxConcurrentGenerations() const
{
    return m_maxConcurrentGenerations;
}

void ThumbnailGenerator::setGenerationPriority(int priority)
{
    m_generationPriority = priority;
}

void ThumbnailGenerator::pauseGeneration()
{
    m_batchTimer->stop();
}

void ThumbnailGenerator::resumeGeneration()
{
    if (!m_generationQueue.isEmpty()) {
        m_batchTimer->start();
    }
}

PerformanceMetrics ThumbnailGenerator::getGenerationMetrics() const
{
    PerformanceMetrics metrics = m_generationMetrics;
    metrics.operationType = "ThumbnailGenerator";

    if (m_generationCount > 0) {
        metrics.operationTimeMs = m_totalGenerationTime / m_generationCount;
    }

    return metrics;
}

void ThumbnailGenerator::resetPerformanceMetrics()
{
    m_generationMetrics = PerformanceMetrics();
    m_totalGenerationTime = 0;
    m_generationCount = 0;
}

void ThumbnailGenerator::clearThumbnailCache()
{
    QDir cacheDir(getThumbnailCachePath());

    if (cacheDir.exists()) {
        qint64 freedBytes = 0;

        QFileInfoList files = cacheDir.entryInfoList(QDir::Files);
        for (const QFileInfo& file : files) {
            freedBytes += file.size();
            QFile::remove(file.absoluteFilePath());
        }

        emit thumbnailCacheCleared();
        emit thumbnailCacheCleaned(freedBytes);

        qInfo() << "Thumbnail cache cleared, freed:" << freedBytes << "bytes";
    }
}

void ThumbnailGenerator::cleanupOldThumbnails(qint64 maxAgeSeconds)
{
    QDir cacheDir(getThumbnailCachePath());
    QDateTime cutoffTime = QDateTime::currentDateTime().addSecs(-maxAgeSeconds);

    if (cacheDir.exists()) {
        qint64 freedBytes = 0;

        QFileInfoList files = cacheDir.entryInfoList(QDir::Files);
        for (const QFileInfo& file : files) {
            if (file.lastModified() < cutoffTime) {
                freedBytes += file.size();
                QFile::remove(file.absoluteFilePath());
            }
        }

        if (freedBytes > 0) {
            emit thumbnailCacheCleaned(freedBytes);
            qInfo() << "Old thumbnails cleaned, freed:" << freedBytes << "bytes";
        }
    }
}

qint64 ThumbnailGenerator::getCacheSize() const
{
    QDir cacheDir(getThumbnailCachePath());
    qint64 totalSize = 0;

    if (cacheDir.exists()) {
        QFileInfoList files = cacheDir.entryInfoList(QDir::Files);
        for (const QFileInfo& file : files) {
            totalSize += file.size();
        }
    }

    return totalSize;
}

QPixmap ThumbnailGenerator::renderModelThumbnail(const ModelData& model, const ThumbnailConfig& config)
{
    // Create thumbnail pixmap
    QPixmap thumbnail(config.size);
    thumbnail.fill(Qt::transparent);

    QPainter painter(&thumbnail);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // Draw background
    drawBackground(painter, config, thumbnail.rect());

    // Render 3D model (simplified for now)
    renderModel(painter, model, config, thumbnail.rect());

    painter.end();

    return thumbnail;
}

QPixmap ThumbnailGenerator::createPlaceholderThumbnail(const QString& modelName, const ThumbnailConfig& config)
{
    QPixmap thumbnail(config.size);
    thumbnail.fill(config.backgroundColor);

    QPainter painter(&thumbnail);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw placeholder text
    QFont font = painter.font();
    font.setPointSize(12);
    painter.setFont(font);

    QRect textRect = thumbnail.rect();
    QString displayText = modelName.length() > 20 ? modelName.left(17) + "..." : modelName;

    painter.setPen(Qt::white);
    painter.drawText(textRect, Qt::AlignCenter, displayText);

    painter.end();

    return thumbnail;
}

void ThumbnailGenerator::drawBackground(QPainter& painter, const ThumbnailConfig& config, const QRect& rect)
{
    if (config.backgroundType == "solid") {
        painter.fillRect(rect, config.backgroundColor);

    } else if (config.backgroundType == "gradient") {
        QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
        gradient.setColorAt(0, config.backgroundColor.lighter(120));
        gradient.setColorAt(1, config.backgroundColor.darker(120));
        painter.fillRect(rect, gradient);

    } else if (config.backgroundType == "grid") {
        painter.fillRect(rect, config.backgroundColor);

        // Draw grid
        QPen gridPen(QColor(255, 255, 255, 30));
        painter.setPen(gridPen);

        int gridSize = 20;
        for (int x = 0; x < rect.width(); x += gridSize) {
            painter.drawLine(x, 0, x, rect.height());
        }
        for (int y = 0; y < rect.height(); y += gridSize) {
            painter.drawLine(0, y, rect.width(), y);
        }
    }
}

void ThumbnailGenerator::renderModel(QPainter& painter, const ModelData& model, const ThumbnailConfig& config, const QRect& rect)
{
    // Simplified 3D model rendering for thumbnails
    // In a full implementation, this would use OpenGL or a software renderer

    if (model.meshes.isEmpty()) {
        return;
    }

    // Calculate model bounds and center it
    QRect modelRect = rect;
    modelRect.adjust(20, 20, -20, -20); // Add padding

    // Draw a simple representation of the model
    painter.setPen(QPen(Qt::white, 2));

    // For now, draw a simple 3D cube representation
    QPoint center = modelRect.center();

    int size = qMin(modelRect.width(), modelRect.height()) / 3;

    // Draw front face
    QRect frontFace(center.x() - size/2, center.y() - size/2, size, size);
    painter.drawRect(frontFace);

    // Draw top face (perspective)
    QPoint topLeft(frontFace.topLeft() + QPoint(-size/4, -size/4));
    QPoint topRight(frontFace.topRight() + QPoint(size/4, -size/4));
    QPoint bottomRight(frontFace.bottomRight());
    QPoint bottomLeft(frontFace.bottomLeft());

    painter.drawLine(topLeft, topRight);
    painter.drawLine(topLeft, bottomLeft);
    painter.drawLine(topRight, bottomRight);

    // Draw model name
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);

    QString displayName = QFileInfo(model.filename).baseName();
    if (displayName.length() > 15) {
        displayName = displayName.left(12) + "...";
    }

    QRect textRect = modelRect;
    textRect.setTop(textRect.bottom() - 30);

    painter.setPen(Qt::white);
    painter.drawText(textRect, Qt::AlignCenter, displayName);
}

QPixmap ThumbnailGenerator::applyPostProcessing(const QPixmap& thumbnail, const ThumbnailConfig& config)
{
    QPixmap processed = thumbnail;

    // Apply wireframe overlay if requested
    if (config.useWireframe) {
        QPainter painter(&processed);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draw wireframe overlay
        QPen wireframePen(QColor(255, 255, 255, static_cast<int>(config.wireframeOpacity * 255)));
        painter.setPen(wireframePen);

        // Draw diagonal lines for wireframe effect
        QRect rect = processed.rect();
        painter.drawLine(rect.topLeft(), rect.bottomRight());
        painter.drawLine(rect.topRight(), rect.bottomLeft());

        painter.end();
    }

    return processed;
}

QPixmap ThumbnailGenerator::resizeThumbnail(const QPixmap& source, const QSize& targetSize)
{
    return source.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QString ThumbnailGenerator::generateThumbnailFilename(const QString& modelId, const QSize& size) const
{
    QString extension = m_defaultConfig.outputFormat.toLower();
    return QString("%1_%2x%3.%4")
           .arg(modelId)
           .arg(size.width())
           .arg(size.height())
           .arg(extension);
}

void ThumbnailGenerator::startBackgroundGeneration(const QString& modelId, const ThumbnailConfig& config)
{
    // Add to generation queue
    m_generationQueue.enqueue(modelId);
    m_generatingModels.insert(modelId);

    emit generationQueueChanged(m_generationQueue.size());

    // Start processing if not already running
    if (!m_batchTimer->isActive()) {
        m_batchTimer->start();
    }
}

void ThumbnailGenerator::processGenerationQueue()
{
    int processed = 0;

    while (!m_generationQueue.isEmpty() && processed < m_maxConcurrentGenerations) {
        QString modelId = m_generationQueue.dequeue();

        // Generate thumbnail in background thread
        QtConcurrent::run([this, modelId]() {
            generateThumbnail(modelId, m_defaultConfig);

            // Remove from generating set when done
            QMetaObject::invokeMethod(this, "removeFromGenerating", Qt::QueuedConnection,
                                     Q_ARG(QString, modelId));
        });

        processed++;
    }

    emit generationQueueChanged(m_generationQueue.size());

    // Continue processing if queue is not empty
    if (!m_generationQueue.isEmpty()) {
        m_batchTimer->start();
    }
}

void ThumbnailGenerator::removeFromGenerating(const QString& modelId)
{
    m_generatingModels.remove(modelId);
}

QString ThumbnailGenerator::getThumbnailCachePath() const
{
    return m_cacheDirectory;
}

void ThumbnailGenerator::ensureCacheDirectory()
{
    QDir dir(m_cacheDirectory);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

QString ThumbnailGenerator::calculateCacheKey(const QString& modelId, const QSize& size) const
{
    return QString("%1_%2x%3").arg(modelId).arg(size.width()).arg(size.height());
}