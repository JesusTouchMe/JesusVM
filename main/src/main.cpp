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

void AddAttribute(moduleweb_attribute_array& array, u16 name_index, std::string_view value, u16 index) {
    array.array[index].name_index = name_index;
    array.array[index].length = value.length();
    array.array[index].info = reinterpret_cast<u8*>(strdup(value.data()));
}

u16 AddConstantAscii(moduleweb_module_info& module, std::string_view string, u16 index) {
    module.constant_pool[index].type = MODULEWEB_CONSTANT_TYPE_ASCII;
    module.constant_pool[index].ascii_info.length = string.length();
    module.constant_pool[index].ascii_info.bytes = reinterpret_cast<u8*>(strdup(string.data()));

    return index;
}

u16 AddConstantName(moduleweb_module_info& module, u16 name, u16 descriptor, u16 index) {
    module.constant_pool[index].type = MODULEWEB_CONSTANT_TYPE_NAME;
    module.constant_pool[index].name_info.name_index = name;
    module.constant_pool[index].name_info.descriptor_index = descriptor;

    return index;
}

void AddClass(moduleweb_module_info& module, u16 name_index, u16 modifiers, u16 super_class, std::vector<moduleweb_attribute_info> attributes, std::vector<moduleweb_field_info> fields, u16 index) {
    moduleweb_class_info& classInfo = module.classes[index];

    classInfo.name_index = name_index;
    classInfo.modifiers = modifiers;
    classInfo.super_class = super_class;

    classInfo.attributes.size = attributes.size();
    classInfo.attributes.array = static_cast<moduleweb_attribute_info*>(std::malloc(sizeof(moduleweb_attribute_info) * classInfo.attributes.size));

    for (u16 i = 0; i < classInfo.attributes.size; i++) {
        classInfo.attributes.array[i] = attributes[i];
    }

    classInfo.field_count = fields.size();
    classInfo.fields = static_cast<moduleweb_field_info*>(std::malloc(sizeof(moduleweb_field_info) * classInfo.field_count));

    for (u16 i = 0; i < classInfo.field_count; i++) {
        classInfo.fields[i] = fields[i];
    }
}

void AddFunction(moduleweb_module_info& module, u16 modifiers, u16 name, const std::vector<moduleweb_attribute_info>& attributes, u16 index) {
    moduleweb_function_info& functionInfo = module.functions[index];

    functionInfo.modifiers = modifiers;
    functionInfo.name_index = name;

    functionInfo.attributes.size = attributes.size();
    functionInfo.attributes.array = static_cast<moduleweb_attribute_info*>(std::malloc(sizeof(moduleweb_attribute_info) * functionInfo.attributes.size));

    for (u16 i = 0; i < functionInfo.attributes.size; i++) {
        functionInfo.attributes.array[i] = attributes[i];
    }
}

void TestModuleWeb() {
    moduleweb_module_info module;

    module.magic = MODULEWEB_MAGIC_NUMBER;
    module.bytecode_version = 1;
    module.name_index = 1;

    module.constant_pool_size = 13;
    module.constant_pool = static_cast<moduleweb_constant_info*>(std::malloc(sizeof(moduleweb_constant_info) * module.constant_pool_size));

    AddConstantAscii(module, "TestModule", 1);

    module.attributes.size = 2;
    module.attributes.array = static_cast<moduleweb_attribute_info*>(std::malloc(sizeof(moduleweb_attribute_info) * module.attributes.size));

    AddAttribute(module.attributes, AddConstantAscii(module, "Fortnite1", 2), "hello world fortnite balls", 0);
    AddAttribute(module.attributes, AddConstantAscii(module, "Fortnite2", 3), "jesusvm on top yeah", 1);

    module.class_count = 1;
    module.classes = static_cast<moduleweb_class_info*>(std::malloc(sizeof(moduleweb_class_info) * module.class_count));

    AddClass(module, AddConstantAscii(module, "SomeClass", 4), MODULEWEB_CLASS_MODIFIER_PUBLIC, 0,
             { { AddConstantAscii(module, "HeyThere", 5), 5, reinterpret_cast<u8*>(const_cast<char*>("hello")) } },
             { { MODULEWEB_FIELD_MODIFIER_PUBLIC, AddConstantName(module, AddConstantAscii(module, "x", 6), AddConstantAscii(module, "I", 7), 8), { 0, static_cast<moduleweb_attribute_info*>(std::malloc(0)) } } }, 0);

    module.function_count = 1;
    module.functions = static_cast<moduleweb_function_info*>(std::malloc(sizeof(moduleweb_function_info) * module.function_count));

    AddFunction(module, MODULEWEB_FUNCTION_MODIFIER_PUBLIC,
                AddConstantName(module, AddConstantAscii(module, "add", 9), AddConstantAscii(module, "(II)I", 10), 11),
                { {AddConstantAscii(module, "Code", 12), sizeof(mainCode), mainCode } }, 0);

    moduleweb_module_info_print(&module, 0);

    moduleweb_module_info_uninit(&module);
}

extern "C" NATIVEEXPORT void NATIVECALL JesusVM_TestModule_print(VMContext ctx, JValue* args, Int i) {
    EXTRACT_ARG(I, i);

    std::cout << i << "\n";
}

int main(int argc, char** argv) {
    TestModuleWeb();
    return 0;

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