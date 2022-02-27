project(GWEN)
cmake_minimum_required(VERSION 3.10)

file(GLOB GWEN_sources src/**.cpp)
file(GLOB GWEN_control_sources src/Controls/**.cpp)
file(GLOB GWEN_unit_test_sources UnitTest/**.cpp)
add_library(GWEN_static ${GWEN_sources} ${GWEN_control_sources} ${GWEN_unit_test_sources} src/Platforms/X11.cpp)
target_include_directories(GWEN_static PUBLIC include)

add_library(GWEN_opengl Renderers/OpenGL/OpenGL.cpp Renderers/OpenGL/DebugFont/OpenGL_DebugFont.cpp)
target_include_directories(GWEN_opengl PUBLIC include)
target_link_libraries(GWEN_opengl freeimage)

# Samples
add_executable(GWEN_cross_platform_sample Samples/CrossPlatform/CrossPlatform.cpp)
target_include_directories(GWEN_cross_platform_sample PUBLIC include)
target_link_libraries(GWEN_cross_platform_sample GWEN_opengl GWEN_static X11 GL)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
