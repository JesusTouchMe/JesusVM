cmake_minimum_required(VERSION 3.26)

set(SOURCES "src/main.cpp")

set(HEADERS
		../moduleweb/include/moduleweb/stream.h)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_executable(JesusVM ${SOURCES} ${HEADERS})
add_executable(JesusVM::main ALIAS JesusVM)

target_include_directories(JesusVM
	PUBLIC 
		include
)

if(WIN32)
	target_compile_definitions(jesusvm-framework-jesusvm-framework PRIVATE PLATFORM_WINDOWS)
elseif(APPLE)
	target_compile_definitions(jesusvm-framework-jesusvm-framework PRIVATE PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
	target_compile_definitions(jesusvm-framework-jesusvm-framework PRIVATE PLATFORM_LINUX)
endif()

target_compile_features(JesusVM PUBLIC cxx_std_20)
target_link_libraries(JesusVM JesusVM::framework)