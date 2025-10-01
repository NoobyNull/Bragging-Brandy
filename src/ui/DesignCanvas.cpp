#include "DesignCanvas.h"
#include "../core/ModelService.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTreeWidget>
#include <QTableWidget>
#include <QProgressBar>
#include <QStatusBar>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QQuaternion>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QDebug>
#include <cmath>

// DesignCanvas::GLWidget implementation
DesignCanvas::GLWidget::GLWidget(DesignCanvas* parent)
    : QOpenGLWidget(parent)
    , m_designCanvas(parent)
    , m_shaderProgram(nullptr)
    , m_animationTimer(nullptr)
    , m_isRotating(false)
    , m_isPanning(false)
    , m_isZooming(false)
{
    // Set focus policy for keyboard events
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(400, 300);

    // Create animation timer
    m_animationTimer = new QTimer(this);
    m_animationTimer->setInterval(16); // ~60 FPS
    connect(m_animationTimer, &QTimer::timeout, this, &GLWidget::onAnimationFrame);

    // Initialize camera matrices
    m_projectionMatrix.setToIdentity();
    m_viewMatrix.setToIdentity();
    m_modelMatrix.setToIdentity();

    // Set initial camera position (looking down -Z axis)
    m_viewMatrix.lookAt(QVector3D(0, 0, 5), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
}

void DesignCanvas::GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // Set clear color
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // Create shader program
    createShaderProgram();

    // Initialize camera matrices
    updateProjectionMatrix();
}

void DesignCanvas::GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    updateProjectionMatrix();
}

void DesignCanvas::GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_designCanvas) {
        return;
    }

    // Update camera matrices
    updateCameraMatrices();

    // Render the 3D scene
    m_designCanvas->render3DScene();

    m_designCanvas->m_frameCount++;
}

void DesignCanvas::GLWidget::mousePressEvent(QMouseEvent* event)
{
    m_lastMousePosition = event->pos();

    if (event->button() == Qt::LeftButton) {
        m_isRotating = true;
    } else if (event->button() == Qt::MiddleButton) {
        m_isPanning = true;
    } else if (event->button() == Qt::RightButton) {
        m_isZooming = true;
    }

    event->accept();
}

void DesignCanvas::GLWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint delta = event->pos() - m_lastMousePosition;
    m_lastMousePosition = event->pos();

    if (m_isRotating) {
        // Rotate camera around target
        float sensitivity = 0.5f;
        m_designCanvas->m_cameraYaw -= delta.x() * sensitivity;
        m_designCanvas->m_cameraPitch -= delta.y() * sensitivity;

        // Clamp pitch to avoid gimbal lock
        m_designCanvas->m_cameraPitch = qBound(-89.0f, m_designCanvas->m_cameraPitch, 89.0f);

        updateCameraFromAngles();
    } else if (m_isPanning) {
        // Pan camera
        float sensitivity = 0.01f;
        QVector3D right = m_designCanvas->m_cameraTarget - m_designCanvas->m_cameraPosition;
        right = QVector3D::crossProduct(right, m_designCanvas->m_cameraUp).normalized();

        m_designCanvas->m_cameraPosition += right * (-delta.x() * sensitivity);
        m_designCanvas->m_cameraPosition += m_designCanvas->m_cameraUp * (delta.y() * sensitivity);
        m_designCanvas->m_cameraTarget += right * (-delta.x() * sensitivity);
        m_designCanvas->m_cameraTarget += m_designCanvas->m_cameraUp * (delta.y() * sensitivity);
    }

    update();
    event->accept();
}

void DesignCanvas::GLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_isRotating = false;
    m_isPanning = false;
    m_isZooming = false;
    event->accept();
}

void DesignCanvas::GLWidget::wheelEvent(QWheelEvent* event)
{
    float delta = event->angleDelta().y() / 120.0f;
    float zoomFactor = 1.0f + delta * 0.1f;

    m_designCanvas->m_cameraDistance *= zoomFactor;
    m_designCanvas->m_cameraDistance = qBound(0.1f, m_designCanvas->m_cameraDistance, 100.0f);

    updateCameraFromDistance();
    update();

    event->accept();
}

