#pragma once

#include "types.h"

#include "JesusVM/Module.h"

#include "JesusVM/constpool/ConstPool.h"

#include <concepts>
#include <iostream>

namespace JesusVM {
	// A stack that allocates frames as needed within a maximum amount of memory (or 0 to use memory until it can't anymore)
	template <u64 maxMemory>
	class Stack {
	public:
		class Frame {
		friend class Stack;
		public:
			Frame(u16 localCount, u16 maxFrameSize, ConstPool& constPool, Module* module, Function* function, u8* returnAddress)
				: mPrevious(nullptr)
				, mLocalCount(localCount)
				, mStackSize(maxFrameSize)
				, mStackTop(0)
				, mLocals(std::make_unique<i32[]>(localCount))
				, mStack(std::make_unique<i32[]>(maxFrameSize))
				, mConstPool(constPool)
				, mCurrentModule(module)
				, mCurrentFunction(function)
				, mReturnAddress(returnAddress) {}

			void push(i32 value) {
				if (mStackTop >= mStackSize) {
					std::cout << "stackoverflow. todo: proper errors\n";
					std::exit(1);
				}

				mStack[mStackTop++] = value;
			}

			void pushLong(i64 value) {
				if (mStackTop + 2 > mStackSize) {
					std::cout << "stackoverflow (long). todo: proper errors\n";
					std::exit(1);
				}

				mStack[mStackTop++] = static_cast<i32>(value >> 32);
				mStack[mStackTop++] = static_cast<i32>(value & 0xFFFFFFFF);
			}

			i32 pop() {
				if (mStackTop <= 0) {
					std::cout << "stackunderflow. todo: proper errors\n";
					std::exit(1);
				}

				return mStack[--mStackTop];
			}

			i64 popLong() {
				if (mStackTop < 2) {
					std::cout << "stackunderflow (long). todo: proper errors\n";
					std::exit(1);
				}

				i32 low = mStack[--mStackTop];
				i32 high = mStack[--mStackTop];

				return (static_cast<i64>(high) << 32) | (static_cast<i64>(low) & 0xFFFFFFFF);
			}

			i8 getLocalByte(u16 index) {
				if (index >= mLocalCount) {
					std::cout << "too high local access. todo: proper errors\n";
					std::exit(1);
				}

				return static_cast<i8>(mLocals[index]);
			}

			i16 getLocalShort(u16 index) {
				if (index >= mLocalCount) {
					std::cout << "too high local access. todo: proper errors\n";
					std::exit(1);
				}

				return static_cast<i16>(mLocals[index]);
			}

			i32 getLocalInt(u16 index) {
				if (index >= mLocalCount) {
					std::cout << "too high local access. todo: proper errors\n";
					std::exit(1);
				}

				return static_cast<i32>(mLocals[index]);
			}

			i64 getLocalLong(u16 index) {
				if (index + 1 >= mLocalCount) {
					std::cout << "too high local access. todo: proper errors\n";
					std::exit(1);
				}

				i32 low = mLocals[index];
				i32 high = mLocals[index + 1];

				return (static_cast<i64>(high) << 32) | (static_cast<i64>(low) & 0xFFFFFFFF);
			}

			void setLocalByte(u16 index, i8 value) {
				if (index >= mLocalCount) {
					std::cout << "too high local access. todo: proper errors\n";
					std::exit(1);
				}

				mLocals[index] = static_cast<i32>(value);
			}

			void setLocalShort(u16 index, i16 value) {
				if (index >= mLocalCount) {
					std::cout << "too high local access. todo: proper errors\n";
					std::exit(1);
				}

				mLocals[index] = static_cast<i32>(value);
			}

			void setLocalInt(u16 index, i32 value) {
				if (index >= mLocalCount) {
					std::cout << "too high local access. todo: proper errors\n";
					std::exit(1);
				}

				mLocals[index] = value;
			}

			void setLocalLong(u16 index, i64 value) {
				if (index + 1 >= mLocalCount) {
					std::cout << "too high local access for long. todo: proper errors\n";
					std::exit(1);
				}

				mLocals[index] = static_cast<i32>(value & 0xFFFFFFFF); // low
				mLocals[index + 1] = static_cast<i32>(value >> 32); // high
			}

			const ConstPool& getConstPool() const {
				return mConstPool;
			}

			Module* getCurrentModule() const {
				return mCurrentModule;
			}

			Function* getCurrentFunction() const {
				return mCurrentFunction;
			}

			u8* getReturnAddress() const {
				return mReturnAddress;
			}

		private:
			std::unique_ptr<Frame> mPrevious;

			u16 mLocalCount;
			u16 mStackSize;
			u16 mStackTop;

			std::unique_ptr<i32[]> mLocals;
			std::unique_ptr<i32[]> mStack;
			
			ConstPool& mConstPool;

			Module* mCurrentModule;
			Function* mCurrentFunction;

			u8* mReturnAddress;
		};

	public:
		Stack()
			requires(maxMemory > 0) : mUsedMemory(0) { }

		Stack() = default;

		Frame* getTopFrame() const {
			return mTop.get();
		}

		Frame* enterFrame(u16 localCount, u16 maxFrameSize, Function* function, u8* returnAddress) {
			if constexpr (maxMemory == 0) {
				std::unique_ptr<Frame> frame = std::make_unique<Frame>(localCount, maxFrameSize, function->getModule()->getConstPool(), function->getModule(), function, returnAddress);
				Frame* result = frame.get();

				frame->mPrevious = std::move(mTop);
				mTop = std::move(frame);

				return result;
			} else {
				u64 requiredMemory = (localCount + maxFrameSize) * sizeof(i32) + sizeof(Frame);

				if (mUsedMemory + requiredMemory > maxMemory) {
					std::cerr << "Out of memory. TODO: proper error stuff\n";
					std::exit(1);
				}

				mUsedMemory += requiredMemory;

				std::unique_ptr<Frame> frame = std::make_unique<Frame>(localCount, maxFrameSize, function->getModule()->getConstPool());
				Frame* result = frame.get();

				frame->mPrevious = std::move(mTop);
				mTop = std::move(frame);

				return result;
			}
		}

		// Returns the new top frame after leaving
		Frame* leaveFrame() {
			if (mTop == nullptr) {
				std::cout << "stackunderflow. todo: proper errors\n";
				std::exit(1);
			}

			auto newFrame = std::move(mTop->mPrevious);

			if (newFrame == nullptr) {
				return nullptr;
			}

			Frame* result = newFrame.get();

			mTop = std::move(newFrame);

			return result;
		}

	private:
		std::conditional_t<(maxMemory > 0), u64, void*> mUsedMemory;
		std::unique_ptr<Frame> mTop;
	};
}