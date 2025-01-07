cmake_minimum_required(VERSION 3.26)

set(SOURCES "src/JesusVM.cpp")

set(HEADERS "include/JesusVM/JesusVM.h")

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_library(jesusvm-framework-jesusvm-framework ${SOURCES} ${HEADERS})
add_library(JesusVM::framework ALIAS jesusvm-framework-jesusvm-framework)

target_include_directories(jesusvm-framework-jesusvm-framework
	PUBLIC
		include
)

target_compile_features(jesusvm-framework-jesusvm-framework PUBLIC cxx_std_20)