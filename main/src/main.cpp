#include "JesusVM/JesusNative.h"
#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Opcodes.h"

#include "JesusVM/constpool/ConstantFunc.h"

#include "JesusVM/executors/Threading.h"

#include "moduleweb/reader.h"
#include "moduleweb/writer.h"

#include "moduleweb/builder/insn_builder.h"
#include "moduleweb/builder/module_builder.h"

int main(int argc, char** argv) {
    JesusVM::JesusVM vm;

    JesusVM::Threading::Init(vm);

    JesusVM::Linker::Init(vm);
    JesusVM::Linker::AddPath("."); // cwd i think

    JesusVM::Module* mainModule = JesusVM::Linker::LoadModule(nullptr, "Main");

    return 0;
}

/*
int main(int argc, char** argv) {
    moduleweb_module_builder builder{};
    moduleweb_attribute_builder attributeBuilder{};
    moduleweb_class_builder classBuilder{};
    moduleweb_field_builder fieldBuilder{};
    moduleweb_function_builder functionBuilder{};

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

    moduleweb_function_builder_modifiers(&functionBuilder, MODULEWEB_FUNCTION_MODIFIER_PUBLIC);
    moduleweb_function_builder_descriptor(&functionBuilder, "main", "()V");

    moduleweb_insn_list insnList;
    moduleweb_insn_list_init(&insnList);
    insnList.module = &builder;

//    moduleweb_insn_list_int(&insnList, JesusVM::Opcodes::IPUSH, 34);
//    moduleweb_insn_list_int(&insnList, JesusVM::Opcodes::IPUSH, 35);
//    moduleweb_insn_list_insn(&insnList, JesusVM::Opcodes::ADD);
    moduleweb_insn_list_insn(&insnList, JesusVM::Opcodes::RETURN);

    moduleweb_attribute_builder_code(&attributeBuilder, &insnList);

    moduleweb_function_builder_add_attribute(&functionBuilder, moduleweb_attribute_builder_build(&attributeBuilder));

    moduleweb_module_builder_add_function(&builder, moduleweb_function_builder_build(&functionBuilder));

    moduleweb_module_info info;

    moduleweb_module_builder_build(&builder, &info);

    moduleweb_module_info_print(&info, 0);

    moduleweb_outstream out;
    moduleweb_outstream_init(&out, "Main.jmod");

    moduleweb_writer writer;
    moduleweb_writer_init(&writer, &out);

    if (moduleweb_writer_accept(&writer, &info)) {
        return 1;
    }

	return 0;
}
 */