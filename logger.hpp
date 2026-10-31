#pragma once

#include <string>
#include <cstdint>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>
#include <mutex>
#include <queue>

using namespace std::chrono_literals;

namespace gs {
    class Logger {
    private:
        enum LogType : std::uint8_t {
            Info,
            Debug,
            Warning,
            Error,
        };

        std::thread _logThread;
        std::thread _syncThread;

        const std::filesystem::path LogFolder{"log"};
        const std::string _name;
        std::filesystem::path _logFilePath;
        std::ofstream _logFile;
        bool _runLogging = true;

        std::queue<std::string> _logQueueOut;
        std::mutex _mLogQueueOut;
        std::queue<std::string> _logQueueIn;
        std::mutex _mLogQueueIn;

        void logThread() {
            _logFile.open(_logFilePath, std::ios::app);
            do {
                _mLogQueueOut.lock();
                do {
                    if ( !_logQueueOut.empty() ) {
                        _logFile << _logQueueOut.front();
                        _logQueueOut.pop();
                    }
                } while (!_logQueueOut.empty());
                _mLogQueueOut.unlock();
                std::this_thread::sleep_for(1s);
            } while (_runLogging);
        }

        void syncThread() {
            do {
                _mLogQueueOut.unlock();
                std::this_thread::sleep_for(1s);
                _mLogQueueOut.lock();
                _mLogQueueIn.lock();
                do {
                    for (int i = 0; i < _logQueueIn.size(); ++i) {
                        _logQueueOut.emplace(_logQueueIn.front());
                        _logQueueIn.pop();
                    }
                } while (!_logQueueIn.empty());
                _mLogQueueIn.unlock();
            } while (_runLogging);
        }

        static std::string createPreamble(const LogType& type) {
            // TODO: add time stamp
            std::string output;
            switch (type) {
                case LogType::Info:
                    output = "I: ";
                    break;
                case LogType::Debug:
                    output = "D: ";
                    break;
                case LogType::Warning:
                    output = "W: ";
                    break;
                case LogType::Error:
                    output = "E: ";
                    break;
            }
            return output;
        }

        void log(const LogType& type, const std::string& message) {
            _mLogQueueIn.lock();
            _logQueueIn.emplace(createPreamble(type) + message + "\n");
            _mLogQueueIn.unlock();
        };

    public:
        explicit Logger(std::string name)
        : _name{std::move(name)}{
            _logFilePath = std::filesystem::path{LogFolder};
            _logFilePath /= _name + ".log";

            if (!std::filesystem::exists(_logFilePath)) {
                std::filesystem::create_directories(LogFolder);
                _mLogQueueIn.lock();
                _logQueueIn.emplace("New Log File Created\n");
                _mLogQueueIn.unlock();
            }
            _logThread = std::thread(&Logger::logThread, this);
            _syncThread = std::thread(&Logger::syncThread, this);
        };

        ~Logger() {
            bool endable = false;
            while (!endable) {
                _mLogQueueOut.lock();
                _mLogQueueIn.lock();
                if (_logQueueOut.empty() && _logQueueIn.empty()) {
                    endable = true;
                }
                _mLogQueueOut.unlock();
                _mLogQueueIn.unlock();
                std::this_thread::sleep_for(1s);
            }

            _runLogging = false;
            _syncThread.join();
            _logThread.join();
            _logFile.close();
        }

        void logInfo(const std::string& message) {log(LogType::Info, message);}
        void logDebug(const std::string& message) {log(LogType::Debug, message);}
        void logWarning(const std::string& message) {log(LogType::Warning, message);}
        void logError(const std::string& message) {log(LogType::Error, message);}
    };
}