cmake_minimum_required(VERSION 3.16.0)

project(pepsi)

set(SRC_FILE
    src/init.c
    src/main.c
    src/instructions.c
)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -g")

include_directories(include)

find_package(SDL2 REQUIRED)

add_executable(pepsi ${SRC_FILE})

target_link_libraries(pepsi PRIVATE SDL2::SDL2)

