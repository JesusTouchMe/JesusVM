cmake_minimum_required(VERSION 3.26)

set(SOURCES "src/JesusNative.cpp")

set(HEADERS "include/JesusVM/JesusNative.h" "include/JesusVM/win32/JesusNative_platform.h")

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_library(jesusvm-native-jesusvm-native ${SOURCES} ${HEADERS})
add_library(JesusVM::native ALIAS jesusvm-native-jesusvm-native)

target_include_directories(jesusvm-native-jesusvm-native
	PUBLIC
		include
)

if(WIN32)
    target_compile_definitions(jesusvm-native-jesusvm-native PRIVATE PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(jesusvm-native-jesusvm-native PRIVATE PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(jesusvm-native-jesusvm-native PRIVATE PLATFORM_LINUX)
endif()

target_compile_features(jesusvm-native-jesusvm-native PUBLIC cxx_std_20)
target_link_libraries(jesusvm-native-jesusvm-native PRIVATE JesusVM::framework)
