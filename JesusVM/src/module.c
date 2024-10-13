#include "module.h"
#include "vm.h"

static const u32 MODULE_MAGIC = 0x4E696765;

#define HEADER_SIZE 48

#define HEADER_MAGIC 0
#define HEADER_NAMEINDEX 4
#define HEADER_ENTRYINDEX 8

#define HEADER_FUNCTIONSSIZE 12
#define HEADER_FUNCTIONCOUNT 16

#define HEADER_CLASSESSIZE 20
#define HEADER_CLASSCOUNT 24

#define HEADER_CONSTPOOLSIZE 28
#define HEADER_CONSTANTCOUNT 32

#define HEADER_STRINGTABLESIZE 36
#define HEADER_BYTECODESIZE 40

#define HEADER_SECTIONCOUNT 44

#define SECTION_HEADER_SIZETOTAL 8

#define SECTION_HEADER_NAMEINDEX 0
#define SECTION_HEADER_SIZE 4

#define GetPrimarySections(m, f, h) Section* functionSection = _GetFunctionSection(m, f, h); \
    Section* classesSection = _GetClassesSection(m, f, h); \
    Section* constPoolSection = _GetConstPoolSection(m, f, h); \
    Section* stringTabSection = _GetStringTableSection(m, f, h); \
    Section* bytecodeSection = _GetBytecodeSection(m, f, h)

static inline u32 HeaderLookup(u8* header, u32 element) {
    header += element;

    return (u32) ((header[0] << 24) |
        (header[1] << 16) |
        (header[2] << 8) |
        header[3]);
}

static inline Section* _GetFunctionSection(Module* module, FILE* file, u8* header) {
    u32 size = HeaderLookup(header, HEADER_FUNCTIONSSIZE);

    if (size == 0) {
		module->sections[FUNCTIONS_SECTION_INDEX] = (Section) { str("functions"), 0, null };
		return GetFunctionsSection(module);
    }

    u8* buffer = malloc(size);

    if (buffer == null) {
        puts("malloc fail");
		module->sections[FUNCTIONS_SECTION_INDEX] = (Section) { str("functions"), 0, null };
		return GetFunctionsSection(module);
    }

    fread_s(buffer, size, 1, size, file);

    module->sections[FUNCTIONS_SECTION_INDEX] = (Section) { str("functions"), size, buffer};
    return GetFunctionsSection(module);
}

static inline Section* _GetClassesSection(Module* module, FILE* file, u8* header) {
    u32 size = HeaderLookup(header, HEADER_CLASSESSIZE);

    if (size == 0) {
		module->sections[CLASSES_SECTION_INDEX] = (Section) { str("classes"), 0, null };
		return GetClassesSection(module);
    }

    u8* buffer = malloc(size);

    if (buffer == null) {
        puts("malloc fail");
		module->sections[CLASSES_SECTION_INDEX] = (Section) { str("classes"), size, null };
		return GetClassesSection(module);
    }

    fread_s(buffer, size, 1, size, file);

    module->sections[CLASSES_SECTION_INDEX] = (Section) { str("classes"), size, buffer};
    return GetClassesSection(module);
}

static inline Section* _GetConstPoolSection(Module* module, FILE* file, u8* header) {
    u32 size = HeaderLookup(header, HEADER_CONSTPOOLSIZE);

    if (size == 0) {
		module->sections[CONSTPOOL_SECTION_INDEX] = (Section) { str("constpool"), 0, null };
		return GetConstPoolSection(module);
    }

    u8* buffer = malloc(size);

    if (buffer == null) {
        puts("malloc fail");
		module->sections[CONSTPOOL_SECTION_INDEX] = (Section) { str("constpool"), size, null };
		return GetConstPoolSection(module);
    }

    fread_s(buffer, size, 1, size, file);

    module->sections[CONSTPOOL_SECTION_INDEX] = (Section) { str("constpool"), size, buffer};
    return GetConstPoolSection(module);
}

