#pragma once

#include "../core/BaseTypes.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QPointF>
#include <QSizeF>
#include <QFuture>
#include <QTimer>
#include <random>

/**
 * @brief Advanced material optimization using genetic algorithms
 *
 * Implements sophisticated optimization algorithms for material nesting including:
 * - Genetic algorithm optimization for irregular shapes
 * - Multi-sheet optimization with sheet size selection
 * - Cost-based optimization considering material prices
 * - Constraint satisfaction for grain direction and edge requirements
 * - Parallel processing for large-scale optimization problems
 */
class MaterialOptimizer : public QObject
{
    Q_OBJECT

public:
    explicit MaterialOptimizer(QObject* parent = nullptr);
    virtual ~MaterialOptimizer() = default;

    // Optimization problem definition
    struct Part {
        QString id;
        QString name;
        QSizeF dimensions;
        int quantity;
        bool canRotate;
        QString material;
        QVariantMap constraints; // Grain direction, edge requirements, etc.

        Part() : quantity(1), canRotate(true) {}
        Part(const QString& id, const QSizeF& size, int qty = 1)
            : id(id), dimensions(size), quantity(qty), canRotate(true) {}
    };

    struct Sheet {
        QString id;
        QString name;
        QSizeF dimensions;
        float cost; // Cost per sheet
        QString material;
        QVariantMap properties;

        Sheet() : cost(0.0f) {}
        Sheet(const QString& id, const QSizeF& size, float cost = 0.0f)
            : id(id), dimensions(size), cost(cost) {}
    };

    struct OptimizationResult {
        QList<Sheet> usedSheets;
        QList<QList<QPointF>> partPositions; // Positions for each sheet
        QList<QList<float>> partRotations;   // Rotations for each sheet
        float totalEfficiency;
        float totalCost;
        int totalSheetsUsed;
        qint64 optimizationTimeMs;

        OptimizationResult() : totalEfficiency(0.0f), totalCost(0.0f), totalSheetsUsed(0), optimizationTimeMs(0) {}
    };

    // Algorithm configuration
    struct OptimizationConfig {
        QString algorithm; // "genetic", "simulated_annealing", "particle_swarm"
        int populationSize;
        int maxGenerations;
        float mutationRate;
        float crossoverRate;
        float elitismRate;

        // Constraints
        float minPartDistance; // Minimum distance between parts
        bool respectGrainDirection;
        bool allowRotation;

        // Performance
        bool useParallelProcessing;
        int maxOptimizationTimeMs;

        OptimizationConfig()
            : algorithm("genetic"), populationSize(100), maxGenerations(200),
              mutationRate(0.1f), crossoverRate(0.8f), elitismRate(0.1f),
              minPartDistance(0.125f), respectGrainDirection(true), allowRotation(true),
              useParallelProcessing(true), maxOptimizationTimeMs(30000) {}
    };

    // Main optimization functions
    virtual QFuture<OptimizationResult> optimizeNestingAsync(const QList<Part>& parts,
                                                           const QList<Sheet>& availableSheets,
                                                           const OptimizationConfig& config = OptimizationConfig()) = 0;

    virtual OptimizationResult optimizeNesting(const QList<Part>& parts,
                                             const QList<Sheet>& availableSheets,
                                             const OptimizationConfig& config = OptimizationConfig()) = 0;

    // Advanced algorithms
    virtual OptimizationResult geneticAlgorithmOptimization(const QList<Part>& parts,
                                                          const QList<Sheet>& sheets,
                                                          const OptimizationConfig& config) = 0;

    virtual OptimizationResult simulatedAnnealingOptimization(const QList<Part>& parts,
                                                            const QList<Sheet>& sheets,
                                                            const OptimizationConfig& config) = 0;

    virtual OptimizationResult particleSwarmOptimization(const QList<Part>& parts,
                                                        const QList<Sheet>& sheets,
                                                        const OptimizationConfig& config) = 0;

    // Multi-objective optimization
    virtual OptimizationResult multiObjectiveOptimization(const QList<Part>& parts,
                                                         const QList<Sheet>& sheets,
                                                         const QStringList& objectives) = 0; // "efficiency", "cost", "sheets"

    // Constraint handling
    virtual bool validatePlacement(const Part& part, const QPointF& position, float rotation,
                                 const QSizeF& sheetSize, const QList<QPointF>& existingPositions) = 0;

    virtual QList<QPointF> findValidPlacements(const Part& part, const QSizeF& sheetSize,
                                             const QList<QPointF>& existingPositions) = 0;

    // Sheet selection optimization
    virtual QList<Sheet> selectOptimalSheetSizes(const QList<Part>& parts,
                                               const QList<Sheet>& availableSheets) = 0;

    // Performance monitoring
    virtual PerformanceMetrics getOptimizationMetrics() const = 0;
    virtual void resetPerformanceMetrics() = 0;

signals:
    // Optimization progress
    void optimizationProgress(int percentage, int generation, float bestFitness);
    void optimizationStarted(const QString& algorithm);
    void optimizationCompleted(const OptimizationResult& result);

    // Algorithm events
    void generationCompleted(int generation, float bestFitness, float averageFitness);
    void populationImproved(float newBestFitness);

    // Performance events
    void performanceMetricsUpdated(const PerformanceMetrics& metrics);

protected:
    // Genetic algorithm implementation
    virtual QList<float> evaluateFitness(const QList<QList<QPointF>>& population,
                                       const QList<Part>& parts,
                                       const QList<Sheet>& sheets) = 0;

    virtual QList<QList<QPointF>> selection(const QList<QList<QPointF>>& population,
                                          const QList<float>& fitness) = 0;

    virtual QList<QList<QPointF>> crossover(const QList<QList<QPointF>>& parents) = 0;
    virtual void mutation(QList<QPointF>& individual, float mutationRate) = 0;

    // Placement algorithms
    virtual QPointF findBestPlacement(const Part& part, const QSizeF& sheetSize,
                                    const QList<QPointF>& existingPositions) = 0;

    virtual bool checkCollisions(const QPointF& newPos, const QSizeF& newSize, float newRotation,
                               const QList<QPointF>& existingPositions,
                               const QList<QSizeF>& existingSizes,
                               const QList<float>& existingRotations) = 0;

    // Fitness functions
    virtual float calculateEfficiencyFitness(const QList<QPointF>& positions,
                                           const QList<Part>& parts,
                                           const QList<Sheet>& sheets) = 0;

    virtual float calculateCostFitness(const QList<QPointF>& positions,
                                     const QList<Part>& parts,
                                     const QList<Sheet>& sheets) = 0;

    virtual float calculateCompactnessFitness(const QList<QPointF>& positions,
                                            const QList<Part>& parts) = 0;

    // Utility functions
    virtual float calculatePartArea(const Part& part) = 0;
    virtual float calculateSheetArea(const Sheet& sheet) = 0;
    virtual bool partsOverlap(const QPointF& pos1, const QSizeF& size1, float rot1,
                            const QPointF& pos2, const QSizeF& size2, float rot2) = 0;

    // Random number generation
    std::mt19937 m_randomEngine;
    std::uniform_real_distribution<float> m_uniformDistribution;

    // Configuration
    OptimizationConfig m_config;

    // Performance tracking
    PerformanceMetrics m_optimizationMetrics;
    qint64 m_totalOptimizationTime;
    int m_optimizationRuns;
};