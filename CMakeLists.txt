cmake_minimum_required(VERSION 2.8.12)
project(LearnOpenGL)

include(common.cmake)

include_directories(dependency/include)
link_directories(dependency/lib)

if (WIN32)
	message(STATUS "win32 platform")
elseif (APPLE)
	message(STATUS "apple platform")
else ()
	message(STATUS "unknown platform")
endif ()

add_subdirectory(learn)
