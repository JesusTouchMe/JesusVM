#pragma once

#include "types.h"

#include <mutex>
#include <unordered_set>
#include <string>

namespace JesusVM {
	// Shared runtime strings
	class StringPool {
	public:
		static std::string_view Intern(std::string_view str);

	private:
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

		static std::unordered_set<std::string, StringViewHash, StringViewEqual> pool;
		static std::mutex mutex;
	};
}