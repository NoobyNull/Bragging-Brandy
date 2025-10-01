#include "MaterialOptimizer.h"
#include <QElapsedTimer>
#include <QDebug>
#include <algorithm>
#include <cmath>
#include <numeric>

MaterialOptimizer::MaterialOptimizer(QObject* parent)
    : QObject(parent)
    , m_randomEngine(std::random_device{}())
    , m_uniformDistribution(0.0f, 1.0f)
    , m_totalOptimizationTime(0)
    , m_optimizationRuns(0)
{
    // Initialize default configuration
    m_config.algorithm = "genetic";
    m_config.populationSize = 100;
    m_config.maxGenerations = 200;
    m_config.mutationRate = 0.1f;
    m_config.crossoverRate = 0.8f;
    m_config.elitismRate = 0.1f;
    m_config.minPartDistance = 0.125f; // 1/8 inch minimum distance
    m_config.respectGrainDirection = true;
    m_config.allowRotation = true;
    m_config.useParallelProcessing = true;
    m_config.maxOptimizationTimeMs = 30000; // 30 seconds

    qRegisterMetaType<OptimizationResult>("OptimizationResult");
    qRegisterMetaType<OptimizationConfig>("OptimizationConfig");
}

QFuture<MaterialOptimizer::OptimizationResult> MaterialOptimizer::optimizeNestingAsync(
    const QList<Part>& parts,
    const QList<Sheet>& availableSheets,
    const OptimizationConfig& config)
{
    return QtConcurrent::run([this, parts, availableSheets, config]() -> OptimizationResult {
        return optimizeNesting(parts, availableSheets, config);
    });
}

MaterialOptimizer::OptimizationResult MaterialOptimizer::optimizeNesting(
    const QList<Part>& parts,
    const QList<Sheet>& availableSheets,
    const OptimizationConfig& config)
{
    QElapsedTimer timer;
    timer.start();

    OptimizationResult result;

    try {
        emit optimizationStarted(config.algorithm);

        // Select optimal sheet sizes first
        QList<Sheet> optimalSheets = selectOptimalSheetSizes(parts, availableSheets);

        if (config.algorithm == "genetic") {
            result = geneticAlgorithmOptimization(parts, optimalSheets, config);
        } else if (config.algorithm == "simulated_annealing") {
            result = simulatedAnnealingOptimization(parts, optimalSheets, config);
        } else if (config.algorithm == "particle_swarm") {
            result = particleSwarmOptimization(parts, optimalSheets, config);
        } else {
            // Default to genetic algorithm
            result = geneticAlgorithmOptimization(parts, optimalSheets, config);
        }

        result.optimizationTimeMs = timer.elapsed();

        // Update performance metrics
        m_totalOptimizationTime += result.optimizationTimeMs;
        m_optimizationRuns++;
        updateOptimizationMetrics(result);

        emit optimizationCompleted(result);

        qInfo() << QString("Material optimization completed in %1ms: %2% efficiency, %3 sheets")
                  .arg(result.optimizationTimeMs)
                  .arg(result.totalEfficiency, 0, 'f', 1)
                  .arg(result.totalSheetsUsed);

        return result;

    } catch (const std::exception& e) {
        qCritical() << "Material optimization failed:" << e.what();
        result.optimizationTimeMs = timer.elapsed();
        return result;
    }
}

