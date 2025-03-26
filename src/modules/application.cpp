#include "application.h"
#include <iostream>

Application::Application(CommandLineOptions options)
    : m_options(std::move(options)) {}

void Application::run() {
    if (!m_options.is_valid()) {
        throw std::invalid_argument("Invalid command line options");
    }

    if (m_options.show_help) {
        return;
    }

    execute_core_logic();
}

void Application::execute_core_logic() const {
    std::cout << "Processing " << m_options.input_file
              << " -> " << m_options.output_file
              << " [verbosity: " << m_options.verbose_level << "]\n";
    // Actual business logic implementation
}
