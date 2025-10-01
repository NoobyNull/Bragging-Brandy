#include "FeedSpeedCalculator.h"
#include <QDebug>
#include <algorithm>

FeedSpeedCalculator::FeedSpeedCalculator(QObject* parent)
    : QObject(parent)
{
    // Initialize material database with comprehensive properties
    initializeMaterialDatabase();
    qRegisterMetaType<CalculationResult>("CalculationResult");
}

FeedSpeedCalculator::CalculationResult FeedSpeedCalculator::calculateOptimalParameters(
    const QString& toolId,
    const QString& material,
    const QVariantMap& operationParams)
{
    CalculationResult result;

    try {
        // Get tool geometry (would normally load from database)
        ToolGeometry tool = getToolGeometry();
        if (toolId.contains("endmill")) {
            tool.type = "endmill";
            tool.diameter = 0.25f;
            tool.fluteCount = 2;
        } else if (toolId.contains("drill")) {
            tool.type = "drill";
            tool.diameter = 0.125f;
            tool.fluteCount = 2;
        }

        // Get material properties
        MaterialProperties mat = getMaterial(material);
        if (mat.name.isEmpty()) {
            mat = getMaterial("aluminum_6061"); // Default fallback
        }

        // Extract operation parameters
        float depthOfCut = operationParams.value("depth_of_cut", 0.125f).toFloat();
        float widthOfCut = operationParams.value("width_of_cut", tool.diameter * 0.5f).toFloat();
        float targetFinish = operationParams.value("surface_finish", 63.0f).toFloat(); // Ra in microinches
        float targetToolLife = operationParams.value("tool_life", 60.0f).toFloat(); // minutes

        // Calculate base parameters
        CuttingParameters baseParams = calculateBasicFeedSpeed(tool, mat, depthOfCut, widthOfCut);

        // Calculate advanced parameters
        CuttingParameters advancedParams = calculateAdvancedParameters(tool, mat, operationParams);

        // Apply optimizations
        result.optimal = optimizeForToolLife(advancedParams, targetToolLife);
        result.optimal = optimizeForSurfaceFinish(result.optimal, targetFinish);

        // Generate conservative and aggressive variants
        result.conservative = applySafetyMargins(result.optimal, 0.7f); // 30% safety margin
        result.aggressive = applySafetyMargins(result.optimal, 1.3f);   // 30% more aggressive

        // Calculate analysis metrics
        result.efficiency = calculateOverallEfficiency(result.optimal, tool, mat);
        result.safetyMargin = 0.2f; // 20% safety margin applied
        result.estimatedCycleTime = calculateCycleTime(result.optimal, operationParams);
        result.estimatedToolLife = calculateToolLife(result.optimal, tool, mat);
        result.estimatedCost = calculateOperationCost(result.optimal, tool, mat, operationParams);

        // Generate warnings and recommendations
        result.warnings = generateWarnings(result.optimal, tool, mat);
        result.recommendations = generateRecommendations(result.optimal, tool, mat, operationParams);

        emit calculationCompleted(result);

        qInfo() << QString("Feed/speed calculation completed: %1 RPM, %2 IPM")
                  .arg(static_cast<int>(result.optimal.spindleSpeed))
                  .arg(result.optimal.feedRate, 0, 'f', 2);

        return result;

    } catch (const std::exception& e) {
        qCritical() << "Feed/speed calculation failed:" << e.what();
        emit calculationFailed(QString("Calculation error: %1").arg(e.what()));
        return result;
    }
}

