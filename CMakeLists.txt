cmake_minimum_required(VERSION 3.19)
project(SynthwaveDrive VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

add_library(synthwave_core src/Camera.cpp)
target_include_directories(synthwave_core PUBLIC src)
add_executable(synthwave_drive
    src/main.cpp src/Shader.cpp src/Mesh.cpp src/Hud.cpp src/Road.cpp src/ReferenceMarker.cpp)
target_link_libraries(synthwave_drive PRIVATE synthwave_core OpenGL::GL glfw GLEW::GLEW)
target_compile_definitions(synthwave_drive PRIVATE GLFW_INCLUDE_NONE)
if(APPLE)
    target_compile_definitions(synthwave_drive PRIVATE GL_SILENCE_DEPRECATION)
endif()
if(MSVC)
    target_compile_options(synthwave_drive PRIVATE /W4)
else()
    target_compile_options(synthwave_drive PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(synthwave_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
# Run on each build so edits to shader files reach the executable's directory.
add_custom_target(synthwave_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
            "$<TARGET_FILE_DIR:synthwave_drive>/shaders"
    COMMENT "Copying runtime shaders" VERBATIM)
add_dependencies(synthwave_drive synthwave_assets)

include(CTest)
if(BUILD_TESTING)
    add_executable(core_tests tests/core_tests.cpp)
    target_link_libraries(core_tests PRIVATE synthwave_core)
    add_test(NAME framework_core COMMAND core_tests)
endif()
install(TARGETS synthwave_drive RUNTIME DESTINATION bin)
install(DIRECTORY shaders DESTINATION bin)
