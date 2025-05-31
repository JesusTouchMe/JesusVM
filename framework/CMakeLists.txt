cmake_minimum_required(VERSION 3.26)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

set(SOURCES "src/JesusVM.cpp" "src/Module.cpp" "src/util/StringPool.cpp" "src/constpool/ConstPool.cpp" "src/Function.cpp" "src/bytecode/Executor.cpp" "src/executors/VThread.cpp" "src/executors/Thread.cpp" "src/heap/Class.cpp" "src/platform/NativePlugin.cpp"
        src/Stack.cpp
        src/heap/Object.cpp
        src/type/Type.cpp
        src/heap/Field.cpp
    src/constpool/ConstantModule.cpp
    src/constpool/ConstantName.cpp
    src/constpool/ConstantAscii.cpp
    src/constpool/ConstantFunc.cpp
    src/constpool/ConstantClass.cpp
        src/Linker.cpp
        src/constpool/Constant.cpp
    src/executors/Threading.cpp
    src/Preload.cpp
    src/runtime/vm/System.cpp
    src/constpool/ConstantField.cpp
        src/runtime/std/Primitives.cpp
    src/platform/Memory.cpp
        src/heap/gc/CycleCollector.cpp
        src/heap/gc/GC-Daemon.cpp
        src/util/Profiler.cpp
        src/GlobalVar.cpp
        src/constpool/ConstantGlobalVar.cpp
    src/bytecode/handlers/BaseHandler.cpp
)

set(HEADERS "include/JesusVM/JesusVM.h" "include/types.h" "include/JesusVM/Module.h" "include/JesusVM/constpool/Constant.h" "include/JesusVM/Stack.h" "include/JesusVM/constpool/ConstPool.h" "include/JesusVM/Function.h" "include/JesusVM/type/Type.h" "include/JesusVM/heap/Class.h" "include/JesusVM/util/StringPool.h" "include/JesusVM/bytecode/Executor.h" "include/JesusVM/executors/Thread.h" "include/JesusVM/executors/VThread.h" "include/JesusVM/bytecode/Opcodes.h" "include/JesusVM/platform/NativePlugin.h"
        include/JesusVM/heap/Object.h
        include/JesusVM/heap/Field.h
    include/JesusVM/constpool/ConstantClass.h
        include/JesusVM/Linker.h
        include/JesusVM/executors/Threading.h
    include/JesusVM/Preload.h
    include/JesusVM/preload/Preload-Modules.h
    include/JesusVM/runtime/vm/System.h
    include/JesusVM/preload/Preload-Classes.h
    include/JesusVM/preload/Preload-Functions.h
    include/JesusVM/constpool/ConstantField.h
        include/JesusVM/runtime/std/Primitives.h
    include/JesusVM/preload/Preload-Fields.h
    include/JesusVM/Options.h
    include/JesusVM/GlobalVar.h
    include/JesusVM/platform/Memory.h
    include/JesusVM/heap/gc/CycleCollector.h
        include/JesusVM/executors/Daemon.h
        src/executors/Daemon.cpp
        include/JesusVM/heap/gc/GC-Daemon.h
        include/JesusVM/util/Logging.h
        include/JesusVM/util/Profiler.h
        include/JesusVM/constpool/ConstantGlobalVar.h
    include/JesusVM/bytecode/handlers/BaseHandler.h
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_library(jesusvm-framework-jesusvm-framework ${SOURCES} ${HEADERS})
add_library(JesusVM::framework ALIAS jesusvm-framework-jesusvm-framework)

target_include_directories(jesusvm-framework-jesusvm-framework
	PUBLIC
		include
)

if(supported)
    if (${CMAKE_BUILD_TYPE} MATCHES Release|RelWithDebInfo)
        message(STATUS "enabling lto")
        set_target_properties(jesusvm-framework-jesusvm-framework PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
else()
    message(STATUS "lto not supported :( <${error}>")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(jesusvm-framework-jesusvm-framework PRIVATE -O3 -march=native)
elseif(MSVC)
    target_compile_options(jesusvm-framework-jesusvm-framework PRIVATE /O2 /arch:AVX2)
endif()

if(WIN32)
    target_compile_definitions(jesusvm-framework-jesusvm-framework PUBLIC PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(jesusvm-framework-jesusvm-framework PUBLIC PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(jesusvm-framework-jesusvm-framework PUBLIC PLATFORM_LINUX)
endif()

#target_compile_definitions(jesusvm-framework-jesusvm-framework PUBLIC JESUSVM_USE_JUMP_THREADING)

target_compile_features(jesusvm-framework-jesusvm-framework PUBLIC cxx_std_20)
target_link_libraries(jesusvm-framework-jesusvm-framework JesusVM::native)
target_link_libraries(jesusvm-framework-jesusvm-framework JesusVM::moduleweb)
