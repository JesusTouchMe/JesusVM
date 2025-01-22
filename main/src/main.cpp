#include "JesusVM/JesusNative.h"
#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Opcodes.h"

#include "JesusVM/constpool/ConstantFunc.h"

u8 mainCode[] = {
	JesusVM::Opcodes::BPUSH, 100,
	JesusVM::Opcodes::BPUSH, 31,
	JesusVM::Opcodes::SUB,
    JesusVM::Opcodes::CALL, 0, 0,
    JesusVM::Opcodes::RETURN,
};

extern "C" NATIVEEXPORT void NATIVECALL JesusVM_TestModule_print(VMContext ctx, JValue* args, Int i) {
    EXTRACT_ARG(I, i);

    std::cout << i << "\n";
}

int main(int argc, char** argv) {
	JesusVM::JesusVM vm;
	std::unique_ptr<JesusVM::Thread> mainThread = std::make_unique<JesusVM::Thread>(vm);

	mainThread->setMainThread();

	JesusVM::Function mainFunc(vm.getTypeSystem(), "main()V", JesusVM::Function::NO_MODIFIERS, 0, 2, mainCode, sizeof(mainCode));
    JesusVM::Function printFunc(vm.getTypeSystem(), "print(I)V", JesusVM::Function::NATIVE, 0, 0, reinterpret_cast<u8*>(JesusVM_TestModule_print), 0);

	std::unique_ptr<JesusVM::Module> module = std::make_unique<JesusVM::Module>("TestModule", 1, std::vector<JesusVM::Class>(), std::vector<JesusVM::Function>({ mainFunc, printFunc }));

    JesusVM::Function* mainFuncP = module->getFunction("main()V");
    JesusVM::Function* printFuncP = module->getFunction("print(I)V");

    module->getConstPool().set(0, std::make_unique<JesusVM::ConstantFunc>(printFuncP));

	vm.addThread(std::move(mainThread));
	vm.addModule(std::move(module));

	vm.start(mainFuncP);

	return 0;
}