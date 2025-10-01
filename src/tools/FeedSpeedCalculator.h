#pragma once

#include "../core/BaseTypes.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QVector>
#include <cmath>

/**
 * @brief Advanced feed and speed calculation system for CNC machining
 *
 * Implements physics-based calculations for optimal machining parameters including:
 * - Surface feet per minute (SFM) calculations
 * - Chip load optimization
 * - Material removal rate (MRR) calculations
 * - Tool deflection and heat management
 * - Safety margin applications
 * - Multi-pass strategy optimization
 */
class FeedSpeedCalculator : public QObject
{
    Q_OBJECT

public:
    explicit FeedSpeedCalculator(QObject* parent = nullptr);
    virtual ~FeedSpeedCalculator() = default;

    // Material properties database
    struct MaterialProperties {
        QString name;
        QString category; // "metal", "wood", "plastic", "composite"

        // Physical properties
        float hardness;      // Brinell hardness
        float density;       // kg/m³
        float thermalConductivity; // W/m·K
        float meltingPoint;  // °C

        // Machining properties
        float surfaceFeetPerMinute; // SFM for HSS tools
        float chipLoad;      // Recommended chip load per tooth
        float maxChipLoad;   // Maximum safe chip load
        float toolWearRate;  // Relative wear rate

        // Cost factors
        float materialCost;  // Cost per cubic inch
        float machinability; // Relative ease of machining (0-100)

        MaterialProperties() : hardness(100), density(2700), thermalConductivity(200),
                              meltingPoint(660), surfaceFeetPerMinute(500), chipLoad(0.002),
                              maxChipLoad(0.005), toolWearRate(1.0), materialCost(1.0), machinability(50) {}
    };

    // Tool geometry and properties
    struct ToolGeometry {
        QString type;        // "endmill", "drill", "router", etc.
        float diameter;      // Tool diameter in inches
        int fluteCount;      // Number of flutes/cutting edges
        float fluteLength;   // Length of cutting flutes
        float helixAngle;    // Helix angle in degrees
        float rakeAngle;     // Rake angle in degrees
        QString material;    // Tool material ("HSS", "Carbide", "Diamond", etc.)

        // Tool-specific properties
        float maxRPM;        // Maximum safe RPM
        float maxFeedRate;   // Maximum feed rate
        float toolLife;      // Expected tool life in minutes

        ToolGeometry() : diameter(0.25), fluteCount(2), fluteLength(1.0), helixAngle(30),
                        rakeAngle(7), maxRPM(10000), maxFeedRate(100), toolLife(60) {}
    };

    // Cutting operation parameters
    struct CuttingParameters {
        float spindleSpeed;  // RPM
        float feedRate;      // IPM (inches per minute)
        float depthOfCut;    // DOC in inches
        float widthOfCut;    // WOC in inches
        int numberOfPasses;  // Number of passes required
        float stepOver;      // Step over percentage for finishing

        // Advanced parameters
        float chipThickness; // Actual chip thickness
        float materialRemovalRate; // MRR in cubic inches per minute
        float cuttingForce;  // Estimated cutting force in lbs
        float spindlePower;  // Required spindle power in HP

        CuttingParameters() : spindleSpeed(3000), feedRate(10), depthOfCut(0.125),
                            widthOfCut(0.25), numberOfPasses(1), stepOver(50),
                            chipThickness(0), materialRemovalRate(0), cuttingForce(0), spindlePower(0) {}
    };

    // Calculation result
    struct CalculationResult {
        CuttingParameters optimal;
        CuttingParameters conservative; // With safety margins
        CuttingParameters aggressive;   // Maximum performance

        // Analysis
        float efficiency;    // Overall efficiency (0-100)
        float safetyMargin;  // Applied safety margin
        QString warnings;    // Safety or performance warnings
        QString recommendations; // Optimization suggestions

        // Performance metrics
        float estimatedCycleTime; // Minutes
        float estimatedToolLife;  // Minutes
        float estimatedCost;      // Cost per part

        CalculationResult() : efficiency(0), safetyMargin(0), estimatedCycleTime(0),
                            estimatedToolLife(0), estimatedCost(0) {}
    };

    // Main calculation functions
    virtual CalculationResult calculateOptimalParameters(const QString& toolId,
                                                        const QString& material,
                                                        const QVariantMap& operationParams) = 0;

    virtual CuttingParameters calculateBasicFeedSpeed(const ToolGeometry& tool,
                                                     const MaterialProperties& material,
                                                     float depthOfCut,
                                                     float widthOfCut) = 0;

