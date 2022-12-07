cmake_minimum_required(VERSION 3.9)

set(PROJECT_ROOT ./)
set(PROJECT_NAME "F21GA")

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

# find_package(OpenGL REQUIRED)
# include_directories(${OPENGL_INCLUDE_DIRS})

# find_package(glfw3 REQUIRED)
# include_directories(${GLFW_INCLUDE_DIRS})
# link_libraries(${GLFW_LIBRARY_DIRS})

set(SRC_FILES
    ./src/glad.c
    ./src/main.cpp
    ./src/Context.cpp
    ./src/Model.cpp

    ./include/tinygltf/tiny_gltf.cc
)

# add_subdirectory(imgui)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
add_library(imgui STATIC)
target_sources(imgui
    PRIVATE
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        # ${IMGUI_DIR}/imconfig.h
        # ${IMGUI_DIR}/imgui.h
        # ${IMGUI_DIR}/imgui_internal.h
        # ${IMGUI_DIR}/backends/imgui_impl_glfw.h
)
target_include_directories(imgui
    PUBLIC
        ${IMGUI_DIR}
        # ${IMGUI_DIR}/backends
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_compile_options(${PROJECT_NAME} PRIVATE -W) # -Wall -Wextra) # -g3
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ./include/
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)
target_link_libraries(${PROJECT_NAME} PRIVATE -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "../"
)

