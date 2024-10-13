#include "vm.h"

#define inst(opcode, ...) case opcode:

#define _op1() (i8) (*vm.ip++)

#define op(name)   register i8 name = _op1()
#define op16(name) register i16 name = _op2()
#define op32(name) register i32 name = _op4()
#define op64(name) register i64 name = _op8()

#define pop(stack, name) register i64 name = StackPop(stack)

#define wide if (wideInst)
#define narrow else

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

// runningCodeBlock is true if this is only meant to run a block of bytecode which returns back to the caller of this function
void ExecuteCode(bool runningCodeBlock, u8* codeStart, u16 localCount) {
	i32 functionDepth = 1;

	bool wideInst = false;

	StackPush(&vm.stack, (i64) vm.ip);
	StackPush(&vm.stack, (i64) vm.currentModule);
	StackPush(&vm.stack, (i64) vm.currentFunction);
	StackPush(&vm.stack, (i64) vm.stack.frame);

	vm.ip = codeStart;
	vm.stack.frame = vm.stack.top;

	vm.stack.top += localCount;

	main_loop:
	while (true) {
		u8 instruction = *vm.ip++;

		switch (instruction) {
			inst(OP_NOP) {
				break;
			}

			inst(OP_POP) {
				StackPop(&vm.stack);
				break;
			}

			inst(OP_DUP) {
				u64 index = vm.stack.top - 1;
				i64 element = vm.stack.data[index];
				bool isObject = vm.stack.references[index];

				if (isObject) {
					StackPushObj(&vm.stack, (Object*) element);
				} else {
					StackPush(&vm.stack, element);
				}

				break;
			}

			inst(OP_LOAD, index8|index16) {
				wide {
					op16(index16);
					i64 value = *(vm.stack.data + vm.stack.frame + index16);

					StackPush(&vm.stack, value);

					break;
				}

				narrow {
					op(index8);
					i64 value = *(vm.stack.data + vm.stack.frame + index8);

					StackPush(&vm.stack, *(vm.stack.data + vm.stack.frame + index8));

					break;
				}
			}

			inst(OP_LOADOBJ, index8|index16) {
				wide {
					op16(index16);
					Object* value = (Object*) *(vm.stack.data + vm.stack.frame + index16);

					StackPushObj(&vm.stack, value);
					
					break;
				}

				narrow {
					op(index8);
					Object* value = (Object*) *(vm.stack.data + vm.stack.frame + index8);

					StackPushObj(&vm.stack, value);

					break;
				}
			}


			inst(OP_STORE, index8|index16) {
				wide {
					op16(index16);

					i64 value = StackPop(&vm.stack);

					*(vm.stack.data + vm.stack.frame + index16) = value;
					*(vm.stack.references + vm.stack.frame + index16) = false; // this instruction treats the value as a non-object no matter what it pops

					break;
				}

				narrow {
					op(index8);

					i64 value = StackPop(&vm.stack);

					*(vm.stack.data + vm.stack.frame + index8) = value;
					*(vm.stack.references + vm.stack.frame + index8) = false;

					break;
				}
			}

			inst(OP_STOREOBJ, index8|index16) {
				wide {
					op16(index16);

					i64 value = StackPop2(&vm.stack); // just gotta hope it's actually an object, otherewise it's a user mistake

					Object* obj = (Object*) *(vm.stack.data + vm.stack.frame + index16);
					if (obj != null) RemoveReference(obj); // this instruction expects the type of the variable index to always be of object type
					
					*(vm.stack.data + vm.stack.frame + index16) = value;
					*(vm.stack.references + vm.stack.frame + index16) = true; // this instruction treats the value as an object no matter what it pops

					break;
				}

				narrow {
					op(index8);

					i64 value = StackPop2(&vm.stack); // just gotta hope it's actually an object, otherewise it's a user mistake

					Object* obj = (Object*) *(vm.stack.data + vm.stack.frame + index8);
					if (obj != null) RemoveReference(obj); // this instruction expects the type of the variable index to always be of object type

					*(vm.stack.data + vm.stack.frame + index8) = value;
					*(vm.stack.references + vm.stack.frame + index8) = true; // this instruction treats the value as an object no matter what it pops

					break;
				}
			}

			inst(OP_ALOAD) {
				break;
			}

			inst(OP_ALOADOBJ) {
				break;
			}

			inst(OP_ASTORE) {
				break;
			}

			inst(OP_ASTOREOBJ) {
				break;
			}

			inst(OP_NEW, const16|const32) {
				Constant* constant;

				wide {
					op32(const32);
					constant = GetConstant(vm.currentModule, (u32) const32);
				}

				narrow {
					op16(const16);
					constant = GetConstant(vm.currentModule, (u32) const16);
				}

				if (constant->kind != CONST_CLASS) {
					puts("Attempt to allocate a non-class constant");
					ExitVM();
					exit(1);
				}

				Object* obj = AllocObject(constant->clas);

				StackPushObj(&vm.stack, obj);

				RemoveReference(obj); // remove our reference gained from AllocObject

				break;
			}

			inst(OP_NEWARRAY, const16|const32) {
				break;
			}

			inst(OP_NEWARRAYPRIM, typeId) {
				break;
			}

			inst(OP_GETFIELD, index16|index32) {
				Constant* constant;

				wide {
					op32(const32);
					constant = GetConstant(vm.currentModule, (u32) const32);
				}

				narrow {
					op16(const16);
					constant = GetConstant(vm.currentModule, (u32) const16);
				}

				if (constant->kind != CONST_FIELD) {
					puts("VM error: attempt to find field from non-field constant");
					ExitVM();
					exit(1);
				}

				Object* object = (Object*) StackPop2(&vm.stack);
				Field* field = &object->fields[constant->field->classFieldIndex];

				if (field->type->isPrimitive) {
					StackPush(&vm.stack, field->value.i);
				} else {
					StackPushObj(&vm.stack, field->value.ref);
				}

				RemoveReference(object);

				break;
			}

			inst(OP_PUTFIELD, index16|index32) {
				Constant* constant;

				wide {
					op32(const32);
					constant = GetConstant(vm.currentModule, (u32) const32);
				}

				narrow {
					op16(const16);
					constant = GetConstant(vm.currentModule, (u32) const16);
				}

				if (constant->kind != CONST_FIELD) {
					puts("VM error: attempt to find field from non-field constant");
					ExitVM();
					exit(1);
				}

				i64 value = StackPop2(&vm.stack);

				Object* object = (Object*) StackPop2(&vm.stack); // StackPop2 cuz we don't wanna accidentally free this before we done
				Field* field = &object->fields[constant->field->classFieldIndex];
				

				if (!field->type->isPrimitive) { // making assumption that the value is a valid object pointer
					RemoveReference((Object*) field->value.ref);
					Object* valueObject = (Object*) value; // no need to add a reference to this cuz it lost 1 from stack and gained 1 from being put in field

					if (field->type->classRef != valueObject->type) { // classes don't match, freak
						puts("VM error: field type doesn't match value type"); // make this explain the error more :pray:
						ExitVM();
						exit(1);
					}
				}

				field->value.i = value;

				RemoveReference(object); // we do this after everything else so there won't be freaky memory issues

				break;
			}

			inst(OP_ADD) {
				pop(&vm.stack, rhs);
				pop(&vm.stack, lhs);

				StackPush(&vm.stack, lhs + rhs);

				break;
			}

			inst(OP_SUB) {
				pop(&vm.stack, rhs);
				pop(&vm.stack, lhs);

				StackPush(&vm.stack, lhs - rhs);

				break;
			}

			inst(OP_MUL) {
				pop(&vm.stack, rhs);
				pop(&vm.stack, lhs);

				StackPush(&vm.stack, lhs * rhs);

				break;
			}

			inst(OP_DIV) {
				pop(&vm.stack, rhs);
				pop(&vm.stack, lhs);

				StackPush(&vm.stack, lhs / rhs);

				break;
			}

			inst(OP_AND) {
				pop(&vm.stack, rhs);
				pop(&vm.stack, lhs);

				StackPush(&vm.stack, lhs & rhs);

				break;
			}

			inst(OP_OR) {
				pop(&vm.stack, rhs);
				pop(&vm.stack, lhs);

				StackPush(&vm.stack, lhs | rhs);

				break;
			}

			inst(OP_XOR) {
				pop(&vm.stack, rhs);
				pop(&vm.stack, lhs);

				StackPush(&vm.stack, lhs ^ rhs);

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
				pop(&vm.stack, rhs);
				pop(&vm.stack, lhs);

				vm.flags = 0;

				if (lhs == rhs) SetFlag(FLAG_EQ);
				if (lhs < rhs) SetFlag(FLAG_LF);
				if (lhs > rhs) SetFlag(FLAG_GF);

				break;
			}

			inst(OP_JMP) {
				pop(&vm.stack, value);
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
				pop(&vm.stack, value);
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
				pop(&vm.stack, value);
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
				pop(&vm.stack, value);
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
				pop(&vm.stack, value);
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
				pop(&vm.stack, value);
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
				pop(&vm.stack, value);
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
					constant = GetConstant(vm.currentModule, (u32) const32);
				}

				narrow {
					op16(const16);
					constant = GetConstant(vm.currentModule, (u32) const16);
				}

				Function* function;

				if (constant->kind == CONST_FUNCTION) {
					function = constant->function;
				} else if (constant->kind == CONST_METHOD) {
					function = constant->method->function;
				} else {
					puts("VM error: attempt to call a non-callable constant");
					ExitVM();
					exit(1);
				}

				i64* params = vm.stack.data + vm.stack.top - function->paramCount;
				bool* paramsRef = vm.stack.references + vm.stack.top - function->paramCount;

				memmove(params + 4, params, function->paramCount * sizeof(i64)); // the +4 is for the 4 pushed to stack later 
				memmove(paramsRef + 4, paramsRef, function->paramCount * sizeof(i64)); // the +4 is for the 4 pushed to stack later 

				vm.stack.top -= function->paramCount;

				StackPush(&vm.stack, (i64) vm.ip);
				StackPush(&vm.stack, (i64) vm.currentModule);
				StackPush(&vm.stack, (i64) vm.currentFunction);
				StackPush(&vm.stack, (i64) vm.stack.frame);

				vm.ip = function->entry;
				vm.currentModule = function->module;
				vm.currentFunction = function;
				vm.stack.frame = vm.stack.top;

				vm.stack.top += function->localCount + function->paramCount;

				for (u64 i = vm.stack.frame + function->paramCount; i < vm.stack.top; i++) {
					vm.stack.data[i] = 0;
					vm.stack.references[i] = false;
				}

				functionDepth++;

				break;
			}

			inst(OP_RET) {
				functionDepth--;
				if (runningCodeBlock && functionDepth <= 0) {
					goto ret;
				}

				bool hasReturnType = vm.currentFunction->type->returnType != &voidType;
				i64 returnValue;

				if (hasReturnType) {
					returnValue = StackPop2(&vm.stack);
				}

				for (u64 i = vm.stack.frame; i < vm.stack.top; i++) {
					if (vm.stack.references[i]) {
						Object* obj = (Object*) vm.stack.data[i];

						if (obj != null) {
							RemoveReference(obj);
						}
					}
				}

				vm.stack.top = vm.stack.frame;

				vm.stack.frame = StackPop2(&vm.stack);
				vm.currentFunction = (Function*) StackPop2(&vm.stack);
				vm.currentModule = (Module*) StackPop2(&vm.stack);
				vm.ip = (u8*) StackPop2(&vm.stack);

				if (hasReturnType) {
					StackPush(&vm.stack, returnValue);
				}

				break;
			}

			inst(OP_LDC, const16|const32) {
				Constant* constant;

				wide {
					op32(const32);
					constant = GetConstant(vm.currentModule, (u32) const32);
				}

				narrow {
					op16(const16);
					constant = GetConstant(vm.currentModule, (u32) const16);
				}

				switch (constant->kind) {
					case CONST_FUNCTION:
					case CONST_CLASS:
					case CONST_FIELD:
					case CONST_METHOD: {
						StackPush(&vm.stack, (i64) constant->generic);
						break;
					}
				}

				break;
			}

			inst(OP_LDI, imm32|imm64) {
				wide {
					op64(imm64);
					StackPush(&vm.stack, imm64);
				}

				narrow {
					op32(imm32);
					StackPush(&vm.stack, imm32);
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
				i64 exitCode = (vm.stack.top != 0) ? StackPop(&vm.stack) : 0;
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

	ret:

	vm.stack.frame = StackPop2(&vm.stack);
	vm.currentFunction = (Function*) StackPop2(&vm.stack);
	vm.currentModule = (Module*) StackPop2(&vm.stack);
	vm.ip = (u8*) StackPop2(&vm.stack);
}