FeedSpeedCalculator::CuttingParameters FeedSpeedCalculator::calculateBasicFeedSpeed(
    const ToolGeometry& tool,
    const MaterialProperties& material,
    float depthOfCut,
    float widthOfCut)
{
    CuttingParameters params;

    // Calculate optimal spindle speed based on SFM
    float sfm = material.surfaceFeetPerMinute;
    params.spindleSpeed = (sfm * 12.0f) / (M_PI * tool.diameter);

    // Apply tool-specific limits
    if (tool.type == "endmill") {
        params.spindleSpeed = qMin(params.spindleSpeed, tool.maxRPM * 0.8f); // 80% of max RPM
    } else if (tool.type == "drill") {
        // Drills typically run at lower speeds
        params.spindleSpeed *= 0.7f;
    }

    // Calculate feed rate based on chip load
    float chipLoad = material.chipLoad;
    params.feedRate = params.spindleSpeed * chipLoad * tool.fluteCount;

    // Adjust for depth and width of cut
    float adjustmentFactor = 1.0f;
    if (depthOfCut > tool.diameter * 0.5f) {
        adjustmentFactor *= 0.9f; // Reduce feed for deep cuts
    }
    if (widthOfCut > tool.diameter * 0.7f) {
        adjustmentFactor *= 0.85f; // Reduce feed for wide cuts
    }

    params.feedRate *= adjustmentFactor;
    params.depthOfCut = depthOfCut;
    params.widthOfCut = widthOfCut;

    // Calculate derived parameters
    params.chipThickness = calculateChipThickness(tool, params.feedRate, params.spindleSpeed);
    params.materialRemovalRate = calculateMaterialRemovalRate(params, tool);
    params.cuttingForce = calculateCuttingForce(params, tool, material);
    params.spindlePower = calculateRequiredPower(params, tool, material);

    return params;
}

FeedSpeedCalculator::CuttingParameters FeedSpeedCalculator::calculateAdvancedParameters(
    const ToolGeometry& tool,
    const MaterialProperties& material,
    const QVariantMap& operationParams)
{
    CuttingParameters params = calculateBasicFeedSpeed(tool, material,
                                                     operationParams.value("depth_of_cut", 0.125f).toFloat(),
                                                     operationParams.value("width_of_cut", tool.diameter * 0.5f).toFloat());

    // Apply material-specific optimizations
    if (material.category == "metal") {
        if (material.name.contains("aluminum")) {
            params = optimizeForAluminum(params, tool);
        } else if (material.name.contains("steel")) {
            params = optimizeForSteel(params, tool);
        }
    } else if (material.category == "wood") {
        params = optimizeForWood(params, tool);
    } else if (material.category == "plastic") {
        params = optimizeForPlastic(params, tool);
    }

    // Apply thermal considerations
    float cuttingTemp = calculateCuttingTemperature(params, tool, material);
    if (cuttingTemp > material.meltingPoint * 0.8f) {
        // Reduce speed to control heat
        params.spindleSpeed *= 0.9f;
        params.feedRate *= 0.95f;
    }

    // Apply tool deflection considerations
    float deflection = calculateToolDeflection(params, tool);
    if (deflection > tool.diameter * 0.01f) { // More than 1% of diameter
        // Reduce feed to minimize deflection
        params.feedRate *= 0.9f;
    }

    return params;
}

void FeedSpeedCalculator::addMaterial(const MaterialProperties& material)
{
    // Check if material already exists
    for (int i = 0; i < m_materials.size(); ++i) {
        if (m_materials[i].name == material.name) {
            m_materials[i] = material;
            return;
        }
    }

    // Add new material
    m_materials.append(material);
}

void FeedSpeedCalculator::updateMaterial(const QString& name, const MaterialProperties& material)
{
    for (int i = 0; i < m_materials.size(); ++i) {
        if (m_materials[i].name == name) {
            m_materials[i] = material;
            return;
        }
    }
}

void FeedSpeedCalculator::removeMaterial(const QString& name)
{
    for (int i = 0; i < m_materials.size(); ++i) {
        if (m_materials[i].name == name) {
            m_materials.removeAt(i);
            return;
        }
    }
}

FeedSpeedCalculator::MaterialProperties FeedSpeedCalculator::getMaterial(const QString& name) const
{
    for (const MaterialProperties& material : m_materials) {
        if (material.name == name) {
            return material;
        }
    }

    return MaterialProperties(); // Return empty material
}

QList<FeedSpeedCalculator::MaterialProperties> FeedSpeedCalculator::getAllMaterials() const
{
    return m_materials;
}

void FeedSpeedCalculator::setToolGeometry(const ToolGeometry& geometry)
{
    m_currentTool = geometry;
}

FeedSpeedCalculator::ToolGeometry FeedSpeedCalculator::getToolGeometry() const
{
    return m_currentTool;
}

