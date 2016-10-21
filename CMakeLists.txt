cmake_minimum_required(VERSION 2.8.12)
project(LearnOpenGL)

if (WIN32)
	message(STATUS "win32 platform")
elseif (APPLE)
	message(STATUS "apple platform")
else ()
	message(STATUS "unknown platform")
endif ()

include(common.cmake)

use_cxx11()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_DEBUG")
include_directories(common dependency/include)
link_directories(dependency/lib)

add_subdirectory(common)
add_subdirectory(learn)
