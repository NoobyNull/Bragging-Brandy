#pragma once

#include "../core/BaseCanvas.h"
#include "../core/BaseTypes.h"
#include "../render/ModelLoader.h"
#include "../render/LODRenderer.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector3D>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

/**
 * @brief Design Canvas - Advanced 3D visualization workspace
 *
 * Provides comprehensive 3D model visualization with:
 * - OpenGL-accelerated rendering
 * - Interactive camera controls
 * - Material and lighting system
 * - Measurement tools
 * - Sectioning and clipping planes
 * - Animation and simulation
 */
class DesignCanvas : public BaseCanvas
{
    Q_OBJECT

public:
    explicit DesignCanvas(QWidget* parent = nullptr);
    virtual ~DesignCanvas() = default;

    // BaseCanvas interface implementation
    QString canvasName() const override { return "DesignCanvas"; }
    QString canvasDescription() const override { return "Advanced 3D visualization and design workspace"; }
    QString canvasType() const override { return "design"; }
    QString canvasVersion() const override { return "1.0.0"; }

    void addWidget(BaseWidget* widget, DockArea area = DockArea::Center) override;
    void removeWidget(BaseWidget* widget) override;
    void removeWidget(const QString& widgetName) override;
    QList<BaseWidget*> getWidgets() const override;
    QList<BaseWidget*> getWidgetsByCategory(WidgetCategory category) const override;
    BaseWidget* getWidget(const QString& widgetName) const override;

    void saveLayout(const QString& name) override;
    void restoreLayout(const QString& name) override;
    QStringList getAvailableLayouts() const override;
    void resetLayout() override;

    void saveState(QJsonObject& state) const override;
    void restoreState(const QJsonObject& state) override;

    // DesignCanvas specific methods
    void loadModel(const QString& modelId);
    void unloadModel();
    void loadModelFromFile(const QString& filepath);

    // Camera controls
    void setCameraPosition(const QVector3D& position);
    void setCameraTarget(const QVector3D& target);
    void setCameraUp(const QVector3D& up);
    void resetCamera();

    // View modes
    void setViewMode(const QString& mode); // "perspective", "orthographic", "top", "front", "right"
    void setShadingMode(const QString& mode); // "wireframe", "shaded", "textured", "rendered"

    // Lighting controls
    void setLightingEnabled(bool enabled);
    void setLightPosition(const QVector3D& position);
    void setLightIntensity(float intensity);
    void setBackgroundType(const QString& type); // "solid", "gradient", "hdri"

    // Selection and measurement
    void enableSelectionMode(bool enabled);
    void enableMeasurementMode(bool enabled);
    void clearSelection();

    // Animation and playback
    void playAnimation();
    void pauseAnimation();
    void stopAnimation();
    void setAnimationSpeed(float speed);

protected:
    void setupDefaultLayout();
    void create3DViewer();
    void createCameraControls();
    void createObjectBrowser();
    void createPropertiesPanel();
    void createAnimationControls();

    // OpenGL initialization
    void initializeOpenGL();
    void resizeOpenGL(int width, int height);

    // Rendering
    void render3DScene();
    void renderMesh(const MeshData& mesh);
    void renderGrid();
    void renderAxes();
    void renderSelection();

    // Camera and interaction
    void updateCameraMatrices();
    void handleMousePress(QMouseEvent* event);
    void handleMouseMove(QMouseEvent* event);
    void handleMouseRelease(QMouseEvent* event);
    void handleWheel(QWheelEvent* event);
    void handleKeyPress(QKeyEvent* event);

    // Model management
    void updateModelDisplay();
    void updateMeshBuffers();

private slots:
    // Camera control events
    void onCameraPresetSelected(const QString& preset);
    void onViewModeChanged(const QString& mode);
    void onShadingModeChanged(const QString& mode);

    // Model events
    void onModelLoaded(const ModelData& model);
    void onModelUnloaded();

    // Animation events
    void onAnimationFrame();
    void onAnimationStateChanged();

    // Interaction events
    void onSelectionChanged();
    void onMeasurementCompleted();

private:
    // Main 3D viewer widget
    class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
    {
    public:
        explicit GLWidget(DesignCanvas* parent = nullptr);
        virtual ~GLWidget() = default;

    protected:
        void initializeGL() override;
        void resizeGL(int width, int height) override;
        void paintGL() override;

        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;
        void keyPressEvent(QKeyEvent* event) override;

    private:
        DesignCanvas* m_designCanvas;
        QOpenGLShaderProgram* m_shaderProgram;
        QTimer* m_animationTimer;

        // Camera matrices
        QMatrix4x4 m_projectionMatrix;
        QMatrix4x4 m_viewMatrix;
        QMatrix4x4 m_modelMatrix;

        // Mouse interaction
        QPoint m_lastMousePosition;
        bool m_isRotating;
        bool m_isPanning;
        bool m_isZooming;

        // OpenGL resources
        QOpenGLBuffer m_vertexBuffer;
        QOpenGLBuffer m_indexBuffer;
        QOpenGLVertexArrayObject m_vao;
    };

    // UI Components
    GLWidget* m_glWidget;
    QWidget* m_cameraPanel;
    QWidget* m_objectPanel;
    QWidget* m_propertiesPanel;
    QWidget* m_animationPanel;

    // Camera state
    QVector3D m_cameraPosition;
    QVector3D m_cameraTarget;
    QVector3D m_cameraUp;
    float m_cameraDistance;
    float m_cameraYaw;
    float m_cameraPitch;
    QString m_currentViewMode;
    QString m_currentShadingMode;

    // Lighting state
    bool m_lightingEnabled;
    QVector3D m_lightPosition;
    float m_lightIntensity;
    QString m_backgroundType;

    // Model state
    ModelData m_currentModel;
    QString m_loadedModelId;
    bool m_modelLoaded;

    // Interaction state
    bool m_selectionMode;
    bool m_measurementMode;
    QList<int> m_selectedMeshIndices;

    // Animation state
    bool m_animationPlaying;
    float m_animationSpeed;
    float m_animationTime;

    // Performance monitoring
    QTimer* m_fpsTimer;
    int m_frameCount;
    float m_currentFPS;
};