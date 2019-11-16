#
#  Copyright (c) 2019 Christof Ruch. All rights reserved.
#
#  Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
#

cmake_minimum_required(VERSION 3.14)

project(juce-spectroscope19-ci)

# We need a little bit support for the JUCE library, and also want to put our own CMake helpers on the module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/third_party/juce-cmake/cmake" "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Include useful scripts for CMake
include(FetchContent)
IF (WIN32)
	FetchContent_Declare(
	  asiosdk
	  URL https://www.steinberg.net/asiosdk
	)
	ADD_DEFINITIONS(-DJUCE_ASIO) # Need to set this to use it
	FetchContent_MakeAvailable(asiosdk)
		FetchContent_Declare(
		glew
		URL https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0-win32.zip
	)
	FetchContent_MakeAvailable(glew)
	set(GLEW_DIRECTORY "${glew_SOURCE_DIR}/include/")
	set(GLEW_LIB_DIRECTORY "${glew_SOURCE_DIR}/lib/Release/x64")
	set(GLEW_DLL_DIRECTORY "${glew_SOURCE_DIR}/bin/Release/x64")
ENDIF()

add_subdirectory(Module)

find_package(JUCE REQUIRED 
	COMPONENTS 
		juce_core		
		juce_events
		juce_audio_basics
		juce_audio_devices
		juce_data_structures
		juce_audio_utils # AudioAppComponent
		juce_dsp
		juce_opengl
)

set(SOURCES
	Source/ExampleWidget.h
	Source/ExampleWidget.cpp
	Source/MainComponent.h
	Source/MainComponent.cpp
	Source/Main.cpp
)

add_executable(JuceSpectroscope19 WIN32 ${SOURCES})
target_include_directories(JuceSpectroscope19 INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_directories(JuceSpectroscope19 PRIVATE ${GLEW_LIB_DIRECTORY})
target_link_libraries(JuceSpectroscope19 PRIVATE ${JUCE_LIBRARIES} glew32 juce-spectroscope19)

IF(WIN32)
	# Post build commands - copy the DLLs (Windows only I guess)
	add_custom_command(
			TARGET JuceSpectroscope19 POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy
					"${GLEW_DLL_DIRECTORY}/glew32.dll"
					${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
ENDIF()