float FeedSpeedCalculator::calculateSurfaceFeetPerMinute(const ToolGeometry& tool,
                                                        const MaterialProperties& material)
{
    // Base SFM calculation
    float baseSFM = material.surfaceFeetPerMinute;

    // Adjust for tool material
    if (tool.material == "Carbide") {
        baseSFM *= 2.0f; // Carbide can run at higher speeds
    } else if (tool.material == "HSS") {
        baseSFM *= 1.0f; // HSS baseline
    } else if (tool.material == "Diamond") {
        baseSFM *= 3.0f; // Diamond can run much faster
    }

    // Adjust for tool diameter (smaller tools can run faster)
    float diameterFactor = 1.0f + (0.5f - tool.diameter) * 0.2f;
    diameterFactor = qBound(0.8f, diameterFactor, 1.2f);

    return baseSFM * diameterFactor;
}

float FeedSpeedCalculator::calculateChipLoad(const ToolGeometry& tool,
                                            const MaterialProperties& material,
                                            float feedRate,
                                            float spindleSpeed)
{
    if (spindleSpeed == 0 || tool.fluteCount == 0) {
        return 0.0f;
    }

    return feedRate / (spindleSpeed * tool.fluteCount);
}

float FeedSpeedCalculator::calculateMaterialRemovalRate(const CuttingParameters& params,
                                                      const ToolGeometry& tool)
{
    return params.feedRate * params.depthOfCut * params.widthOfCut;
}

float FeedSpeedCalculator::calculateCuttingForce(const CuttingParameters& params,
                                                const ToolGeometry& tool,
                                                const MaterialProperties& material)
{
    // Simplified cutting force calculation
    // In practice, this would use more sophisticated formulas

    float baseForce = params.materialRemovalRate * material.hardness * 0.1f;

    // Adjust for tool geometry
    if (tool.type == "endmill") {
        baseForce *= 1.2f; // End mills typically have higher cutting forces
    } else if (tool.type == "drill") {
        baseForce *= 1.5f; // Drills have high axial forces
    }

    // Adjust for material properties
    if (material.category == "metal") {
        baseForce *= 1.5f; // Metals require more force
    } else if (material.category == "wood") {
        baseForce *= 0.8f; // Wood requires less force
    }

    return baseForce;
}

float FeedSpeedCalculator::calculateRequiredPower(const CuttingParameters& params,
                                                 const ToolGeometry& tool,
                                                 const MaterialProperties& material)
{
    // Power calculation: P = Force * Velocity
    float force = calculateCuttingForce(params, tool, material);
    float velocity = params.feedRate / 60.0f; // Convert IPM to inches per second

    // Convert to horsepower
    float powerWatts = force * velocity;
    float powerHP = powerWatts / 745.7f; // Convert watts to HP

    return powerHP;
}

FeedSpeedCalculator::CuttingParameters FeedSpeedCalculator::applySafetyMargins(
    const CuttingParameters& params,
    float safetyFactor)
{
    CuttingParameters safeParams = params;

    // Apply safety factor to speed and feed
    safeParams.spindleSpeed *= safetyFactor;
    safeParams.feedRate *= safetyFactor;

    // Reduce depth of cut for safety
    safeParams.depthOfCut *= safetyFactor;

    // Recalculate dependent parameters
    safeParams.chipThickness = calculateChipThickness(m_currentTool, safeParams.feedRate, safeParams.spindleSpeed);
    safeParams.materialRemovalRate = calculateMaterialRemovalRate(safeParams, m_currentTool);
    safeParams.cuttingForce = calculateCuttingForce(safeParams, m_currentTool, getMaterial("aluminum_6061"));
    safeParams.spindlePower = calculateRequiredPower(safeParams, m_currentTool, getMaterial("aluminum_6061"));

    return safeParams;
}

