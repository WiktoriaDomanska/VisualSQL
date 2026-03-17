cmake_minimum_required(VERSION 3.20)
project(VisualSQL)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)

set(NODE_EDITOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui-node-editor)

set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(NODE_EDITOR_SOURCES
        ${NODE_EDITOR_DIR}/imgui_node_editor.cpp
        ${NODE_EDITOR_DIR}/imgui_node_editor_api.cpp
        ${NODE_EDITOR_DIR}/imgui_canvas.cpp
        ${NODE_EDITOR_DIR}/crude_json.cpp
)

add_executable(VisualSQL main.cpp ${IMGUI_SOURCES} ${NODE_EDITOR_SOURCES}
        Column.cpp
        Column.hpp
        Table.cpp
        Table.hpp
)

target_include_directories(VisualSQL PRIVATE
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${NODE_EDITOR_DIR}
)

target_link_libraries(VisualSQL PRIVATE glfw opengl32)