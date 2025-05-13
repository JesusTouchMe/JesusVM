// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_UTIL_LOGGING_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_UTIL_LOGGING_H

#include <cstdio>
#include <format>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string_view>

namespace JesusVM {
    class LogSink {
    public:
        explicit LogSink(FILE* file) : mFile(file) {}
        ~LogSink() { fflush(mFile); }

        void write(std::string_view str) { fwrite(str.data(), 1, str.length(), mFile); }
        void write(char c) { fputc(c, mFile); }

        template <class T>
        requires std::is_integral_v<T>
        void write(T value) {
            char buffer[32];
            char* end = std::format_to(buffer, "{}", value);

            fwrite(buffer, 1, end - buffer, mFile);
        }

    private:
        FILE* mFile;
    };

    class Logger {
    public:
        static void Init(FILE* file) {
            std::lock_guard<std::mutex> lock(mMutex);

            if (mFile != nullptr)
                fclose(mFile);
            mFile = file;
        }

        static void Close() {
            std::lock_guard<std::mutex> lock(mMutex);

            if (mFile != nullptr) {
                fclose(mFile);
                mFile = nullptr;
            }
        }

        static void Log(std::string_view str) {
            if (mFile == nullptr) return;

            std::lock_guard<std::mutex> lock(mMutex);

            LogSink sink(mFile);

            sink.write(str);
            sink.write('\n');
        }

        template <class... Args>
        static void Format(std::format_string<Args...> fmt, Args&&... args) {
            if (mFile == nullptr) return;

            std::lock_guard<std::mutex> lock(mMutex);

            LogSink sink(mFile);
            std::string text = std::format(fmt, std::forward<Args>(args)...);

            PrintTimestamp(sink);

            sink.write(text);
            sink.write('\n');
        }

    private:
        static inline FILE* mFile = nullptr;
        static inline std::mutex mMutex;

        static void PrintTimestamp(LogSink& sink) {
            using namespace std::chrono;
            auto now = system_clock::now();
            auto now2 = system_clock::to_time_t(now);
            auto msPart = duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000;

            std::ostringstream oss;
            oss << '[' << std::put_time(std::localtime(&now2), "%Y-%m-%d %H:%M:%S")
                << '.' << std::setw(3) << std::setfill('0') << msPart
                << "] ";

            sink.write(oss.view());
        }
    };
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_UTIL_LOGGING_H
