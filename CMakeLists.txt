cmake_minimum_required(VERSION 4.3)
project(OpenGL_Tutorial)

set(CMAKE_CXX_STANDARD 23)

set(DEPENDENCIES_ROOT ${CMAKE_SOURCE_DIR}/dependencies) # --- dependencies root ---
set(GLM_ROOT ${DEPENDENCIES_ROOT}/glm) # --- GLM ---
set(GLAD_ROOT ${DEPENDENCIES_ROOT}/glad) # --- GLAD ---
set(GLFW_ROOT ${DEPENDENCIES_ROOT}/glfw-3.5.1.bin.WIN64) # --- GLFW (prebuilt) ---

add_library(glfw STATIC IMPORTED
        shaders/headers/Shader.h
        src/objects/cpp/light.cpp
        src/objects/headers/light.h
        src/objects/cpp/chunk.cpp
        src/objects/headers/chunk.h)

set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION ${GLFW_ROOT}/lib-mingw-w64/libglfw3.a
        INTERFACE_INCLUDE_DIRECTORIES ${GLFW_ROOT}/include
)

add_executable(OpenGL_Tutorial
        src/main.cpp
        src/objects/headers/voxel.h
        src/objects/headers/object.h
        src/objects/headers/camera.h
        src/objects/cpp/voxel.cpp
        src/objects/cpp/object.cpp
        src/objects/cpp/camera.cpp
        dependencies/texture-loader/stb_image.cpp
        ${GLAD_ROOT}/src/glad.c
        src/objects/cpp/light.cpp
        src/objects/headers/light.h
        src/objects/cpp/chunk.cpp
        src/objects/headers/chunk.h
)

# project root, so shader/texture paths work from any build folder
target_compile_definitions(OpenGL_Tutorial PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

target_include_directories(OpenGL_Tutorial PRIVATE
        ${GLAD_ROOT}/include
        ${GLM_ROOT}
)

target_link_libraries(OpenGL_Tutorial PRIVATE
        glfw
        opengl32
        gdi32
)