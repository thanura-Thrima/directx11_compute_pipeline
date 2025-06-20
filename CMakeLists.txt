cmake_minimum_required(VERSION 3.10)
# Set the project name
project (ComputePipeline)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCL REQUIRED)


add_subdirectory(tests)

set(SOURCES
    src/main.cpp
    src/Application.cpp
    src/geometry.cpp
    src/Utility/Timer.cpp
    src/Graphics/GraphicsManager.cpp
    src/Graphics/IShader.cpp
    src/Graphics/ComputeShader.cpp
    src/Graphics/VertexShader.cpp
    src/Graphics/PixelShader.cpp
    src/Hw/Camera.cpp
)

# Add OpenCL include directories
include_directories(${OpenCL_INCLUDE_DIRS})

# Add an executable
add_executable(ComputePipeline ${SOURCES})

target_include_directories(ComputePipeline
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(ComputePipeline PROPERTIES WIN32_EXECUTABLE TRUE)
# Set the path to the DirectX SDK
#set(DIRECTX_SDK_PATH "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")

# Include the DirectX SDK directories
#include_directories(${DIRECTX_SDK_PATH}/Include)
#link_directories(${DIRECTX_SDK_PATH}/Lib/x86) # or /Lib/x64 for 64-bit
include_directories("ext/")
target_link_libraries(ComputePipeline PRIVATE
    d3d11
    d3dcompiler
    mf
    mfplat
    #mfobjects
    mfuuid
  ${OpenCL_LIBRARIES}
)

add_custom_command(TARGET ComputePipeline POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/data/ ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/data/)

                   add_custom_command(TARGET ComputePipeline POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/shaders/ ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/shaders/)