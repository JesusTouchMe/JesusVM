#include "JesusVM/util/StringPool.h"

#include <iostream>

namespace JesusVM::StringPool {
    struct StringViewHash {
        using is_transparent = void;

        std::size_t operator()(std::string_view sv) const noexcept {
            return std::hash<std::string_view>{}(sv);
        }

        std::size_t operator()(const std::string& s) const noexcept {
            return std::hash<std::string>{}(s);
        }
    };

    struct StringViewEqual {
        using is_transparent = void;

        bool operator()(std::string_view lhs, std::string_view rhs) const noexcept {
            return lhs == rhs;
        }
    };

    struct StringPool {
        std::unordered_set<std::string, StringViewHash, StringViewEqual> pool;
        std::mutex mutex;

        ~StringPool() {
            std::cout << "whar\n";
        }
    };

    //TODO: look for a new solution please this is horrible and should not deserve to live
    static StringPool* pool = new StringPool();

	std::string_view Intern(std::string_view str) {
		std::lock_guard<std::mutex> lock(pool->mutex);

		auto it = pool->pool.find(str);
		if (it != pool->pool.end()) {
			return *it;
		}

		auto inserted = pool->pool.emplace(str);
		return *inserted.first;
	}

    const char* InternCString(std::string_view str) {
	    std::lock_guard<std::mutex> lock(pool->mutex);

	    auto it = pool->pool.find(str);
	    if (it != pool->pool.end()) {
	        return it->c_str();
	    }

	    auto inserted = pool->pool.emplace(str);
	    return inserted.first->c_str();
    }
}
