
set(TARGET_NAME ${CURRENT_DIR_NAME})

add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} ${COMMON_LINK_LIBRARIES})
