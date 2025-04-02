#include "JesusVM/platform/NativePlugin.h"

namespace Platform {
	NativePlugin::NativePlugin(std::string_view name, std::string_view libPath)
		: mName(name)
		, mLibPath(libPath)
		, mHandle(LIBTYPE_NULL) { }

	NativePlugin::~NativePlugin() {
		unload();
	}

	std::string_view NativePlugin::getName() const {
		return mName;
	}

	bool NativePlugin::load() {
        std::lock_guard<std::mutex> lock(mMutex);

		if (mHandle != LIBTYPE_NULL) {
			return true;
		}

#ifdef PLATFORM_WINDOWS
		mHandle = LoadLibraryA(std::string(mLibPath).c_str());
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
		mHandle = dlopen(std::string(mLibPath).c_str(), RTLD_LAZY);
#endif

		if (mHandle == LIBTYPE_NULL) {
			std::cout << "failed to load library: " << mLibPath << " (" << mName << "). TODO: better error stuff\n";
			return false;
		}

		return true;
	}

	void NativePlugin::unload() {
        std::lock_guard<std::mutex> lock(mMutex);

		if (mHandle == LIBTYPE_NULL) return;

#ifdef PLATFORM_WINDOWS
		FreeLibrary(mHandle);
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
		dlclose(mHandle);
#endif
		mHandle = LIBTYPE_NULL;
	}
}