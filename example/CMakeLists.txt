cmake_minimum_required(VERSION 3.17)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(statemachine_example)

add_library(statemachine STATIC ../statemachine.c)
target_include_directories(statemachine PUBLIC .. .)

add_executable(main main.c)
target_link_libraries(main PRIVATE statemachine)
