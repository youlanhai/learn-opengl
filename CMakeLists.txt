cmake_minimum_required(VERSION 2.8.12)
project(LearnOpenGL)

include_directories(dependency/include)
link_directories(dependency/lib)

if (WIN32)
	message(STATUS "win32 platform")
elseif (APPLE)
	message(STATUS "apple platform")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AudioUnit -framework Carbon -framework Cocoa -framework CoreAudio -framework CoreVideo -framework IOKit -framework OpenGL -framework CoreFoundation")
else ()
	message(STATUS "unknown platform")
endif ()

add_subdirectory(learn)
