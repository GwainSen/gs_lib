#pragma once

#include <vector>
#include <functional>
#include <string>

namespace {
    struct ArgumentFunctionStruct {
        std::function<void()> voidFunc;
        std::function<void(const std::string&)> strFunc;

        explicit ArgumentFunctionStruct(const std::function<void()>& function)
        : voidFunc(function) {}
        explicit ArgumentFunctionStruct(const std::function<void(const std::string&)>& function)
        : strFunc(function) {}
    };

    typedef ArgumentFunctionStruct ArgumentFunction;

    struct ShortArgumentStruct {
        char _short_call;
        bool _parameter;
        ArgumentFunction _callback;

        ShortArgumentStruct(const char &short_call, const std::function<void()> &function)
        : _short_call(short_call),
        _parameter(false),
        _callback(function) {}
        ShortArgumentStruct(const char &short_call, const std::function<void(const std::string&)> &function)
        : _short_call(short_call),
        _parameter(true),
        _callback(function) {}
    };

    struct LongArgumentStruct {
        std::string _long_call;
        bool _parameter;
        ArgumentFunction _callback;

        LongArgumentStruct(std::string long_call, const std::function<void()> &function)
        : _long_call(std::move(long_call)),
        _parameter(false),
        _callback(function) {}
        LongArgumentStruct(std::string long_call, const std::function<void(const std::string&)> &function)
        : _long_call(std::move(long_call)),
        _parameter(true),
        _callback(function) {}
    };

    struct CombinedArgumentStruct {
        char _short_call;
        std::string _long_call;
        bool _parameter;
        ArgumentFunction _callback;

        CombinedArgumentStruct(const char &short_call, std::string long_call,
                               const std::function<void()> &function)
        : _short_call(short_call),
        _long_call(std::move(long_call)),
        _parameter(false),
        _callback(function) {}
        CombinedArgumentStruct(const char &short_call, std::string long_call,
                               const std::function<void(const std::string&)> &function)
        : _short_call(short_call),
        _long_call(std::move(long_call)),
        _parameter(true),
        _callback(function) {}
    };

    std::string removeDashes(const char *arg) {
        std::string output = arg;
        output.erase(0, 2);
        return output;
    }
}

namespace gs::argument {
    typedef ShortArgumentStruct ShortArgument;
    typedef LongArgumentStruct LongArgument;
    typedef CombinedArgumentStruct CombinedArgument;

    class Parser {
    private:
        int _defaultArgumentCount {0};
        std::vector<ShortArgument> _shortArgumentsList;
        std::vector<LongArgument> _longArgumentsList;
        std::vector<CombinedArgument> _combinedArgumentsList;

        void iterateLongArguments(const int &argc, char *argv[], const int &iteration) const {
            const std::string arg = removeDashes(argv[iteration]);
            for (auto &x: _longArgumentsList) {
                if (x._long_call == arg) {
                    if (x._parameter)
                        x._callback.strFunc(argv[iteration + 1]);
                    else
                        x._callback.voidFunc();
                }
            }
            for (auto &x: _combinedArgumentsList) {
                if (x._long_call == arg) {
                    if (x._parameter)
                        x._callback.strFunc(argv[iteration + 1]);
                    else
                        x._callback.voidFunc();
                }
            }
        }

        void iterateShortArguments(const int &argc, char *argv[], const int &iteration) const {
            for (auto &x: _shortArgumentsList) {
                if (argv[iteration][1] == x._short_call) {
                    if (x._parameter)
                        x._callback.strFunc(argv[iteration + 1]);
                    else
                        x._callback.voidFunc();
                }
            }
            for (auto &x: _combinedArgumentsList) {
                if (argv[iteration][1] == x._short_call) {
                    if (x._parameter)
                        x._callback.strFunc(argv[iteration + 1]);
                    else
                        x._callback.voidFunc();
                }
            }
        }

    public:
        explicit Parser(const int& defaultArgumentCount) : _defaultArgumentCount(defaultArgumentCount) {}

        void addArgument(ShortArgument arg) {_shortArgumentsList.push_back(std::move(arg));}
        void addArgument(LongArgument arg) {_longArgumentsList.push_back(std::move (arg));}
        void addArgument(CombinedArgument arg) {_combinedArgumentsList.push_back(std::move(arg));}

        [[nodiscard]] std::vector<std::string> parse(const int &argc, char *argv[]) const {
            for (int i = 1; i < argc; i++) {
                if (argv[i][0] == '-') {
                    if (argv[i][1] == '-') { // long argument
                        iterateLongArguments(argc, argv, i);
                    } else { //short argument
                        iterateShortArguments(argc, argv, i);
                    }
                }
            }
            std::vector<std::string> output;
            output.reserve(_defaultArgumentCount);
            for (int i = 0; i < _defaultArgumentCount; ++i)
                output.emplace_back(argv[argc - i]);
            return output;
        }
    };
}
