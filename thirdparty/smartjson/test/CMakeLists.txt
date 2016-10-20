
include_directories(${PROJECT_SOURCE_DIR}/src)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

##################################################
set(TARGET stest)
set(SOURCE_FILES test.cpp)

add_executable(${TARGET} ${SOURCE_FILES})
target_link_libraries(${TARGET} smartjson)

##################################################
set(TARGET smain)
set(SOURCE_FILES main.cpp)

add_executable(${TARGET} ${SOURCE_FILES})
target_link_libraries(${TARGET} smartjson)
