set(SRC_DIR "../../src")
add_library(mock serial_test.c)
target_include_directories (mock PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SRC_DIR})