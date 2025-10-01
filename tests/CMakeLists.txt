# Test configuration for 3D Model Management Utility
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Enable testing
enable_testing()

# Find Qt6 Test module
find_package(Qt6 6.2 REQUIRED COMPONENTS Test)

# Find Google Test (optional, for more advanced testing)
find_package(GTest QUIET)

# Test executable
set(TEST_SOURCES
    test_main.cpp
    core/test_database_manager.cpp
    core/test_model_service.cpp
    core/test_search_service.cpp
    core/test_tag_manager.cpp
    render/test_model_loader.cpp
    render/test_progressive_loader.cpp
    render/test_thumbnail_generator.cpp
    repair/test_mesh_repair.cpp
    nesting/test_material_optimizer.cpp
    tools/test_feed_speed_calculator.cpp
    ui/test_main_window.cpp
    ui/test_project_canvas.cpp
    ui/test_design_canvas.cpp
    ui/test_cut_list_canvas.cpp
    ui/test_cnc_tools_canvas.cpp
    ui/test_settings_canvas.cpp
    utils/test_logger.cpp
    utils/test_command_line_interface.cpp
    integration/test_full_workflow.cpp
    integration/test_performance.cpp
    integration/test_cnc_workflow.cpp
)

if(GTest_FOUND)
    message(STATUS "Google Test found, enabling advanced tests")
    list(APPEND TEST_SOURCES
        advanced/test_genetic_algorithms.cpp
        advanced/test_mesh_algorithms.cpp
        advanced/test_performance_benchmarks.cpp
    )
endif()

# Create test executable
add_executable(3DModelManagementUtilityTests ${TEST_SOURCES})

# Link test libraries
target_link_libraries(3DModelManagementUtilityTests
    Qt6::Test
    3DModelManagementUtilityLib
)

if(GTest_FOUND)
    target_link_libraries(3DModelManagementUtilityTests GTest::gtest GTest::gtest_main)
    target_compile_definitions(3DModelManagementUtilityTests PRIVATE GTEST_ENABLED)
endif()

# Include directories
target_include_directories(3DModelManagementUtilityTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# Test output directory
set_target_properties(3DModelManagementUtilityTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

# Copy test data
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/test_models ${CMAKE_BINARY_DIR}/tests/test_models COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/test_settings.json ${CMAKE_BINARY_DIR}/tests/test_settings.json COPYONLY)

# Register tests
add_test(NAME 3DModelManagementUtilityTests COMMAND 3DModelManagementUtilityTests)

# Performance tests
add_test(NAME PerformanceTests COMMAND 3DModelManagementUtilityTests -performance)
add_test(NAME MemoryTests COMMAND 3DModelManagementUtilityTests -memory)

# Integration tests
add_test(NAME IntegrationTests COMMAND 3DModelManagementUtilityTests -integration)
add_test(NAME CNCWorkflowTests COMMAND 3DModelManagementUtilityTests -cnc)

# Coverage configuration (if available)
if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(3DModelManagementUtilityTests PRIVATE --coverage)
    target_link_libraries(3DModelManagementUtilityTests gcov)
endif()

# Test configuration
set_tests_properties(3DModelManagementUtilityTests PROPERTIES
    TIMEOUT 300
    ENVIRONMENT "QT_LOGGING_RULES=*.debug=false;test.*.debug=true"
)

# Performance test configuration
set_tests_properties(PerformanceTests PROPERTIES
    TIMEOUT 600
    ENVIRONMENT "TEST_PERFORMANCE=1"
)

# Memory test configuration
set_tests_properties(MemoryTests PROPERTIES
    TIMEOUT 900
    ENVIRONMENT "TEST_MEMORY=1"
)

# Integration test configuration
set_tests_properties(IntegrationTests PROPERTIES
    TIMEOUT 1200
    ENVIRONMENT "TEST_INTEGRATION=1"
)

# CNC workflow test configuration
set_tests_properties(CNCWorkflowTests PROPERTIES
    TIMEOUT 1800
    ENVIRONMENT "TEST_CNC=1"
)

message(STATUS "Test configuration completed")
message(STATUS "Run tests with: ctest -C ${CMAKE_BUILD_TYPE}")
message(STATUS "Run specific test: ctest -R TestName -C ${CMAKE_BUILD_TYPE}")