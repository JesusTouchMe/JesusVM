#include "JesusVM/JesusNative.h"
#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Opcodes.h"

u8 mainCode[] = {
	JesusVM::Opcodes::IPUSH_8, 2,
	JesusVM::Opcodes::IPUSH_8, 3,
	JesusVM::Opcodes::ADD,
	JesusVM::Opcodes::PRINT,
	JesusVM::Opcodes::EXIT,
};

extern "C" NATIVEEXPORT void NATIVECALL JesusVM_TestModule_main_I(VMContext ctx, JValue* passedArgs) {
	std::cout << std::thread::hardware_concurrency() << "\n";
}

int main(int argc, char** argv) {
	JesusVM::JesusVM vm;
	std::unique_ptr<JesusVM::Thread> mainThread = std::make_unique<JesusVM::Thread>(vm);

	mainThread->setMainThread();

	JesusVM::Function mainFunc(vm.getTypeSystem(), "main(I)V", JesusVM::Function::NATIVE, 0, 0, reinterpret_cast<u8*>(JesusVM_TestModule_main_I), 0);

	std::unique_ptr<JesusVM::Module> module = std::make_unique<JesusVM::Module>("TestModule", 0, std::vector<JesusVM::Class>(), std::vector<JesusVM::Function>({ mainFunc }), std::vector<JesusVM::Section>());

	vm.addThread(std::move(mainThread));
	vm.addModule(std::move(module));

	JesusVM::Function* func = vm.getModule("TestModule")->getFunction("main(I)V");
	vm.start(func);

	return 0;
}