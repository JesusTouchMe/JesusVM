#include "JesusVM/JesusNative.h"
#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Opcodes.h"

u8 mainCode[] = {
	JesusVM::Opcodes::IPUSH_8, 100,
	JesusVM::Opcodes::IPUSH_8, 31,
	JesusVM::Opcodes::SUB,
	JesusVM::Opcodes::PRINT,
	JesusVM::Opcodes::EXIT,
};

int main(int argc, char** argv) {
	JesusVM::JesusVM vm;
	std::unique_ptr<JesusVM::Thread> mainThread = std::make_unique<JesusVM::Thread>(vm);

	mainThread->setMainThread();

	JesusVM::Function mainFunc(vm.getTypeSystem(), "main()V", JesusVM::Function::NO_MODIFIERS, 0, 2, mainCode, sizeof(mainCode));

	std::unique_ptr<JesusVM::Module> module = std::make_unique<JesusVM::Module>("TestModule", 0, std::vector<JesusVM::Class>(), std::vector<JesusVM::Function>({ mainFunc }), std::vector<JesusVM::Section>());

	vm.addThread(std::move(mainThread));
	vm.addModule(std::move(module));

	JesusVM::Function* func = vm.getModule("TestModule")->getFunction("main()V");
	vm.start(func);

	return 0;
}