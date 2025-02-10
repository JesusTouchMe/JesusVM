#include "JesusVM/JesusNative.h"
#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Opcodes.h"

#include "JesusVM/constpool/ConstantFunc.h"

#include "moduleweb/module_info.h"

u8 mainCode[] = {
        JesusVM::Opcodes::BPUSH, 100,
        JesusVM::Opcodes::BPUSH, 31,
        JesusVM::Opcodes::SUB,
        JesusVM::Opcodes::CALL, 0, 0,
        JesusVM::Opcodes::RETURN,
};

int main(int argc, char** argv) {

	return 0;
}