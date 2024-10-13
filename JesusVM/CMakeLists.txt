cmake_minimum_required(VERSION 3.8)

add_compile_definitions(JESUS_DEBUG)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fsanitize=address")
endif()

set(SOURCES
	"src/constpool.c"
	"src/debug.c"
	"src/function.c"
	"src/main.c"
	"src/stack.c"
	"src/type.c"
	"src/vm.c"

	"src/util/io.c"
	"src/util/string.c"
	"src/util/util.c"
	"src/module.c"
	"src/template/vector.c" 
	"src/util/arrayutil.c" "src/object/class.c" "src/object/field.c" "src/object/method.c" "src/object/object.c"   "src/executor.c")

if (WIN32)
	set(OS_SOURCES
		"src/os/windows/memory.c" "src/os/windows/init.c")
elseif (UNIX AND NOT APPLE)
	set(OS_SOURCES
		"src/os/linux/memory.c")
elseif (APPLE)
	set(OS_SOURCES
		"src/os/macos/memory.c")
endif()

set(HEADERS
	"include/constpool.h"
	"include/debug.h"
	"include/function.h"
	"include/module.h"
	"include/opcodes.h"
	"include/pch.h"
	"include/stack.h"
	"include/type.h"
	"include/vm.h"

	"include/util/arrayutil.h"
	"include/util/io.h"
	"include/util/string.h"
	"include/util/util.h"
	"include/template/vector.h" 
	"include/os/memory.h" "include/os/init.h" "include/object/object.h" "include/object/class.h" "include/object/field.h" "include/object/method.h")

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${OS_SOURCES} ${HEADERS})

add_executable(JesusVM ${SOURCES} ${OS_SOURCES} ${HEADERS})

set_property(TARGET JesusVM PROPERTY C_STANDARD 17)

target_precompile_headers(JesusVM PRIVATE include/pch.h)
target_include_directories(JesusVM PRIVATE include)