void DesignCanvas::GLWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_F:
            m_designCanvas->resetCamera();
            break;
        case Qt::Key_1:
            m_designCanvas->setViewMode("perspective");
            break;
        case Qt::Key_2:
            m_designCanvas->setViewMode("orthographic");
            break;
        case Qt::Key_3:
            m_designCanvas->setViewMode("top");
            break;
        case Qt::Key_4:
            m_designCanvas->setViewMode("front");
            break;
        case Qt::Key_5:
            m_designCanvas->setViewMode("right");
            break;
        case Qt::Key_W:
            m_designCanvas->setShadingMode("wireframe");
            break;
        case Qt::Key_S:
            m_designCanvas->setShadingMode("shaded");
            break;
        case Qt::Key_T:
            m_designCanvas->setShadingMode("textured");
            break;
        case Qt::Key_R:
            m_designCanvas->setShadingMode("rendered");
            break;
        case Qt::Key_Space:
            if (m_designCanvas->m_animationPlaying) {
                m_designCanvas->pauseAnimation();
            } else {
                m_designCanvas->playAnimation();
            }
            break;
    }

    event->accept();
}

void DesignCanvas::GLWidget::createShaderProgram()
{
    // Create basic shader program for 3D rendering
    m_shaderProgram = new QOpenGLShaderProgram(this);

    // Vertex shader
    QString vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;

        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoord = aTexCoord;

            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";

    // Fragment shader
    QString fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoord;

        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 objectColor;
        uniform bool useLighting;

        void main() {
            if (useLighting) {
                // Ambient
                float ambientStrength = 0.1;
                vec3 ambient = ambientStrength * lightColor;

                // Diffuse
                vec3 norm = normalize(Normal);
                vec3 lightDir = normalize(lightPos - FragPos);
                float diff = max(dot(norm, lightDir), 0.0);
                vec3 diffuse = diff * lightColor;

                vec3 result = (ambient + diffuse) * objectColor;
                FragColor = vec4(result, 1.0);
            } else {
                FragColor = vec4(objectColor, 1.0);
            }
        }
    )";

    if (!m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
        qCritical() << "Vertex shader compilation failed:" << m_shaderProgram->log();
    }

    if (!m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
        qCritical() << "Fragment shader compilation failed:" << m_shaderProgram->log();
    }

    if (!m_shaderProgram->link()) {
        qCritical() << "Shader program linking failed:" << m_shaderProgram->log();
    }
}

void DesignCanvas::GLWidget::updateProjectionMatrix()
{
    float aspect = static_cast<float>(width()) / height();
    if (m_designCanvas->m_currentViewMode == "orthographic") {
        float orthoSize = m_designCanvas->m_cameraDistance;
        m_projectionMatrix.setToIdentity();
        m_projectionMatrix.ortho(-orthoSize * aspect, orthoSize * aspect, -orthoSize, orthoSize, 0.1f, 1000.0f);
    } else {
        m_projectionMatrix.perspective(60.0f, aspect, 0.1f, 1000.0f);
    }
}

void DesignCanvas::GLWidget::updateCameraMatrices()
{
    // Update view matrix based on camera position
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt(m_designCanvas->m_cameraPosition,
                       m_designCanvas->m_cameraTarget,
                       m_designCanvas->m_cameraUp);
}

void DesignCanvas::GLWidget::onAnimationFrame()
{
    if (m_designCanvas->m_animationPlaying) {
        m_designCanvas->m_animationTime += m_designCanvas->m_animationSpeed * 0.016f; // 16ms frame time
        update();
    }
}

