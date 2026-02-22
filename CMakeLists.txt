cmake_minimum_required(VERSION 3.20)
project(Iterations)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(Iterations ${SOURCES})

target_include_directories(Iterations PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(Iterations PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)