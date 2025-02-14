cmake_minimum_required(VERSION 3.26)

set(SOURCES src/Main.cpp)
set(HEADERS include/Main.h)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_library(jesusvm-native-test SHARED ${SOURCES} ${HEADERS})

target_include_directories(jesusvm-native-test
    PUBLIC
        include
)

if(WIN32)
    target_compile_definitions(jesusvm-native-test PRIVATE PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(jesusvm-native-test PRIVATE PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(jesusvm-native-test PRIVATE PLATFORM_LINUX)
endif()

set_target_properties(jesusvm-native-test PROPERTIES
    OUTPUT_NAME "Main"
    PREFIX ""
)

set_target_properties(jesusvm-native-test PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/run
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/run
)

target_compile_features(jesusvm-native-jesusvm-native PUBLIC cxx_std_20)
target_link_libraries(jesusvm-native-test PRIVATE JesusVM::native)