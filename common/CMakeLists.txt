
set(TARGET_NAME common)

file(GLOB HEADERS *.hpp *.h)
file(GLOB SOURCES *.cpp *.c)

add_library(${TARGET_NAME} ${HEADERS} ${SOURCES})
