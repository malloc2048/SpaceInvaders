#include <iostream>
#include "game/invaders.h"
#include "game/arg_parser.h"
#include "disassembler/disassembler.h"

std::string generate_rom_filepath(ArgParser& parser) {
    auto rom_filename = parser.has_arg("--rom-file") ? parser.get_arg_value("--rom-file") : ROM_FILENAME();
    return parser.get_program_path() + rom_filename;
}

std::string generate_filepath(ArgParser& parser) {
    auto rom_filename = parser.has_arg("--path") ? parser.get_arg_value("--path") : DATA_FILEPATH();
    return parser.get_program_path() + rom_filename;
}

void disassemble(ArgParser& parser) {
    Disassembler disassembler;
    disassembler.disassemble(generate_filepath(parser));
}

void play_game(ArgParser& parser) {
    auto rom_file = std::ifstream(generate_filepath(parser) + ROM_FILENAME_());
    if (rom_file.is_open()) {
        Invaders game;
        game.load_rom(rom_file);
        rom_file.close();

        game.run();
    } else {
        fprintf(stdout, "loading rom file failed");
    }
}

int main(int argc, char** argv) {
    ArgParser parser;
    parser.add_argument("--rom-file", true);
    parser.add_argument("--disassemble");
    parser.parse(argc, argv);
    if(!parser.validate()) {
        std::cout << "missing command line argument(s) " << std::endl;
        auto missing_args = parser.get_missing_args();
        for(auto& arg: missing_args) {
            std::cout << "  " << arg << std::endl;
        }
        return 0;
    }

    if(parser.has_arg("--disassemble")) {
        disassemble(parser);
    } else {
        play_game(parser);
    }
    return 0;
}
