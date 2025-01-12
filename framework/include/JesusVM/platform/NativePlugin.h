#include "types.h"

#ifdef PLATFORM_WINDOWS

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#elif defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)

#include <dlfcn.h>

#endif

#include <iostream>
#include <string_view>

namespace Platform {
#ifdef PLATFORM_WINDOWS
	
	using NativeLibType = HMODULE;
	using NativeFuncType = FARPROC;

	constexpr NativeLibType LIBTYPE_NULL = nullptr;
	constexpr NativeFuncType FUNCTYPE_NULL = nullptr;

#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)

	using NativeLibType = void*;
	using NativeFuncType = void*;

	constexpr NativeLibType LIBTYPE_NULL = nullptr;
	constexpr NativeFuncType FUNCTYPE_NULL = nullptr;

#endif

	class NativePlugin {
	public:
		NativePlugin(std::string_view name, std::string_view libPath);

		~NativePlugin();

		std::string_view getName() const;

		bool load();
		void unload();

		template <typename FuncT>
		FuncT getFunction(std::string_view name);

	private:
		std::string_view mName;
		std::string_view mLibPath;
		NativeLibType mHandle;
	};

	template <typename FuncT>
	FuncT NativePlugin::getFunction(std::string_view name) {
		if (mHandle == LIBTYPE_NULL) {
			bool loaded = load();
			if (!loaded) {
				std::cout << "failed to load lib " << mLibPath << " (" << mName << "). TODO: better error stuff\n";
				std::exit(1);
			}
		}

#ifdef PLATFORM_WINDOWS
		NativeFuncType func = GetProcAddress(mHandle, std::string(name).c_str());
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
		NativeFuncType func = dlsym(mHandle, std::string(name).c_str());
#endif

		if (func == FUNCTYPE_NULL) {
			return nullptr;
		}

		return reinterpret_cast<FuncT>(func);
	}
}