// DesignCanvas implementation
DesignCanvas::DesignCanvas(QWidget* parent)
    : BaseCanvas(parent)
    , m_glWidget(nullptr)
    , m_cameraPanel(nullptr)
    , m_objectPanel(nullptr)
    , m_propertiesPanel(nullptr)
    , m_animationPanel(nullptr)
    , m_cameraPosition(0, 0, 5)
    , m_cameraTarget(0, 0, 0)
    , m_cameraUp(0, 1, 0)
    , m_cameraDistance(5.0f)
    , m_cameraYaw(0.0f)
    , m_cameraPitch(0.0f)
    , m_currentViewMode("perspective")
    , m_currentShadingMode("shaded")
    , m_lightingEnabled(true)
    , m_lightPosition(2, 2, 2)
    , m_lightIntensity(1.0f)
    , m_backgroundType("gradient")
    , m_modelLoaded(false)
    , m_selectionMode(false)
    , m_measurementMode(false)
    , m_animationPlaying(false)
    , m_animationSpeed(1.0f)
    , m_animationTime(0.0f)
    , m_frameCount(0)
    , m_currentFPS(0.0f)
{
    setCanvasName("DesignCanvas");
    setupDefaultLayout();

    // Create FPS timer
    m_fpsTimer = new QTimer(this);
    m_fpsTimer->setInterval(1000);
    connect(m_fpsTimer, &QTimer::timeout, this, &DesignCanvas::updateFPS);
    m_fpsTimer->start();
}

void DesignCanvas::setupDefaultLayout()
{
    // Create main horizontal splitter
    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setChildrenCollapsible(false);

    // Create 3D viewer (center)
    create3DViewer();

    // Create left panel (camera and object controls)
    createCameraControls();
    createObjectBrowser();

    // Create right panel (properties and animation)
    createPropertiesPanel();
    createAnimationControls();

    // Add panels to splitter
    mainSplitter->addWidget(m_cameraPanel);
    mainSplitter->addWidget(m_glWidget);
    mainSplitter->addWidget(m_objectPanel);
    mainSplitter->addWidget(m_propertiesPanel);

    // Set splitter proportions (15% : 60% : 15% : 10%)
    mainSplitter->setStretchFactor(0, 15);
    mainSplitter->setStretchFactor(1, 60);
    mainSplitter->setStretchFactor(2, 15);
    mainSplitter->setStretchFactor(3, 10);

    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(mainSplitter);

    setLayout(mainLayout);

    // Load default layout
    restoreLayout("design_default");
}

void DesignCanvas::create3DViewer()
{
    m_glWidget = new GLWidget(this);

    // Connect to model service for loading models
    ModelService* modelService = qobject_cast<ModelService*>(parent());
    if (modelService) {
        connect(modelService, &ModelService::modelLoaded, this, &DesignCanvas::onModelLoaded);
    }
}

void DesignCanvas::createCameraControls()
{
    m_cameraPanel = new QWidget();
    m_cameraPanel->setMinimumWidth(200);
    m_cameraPanel->setMaximumWidth(250);

    QVBoxLayout* layout = new QVBoxLayout(m_cameraPanel);

    // Camera presets group
    QGroupBox* presetsGroup = new QGroupBox("Camera Presets", m_cameraPanel);
    QVBoxLayout* presetsLayout = new QVBoxLayout(presetsGroup);

    QStringList presets = {"Perspective", "Top", "Front", "Right", "Left", "Bottom", "Back"};
    for (const QString& preset : presets) {
        QPushButton* button = new QPushButton(preset, presetsGroup);
        connect(button, &QPushButton::clicked, this, [this, preset]() {
            onCameraPresetSelected(preset.toLower());
        });
        presetsLayout->addWidget(button);
    }
    layout->addWidget(presetsGroup);

    // View mode group
    QGroupBox* viewGroup = new QGroupBox("View Mode", m_cameraPanel);
    QVBoxLayout* viewLayout = new QVBoxLayout(viewGroup);

    QComboBox* viewModeCombo = new QComboBox(viewGroup);
    viewModeCombo->addItem("Perspective", "perspective");
    viewModeCombo->addItem("Orthographic", "orthographic");
    connect(viewModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, viewModeCombo](int index) {
        onViewModeChanged(viewModeCombo->itemData(index).toString());
    });
    viewLayout->addWidget(viewModeCombo);
    layout->addWidget(viewGroup);

    // Shading mode group
    QGroupBox* shadingGroup = new QGroupBox("Shading", m_cameraPanel);
    QVBoxLayout* shadingLayout = new QVBoxLayout(shadingGroup);

    QComboBox* shadingCombo = new QComboBox(shadingGroup);
    shadingCombo->addItem("Wireframe", "wireframe");
    shadingCombo->addItem("Shaded", "shaded");
    shadingCombo->addItem("Textured", "textured");
    shadingCombo->addItem("Rendered", "rendered");
    connect(shadingCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, shadingCombo](int index) {
        onShadingModeChanged(shadingCombo->itemData(index).toString());
    });
    shadingLayout->addWidget(shadingCombo);
    layout->addWidget(shadingGroup);

    layout->addStretch();
}

