cmake_minimum_required(VERSION 2.8)

PROJECT(AntTweakBar)

# Define target name
set (TARGET_NAME AntTweakBar)
add_definitions(-DTW_STATIC -DTW_NO_LIB_PRAGMA)

set(H_FILES include/AntTweakBar.h)

set(C_FILES
	src/LoadOGL.cpp
	src/LoadOGLCore.cpp
	src/TwBar.cpp
	src/TwColors.cpp
	src/TwEventGLFW.c
	src/TwFonts.cpp
	src/TwMgr.cpp
	src/TwOpenGL.cpp
	src/TwOpenGLCore.cpp
	src/TwPrecomp.cpp
)

if(WIN32)
	set(C_FILES ${C_FILES} 
		src/TWDirect3D9.cpp
		src/TWDirect3D10.cpp
		#src/TWDirect3D11.cpp
	)
endif()

set (SOURCE_FILES ${C_FILES} ${H_FILES})

include_directories(include)

# Setup target
add_library(${TARGET_NAME} ${SOURCE_FILES})

install(FILES ${H_FILES} DESTINATION include/AntTweakBar)
install(TARGETS ${TARGET_NAME} DESTINATION lib)
