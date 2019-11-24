#
#  Copyright (c) 2019 Christof Ruch. All rights reserved.
#
#  Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
#

cmake_minimum_required(VERSION 3.14)

project(juce-spectroscope19-ci)

# We need a little bit support for the JUCE library, and also want to put our own CMake helpers on the module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/third_party/juce-cmake/cmake" "${CMAKE_CURRENT_LIST_DIR}/cmake")

IF (WIN32)
	# Include useful scripts for CMake
    include(FetchContent REQUIRED)
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

ELSEIF(UNIX)

	# Include useful scripts for CMake
	find_package(PkgConfig REQUIRED) 
	find_package(OpenGL)

	# These calls create special `PkgConfig::<MODULE>` variables
	pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk+-3.0)
	pkg_check_modules(WEBKIT REQUIRED IMPORTED_TARGET webkit2gtk-4.0)
	pkg_check_modules(GLEW REQUIRED IMPORTED_TARGET glew)
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
IF(WIN32)
	target_link_directories(JuceSpectroscope19 PRIVATE ${GLEW_LIB_DIRECTORY})
	target_link_libraries(JuceSpectroscope19 PRIVATE ${JUCE_LIBRARIES} glew32 juce-spectroscope19)
ELSEIF(UNIX)
	target_link_libraries(JuceSpectroscope19 PRIVATE 
		${JUCE_LIBRARIES} 
		PkgConfig::WEBKIT 
		PkgConfig::GTK 
		PkgConfig::GLEW 
		${OPENGL_gl_LIBRARY} 
		Xext 
		X11 
		pthread 
		${CMAKE_DL_LIBS} 
		freetype 
		curl 
		asound juce-spectroscope19)
	target_compile_options(JuceSpectroscope19 PRIVATE -pthread -I/usr/include/webkitgtk-4.0 -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/libsoup-2.4 -I/usr/include/libxml2 -I/usr/include/webkitgtk-4.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -g -DDEBUG -DJUCE_DEBUG)
ENDIF()

IF(WIN32)
	# Post build commands - copy the DLLs (Windows only I guess)
	add_custom_command(
			TARGET JuceSpectroscope19 POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy
					"${GLEW_DLL_DIRECTORY}/glew32.dll"
					${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
ENDIF()

# Use all cores
IF (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()
