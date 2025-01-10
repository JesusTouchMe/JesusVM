#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Opcodes.h"

#include <iostream>

int main(int argc, char** argv) {
	JesusVM::JesusVM vm;
	std::unique_ptr<JesusVM::Thread> mainThread = std::make_unique<JesusVM::Thread>(vm);

	mainThread->setMainThread();

	JesusVM::Function mainFunc(vm.getTypeSystem(), "main()V", 0, 0, 2, )

	std::unique_ptr<JesusVM::Module> module = std::make_unique<JesusVM::Module>("TestModule", 0, std::vector<JesusVM::Class>(), std::vector<JesusVM::Function>(), std::vector<JesusVM::Section>());

	return 0;
}