    virtual CuttingParameters calculateAdvancedParameters(const ToolGeometry& tool,
                                                         const MaterialProperties& material,
                                                         const QVariantMap& operationParams) = 0;

    // Material database management
    virtual void addMaterial(const MaterialProperties& material) = 0;
    virtual void updateMaterial(const QString& name, const MaterialProperties& material) = 0;
    virtual void removeMaterial(const QString& name) = 0;
    virtual MaterialProperties getMaterial(const QString& name) const = 0;
    virtual QList<MaterialProperties> getAllMaterials() const = 0;

    // Tool database integration
    virtual void setToolGeometry(const ToolGeometry& geometry) = 0;
    virtual ToolGeometry getToolGeometry() const = 0;

    // Advanced calculations
    virtual float calculateSurfaceFeetPerMinute(const ToolGeometry& tool,
                                               const MaterialProperties& material) = 0;

    virtual float calculateChipLoad(const ToolGeometry& tool,
                                   const MaterialProperties& material,
                                   float feedRate,
                                   float spindleSpeed) = 0;

    virtual float calculateMaterialRemovalRate(const CuttingParameters& params,
                                             const ToolGeometry& tool) = 0;

    virtual float calculateCuttingForce(const CuttingParameters& params,
                                       const ToolGeometry& tool,
                                       const MaterialProperties& material) = 0;

    virtual float calculateRequiredPower(const CuttingParameters& params,
                                        const ToolGeometry& tool,
                                        const MaterialProperties& material) = 0;

    // Safety and optimization
    virtual CuttingParameters applySafetyMargins(const CuttingParameters& params,
                                                float safetyFactor) = 0;

    virtual CuttingParameters optimizeForToolLife(const CuttingParameters& params,
                                                 float targetToolLife) = 0;

    virtual CuttingParameters optimizeForSurfaceFinish(const CuttingParameters& params,
                                                      float targetFinish) = 0;

    // Multi-pass strategies
    virtual QList<CuttingParameters> calculateMultiPassStrategy(const ToolGeometry& tool,
                                                               const MaterialProperties& material,
                                                               float totalDepth,
                                                               int maxPasses) = 0;

    // Performance monitoring
    virtual PerformanceMetrics getCalculationMetrics() const = 0;
    virtual void resetPerformanceMetrics() = 0;

signals:
    // Calculation events
    void calculationCompleted(const CalculationResult& result);
    void calculationFailed(const QString& error);

    // Warning events
    void safetyWarning(const QString& warning);
    void performanceWarning(const QString& warning);

    // Optimization events
    void parametersOptimized(const CuttingParameters& optimized);

protected:
    // Physics-based calculations
    virtual float calculateOptimalSpindleSpeed(const ToolGeometry& tool,
                                             const MaterialProperties& material) = 0;

    virtual float calculateOptimalFeedRate(const ToolGeometry& tool,
                                          const MaterialProperties& material,
                                          float spindleSpeed) = 0;

    virtual float calculateChipThickness(const ToolGeometry& tool,
                                        float feedRate,
                                        float spindleSpeed) = 0;

    // Thermal calculations
    virtual float calculateCuttingTemperature(const CuttingParameters& params,
                                             const ToolGeometry& tool,
                                             const MaterialProperties& material) = 0;

    virtual bool checkThermalLimits(const CuttingParameters& params,
                                   const ToolGeometry& tool,
                                   const MaterialProperties& material) = 0;

    // Tool deflection calculations
    virtual float calculateToolDeflection(const CuttingParameters& params,
                                         const ToolGeometry& tool) = 0;

    virtual bool checkDeflectionLimits(const CuttingParameters& params,
                                      const ToolGeometry& tool) = 0;

    // Material-specific optimizations
    virtual CuttingParameters optimizeForAluminum(const CuttingParameters& baseParams,
                                                 const ToolGeometry& tool) = 0;

    virtual CuttingParameters optimizeForSteel(const CuttingParameters& baseParams,
                                              const ToolGeometry& tool) = 0;

    virtual CuttingParameters optimizeForWood(const CuttingParameters& baseParams,
                                             const ToolGeometry& tool) = 0;

    virtual CuttingParameters optimizeForPlastic(const CuttingParameters& baseParams,
                                                const ToolGeometry& tool) = 0;

    // Database
    QList<MaterialProperties> m_materials;
    ToolGeometry m_currentTool;
};