MaterialOptimizer::OptimizationResult MaterialOptimizer::geneticAlgorithmOptimization(
    const QList<Part>& parts,
    const QList<Sheet>& sheets,
    const OptimizationConfig& config)
{
    OptimizationResult result;

    if (parts.isEmpty() || sheets.isEmpty()) {
        return result;
    }

    QElapsedTimer generationTimer;

    // Initialize population
    QList<QList<QPointF>> population = initializePopulation(parts, sheets, config.populationSize);

    float bestFitness = 0.0f;
    QList<QPointF> bestIndividual;

    // Evolution loop
    for (int generation = 0; generation < config.maxGenerations; ++generation) {
        generationTimer.restart();

        // Evaluate fitness
        QList<float> fitness = evaluateFitness(population, parts, sheets);

        // Find best individual
        int bestIndex = std::max_element(fitness.begin(), fitness.end()) - fitness.begin();
        float currentBestFitness = fitness[bestIndex];

        if (currentBestFitness > bestFitness) {
            bestFitness = currentBestFitness;
            bestIndividual = population[bestIndex];

            emit populationImproved(bestFitness);
        }

        // Elitism: keep best individuals
        QList<QList<QPointF>> newPopulation = selectElite(population, fitness, config.elitismRate);

        // Generate new individuals through crossover and mutation
        while (newPopulation.size() < config.populationSize) {
            // Selection
            QList<QPointF> parent1 = tournamentSelection(population, fitness);
            QList<QPointF> parent2 = tournamentSelection(population, fitness);

            // Crossover
            QList<QPointF> offspring;
            if (m_uniformDistribution(m_randomEngine) < config.crossoverRate) {
                offspring = crossover(parent1, parent2);
            } else {
                offspring = parent1; // No crossover
            }

            // Mutation
            mutation(offspring, config.mutationRate);

            newPopulation.append(offspring);
        }

        population = newPopulation;

        // Update progress
        int progress = (generation * 100) / config.maxGenerations;
        emit optimizationProgress(progress, generation, bestFitness);

        // Process events for UI responsiveness
        if (generation % 10 == 0) {
            QCoreApplication::processEvents();
        }

        emit generationCompleted(generation, bestFitness, calculateAverageFitness(fitness));
    }

    // Generate final result from best individual
    result = generateResultFromIndividual(bestIndividual, parts, sheets);
    result.totalEfficiency = bestFitness * 100.0f; // Convert to percentage

    return result;
}

MaterialOptimizer::OptimizationResult MaterialOptimizer::simulatedAnnealingOptimization(
    const QList<Part>& parts,
    const QList<Sheet>& sheets,
    const OptimizationConfig& config)
{
    OptimizationResult result;

    if (parts.isEmpty() || sheets.isEmpty()) {
        return result;
    }

    // Initialize with random solution
    QList<QPointF> currentSolution = generateRandomSolution(parts, sheets[0]);
    float currentFitness = calculateEfficiencyFitness(currentSolution, parts, sheets);

    QList<QPointF> bestSolution = currentSolution;
    float bestFitness = currentFitness;

    float temperature = 100.0f;
    float coolingRate = 0.95f;

    for (int iteration = 0; iteration < config.maxGenerations; ++iteration) {
        // Generate neighbor solution
        QList<QPointF> neighborSolution = generateNeighborSolution(currentSolution, parts, sheets[0]);
        float neighborFitness = calculateEfficiencyFitness(neighborSolution, parts, sheets);

        // Accept neighbor if better, or with probability based on temperature
        float fitnessDiff = neighborFitness - currentFitness;
        bool acceptNeighbor = (fitnessDiff > 0) ||
                            (m_uniformDistribution(m_randomEngine) < exp(fitnessDiff / temperature));

        if (acceptNeighbor) {
            currentSolution = neighborSolution;
            currentFitness = neighborFitness;

            if (currentFitness > bestFitness) {
                bestSolution = currentSolution;
                bestFitness = currentFitness;
                emit populationImproved(bestFitness);
            }
        }

        // Cool down temperature
        temperature *= coolingRate;

        // Update progress
        int progress = (iteration * 100) / config.maxGenerations;
        emit optimizationProgress(progress, iteration, bestFitness);

        // Process events for UI responsiveness
        if (iteration % 50 == 0) {
            QCoreApplication::processEvents();
        }
    }

    // Generate final result
    result = generateResultFromIndividual(bestSolution, parts, sheets);
    result.totalEfficiency = bestFitness * 100.0f;

    return result;
}

