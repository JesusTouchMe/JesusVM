#include "JesusVM/util/StringPool.h"

namespace JesusVM {
	std::unordered_set<std::string, StringPool::StringViewHash, StringPool::StringViewEqual> StringPool::pool;
	std::mutex StringPool::mutex;

	std::string_view StringPool::Intern(std::string_view str) {
		std::lock_guard<std::mutex> lock(mutex);

		auto it = pool.find(str);
		if (it != pool.end()) {
			return *it;
		}

		auto inserted = pool.emplace(str);
		return *inserted.first;
	}
}