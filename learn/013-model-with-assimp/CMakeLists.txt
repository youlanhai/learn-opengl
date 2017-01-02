
set(TARGET_NAME "013-model-with-assimp")

add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} ${COMMON_LINK_LIBRARIES} assimp-vc140-mtd)