MaterialOptimizer::OptimizationResult MaterialOptimizer::particleSwarmOptimization(
    const QList<Part>& parts,
    const QList<Sheet>& sheets,
    const OptimizationConfig& config)
{
    OptimizationResult result;

    if (parts.isEmpty() || sheets.isEmpty()) {
        return result;
    }

    const int particleCount = config.populationSize;
    const int maxIterations = config.maxGenerations;

    // Initialize particles
    struct Particle {
        QList<QPointF> position;
        QList<QPointF> velocity;
        QList<QPointF> bestPosition;
        float bestFitness;
    };

    QList<Particle> swarm(particleCount);

    for (Particle& particle : swarm) {
        particle.position = generateRandomSolution(parts, sheets[0]);
        particle.velocity = generateRandomVelocity(parts.count());
        particle.bestPosition = particle.position;
        particle.bestFitness = calculateEfficiencyFitness(particle.position, parts, sheets);
    }

    QList<QPointF> globalBestPosition;
    float globalBestFitness = 0.0f;

    // Find initial global best
    for (const Particle& particle : swarm) {
        if (particle.bestFitness > globalBestFitness) {
            globalBestFitness = particle.bestFitness;
            globalBestPosition = particle.bestPosition;
        }
    }

    // PSO parameters
    float w = 0.7f;  // Inertia weight
    float c1 = 1.5f; // Cognitive coefficient
    float c2 = 1.5f; // Social coefficient

    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        for (Particle& particle : swarm) {
            // Update velocity
            for (int i = 0; i < particle.velocity.size(); ++i) {
                float r1 = m_uniformDistribution(m_randomEngine);
                float r2 = m_uniformDistribution(m_randomEngine);

                QPointF cognitiveVelocity = (particle.bestPosition[i] - particle.position[i]) * c1 * r1;
                QPointF socialVelocity = (globalBestPosition[i] - particle.position[i]) * c2 * r2;

                particle.velocity[i] = particle.velocity[i] * w + cognitiveVelocity + socialVelocity;
            }

            // Update position
            for (int i = 0; i < particle.position.size(); ++i) {
                QPointF newPosition = particle.position[i] + particle.velocity[i];

                // Clamp to sheet boundaries
                newPosition.setX(qBound(0.0f, newPosition.x(), sheets[0].dimensions.width()));
                newPosition.setY(qBound(0.0f, newPosition.y(), sheets[0].dimensions.height()));

                particle.position[i] = newPosition;
            }

            // Evaluate fitness
            float currentFitness = calculateEfficiencyFitness(particle.position, parts, sheets);

            // Update personal best
            if (currentFitness > particle.bestFitness) {
                particle.bestFitness = currentFitness;
                particle.bestPosition = particle.position;
            }

            // Update global best
            if (currentFitness > globalBestFitness) {
                globalBestFitness = currentFitness;
                globalBestPosition = particle.position;
                emit populationImproved(globalBestFitness);
            }
        }

        // Update progress
        int progress = (iteration * 100) / maxIterations;
        emit optimizationProgress(progress, iteration, globalBestFitness);

        // Process events for UI responsiveness
        if (iteration % 20 == 0) {
            QCoreApplication::processEvents();
        }
    }

    // Generate final result
    result = generateResultFromIndividual(globalBestPosition, parts, sheets);
    result.totalEfficiency = globalBestFitness * 100.0f;

    return result;
}

MaterialOptimizer::OptimizationResult MaterialOptimizer::multiObjectiveOptimization(
    const QList<Part>& parts,
    const QList<Sheet>& sheets,
    const QStringList& objectives)
{
    OptimizationResult result;

    // Multi-objective optimization using Pareto front
    QList<OptimizationResult> paretoFront;

    // Run optimization with different objectives
    for (const QString& objective : objectives) {
        OptimizationConfig config = m_config;

        if (objective == "efficiency") {
            // Optimize for maximum efficiency
            config.algorithm = "genetic";
        } else if (objective == "cost") {
            // Optimize for minimum cost
            config.algorithm = "genetic";
        } else if (objective == "sheets") {
            // Optimize for minimum sheets used
            config.algorithm = "genetic";
        }

        OptimizationResult objResult = optimizeNesting(parts, sheets, config);
        paretoFront.append(objResult);
    }

    // Select best compromise solution
    if (!paretoFront.isEmpty()) {
        // Use weighted sum approach for now
        result = paretoFront.first();
    }

    return result;
}

