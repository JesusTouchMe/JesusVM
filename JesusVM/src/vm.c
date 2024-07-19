#include "debug.h"
#include "util/util.h"
#include "vm.h"

#define inst(opcode, ...) case opcode:

#define _op1() (i8) (*vm.ip++)

#define op(name)   register i8 name = _op1()
#define op16(name) register i16 name = _op2()
#define op32(name) register i32 name = _op4()
#define op64(name) register i64 name = _op8()

#define pop(stack, name, isObjectBuf) register i64 name = StackPop(stack, isObjectBuf)

#define wide if (wideInst)
#define narrow else

JesusVM vm = { 0 };

static inline i16 _op2(void) {
	u8 t1 = *vm.ip++;
	u8 t2 = *vm.ip++;
	return (i16)((t1 << 8) | t2);
}

static inline i32 _op4(void) {
	u8 t1 = *vm.ip++;
	u8 t2 = *vm.ip++;
	u8 t3 = *vm.ip++;
	u8 t4 = *vm.ip++;
	return (i32)((t1 << 24) | (t2 << 16) | (t3 << 8) | t4);
}

static inline i64 _op8(void) {
	u64 t1 = *vm.ip++;
	u64 t2 = *vm.ip++;
	u64 t3 = *vm.ip++;
	u64 t4 = *vm.ip++;
	u64 t5 = *vm.ip++;
	u64 t6 = *vm.ip++;
	u64 t7 = *vm.ip++;
	u64 t8 = *vm.ip++;
	return (i64) ((t1 << 56) | (t2 << 48) | (t3 << 40) | (t4 << 32) | (t5 << 24) | (t6 << 16) | (t7 << 8) | t8);
}

static inline void JumpUnconditional(i64 relative) {
	vm.ip += relative;
}

static inline void JumpConditional(i64 relative, u16 flags) {
	if (GetFlag(flags)) {
		JumpUnconditional(relative);
	}
}

static inline void JumpConditionalNot(i64 relative, u16 flags) {
	if (!GetFlag(flags)) {
		JumpUnconditional(relative);
	}
}

static Constant* GetConstant(u32 index) {
	if (index >= vm.constPool.size) {
		printf("VM error: IndexOutOfBoundsException: constant pool has %u elements, but %u was requested\n", vm.constPool.size, index);
		ExitVM();
		exit(1);
	}

	return &vm.constPool.constants[index];
}

void StartVM(u8* bytecode, u32 constPoolSize, u32 functionCount) {
	vm.ip = bytecode;

	vm.constPool.constants = calloc(constPoolSize, sizeof(Constant));
	vm.constPool.size = constPoolSize;
	vm.constPool.index = 0;

	vm.functions = calloc(functionCount, sizeof(Function));
	vm.functionCount = functionCount;
	vm.functionIndex = 0;

	if (vm.functions == null) {
		puts("VM error: NullPointerException: function array allocation returned null");
		exit(1);
	}

	NewStack(&vm.stack, 1024);
}

void ExitVM() {
	FreeFunctionTypes();

	free(vm.constPool.constants);
	free(vm.functions);

	DeleteStack(&vm.stack);

	vm = (JesusVM) {0};
}

