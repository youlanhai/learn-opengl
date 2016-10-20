
set(TARGET 001-window)

set(SOURCE_FILES main.cpp)

add_executable(${TARGET} ${SOURCE_FILES})
target_link_libraries(${TARGET} ${COMMON_LINK_FLAG})
