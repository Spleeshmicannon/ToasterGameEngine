
project ("toasterTest")

set_target_properties(ToasterGameEngineCore PROPERTIES IMPORTED "${sourceDir}/")

# turning off exceptions
set(CMAKE_CXX_EXTENSIONS OFF)

# finding my dll
include_directories("${sourceDir}/")
link_directories("${sourceDir}/out/install/${presetName}/")

# sorting the exe
add_executable(toasterTest "toasterTest.cpp" )

# managing my dll
target_include_directories(ToasterGameEngineCore PUBLIC "${sourceDir}/")
target_link_libraries(toasterTest PUBLIC ToasterGameEngineCore)