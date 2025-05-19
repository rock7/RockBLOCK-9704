cmake_minimum_required(VERSION 3.14)
project(tests)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        6910c9d9165801d8827d628cb72eb7ea9dd538c5 # release-1.16.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(IRIDIUM_IMT_LIB "iridiumImt")

set(MOCK_DIR "mock")
set(MOCK_LIB "mock")
add_subdirectory(${MOCK_DIR} ${MOCK_LIB})

add_executable(
    tests
    test-jspr-parse.cpp
    )

target_link_libraries(
    tests
    ${MOCK_LIB}
    ${IRIDIUM_IMT_LIB}
    GTest::gtest_main
    )

target_include_directories(tests PUBLIC ../src)

include(GoogleTest)
gtest_discover_tests(tests)