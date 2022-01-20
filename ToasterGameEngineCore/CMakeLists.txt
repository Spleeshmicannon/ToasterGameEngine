# CMakeList.txt : CMake project for ToasterGameEngineCore, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# making sure I'm using C++ 20
set(CMAKE_CXX_STANDARD 20)

# turning off exceptions
set(CMAKE_CXX_EXTENSIONS OFF)

# getting vulkan
# find_package(Vulkan REQUIRED)

# getting OpenGL
find_package(OpenGL REQUIRED)
set(GLEW_USE_STATIC_LIBS TRUE)
find_package(GLEW REQUIRED)

# Add source to this project's executable.
add_library (ToasterGameEngineCore SHARED
"application.cpp" "platform/platform.cpp" "logger.cpp" "event.cpp" "input.cpp" 
"render/renderer.cpp" "clock.cpp" "platform/platformState.h"  )

# TODO: Add tests and install targets if needed.
target_include_directories(ToasterGameEngineCore PUBLIC 
	${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR} 
	"${sourceDir}/out/install/${presetName}/toasterTest" )
target_link_libraries(ToasterGameEngineCore PRIVATE OpenGL::GL GLEW::GLEW)


if(UNIX AND NOT APPLE)
	target_link_libraries(ToasterGameEngineCore PRIVATE "/usr/lib/x86_64-linux-gnu/libxcb.so")
	target_link_libraries(ToasterGameEngineCore PRIVATE "/usr/lib/x86_64-linux-gnu/libX11-xcb.so")
	target_link_libraries(ToasterGameEngineCore PRIVATE "/usr/lib/x86_64-linux-gnu/libX11.so")
endif()