FeedSpeedCalculator::CuttingParameters FeedSpeedCalculator::optimizeForToolLife(
    const CuttingParameters& params,
    float targetToolLife)
{
    CuttingParameters optimized = params;

    // Adjust parameters to achieve target tool life
    float currentToolLife = calculateToolLife(params, m_currentTool, getMaterial("aluminum_6061"));

    if (currentToolLife < targetToolLife) {
        // Need longer tool life - reduce speed and feed
        float adjustmentFactor = targetToolLife / currentToolLife;
        adjustmentFactor = qMin(adjustmentFactor, 0.8f); // Don't reduce more than 20%

        optimized.spindleSpeed *= adjustmentFactor;
        optimized.feedRate *= adjustmentFactor;
    } else {
        // Can increase performance while staying within tool life
        float adjustmentFactor = targetToolLife / currentToolLife;
        adjustmentFactor = qMin(adjustmentFactor * 1.1f, 1.1f); // Small increase

        optimized.spindleSpeed *= adjustmentFactor;
        optimized.feedRate *= adjustmentFactor;
    }

    return optimized;
}

FeedSpeedCalculator::CuttingParameters FeedSpeedCalculator::optimizeForSurfaceFinish(
    const CuttingParameters& params,
    float targetFinish)
{
    CuttingParameters optimized = params;

    // Adjust feed rate for surface finish
    // Lower feed rates generally produce better surface finish
    float currentFinish = estimateSurfaceFinish(params, m_currentTool);

    if (currentFinish > targetFinish) {
        // Need better finish - reduce feed rate
        float improvementNeeded = targetFinish / currentFinish;
        optimized.feedRate *= improvementNeeded;
    }

    return optimized;
}

QList<FeedSpeedCalculator::CuttingParameters> FeedSpeedCalculator::calculateMultiPassStrategy(
    const ToolGeometry& tool,
    const MaterialProperties& material,
    float totalDepth,
    int maxPasses)
{
    QList<CuttingParameters> strategy;

    if (totalDepth <= 0 || maxPasses <= 0) {
        return strategy;
    }

    // Calculate depth per pass
    float depthPerPass = totalDepth / maxPasses;

    // First pass: roughing (heavier cut)
    CuttingParameters roughing = calculateBasicFeedSpeed(tool, material, depthPerPass * 1.5f, tool.diameter * 0.6f);
    roughing.numberOfPasses = 1;
    strategy.append(roughing);

    // Intermediate passes
    for (int i = 1; i < maxPasses - 1; ++i) {
        CuttingParameters intermediate = calculateBasicFeedSpeed(tool, material, depthPerPass, tool.diameter * 0.4f);
        intermediate.numberOfPasses = i + 1;
        strategy.append(intermediate);
    }

    // Final pass: finishing (light cut for good surface finish)
    CuttingParameters finishing = calculateBasicFeedSpeed(tool, material, depthPerPass * 0.5f, tool.diameter * 0.2f);
    finishing.numberOfPasses = maxPasses;
    finishing.feedRate *= 0.7f; // Reduce feed for better finish
    strategy.append(finishing);

    return strategy;
}

PerformanceMetrics FeedSpeedCalculator::getCalculationMetrics() const
{
    PerformanceMetrics metrics;
    metrics.operationType = "FeedSpeedCalculator";
    metrics.operationTimeMs = 0; // Would be tracked during calculations
    metrics.memoryUsageBytes = 0; // Would be tracked
    return metrics;
}

void FeedSpeedCalculator::resetPerformanceMetrics()
{
    // Reset performance tracking
}

