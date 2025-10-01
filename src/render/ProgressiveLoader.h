#pragma once

#include "ModelLoader.h"
#include <QObject>
#include <QString>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <atomic>

/**
 * @brief Progressive loading system for large 3D models
 *
 * Handles loading of large models (500MB+) with progressive detail enhancement,
 * memory management, and user interaction during loading.
 */
class ProgressiveLoader : public QObject
{
    Q_OBJECT

public:
    explicit ProgressiveLoader(QObject* parent = nullptr);
    virtual ~ProgressiveLoader();

    // Loading modes
    enum LoadingMode {
        Progressive,    // Load in stages with increasing detail
        Streaming,      // Load on-demand based on view
        Background,     // Load in background thread
        Interactive     // Allow interaction during loading
    };

    // Loading states
    enum LoadingState {
        Idle,
        Initializing,
        LoadingCoarse,
        LoadingMedium,
        LoadingFine,
        Optimizing,
        Completed,
        Failed,
        Cancelled
    };

    // Configuration
    void setLoadingMode(LoadingMode mode);
    void setMaxMemoryUsage(qint64 maxBytes);
    void setChunkSize(int chunkSize);
    void setUpdateInterval(int milliseconds);

    // Progressive loading
    virtual QFuture<ModelData> loadProgressiveAsync(const QString& filepath) = 0;
    virtual void cancelLoading() = 0;
    virtual LoadingState getLoadingState() const = 0;
    virtual int getLoadingProgress() const = 0;

    // Model interaction during loading
    virtual ModelData getCurrentModel() const = 0;
    virtual bool canInteract() const = 0;
    virtual void setInteractionLevel(float level) = 0; // 0.0 = no interaction, 1.0 = full interaction

    // Memory management
    virtual qint64 getMemoryUsage() const = 0;
    virtual void optimizeMemory() = 0;
    virtual void freeUnusedMemory() = 0;

    // Quality settings
    virtual void setQualityLevel(float level) = 0; // 0.0 = lowest, 1.0 = highest
    virtual float getQualityLevel() const = 0;

signals:
    // Progress signals
    void loadingProgress(int percentage, const QString& stage);
    void loadingStateChanged(LoadingState state);
    void modelUpdated(const ModelData& model); // Emitted as model detail increases

    // Memory signals
    void memoryUsageChanged(qint64 bytesUsed);
    void memoryWarning(qint64 requiredBytes, qint64 availableBytes);

    // Quality signals
    void qualityChanged(float newLevel);
    void detailLevelChanged(int level);

    // Error signals
    void loadingFailed(const QString& error);
    void loadingCancelled();

protected:
    // Progressive loading implementation
    virtual void startProgressiveLoading(const QString& filepath) = 0;
    virtual void loadNextChunk() = 0;
    virtual void finalizeLoading() = 0;

    // Memory management
    virtual void allocateMeshChunk(MeshData& mesh, int startIndex, int chunkSize) = 0;
    virtual void deallocateUnusedChunks() = 0;

    // Quality adaptation
    virtual void adaptQualityToViewport(const QRect& viewport) = 0;
    virtual void adaptQualityToMemory(qint64 availableMemory) = 0;

    // Threading
    QThread* m_loadingThread;
    QTimer* m_updateTimer;
    QMutex m_loadingMutex;
    QWaitCondition m_loadingCondition;

    // Loading state
    std::atomic<LoadingState> m_loadingState;
    std::atomic<int> m_loadingProgress;
    std::atomic<bool> m_cancelRequested;

    // Configuration
    LoadingMode m_loadingMode;
    qint64 m_maxMemoryUsage;
    int m_chunkSize;
    int m_updateInterval;

    // Current loading data
    QString m_currentFilepath;
    ModelData m_currentModel;
    qint64 m_memoryUsage;

    // Quality settings
    float m_qualityLevel;
    float m_interactionLevel;
};