bool MaterialOptimizer::validatePlacement(const Part& part, const QPointF& position, float rotation,
                                        const QSizeF& sheetSize, const QList<QPointF>& existingPositions)
{
    // Check sheet boundaries
    float cosRot = cos(rotation);
    float sinRot = sin(rotation);

    float width = part.dimensions.width();
    float height = part.dimensions.height();

    // Calculate rotated bounding box
    float rotatedWidth = abs(width * cosRot) + abs(height * sinRot);
    float rotatedHeight = abs(width * sinRot) + abs(height * cosRot);

    if (position.x() + rotatedWidth > sheetSize.width() ||
        position.y() + rotatedHeight > sheetSize.height() ||
        position.x() < 0 || position.y() < 0) {
        return false;
    }

    // Check collisions with existing parts
    for (const QPointF& existingPos : existingPositions) {
        if (partsOverlap(position, QSizeF(width, height), rotation, existingPos, QSizeF(width, height), 0.0f)) {
            return false;
        }
    }

    // Check minimum distance constraint
    for (const QPointF& existingPos : existingPositions) {
        float distance = QLineF(position, existingPos).length();
        if (distance < m_config.minPartDistance) {
            return false;
        }
    }

    return true;
}

QList<QPointF> MaterialOptimizer::findValidPlacements(const Part& part, const QSizeF& sheetSize,
                                                    const QList<QPointF>& existingPositions)
{
    QList<QPointF> validPlacements;

    // Try different positions and rotations
    float stepSize = 0.5f; // 1/2 inch steps

    for (float x = 0; x < sheetSize.width(); x += stepSize) {
        for (float y = 0; y < sheetSize.height(); y += stepSize) {
            QPointF position(x, y);

            // Try both orientations
            for (float rotation : {0.0f, 90.0f, 180.0f, 270.0f}) {
                if (validatePlacement(part, position, rotation, sheetSize, existingPositions)) {
                    validPlacements.append(position);
                    break; // Only need one valid rotation per position
                }
            }
        }
    }

    return validPlacements;
}

QList<MaterialOptimizer::Sheet> MaterialOptimizer::selectOptimalSheetSizes(
    const QList<Part>& parts,
    const QList<Sheet>& availableSheets)
{
    QList<Sheet> optimalSheets;

    if (parts.isEmpty()) {
        return optimalSheets;
    }

    // Calculate total area needed
    float totalPartArea = 0.0f;
    for (const Part& part : parts) {
        totalPartArea += calculatePartArea(part) * part.quantity;
    }

    // Find sheets that can accommodate the parts efficiently
    for (const Sheet& sheet : availableSheets) {
        float sheetArea = calculateSheetArea(sheet);
        float efficiency = (totalPartArea / sheetArea) * 100.0f;

        // Select sheets where efficiency is reasonable (40-90%)
        if (efficiency >= 40.0f && efficiency <= 90.0f) {
            optimalSheets.append(sheet);
        }
    }

    // If no optimal sheets found, use the largest available
    if (optimalSheets.isEmpty() && !availableSheets.isEmpty()) {
        // Find largest sheet
        Sheet largestSheet = availableSheets[0];
        for (const Sheet& sheet : availableSheets) {
            if (calculateSheetArea(sheet) > calculateSheetArea(largestSheet)) {
                largestSheet = sheet;
            }
        }
        optimalSheets.append(largestSheet);
    }

    return optimalSheets;
}

PerformanceMetrics MaterialOptimizer::getOptimizationMetrics() const
{
    return m_optimizationMetrics;
}

void MaterialOptimizer::resetPerformanceMetrics()
{
    m_optimizationMetrics = PerformanceMetrics();
    m_totalOptimizationTime = 0;
    m_optimizationRuns = 0;
}