void FeedSpeedCalculator::initializeMaterialDatabase()
{
    // Initialize with comprehensive material database

    // Aluminum alloys
    MaterialProperties aluminum6061;
    aluminum6061.name = "Aluminum 6061";
    aluminum6061.category = "metal";
    aluminum6061.hardness = 95.0f;
    aluminum6061.density = 2700.0f;
    aluminum6061.thermalConductivity = 170.0f;
    aluminum6061.meltingPoint = 582.0f;
    aluminum6061.surfaceFeetPerMinute = 800.0f;
    aluminum6061.chipLoad = 0.002f;
    aluminum6061.maxChipLoad = 0.005f;
    aluminum6061.toolWearRate = 0.8f;
    aluminum6061.materialCost = 2.50f;
    aluminum6061.machinability = 85.0f;
    addMaterial(aluminum6061);

    // Steel alloys
    MaterialProperties steel1018;
    steel1018.name = "Steel 1018";
    steel1018.category = "metal";
    steel1018.hardness = 125.0f;
    steel1018.density = 7870.0f;
    steel1018.thermalConductivity = 50.0f;
    steel1018.meltingPoint = 1425.0f;
    steel1018.surfaceFeetPerMinute = 200.0f;
    steel1018.chipLoad = 0.001f;
    steel1018.maxChipLoad = 0.003f;
    steel1018.toolWearRate = 1.5f;
    steel1018.materialCost = 1.20f;
    steel1018.machinability = 55.0f;
    addMaterial(steel1018);

    // Wood materials
    MaterialProperties plywood;
    plywood.name = "Plywood (Birch)";
    plywood.category = "wood";
    plywood.hardness = 50.0f;
    plywood.density = 600.0f;
    plywood.thermalConductivity = 0.15f;
    plywood.meltingPoint = 300.0f;
    plywood.surfaceFeetPerMinute = 1000.0f;
    plywood.chipLoad = 0.005f;
    plywood.maxChipLoad = 0.012f;
    plywood.toolWearRate = 0.6f;
    plywood.materialCost = 3.00f;
    plywood.machinability = 90.0f;
    addMaterial(plywood);

    // Plastic materials
    MaterialProperties acrylic;
    acrylic.name = "Acrylic";
    acrylic.category = "plastic";
    acrylic.hardness = 85.0f;
    acrylic.density = 1190.0f;
    acrylic.thermalConductivity = 0.2f;
    acrylic.meltingPoint = 160.0f;
    acrylic.surfaceFeetPerMinute = 400.0f;
    acrylic.chipLoad = 0.003f;
    acrylic.maxChipLoad = 0.008f;
    acrylic.toolWearRate = 0.9f;
    acrylic.materialCost = 4.50f;
    acrylic.machinability = 70.0f;
    addMaterial(acrylic);

    // Additional materials
    MaterialProperties brass;
    brass.name = "Brass";
    brass.category = "metal";
    brass.hardness = 100.0f;
    brass.density = 8730.0f;
    brass.thermalConductivity = 120.0f;
    brass.meltingPoint = 900.0f;
    brass.surfaceFeetPerMinute = 300.0f;
    brass.chipLoad = 0.002f;
    brass.maxChipLoad = 0.004f;
    brass.toolWearRate = 1.2f;
    brass.materialCost = 5.00f;
    brass.machinability = 75.0f;
    addMaterial(brass);
}

float FeedSpeedCalculator::calculateOptimalSpindleSpeed(const ToolGeometry& tool,
                                                      const MaterialProperties& material)
{
    float sfm = calculateSurfaceFeetPerMinute(tool, material);
    return (sfm * 12.0f) / (M_PI * tool.diameter);
}

float FeedSpeedCalculator::calculateOptimalFeedRate(const ToolGeometry& tool,
                                                   const MaterialProperties& material,
                                                   float spindleSpeed)
{
    float chipLoad = material.chipLoad;
    return spindleSpeed * chipLoad * tool.fluteCount;
}

float FeedSpeedCalculator::calculateChipThickness(const ToolGeometry& tool,
                                                 float feedRate,
                                                 float spindleSpeed)
{
    if (spindleSpeed == 0 || tool.fluteCount == 0) {
        return 0.0f;
    }

    return feedRate / (spindleSpeed * tool.fluteCount);
}

float FeedSpeedCalculator::calculateCuttingTemperature(const CuttingParameters& params,
                                                      const ToolGeometry& tool,
                                                      const MaterialProperties& material)
{
    // Simplified temperature calculation
    // In practice, this would use more sophisticated thermal modeling

    float baseTemp = 100.0f; // Room temperature

    // Heat generation based on material removal rate
    float heatGeneration = params.materialRemovalRate * material.hardness * 0.01f;

    // Heat dissipation based on thermal conductivity
    float heatDissipation = material.thermalConductivity * 0.1f;

    float temperature = baseTemp + heatGeneration / heatDissipation;

    return temperature;
}