void DesignCanvas::createObjectBrowser()
{
    // Object browser would go here
    // For now, just add it to the camera panel
}

void DesignCanvas::createPropertiesPanel()
{
    m_propertiesPanel = new QWidget();
    m_propertiesPanel->setMinimumWidth(200);
    m_propertiesPanel->setMaximumWidth(250);

    QVBoxLayout* layout = new QVBoxLayout(m_propertiesPanel);

    // Model info group
    QGroupBox* infoGroup = new QGroupBox("Model Info", m_propertiesPanel);
    QVBoxLayout* infoLayout = new QVBoxLayout(infoGroup);

    QLabel* modelNameLabel = new QLabel("No model loaded", infoGroup);
    modelNameLabel->setObjectName("modelNameLabel");
    infoLayout->addWidget(modelNameLabel);

    QLabel* meshCountLabel = new QLabel("Meshes: 0", infoGroup);
    meshCountLabel->setObjectName("meshCountLabel");
    infoLayout->addWidget(meshCountLabel);

    QLabel* vertexCountLabel = new QLabel("Vertices: 0", infoGroup);
    vertexCountLabel->setObjectName("vertexCountLabel");
    infoLayout->addWidget(vertexCountLabel);

    layout->addWidget(infoGroup);

    // Material properties group
    QGroupBox* materialGroup = new QGroupBox("Material", m_propertiesPanel);
    QVBoxLayout* materialLayout = new QVBoxLayout(materialGroup);

    QCheckBox* lightingCheck = new QCheckBox("Enable Lighting", materialGroup);
    lightingCheck->setChecked(m_lightingEnabled);
    connect(lightingCheck, &QCheckBox::toggled, this, &DesignCanvas::setLightingEnabled);
    materialLayout->addWidget(lightingCheck);

    layout->addWidget(materialGroup);

    layout->addStretch();
}

void DesignCanvas::createAnimationControls()
{
    m_animationPanel = new QWidget();
    m_animationPanel->setMinimumWidth(150);
    m_animationPanel->setMaximumWidth(200);

    QVBoxLayout* layout = new QVBoxLayout(m_animationPanel);

    // Animation controls group
    QGroupBox* animGroup = new QGroupBox("Animation", m_animationPanel);
    QVBoxLayout* animLayout = new QVBoxLayout(animGroup);

    QPushButton* playButton = new QPushButton("Play", animGroup);
    connect(playButton, &QPushButton::clicked, this, &DesignCanvas::playAnimation);
    animLayout->addWidget(playButton);

    QPushButton* pauseButton = new QPushButton("Pause", animGroup);
    connect(pauseButton, &QPushButton::clicked, this, &DesignCanvas::pauseAnimation);
    animLayout->addWidget(pauseButton);

    QPushButton* stopButton = new QPushButton("Stop", animGroup);
    connect(stopButton, &QPushButton::clicked, this, &DesignCanvas::stopAnimation);
    animLayout->addWidget(stopButton);

    // Animation speed slider
    QHBoxLayout* speedLayout = new QHBoxLayout();
    speedLayout->addWidget(new QLabel("Speed:", animGroup));
    QSlider* speedSlider = new QSlider(Qt::Horizontal, animGroup);
    speedSlider->setRange(1, 500);
    speedSlider->setValue(100);
    connect(speedSlider, &QSlider::valueChanged, this, [this](int value) {
        setAnimationSpeed(value / 100.0f);
    });
    speedLayout->addWidget(speedSlider);
    animLayout->addLayout(speedLayout);

    layout->addWidget(animGroup);
    layout->addStretch();
}

