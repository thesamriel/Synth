CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(Synth)

SET(CMAKE_CXX_STANDARD 17)

#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)


include_directories(include)
include_directories(src)

FIND_PACKAGE(OpenGL REQUIRED)

set(SOURCE_FILES src/synth.cpp src/glad.c src/rectangle.cpp)
add_executable(synth ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(synth glfw dl asound pthread rtaudio X11)