// Protected genetic algorithm implementation
QList<float> MaterialOptimizer::evaluateFitness(const QList<QList<QPointF>>& population,
                                              const QList<Part>& parts,
                                              const QList<Sheet>& sheets)
{
    QList<float> fitness;

    for (const QList<QPointF>& individual : population) {
        float efficiencyFitness = calculateEfficiencyFitness(individual, parts, sheets);
        float costFitness = calculateCostFitness(individual, parts, sheets);
        float compactnessFitness = calculateCompactnessFitness(individual, parts);

        // Weighted combination of fitness functions
        float combinedFitness = efficiencyFitness * 0.6f + costFitness * 0.3f + compactnessFitness * 0.1f;
        fitness.append(combinedFitness);
    }

    return fitness;
}

QList<QList<QPointF>> MaterialOptimizer::selection(const QList<QList<QPointF>>& population,
                                                  const QList<float>& fitness)
{
    QList<QList<QPointF>> selected;

    // Tournament selection
    int tournamentSize = 5;

    for (int i = 0; i < population.size(); ++i) {
        QList<QPointF> bestInTournament = population[0];
        float bestFitness = fitness[0];

        // Run tournament
        for (int j = 0; j < tournamentSize; ++j) {
            int randomIndex = std::uniform_int_distribution<int>(0, population.size() - 1)(m_randomEngine);

            if (fitness[randomIndex] > bestFitness) {
                bestFitness = fitness[randomIndex];
                bestInTournament = population[randomIndex];
            }
        }

        selected.append(bestInTournament);
    }

    return selected;
}

QList<QList<QPointF>> MaterialOptimizer::crossover(const QList<QList<QPointF>>& parents)
{
    QList<QList<QPointF>> offspring;

    // Two-point crossover for part positions
    for (int i = 0; i < parents.size(); i += 2) {
        if (i + 1 >= parents.size()) {
            offspring.append(parents[i]);
            break;
        }

        const QList<QPointF>& parent1 = parents[i];
        const QList<QPointF>& parent2 = parents[i + 1];

        QList<QPointF> child1 = parent1;
        QList<QPointF> child2 = parent2;

        // Choose two crossover points
        int crossoverPoint1 = std::uniform_int_distribution<int>(0, parent1.size() - 1)(m_randomEngine);
        int crossoverPoint2 = std::uniform_int_distribution<int>(crossoverPoint1, parent1.size() - 1)(m_randomEngine);

        // Swap segments between crossover points
        for (int j = crossoverPoint1; j <= crossoverPoint2; ++j) {
            child1[j] = parent2[j];
            child2[j] = parent1[j];
        }

        offspring.append(child1);
        offspring.append(child2);
    }

    return offspring;
}

void MaterialOptimizer::mutation(QList<QPointF>& individual, float mutationRate)
{
    for (QPointF& position : individual) {
        if (m_uniformDistribution(m_randomEngine) < mutationRate) {
            // Random displacement mutation
            float maxDisplacement = 2.0f; // inches
            float dx = std::uniform_real_distribution<float>(-maxDisplacement, maxDisplacement)(m_randomEngine);
            float dy = std::uniform_real_distribution<float>(-maxDisplacement, maxDisplacement)(m_randomEngine);

            position.setX(qMax(0.0f, qMin(96.0f, position.x() + dx))); // Clamp to 8ft sheet
            position.setY(qMax(0.0f, qMin(48.0f, position.y() + dy))); // Clamp to 4ft sheet
        }
    }
}

// Fitness function implementations
float MaterialOptimizer::calculateEfficiencyFitness(const QList<QPointF>& positions,
                                                  const QList<Part>& parts,
                                                  const QList<Sheet>& sheets)
{
    if (positions.isEmpty() || sheets.isEmpty()) {
        return 0.0f;
    }

    float sheetArea = calculateSheetArea(sheets[0]);
    float totalPartArea = 0.0f;

    for (int i = 0; i < parts.size() && i < positions.size(); ++i) {
        totalPartArea += calculatePartArea(parts[i]);
    }

    return totalPartArea / sheetArea;
}

