#ifndef APPLICATION_H
#define APPLICATION_H

#include "command_line_options.h"
#include <stdexcept>

class Application {
public:
    explicit Application(CommandLineOptions options);
    void run();

private:
    void execute_core_logic() const;
    CommandLineOptions m_options;
};

#endif