static inline Section* _GetStringTableSection(Module* module, FILE* file, u8* header) {
    u32 size = HeaderLookup(header, HEADER_STRINGTABLESIZE);

    if (size == 0) {
		module->sections[STRTAB_SECTION_INDEX] = (Section) { str("strtab"), 0, null };
		return GetStrtabSection(module);
    }

    u8* buffer = malloc(size);

    if (buffer == null) {
        puts("malloc fail");
		module->sections[STRTAB_SECTION_INDEX] = (Section) { str("strtab"), size, null };
		return GetStrtabSection(module);
    }

    fread_s(buffer, size, 1, size, file);

    module->sections[STRTAB_SECTION_INDEX] = (Section) { str("strtab"), size, buffer};
    return GetStrtabSection(module);
}

static inline Section* _GetBytecodeSection(Module* module, FILE* file, u8* header) {
    u32 size = HeaderLookup(header, HEADER_BYTECODESIZE);

    if (size == 0) {
		module->sections[CODE_SECTION_INDEX] = (Section) { str("code"), 0, null };
		return GetCodeSection(module);
    }

    u8* buffer = malloc(size);

    if (buffer == null) {
        puts("malloc fail");
		module->sections[CODE_SECTION_INDEX] = (Section) { str("code"), size, null };
		return GetCodeSection(module);
    }

    fread_s(buffer, size, 1, size, file);

    module->sections[CODE_SECTION_INDEX] = (Section) { str("code"), size, buffer};
    return GetCodeSection(module);
}

static inline String ModuleGetString_internal(Section* stringTab, u32 index) {
    u8* buffer = stringTab->buffer + index;
    u16 size = Immediate16(&buffer);

    return (String) { buffer, size, false, false, true };
}

static Type* ParseType(Module* module, String string, char** buffer) {
    switch (*string.buffer) {
        case TYPE_BYTE_ID: {
            if (buffer != null) *buffer += 1;
            return &byteType;
        }

        case TYPE_SHORT_ID: {
            if (buffer != null) *buffer += 1;
            return &shortType;
        }

        case TYPE_INT_ID: {
            if (buffer != null) *buffer += 1;
            return &intType;
        }

        case TYPE_LONG_ID: {
            if (buffer != null) *buffer += 1;
            return &longType;
        }

        case TYPE_BOOL_ID: {
            if (buffer != null) *buffer += 1;
            return &boolType;
        }

        case TYPE_VOID_ID: {
            if (buffer != null) *buffer += 1;
            return &voidType;
        }

        case TYPE_CLASS_ID: {
            string = SubStringFrom(&string, 1);

            u32 index = IndexOf(&string, ';');
            String type = SubStringTo(&string, index);

            u32 moduleIndex = LastIndexOf(&type, '/');
            String moduleName = SubStringTo(&type, moduleIndex);
            String name = SubStringFrom(&type, moduleIndex + 1);

            Module* module = VMGetModule(moduleName);
            Class* class = GetClass(module, name);

            if (buffer != null) *buffer += index + 2;
            return GetClassType(class);
        }

        default: {
            printf("bad type %c\n", **buffer);
            return -1;
        }
    }
}

// please fix this function it's inhumane for anyone reading it
static FunctionType* ParseFunctionType(Module* module, String string, u16 argumentCount) {
    char* buffer = string.buffer + 1; // +1 because '('

    Type** argumentTypes = malloc(sizeof(Type*) * argumentCount);
    if (argumentTypes == null) {
        puts("functiontype malloc error");
        return null;
    }

    u16 index = 0;

    while (*buffer != ')') {
        Type* type = ParseType(module, NewStringWithLength(buffer, string.length - (buffer - string.buffer)), &buffer);
        if (type == -1) {
            return null;
        }

        argumentTypes[index++] = type;
    }
    buffer++;

    Type* returnType = ParseType(module, NewStringWithLength(buffer, string.length - (buffer - string.buffer)), &buffer);

    if (returnType == -1) {
        return null;
    }

    FunctionType* type = GetFunctionType(returnType, argumentTypes, argumentCount);
    free(argumentTypes);
    
    return type;
}

static bool IsFunctionBroken(Function* function) {
    for (u16 j = 0; j < function->type->argumentCount; j++) {
        if (function->type->argumentTypes[j] == null) {
            return true;
        }
    }

    return false;
}