float MaterialOptimizer::calculateCostFitness(const QList<QPointF>& positions,
                                             const QList<Part>& parts,
                                             const QList<Sheet>& sheets)
{
    if (sheets.isEmpty()) {
        return 0.0f;
    }

    // Lower cost is better (negative fitness)
    float totalCost = sheets[0].cost;
    return 1.0f / (totalCost + 1.0f); // Add 1 to avoid division by zero
}

float MaterialOptimizer::calculateCompactnessFitness(const QList<QPointF>& positions,
                                                   const QList<Part>& parts)
{
    if (positions.size() < 2) {
        return 1.0f; // Perfect compactness for single part
    }

    // Calculate average distance between parts (lower is more compact)
    float totalDistance = 0.0f;
    int pairCount = 0;

    for (int i = 0; i < positions.size(); ++i) {
        for (int j = i + 1; j < positions.size(); ++j) {
            float distance = QLineF(positions[i], positions[j]).length();
            totalDistance += distance;
            pairCount++;
        }
    }

    if (pairCount == 0) {
        return 1.0f;
    }

    float averageDistance = totalDistance / pairCount;

    // Convert to fitness (lower distance = higher fitness)
    return 1.0f / (averageDistance + 1.0f);
}

// Helper functions
QList<QList<QPointF>> MaterialOptimizer::initializePopulation(const QList<Part>& parts,
                                                            const QList<Sheet>& sheets,
                                                            int populationSize)
{
    QList<QList<QPointF>> population;

    if (sheets.isEmpty()) {
        return population;
    }

    const QSizeF& sheetSize = sheets[0].dimensions;

    for (int i = 0; i < populationSize; ++i) {
        QList<QPointF> individual;

        for (const Part& part : parts) {
            QPointF randomPosition(
                std::uniform_real_distribution<float>(0.0f, sheetSize.width() - part.dimensions.width())(m_randomEngine),
                std::uniform_real_distribution<float>(0.0f, sheetSize.height() - part.dimensions.height())(m_randomEngine)
            );
            individual.append(randomPosition);
        }

        population.append(individual);
    }

    return population;
}

QList<QPointF> MaterialOptimizer::generateRandomSolution(const QList<Part>& parts, const Sheet& sheet)
{
    QList<QPointF> solution;

    for (const Part& part : parts) {
        QPointF randomPosition(
            std::uniform_real_distribution<float>(0.0f, sheet.dimensions.width() - part.dimensions.width())(m_randomEngine),
            std::uniform_real_distribution<float>(0.0f, sheet.dimensions.height() - part.dimensions.height())(m_randomEngine)
        );
        solution.append(randomPosition);
    }

    return solution;
}

QList<QPointF> MaterialOptimizer::generateRandomVelocity(int partCount)
{
    QList<QPointF> velocity;

    for (int i = 0; i < partCount; ++i) {
        QPointF randomVelocity(
            std::uniform_real_distribution<float>(-1.0f, 1.0f)(m_randomEngine),
            std::uniform_real_distribution<float>(-1.0f, 1.0f)(m_randomEngine)
        );
        velocity.append(randomVelocity);
    }

    return velocity;
}

QList<QPointF> MaterialOptimizer::tournamentSelection(const QList<QList<QPointF>>& population,
                                                    const QList<float>& fitness)
{
    int tournamentSize = 3;
    int bestIndex = 0;
    float bestFitness = fitness[0];

    for (int i = 1; i < tournamentSize; ++i) {
        int randomIndex = std::uniform_int_distribution<int>(0, population.size() - 1)(m_randomEngine);

        if (fitness[randomIndex] > bestFitness) {
            bestFitness = fitness[randomIndex];
            bestIndex = randomIndex;
        }
    }

    return population[bestIndex];
}

QList<QList<QPointF>> MaterialOptimizer::selectElite(const QList<QList<QPointF>>& population,
                                                   const QList<float>& fitness,
                                                   float elitismRate)
{
    QList<QList<QPointF>> elite;

    int eliteCount = static_cast<int>(population.size() * elitismRate);

    // Create index-fitness pairs and sort
    QList<QPair<int, float>> indexedFitness;
    for (int i = 0; i < fitness.size(); ++i) {
        indexedFitness.append(QPair<int, float>(i, fitness[i]));
    }

    std::sort(indexedFitness.begin(), indexedFitness.end(),
              [](const QPair<int, float>& a, const QPair<int, float>& b) {
                  return a.second > b.second;
              });

    // Select elite individuals
    for (int i = 0; i < eliteCount; ++i) {
        elite.append(population[indexedFitness[i].first]);
    }

    return elite;
}

