
set(COMMON_LINK_LIBRARIES glfw3 glad stb smartjson common)

add_definitions(-DTW_STATIC -DTW_NO_LIB_PRAGMA)

if (WIN32)
	set(COMMON_LINK_LIBRARIES ${COMMON_LINK_LIBRARIES} winmm imm32 version opengl32 shlwapi Ws2_32 assimp-vc140-mtd)
elseif (APPLE)
	set(COMMON_LINK_LIBRARIES ${COMMON_LINK_LIBRARIES} assimp)
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework CoreVideo -framework IOKit -framework OpenGL -framework CoreFoundation")
endif ()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
listsubdir(SUB_DIRS ${CMAKE_CURRENT_LIST_DIR})

foreach(subdir ${SUB_DIRS})
	message(STATUS "subdirectory: " ${subdir})
	set(CURRENT_DIR_NAME ${subdir})

	add_subdirectory(${subdir})
endforeach()
