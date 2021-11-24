# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("ToasterGameEngine")

# making sure I'm using C++ 20
set(CMAKE_CXX_STANDARD 20)

# setting a proper deploy dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/deploy")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/deploy")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/deploy")

# Include sub-projects.
add_subdirectory ("ToasterGameEngineCore")
add_subdirectory("toasterTest")