QList<QPointF> MaterialOptimizer::crossover(const QList<QPointF>& parent1, const QList<QPointF>& parent2)
{
    QList<QPointF> child;

    // Uniform crossover
    for (int i = 0; i < parent1.size(); ++i) {
        if (m_uniformDistribution(m_randomEngine) < 0.5f) {
            child.append(parent1[i]);
        } else {
            child.append(parent2[i]);
        }
    }

    return child;
}

QList<QPointF> MaterialOptimizer::generateNeighborSolution(const QList<QPointF>& currentSolution,
                                                         const QList<Part>& parts,
                                                         const Sheet& sheet)
{
    QList<QPointF> neighbor = currentSolution;

    // Select random part to modify
    int partIndex = std::uniform_int_distribution<int>(0, currentSolution.size() - 1)(m_randomEngine);

    // Generate new position for that part
    QPointF newPosition(
        std::uniform_real_distribution<float>(0.0f, sheet.dimensions.width() - parts[partIndex].dimensions.width())(m_randomEngine),
        std::uniform_real_distribution<float>(0.0f, sheet.dimensions.height() - parts[partIndex].dimensions.height())(m_randomEngine)
    );

    neighbor[partIndex] = newPosition;

    return neighbor;
}

float MaterialOptimizer::calculateAverageFitness(const QList<float>& fitness)
{
    if (fitness.isEmpty()) {
        return 0.0f;
    }

    float sum = std::accumulate(fitness.begin(), fitness.end(), 0.0f);
    return sum / fitness.size();
}

MaterialOptimizer::OptimizationResult MaterialOptimizer::generateResultFromIndividual(
    const QList<QPointF>& individual,
    const QList<Part>& parts,
    const QList<Sheet>& sheets)
{
    OptimizationResult result;

    if (sheets.isEmpty()) {
        return result;
    }

    // Use first sheet for now (could be extended for multi-sheet)
    result.usedSheets.append(sheets[0]);
    result.partPositions.append(QList<QPointF>());
    result.partRotations.append(QList<float>());

    QList<QPointF>& positions = result.partPositions[0];
    QList<float>& rotations = result.partRotations[0];

    for (int i = 0; i < individual.size() && i < parts.size(); ++i) {
        positions.append(individual[i]);
        rotations.append(0.0f); // No rotation for now
    }

    result.totalSheetsUsed = 1;
    result.totalCost = sheets[0].cost;

    // Calculate efficiency
    float sheetArea = calculateSheetArea(sheets[0]);
    float totalPartArea = 0.0f;

    for (const Part& part : parts) {
        totalPartArea += calculatePartArea(part);
    }

    result.totalEfficiency = (totalPartArea / sheetArea) * 100.0f;

    return result;
}

float MaterialOptimizer::calculatePartArea(const Part& part)
{
    return part.dimensions.width() * part.dimensions.height();
}

float MaterialOptimizer::calculateSheetArea(const Sheet& sheet)
{
    return sheet.dimensions.width() * sheet.dimensions.height();
}

bool MaterialOptimizer::partsOverlap(const QPointF& pos1, const QSizeF& size1, float rot1,
                                   const QPointF& pos2, const QSizeF& size2, float rot2)
{
    // Simplified overlap detection (axis-aligned for now)
    // In a full implementation, this would handle rotation properly

    QRectF rect1(pos1, size1);
    QRectF rect2(pos2, size2);

    return rect1.intersects(rect2);
}

void MaterialOptimizer::updateOptimizationMetrics(const OptimizationResult& result)
{
    m_optimizationMetrics.operationTimeMs = result.optimizationTimeMs;
    m_optimizationMetrics.memoryUsageBytes = 0; // Would be tracked during optimization
}