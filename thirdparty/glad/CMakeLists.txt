cmake_minimum_required(VERSION 2.8)

set (TARGET_NAME glad)

PROJECT(${TARGET_NAME})

include_directories(include)

# Setup target
add_library(${TARGET_NAME} src/glad.c)

install(DIRECTORY include/glad include/KHR DESTINATION include)
install(TARGETS ${TARGET_NAME} DESTINATION lib)