static bool CheckModulesCapacity() {
	if (vm.modules == null) return false;

	if (vm.moduleCount >= vm.moduleCapacity) {
		vm.moduleCapacity *= 2;

		Module* newModules = realloc(vm.modules, sizeof(Module) * vm.moduleCapacity);
		if (newModules == null) return false;

		vm.modules = newModules;
	}

	return true;
}

Section* AllocSection(Module* module, u32 index) {
    return &module->sections[PRIMARY_SECTION_COUNT + index];
}

void FreeSection(Section* section) {
    if (section->buffer != null) {
        free(section->buffer);
    }
}

Section* GetSectionByName(Module* module, String name) {
    for (u32 i = 0; i < module->sectionCount; i++) {
        Section* section = &module->sections[i];

        if (StringEquals(&section->name, &name)) {
            return section;
        }
    }

    return null;
}

// TODO: new solution to dynamically resizing the module array. currently there's gonna be garbage pointers everywhere if there's over the first size of the modules array (16)
Module* AllocModule() {
	if (!CheckModulesCapacity()) return null;

	return &vm.modules[vm.moduleCount++];
}

void NewModule(Module* module, String name, u32 constPoolSize, u32 classCount, u32 functionCount) {
	module->name = name;
	
	NewConstPool(&module->constPool, constPoolSize);

    module->classes = malloc(sizeof(Class) * classCount);
    module->classCount = classCount;

	module->functions = malloc(sizeof(Function) * functionCount);
	module->functionCount = functionCount;
	module->functionIndex = 0;
}

void DeleteModule(Module* module) {
	free(module->constPool.constants);
    free(module->classes);
	free(module->functions);
}