bool FeedSpeedCalculator::checkThermalLimits(const CuttingParameters& params,
                                            const ToolGeometry& tool,
                                            const MaterialProperties& material)
{
    float cuttingTemp = calculateCuttingTemperature(params, tool, material);

    // Check against material melting point
    if (cuttingTemp > material.meltingPoint * 0.9f) {
        return false; // Too hot
    }

    // Check against tool limits
    if (tool.material == "HSS" && cuttingTemp > 600.0f) {
        return false; // HSS tools overheat
    } else if (tool.material == "Carbide" && cuttingTemp > 900.0f) {
        return false; // Carbide tools overheat
    }

    return true;
}

float FeedSpeedCalculator::calculateToolDeflection(const CuttingParameters& params,
                                                  const ToolGeometry& tool)
{
    // Simplified deflection calculation
    // In practice, this would use beam deflection formulas

    float force = calculateCuttingForce(params, tool, getMaterial("aluminum_6061"));
    float length = tool.fluteLength;

    // Simplified cantilever beam deflection
    float deflection = (force * length * length * length) / (3.0f * 30000000.0f * M_PI * tool.diameter * tool.diameter * tool.diameter / 64.0f);

    return deflection;
}

bool FeedSpeedCalculator::checkDeflectionLimits(const CuttingParameters& params,
                                               const ToolGeometry& tool)
{
    float deflection = calculateToolDeflection(params, tool);

    // Deflection should be less than 1% of tool diameter
    float maxDeflection = tool.diameter * 0.01f;

    return deflection <= maxDeflection;
}

FeedSpeedCalculator::CuttingParameters FeedSpeedCalculator::optimizeForAluminum(
    const CuttingParameters& baseParams,
    const ToolGeometry& tool)
{
    CuttingParameters optimized = baseParams;

    // Aluminum can handle higher speeds and feeds
    optimized.spindleSpeed *= 1.2f;
    optimized.feedRate *= 1.1f;

    // Aluminum generates less heat
    if (checkThermalLimits(optimized, tool, getMaterial("aluminum_6061"))) {
        optimized.spindleSpeed *= 1.1f; // Additional speed increase
    }

    return optimized;
}

FeedSpeedCalculator::CuttingParameters FeedSpeedCalculator::optimizeForSteel(
    const CuttingParameters& baseParams,
    const ToolGeometry& tool)
{
    CuttingParameters optimized = baseParams;

    // Steel needs more conservative parameters
    optimized.spindleSpeed *= 0.8f;
    optimized.feedRate *= 0.9f;
    optimized.depthOfCut *= 0.9f;

    return optimized;
}

FeedSpeedCalculator::CuttingParameters FeedSpeedCalculator::optimizeForWood(
    const CuttingParameters& baseParams,
    const ToolGeometry& tool)
{
    CuttingParameters optimized = baseParams;

    // Wood can handle higher feeds but needs dust extraction consideration
    optimized.feedRate *= 1.3f;
    optimized.spindleSpeed *= 1.1f;

    return optimized;
}

FeedSpeedCalculator::CuttingParameters FeedSpeedCalculator::optimizeForPlastic(
    const CuttingParameters& baseParams,
    const ToolGeometry& tool)
{
    CuttingParameters optimized = baseParams;

    // Plastics need lower temperatures to avoid melting
    optimized.spindleSpeed *= 0.9f;
    optimized.feedRate *= 0.95f;

    // Check thermal limits
    if (!checkThermalLimits(optimized, tool, getMaterial("acrylic"))) {
        optimized.spindleSpeed *= 0.9f;
        optimized.feedRate *= 0.95f;
    }

    return optimized;
}

float FeedSpeedCalculator::calculateOverallEfficiency(const CuttingParameters& params,
                                                     const ToolGeometry& tool,
                                                     const MaterialProperties& material)
{
    // Calculate overall efficiency based on multiple factors
    float speedEfficiency = qMin(params.spindleSpeed / tool.maxRPM, 1.0f);
    float feedEfficiency = qMin(params.feedRate / tool.maxFeedRate, 1.0f);
    float mrrEfficiency = qMin(params.materialRemovalRate / (tool.maxFeedRate * tool.diameter), 1.0f);

    // Weighted average
    return (speedEfficiency * 0.3f + feedEfficiency * 0.4f + mrrEfficiency * 0.3f) * 100.0f;
}

