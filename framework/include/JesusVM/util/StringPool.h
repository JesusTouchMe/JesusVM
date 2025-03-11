#ifndef JESUSVM_STRINGPOOL_H
#define JESUSVM_STRINGPOOL_H

#include "types.h"

#include <mutex>
#include <unordered_set>
#include <string>

namespace JesusVM::StringPool {
    std::string_view Intern(std::string_view str);
}

#endif // JESUSVM_STRINGPOOL_H