// BaseCanvas interface implementation
void DesignCanvas::addWidget(BaseWidget* widget, DockArea area)
{
    if (!widget) {
        return;
    }

    // Add to appropriate panel based on area
    switch (area) {
        case DockArea::Left:
            // Add to camera panel
            break;
        case DockArea::Right:
            // Add to properties panel
            break;
        default:
            break;
    }

    emit widgetAdded(widget);
}

void DesignCanvas::removeWidget(BaseWidget* widget)
{
    // Widget removal implementation
}

void DesignCanvas::removeWidget(const QString& widgetName)
{
    // Widget removal by name implementation
}

QList<BaseWidget*> DesignCanvas::getWidgets() const
{
    return QList<BaseWidget*>();
}

QList<BaseWidget*> DesignCanvas::getWidgetsByCategory(WidgetCategory category) const
{
    return QList<BaseWidget*>();
}

BaseWidget* DesignCanvas::getWidget(const QString& widgetName) const
{
    return nullptr;
}

void DesignCanvas::saveLayout(const QString& name)
{
    // Layout saving implementation
}

void DesignCanvas::restoreLayout(const QString& name)
{
    // Layout restoration implementation
}

QStringList DesignCanvas::getAvailableLayouts() const
{
    return QStringList() << "design_default" << "design_compact" << "design_detailed";
}

void DesignCanvas::resetLayout()
{
    restoreLayout("design_default");
}

void DesignCanvas::saveState(QJsonObject& state) const
{
    state["camera_position"] = QString("%1,%2,%3").arg(m_cameraPosition.x()).arg(m_cameraPosition.y()).arg(m_cameraPosition.z());
    state["camera_target"] = QString("%1,%2,%3").arg(m_cameraTarget.x()).arg(m_cameraTarget.y()).arg(m_cameraTarget.z());
    state["camera_distance"] = m_cameraDistance;
    state["view_mode"] = m_currentViewMode;
    state["shading_mode"] = m_currentShadingMode;
    state["lighting_enabled"] = m_lightingEnabled;
    state["loaded_model"] = m_loadedModelId;
}

void DesignCanvas::restoreState(const QJsonObject& state)
{
    QStringList pos = state["camera_position"].toString().split(",");
    if (pos.size() == 3) {
        m_cameraPosition = QVector3D(pos[0].toFloat(), pos[1].toFloat(), pos[2].toFloat());
    }

    QStringList target = state["camera_target"].toString().split(",");
    if (target.size() == 3) {
        m_cameraTarget = QVector3D(target[0].toFloat(), target[1].toFloat(), target[2].toFloat());
    }

    m_cameraDistance = state["camera_distance"].toFloat();
    setViewMode(state["view_mode"].toString());
    setShadingMode(state["shading_mode"].toString());
    setLightingEnabled(state["lighting_enabled"].toBool());

    QString modelId = state["loaded_model"].toString();
    if (!modelId.isEmpty()) {
        loadModel(modelId);
    }
}

// DesignCanvas specific methods
void DesignCanvas::loadModel(const QString& modelId)
{
    ModelService* modelService = qobject_cast<ModelService*>(parent());
    if (!modelService) {
        return;
    }

    ModelMetadata metadata = modelService->getModel(QUuid(modelId));
    if (metadata.id.isNull()) {
        return;
    }

    // Load model using ModelLoader
    ModelLoader loader;
    m_currentModel = loader.loadModel(modelService->getModelFilePath(metadata.id));

    if (!m_currentModel.meshes.isEmpty()) {
        m_loadedModelId = modelId;
        m_modelLoaded = true;

        // Update model info display
        updateModelInfoDisplay();

        // Fit camera to model
        fitCameraToModel();

        qInfo() << QString("Model loaded in Design Canvas: %1").arg(metadata.filename);
    }
}

void DesignCanvas::unloadModel()
{
    m_currentModel = ModelData();
    m_loadedModelId = "";
    m_modelLoaded = false;

    updateModelInfoDisplay();
    emit modelUnloaded();
}

