
project ("toasterTest")

set_target_properties(ToasterGameEngineCore PROPERTIES IMPORTED "${sourceDir}/")

# turning off exceptions
set(CMAKE_CXX_EXTENSIONS OFF)

# this is probably bad but I'm lazy
if(WIN32)
	file(
		INSTALL "${CMAKE_INSTALL_PREFIX}/ToasterGameEngineCore/" 
		DESTINATION "${CMAKE_INSTALL_PREFIX}/toasterTest/"
	)
endif()

# finding my dll
include_directories("${sourceDir}/")
link_directories("${sourceDir}/out/install/${presetName}/")

# sorting the exe
add_executable(toasterTest "toasterTest.cpp" )

# managing my dll
target_include_directories(ToasterGameEngineCore PUBLIC "${sourceDir}/")
target_link_libraries(toasterTest PRIVATE ToasterGameEngineCore)