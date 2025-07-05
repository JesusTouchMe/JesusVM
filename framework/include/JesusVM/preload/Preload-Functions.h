// Copyright 2025 JesusTouchMe

PRELOAD_FUNCTION(rt::vm::System::self, "exit", "(I)V", rt::vm::System::exit, false)
PRELOAD_FUNCTION(rt::vm::System::self, "trap", "(Rvm/System:TrapInfo;)V", rt::vm::System::trap, false)
PRELOAD_FUNCTION(rt::vm::System::self, "loadPlugin", "(Rstd/Primitives:String;)V", rt::vm::System::loadPlugin, false)

PRELOAD_FUNCTION(rt::std::Primitives::self, "String::#Init", "(Rstd/Primitives:String;[C)V", rt::std::Primitives::String::init, false)