cmake_minimum_required(VERSION 3.20)

project(
    ForjeEngine
    VERSION 0.2.0
    DESCRIPTION "A modular C++17 3D game engine foundation"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 CONFIG QUIET)

include(FetchContent)

if(NOT TARGET glfw)
    message(STATUS "GLFW was not found locally; fetching GLFW 3.4")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(glfw)
endif()

find_package(glad CONFIG QUIET)
if(TARGET glad::glad)
    set(FORJE_GLAD_TARGET glad::glad)
else()
    message(STATUS "GLAD was not found locally; fetching GLAD 2.0.8")
    find_package(Python COMPONENTS Interpreter REQUIRED)
    execute_process(
        COMMAND "${Python_EXECUTABLE}" -c "import jinja2"
        RESULT_VARIABLE FORJE_PYTHON_HAS_JINJA2
        OUTPUT_QUIET
        ERROR_QUIET
    )
    if(NOT FORJE_PYTHON_HAS_JINJA2 EQUAL 0 AND APPLE AND EXISTS "/usr/bin/python3")
        execute_process(
            COMMAND "/usr/bin/python3" -c "import jinja2"
            RESULT_VARIABLE FORJE_XCODE_PYTHON_HAS_JINJA2
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(FORJE_XCODE_PYTHON_HAS_JINJA2 EQUAL 0)
            set(Python_EXECUTABLE "/usr/bin/python3" CACHE FILEPATH
                "Python interpreter used to generate GLAD" FORCE)
            set(FORJE_PYTHON_HAS_JINJA2 0)
        endif()
    endif()
    if(NOT FORJE_PYTHON_HAS_JINJA2 EQUAL 0)
        message(FATAL_ERROR
            "GLAD generation requires the Python 'jinja2' package. "
            "Install it for ${Python_EXECUTABLE}, or set Python_EXECUTABLE "
            "to an interpreter that provides it.")
    endif()

    FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v2.0.8
        GIT_SHALLOW TRUE
        SOURCE_SUBDIR cmake
    )
    FetchContent_MakeAvailable(glad)
    glad_add_library(
        forje_glad
        STATIC
        REPRODUCIBLE
        API gl:core=3.3
        EXTENSIONS NONE
    )
    set(FORJE_GLAD_TARGET forje_glad)
endif()

function(forje_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

set(FORJE_RUNTIME_SOURCES
    src/Camera/Camera.cpp
    src/Core/Application.cpp
    src/Graphics/Mesh.cpp
    src/Graphics/Renderer.cpp
    src/Graphics/Shader.cpp
    src/Input/Input.cpp
    src/Physics/PhysicsSystem.cpp
    src/Scene/Scene.cpp
)

set(FORJE_RUNTIME_HEADERS
    src/Camera/Camera.h
    src/Core/Application.h
    src/Graphics/Mesh.h
    src/Graphics/Renderer.h
    src/Graphics/Shader.h
    src/Input/Input.h
    src/Math/Matrix4.h
    src/Math/Vector3.h
    src/Physics/PhysicsSystem.h
    src/Scene/Components.h
    src/Scene/Entity.h
    src/Scene/Scene.h
)

add_library(ForjeEngineRuntime STATIC
    ${FORJE_RUNTIME_SOURCES}
    ${FORJE_RUNTIME_HEADERS}
)

target_include_directories(
    ForjeEngineRuntime
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    ForjeEngineRuntime
    PRIVATE
        OpenGL::GL
        glfw
        ${FORJE_GLAD_TARGET}
)

if(APPLE)
    target_compile_definitions(ForjeEngineRuntime PRIVATE GL_SILENCE_DEPRECATION)
endif()
forje_enable_warnings(ForjeEngineRuntime)

add_executable(ForjeEngine src/main.cpp)
target_link_libraries(ForjeEngine PRIVATE ForjeEngineRuntime)
forje_enable_warnings(ForjeEngine)

add_custom_command(
    TARGET ForjeEngine
    POST_BUILD
    COMMAND
        "${CMAKE_COMMAND}" -E make_directory
        "$<TARGET_FILE_DIR:ForjeEngine>/assets/shaders"
    COMMAND
        "${CMAKE_COMMAND}" -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/cube.vert"
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/cube.frag"
        "$<TARGET_FILE_DIR:ForjeEngine>/assets/shaders"
    COMMENT "Copying ForjeEngine assets"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES
    src/main.cpp
    ${FORJE_RUNTIME_SOURCES}
    ${FORJE_RUNTIME_HEADERS}
)

include(CTest)
if(BUILD_TESTING)
    add_executable(ForjeEngineTests
        tests/PhysicsTests.cpp
        src/Physics/PhysicsSystem.cpp
        src/Scene/Scene.cpp
    )
    target_include_directories(ForjeEngineTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    forje_enable_warnings(ForjeEngineTests)
    add_test(NAME ForjeEngine.Physics COMMAND ForjeEngineTests)
endif()
