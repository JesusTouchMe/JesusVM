#include "JesusVM/JesusNative.h"
#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Opcodes.h"

#include "JesusVM/constpool/ConstantFunc.h"

u8 mainCode[] = {
	JesusVM::Opcodes::BPUSH, 100,
	JesusVM::Opcodes::BPUSH, 31,
	JesusVM::Opcodes::SUB,
    JesusVM::Opcodes::RETURN,
};

int main(int argc, char** argv) {
	JesusVM::JesusVM vm;
	std::unique_ptr<JesusVM::Thread> mainThread = std::make_unique<JesusVM::Thread>(vm);

	mainThread->setMainThread();

	JesusVM::Function mainFunc(vm.getTypeSystem(), "main()V", JesusVM::Function::NO_MODIFIERS, 0, 2, mainCode, sizeof(mainCode));

	std::unique_ptr<JesusVM::Module> module = std::make_unique<JesusVM::Module>("TestModule", 1, std::vector<JesusVM::Class>(), std::vector<JesusVM::Function>({ mainFunc }), std::vector<JesusVM::Section>());

    JesusVM::Function* mainFuncP = vm.getModule("TestModule")->getFunction("main()V");

    module->getConstPool().set(0, std::make_unique<JesusVM::ConstantFunc>(mainFuncP));

	vm.addThread(std::move(mainThread));
	vm.addModule(std::move(module));

	vm.start(mainFuncP);

	return 0;
}