void DesignCanvas::loadModelFromFile(const QString& filepath)
{
    ModelLoader loader;
    m_currentModel = loader.loadModel(filepath);

    if (!m_currentModel.meshes.isEmpty()) {
        m_modelLoaded = true;
        updateModelInfoDisplay();
        fitCameraToModel();
    }
}

void DesignCanvas::setCameraPosition(const QVector3D& position)
{
    m_cameraPosition = position;
    updateCameraFromPosition();
}

void DesignCanvas::setCameraTarget(const QVector3D& target)
{
    m_cameraTarget = target;
    updateCameraFromPosition();
}

void DesignCanvas::setCameraUp(const QVector3D& up)
{
    m_cameraUp = up.normalized();
    updateCameraFromPosition();
}

void DesignCanvas::resetCamera()
{
    if (m_modelLoaded) {
        fitCameraToModel();
    } else {
        // Reset to default position
        m_cameraPosition = QVector3D(0, 0, 5);
        m_cameraTarget = QVector3D(0, 0, 0);
        m_cameraUp = QVector3D(0, 1, 0);
        m_cameraDistance = 5.0f;
        m_cameraYaw = 0.0f;
        m_cameraPitch = 0.0f;
    }

    updateCameraFromPosition();
}

void DesignCanvas::setViewMode(const QString& mode)
{
    m_currentViewMode = mode;

    if (mode == "top") {
        m_cameraPosition = QVector3D(0, 5, 0);
        m_cameraTarget = QVector3D(0, 0, 0);
        m_cameraUp = QVector3D(0, 0, -1);
    } else if (mode == "front") {
        m_cameraPosition = QVector3D(0, 0, 5);
        m_cameraTarget = QVector3D(0, 0, 0);
        m_cameraUp = QVector3D(0, 1, 0);
    } else if (mode == "right") {
        m_cameraPosition = QVector3D(5, 0, 0);
        m_cameraTarget = QVector3D(0, 0, 0);
        m_cameraUp = QVector3D(0, 1, 0);
    }

    updateCameraFromPosition();
}

void DesignCanvas::setShadingMode(const QString& mode)
{
    m_currentShadingMode = mode;
    // Update OpenGL rendering state
}

void DesignCanvas::setLightingEnabled(bool enabled)
{
    m_lightingEnabled = enabled;
    // Update OpenGL lighting state
}

void DesignCanvas::setLightPosition(const QVector3D& position)
{
    m_lightPosition = position;
    // Update OpenGL light position
}

void DesignCanvas::setLightIntensity(float intensity)
{
    m_lightIntensity = intensity;
    // Update OpenGL light intensity
}

void DesignCanvas::setBackgroundType(const QString& type)
{
    m_backgroundType = type;
    // Update OpenGL background rendering
}

void DesignCanvas::enableSelectionMode(bool enabled)
{
    m_selectionMode = enabled;
    // Update interaction mode
}

void DesignCanvas::enableMeasurementMode(bool enabled)
{
    m_measurementMode = enabled;
    // Update interaction mode
}

void DesignCanvas::clearSelection()
{
    m_selectedMeshIndices.clear();
    // Update visual selection
}

void DesignCanvas::playAnimation()
{
    m_animationPlaying = true;
    if (m_glWidget) {
        static_cast<GLWidget*>(m_glWidget)->m_animationTimer->start();
    }
}

void DesignCanvas::pauseAnimation()
{
    m_animationPlaying = false;
    if (m_glWidget) {
        static_cast<GLWidget*>(m_glWidget)->m_animationTimer->stop();
    }
}

void DesignCanvas::stopAnimation()
{
    m_animationPlaying = false;
    m_animationTime = 0.0f;
    if (m_glWidget) {
        static_cast<GLWidget*>(m_glWidget)->m_animationTimer->stop();
    }
}

void DesignCanvas::setAnimationSpeed(float speed)
{
    m_animationSpeed = qBound(0.1f, speed, 5.0f);
}

