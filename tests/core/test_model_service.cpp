#include <QtTest>
#include "../../src/core/ModelService.h"
#include "../../src/core/DatabaseManager.h"
#include "../../src/core/FileSystemManager.h"
#include "../../src/core/CacheManager.h"
#include "../test_main.h"

class TestModelService : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testModelLoading();
    void testModelMetadata();
    void testModelSearch();
    void testBatchOperations();
    void testModelValidation();
    void testPerformanceMetrics();

private:
    ModelService* m_modelService;
    DatabaseManager* m_databaseManager;
    FileSystemManager* m_fileSystemManager;
    CacheManager* m_cacheManager;
};

void TestModelService::initTestCase()
{
    // Initialize test services
    m_databaseManager = new DatabaseManager(this);
    m_fileSystemManager = new FileSystemManager(this);
    m_cacheManager = new CacheManager(this);
    m_modelService = new ModelService(this);

    // Initialize database
    QString testDbPath = TestUtils::createTestFile("", "db");
    QVERIFY(m_databaseManager->initialize(testDbPath));

    // Initialize file system
    QVERIFY(m_fileSystemManager->initializeDirectories());
}

void TestModelService::cleanupTestCase()
{
    // Cleanup test data
    TestUtils::cleanupTestData();
}

void TestModelService::testModelLoading()
{
    // Test basic model loading
    ModelMetadata model = TestUtils::createTestModel("test_model.stl");

    // Test model validation
    QVERIFY(model.id.isNull() == false);
    QVERIFY(model.filename == "test_model.stl");
    QVERIFY(model.fileSize > 0);

    // Test model storage
    QVERIFY(m_databaseManager->insertModel(model));

    // Test model retrieval
    ModelMetadata retrieved = m_databaseManager->getModel(model.id);
    QVERIFY(retrieved.id == model.id);
    QVERIFY(retrieved.filename == model.filename);
}

void TestModelService::testModelMetadata()
{
    // Test metadata operations
    ModelMetadata model = TestUtils::createTestModel();

    // Test tag operations
    model.tags << "test" << "stl" << "model";
    QVERIFY(m_databaseManager->updateModel(model));

    ModelMetadata updated = m_databaseManager->getModel(model.id);
    QVERIFY(updated.tags == model.tags);

    // Test custom fields
    model.customFields["material"] = "steel";
    model.customFields["units"] = "mm";
    QVERIFY(m_databaseManager->updateModel(model));

    ModelMetadata withCustom = m_databaseManager->getModel(model.id);
    QVERIFY(withCustom.customFields["material"] == "steel");
}

void TestModelService::testModelSearch()
{
    // Create test models
    QList<ModelMetadata> testModels;

    for (int i = 0; i < 5; ++i) {
        ModelMetadata model = TestUtils::createTestModel(QString("test_model_%1.stl").arg(i));
        model.tags << QString("tag_%1").arg(i) << "test";
        testModels.append(model);
        m_databaseManager->insertModel(model);
    }

    // Test search functionality
    QList<ModelMetadata> results = m_databaseManager->searchModels("test");
    QVERIFY(results.size() >= 5);

    // Test tag filtering
    QList<ModelMetadata> tagResults = m_databaseManager->searchModels("", QStringList() << "tag_1");
    QVERIFY(tagResults.size() >= 1);
}

void TestModelService::testBatchOperations()
{
    // Create test models
    QList<QUuid> modelIds;
    for (int i = 0; i < 3; ++i) {
        ModelMetadata model = TestUtils::createTestModel();
        m_databaseManager->insertModel(model);
        modelIds.append(model.id);
    }

    // Test batch tagging
    QStringList tags = {"batch", "test", "operation"};
    QVERIFY(m_modelService->tagModels(modelIds, tags));

    // Verify tags were applied
    for (const QUuid& id : modelIds) {
        ModelMetadata model = m_databaseManager->getModel(id);
        QVERIFY(model.tags.contains("batch"));
    }

    // Test batch deletion
    QVERIFY(m_modelService->deleteModels(modelIds));

    // Verify models were deleted
    for (const QUuid& id : modelIds) {
        ModelMetadata model = m_databaseManager->getModel(id);
        QVERIFY(model.id.isNull());
    }
}

void TestModelService::testModelValidation()
{
    // Test file validation
    QString validFile = TestUtils::createTestFile("solid test", "stl");
    QVERIFY(m_modelService->validateModelFile(validFile));

    QString invalidFile = TestUtils::createTestFile("invalid content", "stl");
    QVERIFY(m_modelService->validateModelFile(invalidFile)); // Basic validation

    // Test format detection
    QVERIFY(m_modelService->detectModelFormat(validFile) == "stl");
}

void TestModelService::testPerformanceMetrics()
{
    // Test performance monitoring
    QElapsedTimer timer;
    timer.start();

    // Perform some operations
    for (int i = 0; i < 100; ++i) {
        ModelMetadata model = TestUtils::createTestModel();
        m_databaseManager->insertModel(model);
    }

    qint64 elapsed = timer.elapsed();

    // Performance should be reasonable (< 1 second for 100 operations)
    QVERIFY(elapsed < 1000);

    PerformanceMetrics metrics = m_modelService->getServiceMetrics();
    QVERIFY(metrics.operationType == "ModelService");
}

// Test runner
QTEST_MAIN(TestModelService)
#include "test_model_service.moc"