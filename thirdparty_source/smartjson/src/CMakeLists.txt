
set(TARGET smartjson)

file (GLOB HEADER_FILES *.hpp *.ipp)
file (GLOB SOURCE_FILES *.cpp)

add_library(${TARGET} ${HEADER_FILES} ${SOURCE_FILES})

install(FILES ${HEADER_FILES} DESTINATION include/smartjson)
install(TARGETS ${TARGET} DESTINATION lib)
