cmake_minimum_required(VERSION 3.12)

project(cg C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB_RECURSE SRC "src/**.c*")

add_executable(
    ${PROJECT_NAME}
    ${SRC}
)
target_include_directories(${PROJECT_NAME} PRIVATE "include")
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

