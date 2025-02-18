#ifndef JESUSVM_OPCODES_H
#define JESUSVM_OPCODES_H

#include "types.h"

namespace JesusVM {
	namespace Opcodes {
#       include "moduleweb/builder/opcodes.h"

        using EnumOpcodes = moduleweb_opcodes;
	}
}

#endif // JESUSVM_OPCODES_H
