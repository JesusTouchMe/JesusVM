#ifndef JESUSVM_MODULE_H
#define JESUSVM_MODULE_H

#include "types.h"

#include "JesusVM/Function.h"

#include "JesusVM/constpool/ConstPool.h"

#include "JesusVM/heap/Class.h"

#include <memory>
#include <string_view>

namespace JesusVM {
	class Section {
	public:
		Section(std::string_view name, u32 size, std::unique_ptr<u8[]> buffer);

		std::string_view getName() const;
		u32 getSize() const;
		u8* getBuffer();
		const u8* getBuffer() const;

	private:
		std::string_view mName;
		u32 mSize;
		std::unique_ptr<u8[]> mBuffer;
	};

	class Module {
	public:
		Module(std::string_view name, u32 constCount, std::vector<Class> classes, std::vector<Function> functions, std::vector<Section> sections);

		std::string_view getName() const;
		ConstPool& getConstPool();

		Class* getClass(std::string_view name);
		Function* getFunction(std::string_view descriptor);
		Section* getSection(std::string_view name);

	private:
		std::string_view mName;

		ConstPool mConstPool;
		
		std::vector<Class> mClasses;
		std::vector<Function> mFunctions;
		std::vector<Section> mSections;
	};
}

#endif // JESUSVM_MODULE_H