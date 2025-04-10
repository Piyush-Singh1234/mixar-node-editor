cmake_minimum_required(VERSION 3.10)
project(NodeEditor)

set(CMAKE_CXX_STANDARD 17)

# ---- Source Files ----
file(GLOB SOURCES src/*.cpp)

# ---- Include Directories ----
include_directories(
    imgui
    imnodes
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

# ---- OpenCV ----
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# ---- OpenGL + GLFW ----
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# ---- GLEW ----
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# ---- ImGui and ImNodes Sources ----
file(GLOB IMGUI_SOURCES
    imgui/*.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

file(GLOB IMNODES_SOURCES
    imnodes/*.cpp
)

# ---- Build Executable ----
add_executable(NodeEditor
    ${SOURCES}
    ${IMGUI_SOURCES}
    ${IMNODES_SOURCES}
)
target_include_directories(NodeEditor PRIVATE include)

# ---- Link Everything ----
target_link_libraries(NodeEditor
    ${OpenCV_LIBS}
    glfw
    GLEW::GLEW
    OpenGL::GL
    dl
)
