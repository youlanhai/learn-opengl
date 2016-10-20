cmake_minimum_required(VERSION 2.8)

PROJECT(SmartJson)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

add_subdirectory(src)

if((NOT IOS) AND (NOT ANDROID))
	add_subdirectory(test)
endif()
