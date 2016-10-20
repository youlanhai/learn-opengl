
set(COMMON_LINK_FLAG glfw3)

if (WIN32)
	set(COMMON_LINK_FLAG ${COMMON_LINK_FLAG} winmm imm32 version opengl32 shlwapi Ws2_32)
elseif (APPLE)
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework CoreVideo -framework IOKit -framework OpenGL -framework CoreFoundation")
endif ()

add_subdirectory(001-window)
