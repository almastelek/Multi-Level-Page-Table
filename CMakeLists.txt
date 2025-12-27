cmake_minimum_required(VERSION 3.16)
project(mlpt_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(mlpt STATIC
    src/mlpt.cpp
)

target_include_directories(mlpt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(mlpt PRIVATE
    -Wall -Wextra -Werror
)

add_executable(mlpt_demo examples/demo.cpp)
target_link_libraries(mlpt_demo PRIVATE mlpt)
