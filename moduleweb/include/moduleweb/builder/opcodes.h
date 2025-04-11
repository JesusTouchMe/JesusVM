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
    SHL,
    SHR,
    LAND,
    LOR,
    LXOR,
    LSHL,
    LSHR,

    NOT,
    NEG,
    LNOT,
    LNEG,

    POP,

    DUP,
    DUP2,

    SWAP,
    SWAP2,

    ILOAD,
    ISTORE,

    LLOAD,
    LSTORE,

    HLOAD,
    HSTORE,

    RLOAD,
    RSTORE,
    RLOAD_0, // load `this` shortcut

    BALOAD,
    BASTORE,

    CALOAD,
    CASTORE,

    SALOAD,
    SASTORE,

    IALOAD,
    IASTORE,

    LALOAD,
    LASTORE,

    HALOAD,
    HASTORE,

    RALOAD,
    RASTORE,

    ARRAYLENGTH,

    NEW,
    NEWARRAY,
    RNEWARRAY,

    ISINSTANCE,

    GETFIELD,
    SETFIELD,

    GETGLOBAL,
    SETGLOBAL,

    JMP_ICMPEQ,
    JMP_ICMPNE,
    JMP_ICMPLT,
    JMP_ICMPGT,
    JMP_ICMPLE,
    JMP_ICMPGE,

    JMP_HCMPEQ,
    JMP_HCMPNE,

    JMP_RCMPEQ,
    JMP_RCMPNE,

    JMP_HNULL,
    JMP_HNONNULL,

    JMP_RNULL,
    JMP_RNONNULL,

    JMPEQ,
    JMPNE,
    JMPLT,
    JMPGT,
    JMPLE,
    JMPGE,
    JMP,

    ICMP,
    LCMP,
    HCMP,
    RCMP,

    BPUSH,
    SPUSH,
    IPUSH,
    LPUSH,

    I2B,
    I2S,
    I2L,
    L2I,

    CONST_M1,
    CONST_0,
    CONST_1,

    LCONST_0,
    LCONST_1,

    HCONST_NULL,

    RCONST_NULL,

    CALL,

    RETURN,
    IRETURN,
    LRETURN,
    HRETURN,
    RRETURN,

    LDC,

    WIDE,

    // reserved opcodes. usually for debugger use
    BREAKPOINT = 253,
    RESERVE1 = 254,
    RESERVE2 = 255,
} moduleweb_opcodes;

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_OPCODES_H
