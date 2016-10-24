
set(COMMON_LINK_LIBRARIES glfw3 glad stb smartjson common)

if (WIN32)
	set(COMMON_LINK_LIBRARIES ${COMMON_LINK_LIBRARIES} winmm imm32 version opengl32 shlwapi Ws2_32)
elseif (APPLE)
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework CoreVideo -framework IOKit -framework OpenGL -framework CoreFoundation")
endif ()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
listsubdir(SUB_DIRS ${CMAKE_CURRENT_LIST_DIR})

foreach(subdir ${SUB_DIRS})
	message(STATUS "add subdirectory: " ${subdir})
	add_subdirectory(${subdir})
endforeach()
