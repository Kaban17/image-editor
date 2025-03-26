#include "command_line_options.h"

bool CommandLineOptions::is_valid() const {
    return !input_file.empty();
}
