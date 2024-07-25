cmake_minimum_required(VERSION 3.8)

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
)

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

	"include/util/io.h"
	"include/util/string.h"
	"include/util/util.h"
 )

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_executable (JesusVM ${SOURCES} ${HEADERS})

set_property(TARGET JesusVM PROPERTY C_STANDARD 17)

target_precompile_headers(JesusVM PRIVATE include/pch.h)
target_include_directories(JesusVM PRIVATE include)