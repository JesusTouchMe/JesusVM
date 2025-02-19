// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_OPCODES_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_OPCODES_H 1

typedef enum moduleweb_opcodes {
    NOP,

    ADD,
    SUB,
    MUL,
    DIV,
    REM,
    LADD,
    LSUB,
    LMUL,
    LDIV,
    LREM,

    AND,
    OR,
    XOR,
    LAND,
    LOR,
    LXOR,

    NOT,
    NEG,
    LNOT,
    LNEG,

    JMP_ICMPEQ,
    JMP_ICMPNE,
    JMP_ICMPLT,
    JMP_ICMPGT,
    JMP_ICMPLE,
    JMP_ICMPGE,

    JMPEQ,
    JMPNE,
    JMPLT,
    JMPGT,
    JMPLE,
    JMPGE,
    JMP,

    ICMP,
    LCMP,

    BPUSH,
    SPUSH,
    IPUSH,
    LPUSH,

    CONST_M1,
    CONST_0,
    CONST_1,

    LCONST_0,
    LCONST_1,

    CALL,

    RETURN,
    IRETURN,
    LRETURN,

    WIDE,
} moduleweb_opcodes;

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_OPCODES_H