Module* OpenModule(String fileName) {
    FILE* file = fopen(StringCStr(&fileName), "rb");
    if (file == null) {
        puts("file null");
        return;
    }

	u8 header[HEADER_SIZE];
    fread_s(header, HEADER_SIZE, 1, HEADER_SIZE, file);

    if (HeaderLookup(header, HEADER_MAGIC) != MODULE_MAGIC) {
        puts("file isn't a valid jesusmodule file");
        fclose(file);
        return;
    }

    Module* module = AllocModule();

    u32 sectionCount = PRIMARY_SECTION_COUNT + HeaderLookup(header, HEADER_SECTIONCOUNT);
    module->sectionCount = sectionCount;
    module->sections = calloc(sectionCount, sizeof(Section));

    GetPrimarySections(module, file, header);

    String moduleName = ModuleGetString_internal(stringTabSection, HeaderLookup(header, HEADER_NAMEINDEX));

    u32 functionCount = HeaderLookup(header, HEADER_FUNCTIONCOUNT);
    u32 classCount = HeaderLookup(header, HEADER_CLASSCOUNT);
    u32 constantCount = HeaderLookup(header, HEADER_CONSTANTCOUNT);

    NewModule(module, moduleName, constantCount, classCount, functionCount);

    // functions

    u8* buffer = functionSection->buffer;

    for (u32 i = 0; i < functionCount; i++) {
        Function* function = AllocFunction(module);

        u16 modifiers = Immediate16(&buffer);
        u16 argumentCount = Immediate16(&buffer);
        u16 localCount = Immediate16(&buffer);

        u32 descriptorIndex = Immediate32(&buffer);
        String descriptor = ModuleGetString_internal(stringTabSection, descriptorIndex);

        u8* entry = bytecodeSection->buffer + Immediate64(&buffer);

        u32 index = LastIndexOf(&descriptor, '(');

        String name = SubStringTo(&descriptor, index);
        String typeString = SubStringFrom(&descriptor, index);

        FunctionType* type = ParseFunctionType(module, typeString, argumentCount);
        type->id = descriptor;

        function->type = type;
        function->name = name;
        function->fullDescriptor = descriptor;
        function->module = module;
        function->modifiers = modifiers;
        function->localCount = localCount;
        function->paramCount = argumentCount;
        function->entry = entry;

        if (!ValidateFunction(function)) {
            puts("TODO: handle validation errors");
        }
    }

    // classes

    buffer = classesSection->buffer;

    for (u32 i = 0; i < classCount; i++) {
        Class* class = AllocClass(module, i);

        u16 modifiers = Immediate16(&buffer);

        u32 nameIndex = Immediate32(&buffer);
        u32 superModuleIndex = Immediate32(&buffer);
        u32 superClassIndex = Immediate32(&buffer);

        u16 fieldCount = Immediate16(&buffer);
        u16 methodCount = Immediate16(&buffer);

        String name = ModuleGetString_internal(stringTabSection, nameIndex);

        Class* superClass = null;

        if (superModuleIndex == 0 && superClassIndex == 0) {
        } else {
            String superModuleName = ModuleGetString_internal(stringTabSection, superModuleIndex);
            String superClassName = ModuleGetString_internal(stringTabSection, superClassIndex);

            Module* superModule = VMGetModule(superModuleName);
            superClass = GetClass(superModule, superClassName);
        }

        class->name = name;
        class->superClass = superClass;
        class->modifiers = modifiers;
        class->fieldRefCount = fieldCount;
        class->methodRefCount = methodCount;
        class->fieldRefs = malloc(sizeof(FieldRef) * fieldCount);
        class->methodRefs = malloc(sizeof(MethodRef) * methodCount);

        for (u16 j = 0; j < fieldCount; j++) {
            u32 descriptorIndex = Immediate32(&buffer);
            u32 nameIndex = Immediate32(&buffer);

            u16 modifiers = Immediate16(&buffer);

            String descriptor = ModuleGetString_internal(stringTabSection, descriptorIndex);
            String name = ModuleGetString_internal(stringTabSection, nameIndex);

            FieldRef* field = AllocFieldRef(class, j);

            field->thisClass = class;
            field->type = ParseType(module, descriptor, null);
            field->name = name;
            field->modifiers = modifiers;
            field->classFieldIndex = j; // maybe sort somehow so it does some smart memory stuff :speaking_head:
        }

        for (u16 j = 0; j < methodCount; j++) {
            u32 functionNameIndex = Immediate32(&buffer);
            String functionName = ModuleGetString_internal(stringTabSection, functionNameIndex); // expected to be the descriptor

            Function* underlyingFunction = GetFunction(module, functionName);

            MethodRef* method = AllocMethodRef(class, j);

            method->thisClass = class;
            method->function = underlyingFunction;
            method->classMethodIndex = j; // maybe sort somehow so it does some smart memory stuff :speaking_head:
        }

        if (!ValidateClass(class)) {
            puts("TODO: handle validation errors");
        }
    }

    // fix functions lol
    
    for (u32 i = 0; i < functionCount; i++) {
        Function* function = &module->functions[i];

        if (!IsFunctionBroken(function)) {
            continue;
        }

        u32 index = LastIndexOf(&function->fullDescriptor, '(');
        String typeString = SubStringFrom(&function->fullDescriptor, index);

        function->type = ParseFunctionType(module, typeString, function->type->argumentCount); // fix the memory wasted by making the type twice :sob:
    }


    if (sectionCount > PRIMARY_SECTION_COUNT) {
        sectionCount -= PRIMARY_SECTION_COUNT;

        for (u32 i = 0; i < sectionCount; i++) {
            u8 header[SECTION_HEADER_SIZETOTAL];
            fread_s(header, SECTION_HEADER_SIZETOTAL, 1, SECTION_HEADER_SIZETOTAL, file);

            u32 nameIndex = HeaderLookup(header, SECTION_HEADER_NAMEINDEX);
            u32 size = HeaderLookup(header, SECTION_HEADER_SIZE);

            u8* buffer = malloc(size);
            if (buffer == null) {
                puts("alloc error");
                continue;
            }

            fread_s(buffer, size, 1, size, file);

            Section* section = AllocSection(module, i);

            section->name = ModuleGetString_internal(GetStrtabSection(module), nameIndex);
            section->size = size;
            section->buffer = buffer;
        }
    }

    module->functionIndex = HeaderLookup(header, HEADER_ENTRYINDEX);

    fclose(file);

    Section* init = GetSectionByName(module, str("init"));
    if (init != null) {
        u8* buff = init->buffer;
        u16 localCount = Immediate16(&buff);

        ExecuteCode(true, buff, localCount);
    }

    return module;
}

String ModuleGetString(Module* module, u32 index) {
    if (index >= GetStrtabSection(module)->size) {
        return NullString;
    }

    return ModuleGetString_internal(GetStrtabSection(module), index);
}