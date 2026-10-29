#pragma once

#include <string>
#include <cstdint>
#include <filesystem>
#include <fstream>

namespace gs {
    class Logger {
    private:
        enum LogType : std::uint8_t {
            Info,
            Debug,
            Warning,
            Error,
        };

        const std::filesystem::path LogFolder{"log"};
        const std::string _name;
        std::filesystem::path _logFilePath;
        std::ofstream _logFile;

        static std::string createPreamble(const LogType& type) {
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
            _logFile << createPreamble(type) + message;
        };

    public:
        explicit Logger(std::string name)
        : _name{std::move(name)}{
            _logFilePath = std::filesystem::path{LogFolder};
            _logFilePath /= _name + ".log";

            if (!std::filesystem::exists(_logFilePath)) {
                std::filesystem::create_directories(LogFolder);
                _logFile.open(_logFilePath, std::ios::app);
                _logFile << "New Log File Created\n";
            } else {
                _logFile.open(_logFilePath, std::ios::app);
            }
        };

        ~Logger() {_logFile.close();}

        void logInfo(const std::string& message) {log(LogType::Info, message);}
        void logDebug(const std::string& message) {log(LogType::Debug, message);}
        void logWarning(const std::string& message) {log(LogType::Warning, message);}
        void logError(const std::string& message) {log(LogType::Error, message);}
    };
}