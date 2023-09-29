#ifndef EMULATION_ARG_PARSER_H
#define EMULATION_ARG_PARSER_H

#include <map>
#include <string>
#include <utility>

struct Argument {
public:
    std::string name;
    bool optional = true;
    bool present = false;
    bool has_value = false;

    std::string value;
    std::string description;
};

class ArgParser {
public:
    ArgParser() = default;
    ~ArgParser() = default;

    void parse(int argc, char** argv);
    [[nodiscard]] bool validate() const;
    bool has_arg(const std::string& name);
    std::vector<std::string> get_missing_args();
    std::string get_arg_value(const std::string& name);
    void add_argument(const std::string& name, bool has_value = false, bool optional = true);

    std::string get_program_path() const;

protected:
private:
    std::string program_name;
    std::string program_path;
    std::map<std::string, Argument> arguments;
    std::map<std::string, Argument>  unexpected_args;
};
#endif
