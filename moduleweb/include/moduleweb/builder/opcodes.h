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

    AND,
    OR,
    XOR,
    SHL,
    SHR,

    NOT,
    NEG,

    POP,

    DUP,
    DUP2,
    DUP_X1, // straight out of java
    DUP_X2, // this too

    SWAP,

    INC,

    LOAD,
    STORE,

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

    JMP_CMPEQ,
    JMP_CMPNE,
    JMP_CMPLT,
    JMP_CMPGT,
    JMP_CMPLE,
    JMP_CMPGE,

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

    CMP,
    HCMP,
    RCMP,

    BPUSH,
    SPUSH,
    IPUSH,
    LPUSH,

    L2B,
    L2S,
    L2I,

    CONST_M1,
    CONST_0,
    CONST_1,
    CONST_2,
    CONST_3,
    CONST_4,

    HCONST_NULL,

    RCONST_NULL,

    CALL,
    CALLVIRTUAL,

    RETURN,
    LRETURN,
    HRETURN,
    RRETURN,

    LDC,

    WIDE,

    EXT0 = 0xF0,
    EXT1 = 0xF1,
    EXT2 = 0xF2,
    EXT3 = 0xF3,
    EXT4 = 0xF4,
    EXT5 = 0xF5,
    EXT6 = 0xF6,
    EXT7 = 0xF7,
    EXT8 = 0xF8,
    EXT9 = 0xF9,
    EXTA = 0xFA,
    EXTB = 0xFB,
    EXTC = 0xFC,
    EXTD = 0xFD,
    EXTE = 0xFE,
    EXTF = 0xFF,
} moduleweb_opcodes;

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_OPCODES_H
