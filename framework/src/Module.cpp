#include "JesusVM/Module.h"

namespace JesusVM {
	Section::Section(std::string_view name, u32 size, std::unique_ptr<u8[]> buffer)
		: mName(name)
		, mSize(size)
		, mBuffer(std::move(buffer)) { }

	std::string_view Section::getName() const {
		return mName;
	}

	u32 Section::getSize() const {
		return mSize;
	}

	u8* Section::getBuffer() {
		return mBuffer.get();
	}

	const u8* Section::getBuffer() const {
		return mBuffer.get();
	}

	Module::Module(std::string_view name, u32 constCount, std::vector<Class> classes, std::vector<Function> functions, std::vector<Section> sections)
		: mName(name)
		, mConstPool(constCount)
		, mClasses(std::move(classes))
		, mFunctions(std::move(functions))
		, mSections(std::move(sections)) {
		for (auto& function : mFunctions) {
			function.mModule = this; // todo: better impl
		}
	}

	std::string_view Module::getName() const {
		return mName;
	}

	ConstPool& Module::getConstPool() {
		return mConstPool;
	}

	Class* Module::getClass(std::string_view name) {
		auto it = std::find_if(mClasses.begin(), mClasses.end(), [name](Class& clas) { return clas.getName() == name; });
		if (it != mClasses.end()) return &*it;
		
		return nullptr;
	}

	Function* Module::getFunction(std::string_view descriptor) {
		auto it = std::find_if(mFunctions.begin(), mFunctions.end(), [descriptor](Function& function) { return function.getDescriptor() == descriptor; });
		if (it != mFunctions.end()) return &*it;

		return nullptr;
	}

	Section* Module::getSection(std::string_view name) {
		auto it = std::find_if(mSections.begin(), mSections.end(), [name](Section& section) { return section.getName() == name; });
		if (it != mSections.end()) return &*it;

		return nullptr;
	}
}