cmake_minimum_required(VERSION 4.0.0)
project(bitwise_calculator VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git  
  GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(SOURCE_FILE
  sources/parser/parser.cpp
  sources/token/token.cpp
  sources/token_stream/token_stream.cpp
)

add_executable(bitwise_calculator sources/main.cpp ${SOURCE_FILE})

add_executable(tests tests/tests.cpp ${SOURCE_FILE})

target_link_libraries(tests gtest gmock)

add_test(NAME bitwise_calculator_tests COMMAND tests)