float FeedSpeedCalculator::calculateCycleTime(const CuttingParameters& params,
                                             const QVariantMap& operationParams)
{
    float cutLength = operationParams.value("cut_length", 10.0f).toFloat();
    float rapidSpeed = operationParams.value("rapid_speed", 200.0f).toFloat();

    // Calculate cutting time
    float cuttingTime = cutLength / params.feedRate;

    // Add rapid traverse time (simplified)
    float rapidTime = cutLength / rapidSpeed * 0.1f; // Assume 10% rapid moves

    return cuttingTime + rapidTime;
}

float FeedSpeedCalculator::calculateToolLife(const CuttingParameters& params,
                                           const ToolGeometry& tool,
                                           const MaterialProperties& material)
{
    // Simplified tool life calculation
    // In practice, this would use Taylor's tool life equation

    float baseLife = tool.toolLife;

    // Adjust for material hardness
    float hardnessFactor = material.hardness / 100.0f;
    baseLife /= hardnessFactor;

    // Adjust for cutting speed
    float speedFactor = params.spindleSpeed / 5000.0f;
    baseLife /= pow(speedFactor, 2.0f);

    return baseLife;
}

float FeedSpeedCalculator::calculateOperationCost(const CuttingParameters& params,
                                                 const ToolGeometry& tool,
                                                 const MaterialProperties& material,
                                                 const QVariantMap& operationParams)
{
    float cycleTime = calculateCycleTime(params, operationParams);
    float toolLife = calculateToolLife(params, tool, material);

    // Tool cost per minute
    float toolCostPerMinute = tool.replacementCost / toolLife;

    // Machine cost per minute (assume $50/hour)
    float machineCostPerMinute = 50.0f / 60.0f;

    // Material cost (simplified)
    float materialCost = material.materialCost * params.materialRemovalRate * cycleTime;

    return (toolCostPerMinute + machineCostPerMinute) * cycleTime + materialCost;
}

float FeedSpeedCalculator::estimateSurfaceFinish(const CuttingParameters& params,
                                                const ToolGeometry& tool)
{
    // Simplified surface finish estimation
    // In practice, this would use more sophisticated models

    float baseFinish = 125.0f; // Base Ra in microinches

    // Feed rate affects surface finish
    float feedFactor = params.feedRate / 10.0f;
    baseFinish *= pow(feedFactor, 0.3f);

    // Tool sharpness affects finish
    float sharpnessFactor = 1.0f; // Would be based on tool wear
    baseFinish *= sharpnessFactor;

    return baseFinish;
}

QString FeedSpeedCalculator::generateWarnings(const CuttingParameters& params,
                                             const ToolGeometry& tool,
                                             const MaterialProperties& material)
{
    QStringList warnings;

    // Check thermal limits
    if (!checkThermalLimits(params, tool, material)) {
        warnings.append("Cutting temperature may exceed safe limits");
    }

    // Check deflection limits
    if (!checkDeflectionLimits(params, tool)) {
        warnings.append("Tool deflection may affect accuracy");
    }

    // Check power requirements
    if (params.spindlePower > 5.0f) { // Assume 5HP machine
        warnings.append("Cutting power may exceed machine capacity");
    }

    // Check chip load limits
    float chipLoad = calculateChipLoad(tool, material, params.feedRate, params.spindleSpeed);
    if (chipLoad > material.maxChipLoad) {
        warnings.append("Chip load exceeds material recommendations");
    }

    return warnings.join("; ");
}

QString FeedSpeedCalculator::generateRecommendations(const CuttingParameters& params,
                                                    const ToolGeometry& tool,
                                                    const MaterialProperties& material,
                                                    const QVariantMap& operationParams)
{
    QStringList recommendations;

    // Analyze current parameters and suggest improvements
    float efficiency = calculateOverallEfficiency(params, tool, material);

    if (efficiency < 60.0f) {
        recommendations.append("Consider optimizing feed and speed for better efficiency");
    }

    if (params.depthOfCut > tool.diameter * 0.5f) {
        recommendations.append("Consider multiple passes for deep cuts");
    }

    if (params.widthOfCut > tool.diameter * 0.6f) {
        recommendations.append("Consider smaller step-over for better finish");
    }

    return recommendations.join("; ");
}