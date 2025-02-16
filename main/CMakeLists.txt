cmake_minimum_required(VERSION 3.26)

set(SOURCES "src/main.cpp")

set(HEADERS)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_executable(JesusVM ${SOURCES} ${HEADERS})
add_executable(JesusVM::main ALIAS JesusVM)

target_include_directories(JesusVM
	PUBLIC 
		include
)

target_compile_definitions(JesusVM PUBLIC MODULEWEB_OPT_VERIFY_INSN)

target_compile_features(JesusVM PUBLIC cxx_std_20)
target_link_libraries(JesusVM JesusVM::framework)