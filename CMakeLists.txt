# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
project ("MidiPrinter")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(get_cpm)

# Include sub-projects.
#add_subdirectory ("src")

#CPMAddPackage(
#    NAME midifile
#    GITHUB_REPOSITORY craigsapp/midifile
#    #VERSION 3.11.3
#    #GIT_TAG 3.11.5
#    DOWNLOAD_ONLY TRUE
#)

add_executable(${PROJECT_NAME}
	"src/midi/Binasc.cpp"
	"src/midi/MidiEvent.cpp"
	"src/midi/MidiEventList.cpp"
	"src/midi/MidiFile.cpp"
	"src/midi/MidiMessage.cpp"

	"src/Main.cpp"
	"src/MidiPrinter.cpp"
	"src/Sound.cpp"
	"src/Stepper.cpp"
 "src/Types.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

set_target_properties(${PROJECT_NAME} PROPERTIES
    #VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/run/
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)



#set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Valhalla)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /bigobj "/diagnostics:caret")
endif()
if(MSVC AND CMAKE_BUILD_TYPE MATCHES Release)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})