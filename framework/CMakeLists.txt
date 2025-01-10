cmake_minimum_required(VERSION 3.26)

set(SOURCES "src/JesusVM.cpp" "src/Module.cpp" "src/type/PrimitiveType.cpp" "src/type/VoidType.cpp" "src/type/FunctionType.cpp" "src/util/StringPool.cpp" "src/type/TypeSystem.cpp" "src/type/ReferenceType.cpp" "src/constpool/ConstPool.cpp" "src/Function.cpp" "src/executors/Executor.cpp" "src/executors/VThread.cpp" "src/executors/Thread.cpp")

set(HEADERS "include/JesusVM/JesusVM.h" "include/types.h" "include/JesusVM/Module.h" "include/JesusVM/constpool/Constant.h" "include/JesusVM/Stack.h" "include/JesusVM/constpool/ConstPool.h" "include/JesusVM/Function.h" "include/JesusVM/type/Type.h" "include/JesusVM/heap/Class.h" "include/JesusVM/type/PrimitiveType.h" "include/JesusVM/type/VoidType.h" "include/JesusVM/type/FunctionType.h" "include/JesusVM/util/StringPool.h" "include/JesusVM/type/TypeSystem.h" "include/JesusVM/type/ReferenceType.h" "include/JesusVM/executors/Executor.h" "include/JesusVM/executors/Thread.h" "include/JesusVM/executors/VThread.h" "include/JesusVM/bytecode/Opcodes.h")

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_library(jesusvm-framework-jesusvm-framework ${SOURCES} ${HEADERS})
add_library(JesusVM::framework ALIAS jesusvm-framework-jesusvm-framework)

target_include_directories(jesusvm-framework-jesusvm-framework
	PUBLIC
		include
)

target_compile_features(jesusvm-framework-jesusvm-framework PUBLIC cxx_std_20)