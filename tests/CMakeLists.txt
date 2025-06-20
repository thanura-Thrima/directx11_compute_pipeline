include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB TEST_SOURCES
    main.cpp
    StateMachineTests.cpp
    ../src/Hw/Camera.cpp
    CameraTest.cpp)

add_executable(ComputePipelineTests ${TEST_SOURCES})

target_link_libraries(ComputePipelineTests
    PRIVATE
        gtest_main
)

target_include_directories(ComputePipelineTests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

add_test(NAME ComputePipelineTests COMMAND ComputePipelineTests)