// Protected methods
void DesignCanvas::initializeOpenGL()
{
    // OpenGL initialization would go here
}

void DesignCanvas::resizeOpenGL(int width, int height)
{
    // OpenGL resize handling would go here
}

void DesignCanvas::render3DScene()
{
    if (!m_modelLoaded) {
        renderGrid();
        renderAxes();
        return;
    }

    // Render model meshes
    for (int i = 0; i < m_currentModel.meshes.size(); ++i) {
        const MeshData& mesh = m_currentModel.meshes[i];

        if (m_selectedMeshIndices.contains(i)) {
            // Render selected mesh with highlight
            renderMesh(mesh, true);
        } else {
            renderMesh(mesh, false);
        }
    }

    // Render grid and axes
    renderGrid();
    renderAxes();

    // Render selection overlay
    if (m_selectionMode) {
        renderSelection();
    }
}

void DesignCanvas::renderMesh(const MeshData& mesh, bool selected)
{
    // Simplified mesh rendering
    // In a full implementation, this would use OpenGL buffers and shaders

    if (mesh.vertices.isEmpty()) {
        return;
    }

    // Set material color (highlighted if selected)
    QColor meshColor = mesh.diffuseColor.toVector3D().toVector4D().toQColor();
    if (selected) {
        meshColor = meshColor.lighter(150);
    }

    // Render mesh based on shading mode
    if (m_currentShadingMode == "wireframe") {
        // Render as wireframe
        renderWireframe(mesh, meshColor);
    } else if (m_currentShadingMode == "shaded") {
        // Render with lighting
        renderShaded(mesh, meshColor);
    } else {
        // Render solid
        renderSolid(mesh, meshColor);
    }
}

void DesignCanvas::renderGrid()
{
    // Render ground grid
    // This would use OpenGL to draw a grid
}

void DesignCanvas::renderAxes()
{
    // Render coordinate axes
    // This would use OpenGL to draw X, Y, Z axes
}

void DesignCanvas::renderSelection()
{
    // Render selection overlay
    // This would highlight selected objects
}

void DesignCanvas::updateCameraMatrices()
{
    // Update camera matrices for rendering
}

void DesignCanvas::updateCameraFromAngles()
{
    // Convert yaw/pitch to camera position
    float radYaw = qDegreesToRadians(m_cameraYaw);
    float radPitch = qDegreesToRadians(m_cameraPitch);

    m_cameraPosition.setX(m_cameraTarget.x() + m_cameraDistance * cos(radYaw) * cos(radPitch));
    m_cameraPosition.setY(m_cameraTarget.y() + m_cameraDistance * sin(radPitch));
    m_cameraPosition.setZ(m_cameraTarget.z() + m_cameraDistance * sin(radYaw) * cos(radPitch));
}

void DesignCanvas::updateCameraFromDistance()
{
    updateCameraFromAngles();
}

void DesignCanvas::updateCameraFromPosition()
{
    // Update yaw and pitch from current position
    QVector3D direction = m_cameraPosition - m_cameraTarget;
    m_cameraDistance = direction.length();

    m_cameraYaw = qRadiansToDegrees(atan2(direction.z(), direction.x()));
    m_cameraPitch = qRadiansToDegrees(asin(direction.y() / m_cameraDistance));
}

void DesignCanvas::updateModelDisplay()
{
    // Update model information display
    updateModelInfoDisplay();
    updateMeshBuffers();
}

void DesignCanvas::updateModelInfoDisplay()
{
    // Update UI labels with model information
    QLabel* modelNameLabel = findChild<QLabel*>("modelNameLabel");
    QLabel* meshCountLabel = findChild<QLabel*>("meshCountLabel");
    QLabel* vertexCountLabel = findChild<QLabel*>("vertexCountLabel");

    if (modelNameLabel) {
        modelNameLabel->setText(m_modelLoaded ? QFileInfo(m_currentModel.filename).baseName() : "No model loaded");
    }

    if (meshCountLabel) {
        meshCountLabel->setText(QString("Meshes: %1").arg(m_currentModel.meshes.size()));
    }

    if (vertexCountLabel) {
        int totalVertices = 0;
        for (const MeshData& mesh : m_currentModel.meshes) {
            totalVertices += mesh.vertexCount;
        }
        vertexCountLabel->setText(QString("Vertices: %1").arg(totalVertices));
    }
}