void VMBeginExecution(nullable() Function* entry) {
	if (entry->paramCount > 0) {
		puts("TODO: param stuff");
		ExitVM();
		exit(2);
	}

	if (entry != null) {
		vm.ip = entry->entry;
		vm.currentFunction = entry;

		vm.stack.top += entry->localCount;
		vm.stackFrame = 0;
	}

	bool wideInst = false;

	main_loop:
	for (;;) { // weird trick for infinite loop
		u8 instruction = *vm.ip++;

		switch (instruction) {
			inst(OP_NOP) {
				break;
			}

			inst(OP_POP) {
				StackPop(&vm.stack, null);
				break;
			}

			inst(OP_DUP) {
				u64 index = vm.stack.top - 1;

				StackPush(&vm.stack, vm.stack.data[index], StackIsObject(&vm.stack, index));

				break;
			}

			inst(OP_LOAD, index8|index16) {
				wide {
					op16(index16);
					StackPush(&vm.stack, *(vm.stack.data + vm.stackFrame + index16), StackIsObject(&vm.stack, vm.stackFrame + index16));
					break;
				}

				narrow {
					op(index8);
					StackPush(&vm.stack, *(vm.stack.data + vm.stackFrame + index8), StackIsObject(&vm.stack, vm.stackFrame + index8));
					break;
				}
			}

			inst(OP_STORE, index8|index16) {
				wide {
					op16(index16);

					bool isObject;
					i64 value = StackPop(&vm.stack, &isObject);

					*(vm.stack.data + vm.stackFrame + index16) = value;
					StackSetObject(&vm.stack, vm.stackFrame + index16, isObject);

					break;
				}

				narrow {
					op16(index8);

					bool isObject;
					i64 value = StackPop(&vm.stack, &isObject);

					*(vm.stack.data + vm.stackFrame + index8) = value;
					StackSetObject(&vm.stack, vm.stackFrame + index8, isObject);

					break;
				}
			}

			inst(OP_ADD) {
				pop(&vm.stack, rhs, null);
				pop(&vm.stack, lhs, null);

				StackPush(&vm.stack, lhs + rhs, false);

				break;
			}

			inst(OP_SUB) {
				pop(&vm.stack, rhs, null);
				pop(&vm.stack, lhs, null);

				StackPush(&vm.stack, lhs - rhs, false);

				break;
			}

			inst(OP_MUL) {
				pop(&vm.stack, rhs, null);
				pop(&vm.stack, lhs, null);

				StackPush(&vm.stack, lhs * rhs, false);

				break;
			}

			inst(OP_DIV) {
				pop(&vm.stack, rhs, null);
				pop(&vm.stack, lhs, null);

				StackPush(&vm.stack, lhs / rhs, false);

				break;
			}

			inst(OP_AND) {
				pop(&vm.stack, rhs, null);
				pop(&vm.stack, lhs, null);

				StackPush(&vm.stack, lhs & rhs, false);

				break;
			}

			inst(OP_OR) {
				pop(&vm.stack, rhs, null);
				pop(&vm.stack, lhs, null);

				StackPush(&vm.stack, lhs | rhs, false);

				break;
			}

			inst(OP_XOR) {
				pop(&vm.stack, rhs, null);
				pop(&vm.stack, lhs, null);

				StackPush(&vm.stack, lhs ^ rhs, false);

				break;
			}

			inst(OP_INC) {
				vm.stack.data[vm.stack.top - 1] += 1;
				break;
			}

			inst(OP_DEC) {
				vm.stack.data[vm.stack.top - 1] -= 1;
				break;
			}

			inst(OP_NOT) {
				vm.stack.data[vm.stack.top - 1] = ~vm.stack.data[vm.stack.top - 1];
				break;
			}

			inst(OP_NEG) {
				vm.stack.data[vm.stack.top - 1] = -vm.stack.data[vm.stack.top - 1];
				break;
			}

			inst(OP_CMP) {
				pop(&vm.stack, rhs, null);
				pop(&vm.stack, lhs, null);

				vm.flags = 0;

				if (lhs == rhs) SetFlag(FLAG_EQ);
				if (lhs < rhs) SetFlag(FLAG_LF);
				if (lhs > rhs) SetFlag(FLAG_GF);

				break;
			}

			inst(OP_JMP) {
				pop(&vm.stack, value, null);
				JumpUnconditional(value);

				break;
			}

			inst(OP_JMPREL, rel8|rel32) {
				wide {
					op32(rel32);
					JumpUnconditional(rel32);
				}

				narrow {
					op(rel8);
					JumpUnconditional(rel8);
				}

				break;
			}

			inst(OP_JEQ) {
				pop(&vm.stack, value, null);
				JumpConditional(value, FLAG_EQ);

				break;
			}

			inst(OP_JEQREL, rel8|rel32) {
				wide {
					op32(rel32);
					JumpConditional(rel32, FLAG_EQ);
				}

				narrow {
					op(rel8);
					JumpConditional(rel8, FLAG_EQ);
				}

				break;
			}

			inst(OP_JNE) {
				pop(&vm.stack, value, null);
				JumpConditionalNot(value, FLAG_EQ | FLAG_LF | FLAG_GF);

				break;
			}

			inst(OP_JNEREL, rel8|rel32) {
				wide {
					op32(rel32);
					JumpConditionalNot(rel32, FLAG_EQ | FLAG_LF | FLAG_GF);
				}

				narrow {
					op(rel8);
					JumpConditionalNot(rel8, FLAG_EQ | FLAG_LF | FLAG_GF);
				}

				break;
			}

			inst(OP_JLT) {
				pop(&vm.stack, value, null);
				JumpConditional(value, FLAG_LF);

				break;
			}

			inst(OP_JLTREL, rel8|rel32) {
				wide {
					op32(rel32);
					JumpConditional(rel32, FLAG_LF);
				}

				narrow {
					op(rel8);
					JumpConditional(rel8, FLAG_LF);
				}

				break;
			}

			inst(OP_JGT) {
				pop(&vm.stack, value, null);
				JumpConditional(value, FLAG_GF);

				break;
			}

			inst(OP_JGTREL, rel8|rel32) {
				wide {
					op32(rel32);
					JumpConditional(rel32, FLAG_GF);
				}

				narrow {
					op(rel8);
					JumpConditional(rel8, FLAG_GF);
				}

				break;
			}

			inst(OP_JLE) {
				pop(&vm.stack, value, null);
				JumpConditional(value, FLAG_EQ | FLAG_LF);

				break;
			}

			inst(OP_JLEREL, rel8|rel32) {
				wide {
					op32(rel32);
					JumpConditional(rel32, FLAG_EQ | FLAG_LF);
				}

				narrow {
					op(rel8);
					JumpConditional(rel8, FLAG_EQ | FLAG_LF);
				}

				break;
			}

			inst(OP_JGE) {
				pop(&vm.stack, value, null);
				JumpConditional(value, FLAG_EQ | FLAG_GF);

				break;
			}

			inst(OP_JGEREL, rel8|rel32) {
				wide {
					op32(rel32);
					JumpConditional(rel32, FLAG_EQ | FLAG_GF);
				}

				narrow {
					op(rel8);
					JumpConditional(rel8, FLAG_EQ | FLAG_GF);
				}

				break;
			}

			inst(OP_CALL, const16|const32) {
				Constant* constant;
				
				wide {
					op32(const32);
					constant = GetConstant((u32) const32);
				}

				narrow {
					op16(const16);
					constant = GetConstant((u32) const16);
				}

				if (constant->kind != CONST_FUNCTION) {
					puts("VM error: attempt to call a non-function constant");
					ExitVM();
					exit(1);
				}

				i64* params = vm.stack.data + vm.stack.top - constant->function->paramCount;
				memmove(params + 3, params, constant->function->paramCount * sizeof(i64));

				u64 index = vm.stack.top - constant->function->paramCount;
				u64 byteOffset = index / 8;
				u64 bitOffset = index % 8;

				u64 shiftedBits = 0;

				for (u16 i = 0; i < constant->function->paramCount; i++) {
					u64 bitPosition = index + i;
					u64 byteIndex = bitPosition / 8;
					u64 bitInByte = bitPosition % 8;

					u64 bitValue = (vm.stack.typeInfos[byteIndex] >> bitInByte) & 0x01;
					shiftedBits |= (bitValue << i);

					vm.stack.typeInfos[byteIndex] &= ~(1 << bitInByte);
				}

				shiftedBits >>= 3;

				for (u16 i = 0; i < constant->function->paramCount; i++) {
					u64 bitPosition = index + i;
					u64 byteIndex = bitPosition / 8;
					u64 bitInByte = bitPosition % 8;

					vm.stack.typeInfos[byteIndex] |= ((shiftedBits >> i) & 0x01) << bitInByte;
				}

				vm.stack.top -= constant->function->paramCount;

				StackPush(&vm.stack, (i64) vm.ip, false);
				StackPush(&vm.stack, (i64) vm.currentFunction, false);
				StackPush(&vm.stack, (i64) vm.stackFrame, false);

				vm.ip = constant->function->entry;
				vm.currentFunction = constant->function;
				vm.stackFrame = vm.stack.top;

				vm.stack.top += constant->function->localCount + constant->function->paramCount;

				break;
			}

			inst(OP_RET) {
				bool hasReturnType = vm.currentFunction->type->returnType != &voidType;
				i64 returnValue;
				bool isReturnValueObject;

				if (hasReturnType) {
					returnValue = StackPop(&vm.stack, &isReturnValueObject);
				}

				vm.stack.top = vm.stackFrame;

				vm.stackFrame = StackPop(&vm.stack, null);
				vm.currentFunction = (Function*) StackPop(&vm.stack, null);
				vm.ip = (u8*) StackPop(&vm.stack, null);

				if (hasReturnType) {
					StackPush(&vm.stack, returnValue, isReturnValueObject);
				}

				break;
			}

			inst(OP_CONSTLOAD, const16|const32) {
				Constant* constant;

				wide {
					op32(const32);
					constant = GetConstant((u32) const32);
				}

				narrow {
					op16(const16);
					constant = GetConstant((u32) const16);
				}

				switch (constant->kind) {
					case CONST_FUNCTION: {
						StackPush(&vm.stack, (i64) constant->function, false);
						break;
					}
				}

				break;
			}

			inst(OP_LDI, imm32|imm64) {
				wide {
					op64(imm64);
					StackPush(&vm.stack, imm64, false);
				}

				narrow {
					op32(imm32);
					StackPush(&vm.stack, imm32, false);
				}

				break;
			}

			inst(OP_WIDE) {
				wideInst = true;
				goto main_loop;
			}

			inst(OP_DEBUG) {
				PrintVMDebug();

				break;
			}

			inst(OP_HLT) {
				register i64 exitCode = (vm.stack.top != 0) ? StackPop(&vm.stack, null) : 0;
				ExitVM();
				exit((int) exitCode);
			}

			default: {
				printf("VM error: unknown opcode %u\n", instruction);
				ExitVM();
				exit(1);
			}
		}

		wideInst = false;
	}
}