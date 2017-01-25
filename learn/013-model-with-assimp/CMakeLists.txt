
set(TARGET_NAME "013-model-with-assimp")

add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} ${COMMON_LINK_LIBRARIES} AntTweakBar)

if(WIN32)
	target_link_libraries(${TARGET_NAME} assimp-vc140-mtd)
elseif(APPLE)
	target_link_libraries(${TARGET_NAME} assimp)
endif()
