cmake_minimum_required(VERSION 3.26)

project(JesusVM)

add_subdirectory(lib/dyncall)
add_subdirectory(moduleweb)
add_subdirectory(framework)
add_subdirectory(native)
add_subdirectory(native-test)
add_subdirectory(main)