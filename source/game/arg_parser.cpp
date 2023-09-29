#include <iostream>
#include <algorithm>
#include "constants.h"
#include "arg_parser.h"

bool ArgParser::validate() const {
    return std::all_of(arguments.begin(), arguments.end(),
                       [](auto const& pair) {
        return pair.second.present || pair.second.optional;
    });
}

void ArgParser::parse(int argc, char** argv) {
    auto program = std::string_view(argv[0]);
    program_name = program.substr(program.find_last_of('/') + 1);
    program_path = program.substr(0,program.find_last_of('/') + 1);

    for(auto i = 1; i < argc; i++) {
        try {
            arguments.at(argv[i]).present = true;
            if (arguments.at(argv[i]).has_value) {
                arguments.at(argv[i]).value = argv[i+1];
                i += 1;
            }
        } catch (std::out_of_range& e) {
            std::cout << "unexpected argument " << argv[i] << std::endl;
        }
    }
}

std::vector<std::string> ArgParser::get_missing_args() {
    std::vector<std::string> missing_args;
    for(auto& arg: arguments) {
        if(!arg.second.present) {
            missing_args.push_back(arg.first);
        }
    }
    return missing_args;
}

bool ArgParser::has_arg(const std::string &name) {
    try {
        return arguments.at(name).present;
    } catch(std::out_of_range& e) {
        return false;
    }
}

std::string ArgParser::get_arg_value(const std::string &name) {
    try {
        if(arguments.at(name).present)
            return arguments.at(name).value;
    } catch(std::out_of_range& e) {
        return "";
    }
    return "";
}

void ArgParser::add_argument(const std::string &name, bool has_value, bool optional) {
    // only add if the argument does not exist already
    if(arguments.find(name) == arguments.end()) {
        arguments.insert({name, Argument{name, optional, false, has_value}});
    } else {
        std::cout << "attempt to adding existing argument, did you mean to update?" << std::endl;
    }
}

std::string ArgParser::get_program_path() const {
    return program_path;
}
