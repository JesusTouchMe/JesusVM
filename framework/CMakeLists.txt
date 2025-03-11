cmake_minimum_required(VERSION 3.26)

set(SOURCES "src/JesusVM.cpp" "src/Module.cpp" "src/util/StringPool.cpp" "src/constpool/ConstPool.cpp" "src/Function.cpp" "src/executors/Executor.cpp" "src/executors/VThread.cpp" "src/executors/Thread.cpp" "src/heap/Class.cpp" "src/platform/NativePlugin.cpp"
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
)

set(HEADERS "include/JesusVM/JesusVM.h" "include/types.h" "include/JesusVM/Module.h" "include/JesusVM/constpool/Constant.h" "include/JesusVM/Stack.h" "include/JesusVM/constpool/ConstPool.h" "include/JesusVM/Function.h" "include/JesusVM/type/Type.h" "include/JesusVM/heap/Class.h" "include/JesusVM/util/StringPool.h" "include/JesusVM/executors/Executor.h" "include/JesusVM/executors/Thread.h" "include/JesusVM/executors/VThread.h" "include/JesusVM/bytecode/Opcodes.h" "include/JesusVM/platform/NativePlugin.h"
        include/JesusVM/heap/Object.h
        include/JesusVM/heap/Field.h
    include/JesusVM/constpool/ConstantClass.h
        include/JesusVM/Linker.h
        include/JesusVM/executors/Threading.h
    include/JesusVM/Preload.h
    include/JesusVM/Preload-Modules.h
    include/JesusVM/runtime/vm/System.h
    include/JesusVM/Preload-Classes.h
    include/JesusVM/Preload-Functions.h
    include/JesusVM/constpool/ConstantField.h
        include/JesusVM/runtime/std/Primitives.h
        include/JesusVM/Preload-Fields.h
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_library(jesusvm-framework-jesusvm-framework ${SOURCES} ${HEADERS})
add_library(JesusVM::framework ALIAS jesusvm-framework-jesusvm-framework)

target_include_directories(jesusvm-framework-jesusvm-framework
	PUBLIC
		include
)

if(WIN32)
    target_compile_definitions(jesusvm-framework-jesusvm-framework PUBLIC PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(jesusvm-framework-jesusvm-framework PUBLIC PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(jesusvm-framework-jesusvm-framework PUBLIC PLATFORM_LINUX)
endif()

target_compile_features(jesusvm-framework-jesusvm-framework PUBLIC cxx_std_20)
target_link_libraries(jesusvm-framework-jesusvm-framework JesusVM::native)
target_link_libraries(jesusvm-framework-jesusvm-framework JesusVM::moduleweb)
