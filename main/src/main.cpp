#include "JesusVM/JesusVM.h"
#include "JesusVM/Preload.h"

#include "JesusVM/bytecode/Opcodes.h"

#include "JesusVM/constpool/ConstantFunc.h"

#include "JesusVM/executors/Threading.h"

#include "moduleweb/reader.h"
#include "moduleweb/writer.h"

#include "moduleweb/builder/insn_builder.h"
#include "moduleweb/builder/module_builder.h"

///*
int main(int argc, char** argv) {
    JesusVM::JesusVM vm;

    JesusVM::Linker::Init(vm);
    JesusVM::Linker::AddPath("."); // cwd i think
    JesusVM::Linker::AddPluginPath(".");

    JesusVM::Preload::PreloadSystemModules();

    JesusVM::Threading::Init(vm);

    JesusVM::Module* mainModule = JesusVM::Linker::LoadModule(nullptr, "Main");

    JesusVM::Function* mainFunction = mainModule->getFunction("main", "()V");
    mainFunction->invoke<void>();

    JesusVM::Threading::WaitForAllThreads();

    return 0;
}
//*/

/*
int main(int argc, char** argv) {
    moduleweb_module_builder builder{};
    moduleweb_attribute_builder attributeBuilder{};
    moduleweb_class_builder classBuilder{};
    moduleweb_field_builder fieldBuilder{};
    moduleweb_function_builder functionBuilder{};

    builder.constant_pool_index = 1; //TODO: fix this

    attributeBuilder.module = &builder;
    classBuilder.module = &builder;
    fieldBuilder.module = &builder;
    functionBuilder.module = &builder;

    moduleweb_module_builder_version(&builder, 1);
    moduleweb_module_builder_name(&builder, "Main");

    moduleweb_class_builder_name(&classBuilder, "Vec2");
    moduleweb_class_builder_modifiers(&classBuilder, MODULEWEB_CLASS_MODIFIER_PUBLIC);

    moduleweb_field_builder_modifiers(&fieldBuilder, MODULEWEB_FIELD_MODIFIER_PUBLIC);
    moduleweb_field_builder_descriptor(&fieldBuilder, "x", "I");

    moduleweb_class_builder_add_field(&classBuilder, moduleweb_field_builder_build(&fieldBuilder));

    moduleweb_field_builder_modifiers(&fieldBuilder, MODULEWEB_FIELD_MODIFIER_PUBLIC);
    moduleweb_field_builder_descriptor(&fieldBuilder, "y", "I");

    moduleweb_class_builder_add_field(&classBuilder, moduleweb_field_builder_build(&fieldBuilder));

    moduleweb_module_builder_add_class(&builder, moduleweb_class_builder_build(&classBuilder));

    moduleweb_function_builder_modifiers(&functionBuilder, MODULEWEB_FUNCTION_MODIFIER_PUBLIC | MODULEWEB_FUNCTION_MODIFIER_NATIVE);
    moduleweb_function_builder_descriptor(&functionBuilder, "print", "(I)V");

    moduleweb_module_builder_add_function(&builder, moduleweb_function_builder_build(&functionBuilder));

    moduleweb_function_builder_modifiers(&functionBuilder, MODULEWEB_FUNCTION_MODIFIER_PUBLIC);
    moduleweb_function_builder_descriptor(&functionBuilder, "main", "()V");

    moduleweb_insn_list insnList;
    moduleweb_insn_list_init(&insnList);
    insnList.module = &builder;

    moduleweb_insn_list_int(&insnList, JesusVM::Opcodes::IPUSH, 34);
    moduleweb_insn_list_int(&insnList, JesusVM::Opcodes::IPUSH, 35);
    moduleweb_insn_list_insn(&insnList, JesusVM::Opcodes::ADD);
    moduleweb_insn_list_call(&insnList, JesusVM::Opcodes::CALL, "Main", "print", "(I)V");
    moduleweb_insn_list_insn(&insnList, JesusVM::Opcodes::RETURN);

    moduleweb_attribute_builder_code(&attributeBuilder, &insnList);

    moduleweb_function_builder_add_attribute(&functionBuilder, moduleweb_attribute_builder_build(&attributeBuilder));

    moduleweb_module_builder_add_function(&builder, moduleweb_function_builder_build(&functionBuilder));

    moduleweb_module_info mainInfo;

    moduleweb_module_builder_build(&builder, &mainInfo);

    builder.constant_pool_index = 1;

    moduleweb_module_builder_version(&builder, 1);
    moduleweb_module_builder_name(&builder, "vm/System");

    moduleweb_module_info systemInfo;

    moduleweb_module_builder_build(&builder, &systemInfo);

    moduleweb_module_info_print(&systemInfo, 0);
    moduleweb_module_info_print(&mainInfo, 0);

    moduleweb_outstream out;
    if (moduleweb_outstream_init(&out, "Main.jmod")) {
        std::cout << moduleweb_outstream_strerror(&out) << "\n";
        return 1;
    }

    moduleweb_writer writer;
    moduleweb_writer_init(&writer, &out);

    if (moduleweb_writer_accept(&writer, &mainInfo)) {
        return 1;
    }

    moduleweb_outstream_uninit(&out);

    if (moduleweb_outstream_init(&out, "vm/System.jmod")) {
        std::cout << moduleweb_outstream_strerror(&out) << "\n";
        return 1;
    }

    moduleweb_writer_init(&writer, &out);

    if (moduleweb_writer_accept(&writer, &systemInfo)) {
        return 1;
    }

	return 0;
}
//*/