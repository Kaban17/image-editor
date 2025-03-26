#ifndef COMMAND_LINE_OPTIONS_H
#define COMMAND_LINE_OPTIONS_H

#include <string>

struct CommandLineOptions {
    bool show_help{false};
    bool show_version{false};
    std::string input_file;
    std::string output_file;
    int verbose_level{0};

    bool is_valid() const;
};

#endif