void DesignCanvas::updateMeshBuffers()
{
    // Update OpenGL buffers for meshes
    // This would create/update VBOs and VAOs for each mesh
}

void DesignCanvas::fitCameraToModel()
{
    if (m_currentModel.meshes.isEmpty()) {
        return;
    }

    // Calculate model bounds
    QVector3D minBounds = m_currentModel.meshes[0].minBounds;
    QVector3D maxBounds = m_currentModel.meshes[0].maxBounds;

    for (const MeshData& mesh : m_currentModel.meshes) {
        minBounds.setX(qMin(minBounds.x(), mesh.minBounds.x()));
        minBounds.setY(qMin(minBounds.y(), mesh.minBounds.y()));
        minBounds.setZ(qMin(minBounds.z(), mesh.minBounds.z()));

        maxBounds.setX(qMax(maxBounds.x(), mesh.maxBounds.x()));
        maxBounds.setY(qMax(maxBounds.y(), mesh.maxBounds.y()));
        maxBounds.setZ(qMax(maxBounds.z(), mesh.maxBounds.z()));
    }

    // Calculate center and size
    QVector3D center = (minBounds + maxBounds) * 0.5f;
    QVector3D size = maxBounds - minBounds;

    // Position camera to view entire model
    float maxDim = qMax(size.x(), qMax(size.y(), size.z()));
    m_cameraDistance = maxDim * 1.5f;
    m_cameraTarget = center;
    m_cameraPosition = center + QVector3D(0, 0, m_cameraDistance);

    updateCameraFromPosition();
}

// Event handlers
void DesignCanvas::onCameraPresetSelected(const QString& preset)
{
    setViewMode(preset);
}

void DesignCanvas::onViewModeChanged(const QString& mode)
{
    setViewMode(mode);
}

void DesignCanvas::onShadingModeChanged(const QString& mode)
{
    setShadingMode(mode);
}

void DesignCanvas::onModelLoaded(const ModelData& model)
{
    m_currentModel = model;
    m_modelLoaded = true;
    updateModelInfoDisplay();
    fitCameraToModel();
}

void DesignCanvas::onModelUnloaded()
{
    m_modelLoaded = false;
    updateModelInfoDisplay();
}

void DesignCanvas::onAnimationFrame()
{
    if (m_animationPlaying) {
        m_animationTime += m_animationSpeed * 0.016f;
        // Update animated objects
    }
}

void DesignCanvas::onAnimationStateChanged()
{
    // Update animation UI controls
}

void DesignCanvas::onSelectionChanged()
{
    // Update selection display
}

void DesignCanvas::onMeasurementCompleted()
{
    // Handle measurement completion
}

void DesignCanvas::updateFPS()
{
    // Calculate FPS
    static int lastFrameCount = 0;
    static QTime lastTime = QTime::currentTime();

    QTime currentTime = QTime::currentTime();
    int elapsed = lastTime.msecsTo(currentTime);

    if (elapsed >= 1000) {
        int frames = m_frameCount - lastFrameCount;
        m_currentFPS = (frames * 1000.0f) / elapsed;

        lastFrameCount = m_frameCount;
        lastTime = currentTime;

        // Update status display
        if (statusBar()) {
            statusBar()->showMessage(QString("FPS: %1").arg(static_cast<int>(m_currentFPS)));
        }
    }
}

// Rendering helper methods
void DesignCanvas::renderWireframe(const MeshData& mesh, const QColor& color)
{
    // Wireframe rendering implementation
    // This would use OpenGL line rendering
}

void DesignCanvas::renderShaded(const MeshData& mesh, const QColor& color)
{
    // Shaded rendering implementation
    // This would use OpenGL with lighting calculations
}

void DesignCanvas::renderSolid(const MeshData& mesh, const QColor& color)
{
    // Solid rendering implementation
    // This would use OpenGL with flat shading
}